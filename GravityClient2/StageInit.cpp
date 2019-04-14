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

    //
    // Load Fonts
    //
    context->systemfont.loadFromFile("assets\\neuropol x rg.ttf");

    //
    // Load TMX from file, into map
    //
    context->map = std::make_shared<bali::TMX::Map>();
    TMX::TMXReader::load("assets\\level_test4.tmx", context->map);

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
            context->protos.back().shapes.back().setFillColor(GetRandomColor(0));
        }
    }
    //context->player0spritesheet.img.loadFromFile("assets\\evilman.png");
    //context->player0spritesheet.tex.loadFromImage(context->player0spritesheet.img);
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


    //
    // Waypoints
    //
    if (context->map->getObjectGroup("Waypoints0", ogptr))
    {
        buildWaypoints10(context->waypoints, ogptr);
    }

    //
    // Load Collision Polygon geometry from TMX map
    //
    if (context->map->getObjectGroup("Collision0", ogptr))
    {
        loadPolygons(context->allcollisionshapes, ogptr);
    }

    if (context->map->getObjectGroup("GravityZones", ogptr))
    {
        loadPolygons(context->gravityzones, ogptr);
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
    }
    else
    {
        std::cout << "tilesetFore not found" << std::endl;
    }


    //
    // Put the tiles in a quad tree
    //
    context->entitybuckets.initialize(0, 0, 1600, 1600,20, 20);
    context->cpolybuckets.initialize(0, 0, 2500, 2500, 15, 15);

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

    if (tsb)
    {
        loadTexture(context->backgroundtilesettexture,
            context->map,
            "tilesetBack",
            sf::Color::Blue);
    }

    if (tsf)
    {
        loadTexture(context->foregroundtilesettexture,
            context->map,
            "tilesetFore",
            sf::Color::Blue);
    }
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
                                    sf::Vector2f(900,900));

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
        ImGui::Button("Button 1");
        ImGui::Button("Button 2");
        ImGui::Button("Button C");
        static bool v = true;
        ImGui::Checkbox("Collision Polygon Visible", &v);
        ImGui::SliderFloat("MOVE_STRENGTH", &context->physicsConfig.MOVE_STRENGTH, 1.f, 500.f);
        ImGui::SliderFloat("GRAVITY_CONSTANT", &context->physicsConfig.GRAVITY_CONSTANT, 100.f, 5000.f);
        ImGui::SliderFloat("FIXED_DELTA", &context->physicsConfig.FIXED_DELTA, 0.f, 1.0f);
        ImGui::SliderFloat("DRAG_CONSTANT", &context->physicsConfig.DRAG_CONSTANT, 0.f, 1.0f);
        ImGui::SliderFloat("RESTITUTION", &context->physicsConfig.RESTITUTION, 0.f, 1.0f);
        ImGui::SliderFloat("SLOW_THRESHOLD", &context->physicsConfig.SLOW_THRESHOLD, 0.f, 2500.0f);
        ImGui::SliderFloat("STATIC_FRICTION_VELOCITY_MAX", &context->physicsConfig.STATIC_FRICTION_VELOCITY_MAX, 0.f, 2500.0f);
        ImGui::SliderFloat("STATIC_FRICTION", &context->physicsConfig.STATIC_FRICTION, 0.f, 2500.0f);
        ImGui::SliderFloat("DYNAMIC_FRICTION", &context->physicsConfig.DYNAMIC_FRICTION, 0.f, 1.0f);
        ImGui::SliderFloat("FAST_JUMP_THRESHOLD", &context->physicsConfig.FAST_JUMP_THRESHOLD, 0.f, 1.0f);
        ImGui::SliderFloat("JUMP_STRENGTH", &context->physicsConfig.JUMP_STRENGTH, 0.f, 1.0f);
        ImGui::SliderFloat("FREEFALL_MOVE_STRENGTH", &context->physicsConfig.FREEFALL_MOVE_STRENGTH, 0.f, 500.0f);
        ImGui::SliderFloat("VELOCITY_MAX", &context->physicsConfig.VELOCITY_MAX, 0.f, 2500.0f);
        ImGui::SliderFloat("JUMP_VELOCITY_MAX", &context->physicsConfig.JUMP_VELOCITY_MAX, 0.f, 2500.0f);
        ImGui::SliderInt("JUMP_COUNT", (int*)&(context->physicsConfig.JUMP_COUNT), 0.f, 100.0f);
        ImGui::SliderFloat("MOVE_VELOCITY_MAX", &context->physicsConfig.MOVE_VELOCITY_MAX, 0.f, 2500.0f);
        ImGui::SliderFloat("CHARGE_VELOCITY_MAX", &context->physicsConfig.CHARGE_VELOCITY_MAX, 0.f, 2500.0f);
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
