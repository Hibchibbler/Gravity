///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageInit.h"
#include "imgui.h"
#include "imgui-sfml/imgui-SFML.h"
#include "GravityCommon/Builders.h"
#include "ClientContext.h"
#include "KeyboardHandlers.h"
#include "XMLLoaders/TMXReader.h"
#include "GravityCommon\Pathfinding.h"


#include <math.h>
#include  <assert.h>
#include <string>
namespace bali
{
StageInit::StageInit(Context::Ptr context)
    : Stage(context)
{
}

StageInit::~StageInit()
{
}



uint32_t StageInit::initialize()
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
    context->generalConfig= loadGeneralConfig("assets\\general.config.txt");
    //context->aiConfig = loadAIConfig("assets\\ai.config.txt");//TODO

    //
    // Load Fonts
    //
    context->systemfont.loadFromFile("assets\\neuropol x rg.ttf");

    //
    // Load TMX from file, into map
    //
    context->map = std::make_shared<bali::TMX::Map>();
    TMX::TMXReader::load("assets\\level_test6.tmx", context->map);


    
    //TMX::Layer::Ptr layer = context->map->getLayer("BackgroundImage");
    //context->backgroundImage;
    //
    // Load protos from TMX map
    //
    // Entity Protos
    std::string objTypeName[] = {
        "Player0",
        "Monster0",
        "Monster1",
        "Monster2",
        "Consumable0"
    };

    for (auto s : objTypeName)
    {
        if (context->map->getObjectGroup(s, ogptr))
        {
            context->protos.push_back(Proto());
            loadPrototype(context->protos.back(), ogptr);
            context->protos.back().pid = s;
            //context->protos.back().shapes.back().setFillColor(GetRandomColor(0));
        }
        else
        {
            std::cout << "ObjectGroup " << s << " doesn't exist\n";
        }
    }

    //
    // Map animation spritesheets to protos
    //
    for (auto & proto : context->protos)
    {
        std::string type = proto.pid;
        std::string spritesheetname = proto.spritesheetname;

        if (spritesheetname != "")
        {
            std::string jsonstr;
            ASE::LoadJSONString(spritesheetname, jsonstr);
            ASE::LoadSpriteSheet((char*)jsonstr.c_str(), proto.spritesheet);
            bali::LoadSequences(proto.spritesheet, proto.sequences);

            for (auto & seqpair : proto.sequences)
            {
                std::string name = seqpair.first;
                Sequence & seq = seqpair.second;

                bali::Animation newani;
                
                newani.sequence = seq; // Copy the sequence.
                newani.initialize(seq.frametagname, seq.cels.size(), true);
                newani.start();
                proto.wardrobe.animations[seq.frametagname] = newani;
            }
            proto.sstex = std::make_shared<Texture>();
            (*proto.sstex).img.loadFromFile(proto.spritesheet.meta.image);
            (*proto.sstex).tex.loadFromImage((*proto.sstex).img);
        }
        else
        {
            std::cout << "Proto Id=" << proto.tiledid << " doesn't have the property 'spritesheetname'\n";
        }
    }

    //
    //  Load Entity instance layers
    //   Player, Monster, Consumable
    //
    struct EntitySetup 
    {
        std::string iname;
        bool registerwithaidirectory;
        bool ignoreentitycollision;
        bool disablefriction;
    };

    EntitySetup isetup[] = {
        //name, registerwithaidirectory, ignoreentitycollision, disablefriction
        { "PlayerInstances", false, true, false },
        { "MonsterInstances", true, true , true },
        { "ConsumableInstances", false, true, false }
    };

    for (auto s : isetup)
    {
        if (context->map->getObjectGroup(s.iname, ogptr))
        {
            addEntity(s.registerwithaidirectory,
                      s.ignoreentitycollision,
                      s.disablefriction,
                      context->entities,
                      context->protos,
                      ogptr);
        }
    }
    context->shadowcopy = context->entities;

    //
    // Waypoints
    //
    if (context->map->getObjectGroup("Waypoints0", ogptr))
    {
        buildWaypoints10(context->waypoints, ogptr);
    }
    else
    {
        std::cout << "ERROR: could not find Waypoints0 in tmx file" << std::endl;
    }

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
    // Load tiles from TMX map
    //
    TMX::Tileset::Ptr tsb;
    if (context->map->getTileset("tilesetBack", tsb))
    {
        loadTileLayer(context->backgroundtiles,
                      tsb,
                      context->map->getLayer("BackgroundLayer"));
        loadTexture(context->backgroundtilesettexture,
                    tsb,
                    sf::Color::Magenta);
    }
    else
    {
        std::cout << "tilesetBack not found" << std::endl;
    }

    TMX::Tileset::Ptr tsf;
    if (context->map->getTileset("tilesetFore", tsf))
    {
        loadTileLayer(context->foregroundtiles,
                      tsf,
                      context->map->getLayer("ForegroundLayer"));
        loadTexture(context->foregroundtilesettexture,
                    tsf,
                    sf::Color::Black);
    }
    else
    {
        std::cout << "tilesetFore not found" << std::endl;
    }

    //
    // Put the tiles in a quad tree
    //
    uint32_t dimx = context->map->width * context->map->tilewidth;
    uint32_t dimy = context->map->height * context->map->tileheight;

    context->entitybuckets.initialize(0, 0, dimx, dimy, dimx/32, dimy/32);
    context->cpolybuckets.initialize(0, 0, dimx, dimy, dimx/200,dimy/200);// context->map->width / 2.0f, context->map->height / 2.0f);

    CreateCPolyBucket(context->allcollisionshapes,
                      context->cpolybuckets);


    //
    // Initialize Entity AI
    //
    context->AIDirector.initialize(context->entities);

    //
    // Create vertex arrays from the Tile data
    //  to be used for rendering.
    //
    createVertexLayer(context->backgroundvertices,
                      context->backgroundtiles,
                      context->map->tilewidth,
                      context->map->tileheight);

    createVertexLayer(context->foregroundvertices,
                      context->foregroundtiles,
                      context->map->tilewidth,
                      context->map->tileheight);


    context->canvas.create(context->map->width * context->map->tilewidth, 
                           context->map->height * context->map->tileheight);

    ////
    //
    // Phase 2: Configure Keyboard and Mouse
    //
    ////

    //
    // Register keys and buttons, associate handlers, for local player
    //
    context->players.clear();
    context->players.reserve(10);
    context->players.push_back(Player()); // Local player, or 0th entity
    context->players[0].entity = &context->entities[0];// Player0 is Local Player
    context->players[0].controller.initialize(context,
                                              context->keyboardConfig,
                                              KeyPressedHandler, 
                                              KeyDblPressedHandler, 
                                              KeyHeldHandler, 
                                              KeyReleasedHandler);

    context->camera.center = sf::Vector2f(32.f * (75.f / 2.f), 
                                          32.f * (75.f / 2.f));
    context->camera.view = sf::View(context->camera.center, 
                                    sf::Vector2f(1000,1000));
    context->mainZoomFactor = 1.f;
    //
    //
    // Last things? 
    //
    ////

    return 0;
}

uint32_t StageInit::doWindowEvent(sf::Event & event)
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

uint32_t StageInit::doUpdate()
{
    //
    //
    //
    ImGui::SFML::Update(context->gameWindow.window, context->frametime);

    //
    // Get User Input, and apply
    //
    context->players.back().controller.mk.Update(context->frametime);

    return 0;
}

uint32_t StageInit::doDraw()
{
    context->gameWindow.window.clear(sf::Color::Black);
    ImGui::Begin("Configuration");
        ImGui::Checkbox("Show Wall Poly", (bool*)&context->generalConfig.SHOW_OBSTRUCTION_POLYGON);
        ImGui::SameLine();
        ImGui::Checkbox("Show Entity Poly", (bool*)&context->generalConfig.SHOW_ENTITY_POLYGON);
        ImGui::Checkbox("Show Waypoints", (bool*)&context->generalConfig.SHOW_WAYPOINTS);
        ImGui::Checkbox("Show Centroids", (bool*)&context->generalConfig.SHOW_ENTITY_CENTROID);
        ImGui::Checkbox("Auto Gravity Entities", (bool*)&context->generalConfig.AUTO_GRAVITY_ENTITIES);
        ImGui::SameLine();
        ImGui::Checkbox("Auto Gravity Player", (bool*)&context->generalConfig.AUTO_GRAVITY_PLAYERS);
        ImGui::SliderFloat("FIXED_DELTA", &context->physicsConfig.FIXED_DELTA, 0.001f, 0.03f);
        ImGui::SliderFloat("GRAVITY_CONSTANT", &context->physicsConfig.GRAVITY_CONSTANT, 100.f, 10000.f);
        ImGui::SliderFloat("MOVE_STRENGTH", &context->physicsConfig.MOVE_STRENGTH, 1.f, 500.f);
        ImGui::SliderFloat("JUMP_STRENGTH", &context->physicsConfig.JUMP_STRENGTH, 1.f, 10000.f);
        ImGui::SliderInt("JUMP_COUNT (Additional Jumps)", (int*)&(context->physicsConfig.JUMP_COUNT), 0.f, 10.0f);
        ImGui::SliderFloat("FREEFALL_MOVE_STRENGTH", &context->physicsConfig.FREEFALL_MOVE_STRENGTH, 0.f, 500.0f);
        ImGui::SliderFloat("VELOCITY_MAX", &context->physicsConfig.VELOCITY_MAX, 0.f, 2500.0f);
        ImGui::SliderFloat("RESTITUTION", &context->physicsConfig.RESTITUTION, 0.f, 1.0f);
        ImGui::SliderFloat("DRAG_CONSTANT", &context->physicsConfig.DRAG_CONSTANT, 0.f, 1.0f);
        ImGui::SliderFloat("STATIC_FRICTION", &context->physicsConfig.STATIC_FRICTION, 0.f, 2500.0f);
        ImGui::SliderFloat("DYNAMIC_FRICTION", &context->physicsConfig.DYNAMIC_FRICTION, 0.f, 1.0f);
    ImGui::End();
    ImGui::SFML::Render(context->gameWindow.window);

    context->gameWindow.window.display();
    return 0;
}

uint32_t StageInit::cleanup()
{
    ImGui::SFML::Shutdown();
    context->players.back().controller.mk.Cleanup();
    return 0;
}




}//end namespace bali
