///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2019
///////////////////////////////////////////////////////////////////////////////

#include "StageOne.h"
#include "imgui.h"
#include "imgui-sfml/imgui-SFML.h"
#include "GravityCommon/Builders.h"
//#include "ClientContext.h"
//#include "KeyboardHandlers.h"
#include "XMLLoaders/TMXReader.h"
#include "GravityCommon\Pathfinding.h"


#include <math.h>
#include  <assert.h>
#include <string>
namespace bali
{

    void
    StageOne::OnEvent(
            void* context,
            ConnManState::OnEventType t,
            uint32_t uid,
            Packet* packet
        )
    {
        static bool timeout_seen = false;
        SharedContext* pSharedContext = (SharedContext*)context;
        ConnMan& cm = *pSharedContext->pConnMan;
        Context* pContext = pSharedContext->pContext;

        switch (t)
        {
        case ConnManState::OnEventType::MESSAGE_RECEIVED:
            std::cout << "MESSAGE_RECIEVED" << std::endl;
            break;
        case ConnManState::OnEventType::CONNECTION_TIMEOUT:
            std::cout << "CONNECTION_TIMEOUT: " << std::endl;
            break;
        case ConnManState::OnEventType::MESSAGE_ACK_TIMEOUT: {
            MESG * m = (MESG*)packet->buffer;
            std::cout << "MESSAGE_ACK_TIMEOUT: " << CodeName[m->header.code] << std::endl;
            break;
        }case ConnManState::OnEventType::MESSAGE_ACK_RECEIVED: {
            MESG* m = (MESG*)packet->buffer;
            std::cout << "MESSAGE_ACK_RECEIVED: " << CodeName[m->header.code] << std::endl;
            break;
        }case ConnManState::OnEventType::CONNECTION_HANDSHAKE_GRANTED:{
            MESG* m = (MESG*)packet->buffer;
            std::cout << "CONNECTION_HANDSHAKE_GRANTED: " << std::endl;
            TMX::Objectgroup::Ptr ogptr;
            TMX::Object::Ptr optr;
            pContext->map->getObjectGroup("PlayerInstances", ogptr);
            optr = ogptr->objects.back();

            pContext->entities.push_back(Entity());
            CreateEntity(optr, false, true, false, pContext->protos, pContext->entities.back());
            pContext->remoteplayers.push_back(Player());
            pContext->remoteplayers.back().eid = pContext->entities.size() - 1;
            pContext->entities[pContext->entities.size() - 1].etype = Entity::EntityType::REMOTEPLAYER;
            pContext->remoteplayers.back().controller.initialize(pContext, uid);// TODO: controller is network input
            break;
        }case ConnManState::OnEventType::CONNECTION_HANDSHAKE_DENIED:
            std::cout << "CONNECTION_HANDSHAKE_DENIED: " << std::endl;
            break;
        case ConnManState::OnEventType::CONNECTION_HANDSHAKE_TIMEOUT:
            std::cout << "CONNECTION_HANDSHAKE_TIMEOUT: " << std::endl;
            break;
        case ConnManState::OnEventType::CONNECTION_HANDSHAKE_TIMEOUT_NOGRACK:
            std::cout << "CONNECTION_HANDSHAKE_TIMEOUT_NOGRACK: " << std::endl;
            break;
        }
    }

    StageOne::StageOne(Context::Ptr context)
        : Stage(context)
    {
    }

    StageOne::~StageOne()
    {
    }

    uint32_t StageOne::initialize()
    {
        TMX::Objectgroup::Ptr ogptr;
        srand(time(NULL));
        ////
        //
        // Phase 1: Load Level Assets
        //

        //
        // Initialize Dear imGUI
        //
        ImGui::SFML::Init(context->gameWindow.window);

        //
        // Load Configuration files for tweakable settings.
        //
        context->physicsConfig = loadPhysicsConfig("assets\\physics.config.txt");
        context->keyboardConfig = loadKeyboardConfig("assets\\keyboard.config.txt");
        context->generalConfig = loadGeneralConfig("assets\\general.config.txt");
        context->netConfig = LoadNetworkConfig("network.config.txt");
        //context->aiConfig = loadAIConfig("assets\\ai.config.txt");//TODO

        //
        // Load TMX from file, into map
        //
        context->map = std::make_shared<bali::TMX::Map>();
        TMX::TMXReader::load("assets\\level_test8_matt.tmx", context->map);


        //TMX::Layer::Ptr layer = context->map->getLayer("BackgroundImage");
        //context->backgroundImage;
        //
        // Load protos from TMX map
        //
        // Entity Protos
        std::string objTypeName[] = {
            "Player0",
            "Monster0"
        };

        for (auto s : objTypeName)
        {
            if (context->map->getObjectGroup(s, ogptr))
            {
                context->protos.push_back(Proto());
                loadPrototype(context->protos.back(), ogptr);
                context->protos.back().pid = s;
            }
            else
            {
                std::cout << "ObjectGroup " << s << " doesn't exist\n";
            }
        }


        ////
        ////  Load Entity instance layers
        ////   Player, Monster, Consumable
        ////
        //struct EntitySetup
        //{
        //    std::string iname;
        //    bool registerwithaidirectory;
        //    bool ignoreentitycollision;
        //    bool disablefriction;
        //};
        //EntitySetup isetup[] = {
        //    //name, registerwithaidirectory, ignoreentitycollision, disablefriction
        //    { "PlayerInstances", false, true, false },
        //{ "MonsterInstances", true, true , true },
        //{ "ConsumableInstances", false, true, false }
        //};
        //for (auto s : isetup)
        //{
        //    if (context->map->getObjectGroup(s.iname, ogptr))
        //    {
        //        addEntity(s.registerwithaidirectory,
        //            s.ignoreentitycollision,
        //            s.disablefriction,
        //            context->entities,
        //            context->protos,
        //            ogptr);
        //    }
        //}
        //context->shadowcopy = context->entities;
        ////
        //// Waypoints
        ////
        //if (context->map->getObjectGroup("Waypoints0", ogptr))
        //{
        //    buildWaypoints10(context->waypoints, ogptr);
        //}
        //else
        //{
        //    std::cout << "ERROR: could not find Waypoints0 in tmx file" << std::endl;
        //}

        //
        // Load Collision Polygon geometry from TMX map
        //
        if (context->map->getObjectGroup("Collision0", ogptr))
        {
            loadPolygons(context->allcollisionshapes, ogptr);
        }
        else
        {
            std::cout << "ERROR: could not find Collision0 in tmx file" << std::endl;
        }

        //
        // Initialize the Entity, and Collision Polygon spatial buckets.
        //
        uint32_t dimx = context->map->width * context->map->tilewidth;
        uint32_t dimy = context->map->height * context->map->tileheight;

        context->entitybuckets.initialize(0, 0, dimx, dimy, dimx / 32, dimy / 32);
        context->cpolybuckets.initialize(0, 0, dimx, dimy, dimx / 200, dimy / 200);// context->map->width / 2.0f, context->map->height / 2.0f);

        CreateCPolyBucket(context->allcollisionshapes,
                          context->cpolybuckets);


        ////
        //// Initialize Entity AI
        ////
        //context->AIDirector.initialize(context->entities);
        OnEventContext.pConnMan = &context->cm;
        OnEventContext.pContext = context;
        context->cm.Initialize(
            context->netConfig,
            ConnManState::ConnManType::SERVER,
            21025,
            OnEvent,
            &OnEventContext
        );
        // Last things? 
        //
        ////

        return 0;
    }

    uint32_t StageOne::doWindowEvent(sf::Event & event)
    {
        ImGui::SFML::ProcessEvent(event);
        switch (event.type) {
        case sf::Event::LostFocus:
            break;
        case sf::Event::GainedFocus:
            break;
        case sf::Event::MouseWheelScrolled: {
            this->done();
            break;
        }case sf::Event::Resized: {
            break;
        }case sf::Event::MouseMoved: {
            break;
        }case sf::Event::KeyPressed:
            break;
        case sf::Event::KeyReleased:
            break;
        case sf::Event::Closed:
            break;
        }
        return 0;
    }

    uint32_t StageOne::doUpdate()
    {
        //
        //
        //
        ImGui::SFML::Update(context->gameWindow.window, context->frametime);

        //
        // Get User Input, and apply
        //
        //context->players.back().controller.mk.Update(context->frametime);

        return 0;
    }

    uint32_t StageOne::doDraw()
    {
        const char* names[] = {
            "Dog",
            "Cat",
            "Mouse",
            "Ricky",
            "Matt",
            "Jackalo"
        };
        int index = 0;

        context->gameWindow.window.clear(sf::Color::Black);
        ImGui::Begin("Lobby");
        ImGui::ListBox("",&index, names, 6);
        //ImGui::SameLine();
        //ImGui::Button("Ready");

        ImGui::End();
        ImGui::SFML::Render(context->gameWindow.window);

        context->gameWindow.window.display();
        return 0;
    }

    uint32_t StageOne::cleanup()
    {
        ImGui::SFML::Shutdown();

        return 0;
    }




}//end namespace bali
