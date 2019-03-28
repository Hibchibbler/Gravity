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
    TMX::TMXReader::load("assets\\level_test3.tmx", context->map);

    //
    // Load protos from TMX map
    //

    // Entity Protos
    context->protos.push_back(Proto());
    loadPrototype(context->protos.back(),
                  context->map->getObjectGroup("Player0"));
    context->protos.back().shapes.back().setFillColor(sf::Color::Green);

    context->protos.push_back(Proto());
    loadPrototype(context->protos.back(),
                  context->map->getObjectGroup("Monster0"));
    context->protos.back().shapes.back().setFillColor(GetRandomColor(0));

    context->protos.push_back(Proto());
    loadPrototype(context->protos.back(),
                  context->map->getObjectGroup("Monster1"));
            context->protos.back().shapes.back().setFillColor(GetRandomColor(0));

    context->protos.push_back(Proto());
    loadPrototype(context->protos.back(),
                  context->map->getObjectGroup("Monster2"));
            context->protos.back().shapes.back().setFillColor(GetRandomColor(0));

    context->protos.push_back(Proto());
    loadPrototype(context->protos.back(),
                  context->map->getObjectGroup("Consumable0"));
    context->protos.back().shapes.back().setFillColor(GetRandomColor(0));

    // Waypoints
    //buildWaypoints(context->waypoints, context->map->getObjectGroup("Waypoints0"));

    buildWaypoints10(context->waypoints, context->map->getObjectGroup("Waypoints10"));

    //for (auto obj = ogw->objects.begin(); obj != ogw->objects.end(); ++obj)
    //{
    //    bool status = false;
    //    if ((*obj)->point != nullptr)
    //    {
    //        sf::Vector2f point((*obj)->x, (*obj)->y);
    //        Waypoint wp;
    //        wp.pos = point;
    //        wp.id = std::strtol((*obj)->properties.back()->value.c_str(),NULL, 10);
    //        context->waypoints.push_back(wp);
    //    }
    //}



    //
    // Fuck it, Let's create a player
    // and some monsters
    //
    Proto p = context->protos[0]; // Player0
    Proto m = context->protos[1]; // Monster0
    Proto c = context->protos[2]; // Consumable0
    float x, y;

    context->entities.push_back(Entity());
    context->entities.back().registerwithaidirector = false;
    context->entities.back().ignoreentitycollision = true;
    context->entities.back().proto = p;
    x = context->map->getObjectGroup("Player0")->objects[0]->x;
    y = context->map->getObjectGroup("Player0")->objects[0]->y;
    context->entities.back().proto.body.pos = sf::Vector2f(x, y);
    //context->entities.back().proto.shapes[0].setPosition(sf::Vector2f(x, y));

    context->entities.push_back(Entity());
    context->entities.back().registerwithaidirector = true;
    context->entities.back().proto = c;
    x = context->map->getObjectGroup("Consumable0")->objects[0]->x;
    y = context->map->getObjectGroup("Consumable0")->objects[0]->y;
    context->entities.back().proto.body.pos = sf::Vector2f(x, y);
    context->entities.back().moving = true;

    srand(time(NULL));
    TMX::Objectgroup::Ptr og = context->map->getObjectGroup("MonsterInstances");
    for (int g = 0; g < og->objects.size(); g++) {
        context->entities.push_back(Entity());
        context->entities.back().registerwithaidirector = true;
        context->entities.back().ignoreentitycollision = true;
        std::string strid = og->objects[g]->properties.back()->value;
        size_t id = std::strtol(strid.c_str(), NULL, 10);
        std::cout << "[" << id << "]" << std::endl;
        bool yes = false;
        for (auto i = context->protos.begin();
            i != context->protos.end();
            i++)
        {
            if (i->id == id)
            {
                context->entities.back().proto = *i;
                yes = true;
                break;
            }
        }
        assert(yes == true);
        //if (yes == false)
        //{
        //    int a = 42;
        //}
//        context->entities.back().proto = m;

        context->entities.back().moving = true;// (rand() % 2 == 0 ? true : false);
        context->entities.back().proto.shapes.back().setFillColor(GetRandomColor(0));

        float x, y;
        float a = og->objects[g]->point->x;
        float b = og->objects[g]->point->y;
        context->entities.back().proto.body.pos = sf::Vector2f(a, b);
        context->entities.back().proto.shapes[0].setPosition(context->entities.back().proto.body.pos);
    }

    //
    // Load Collision Polygon geometry from TMX map
    //
    loadPolygons(context->allcollisionshapes,
                 context->map->getObjectGroup("Collision0"));

    loadPolygons(context->gravityzones,
        context->map->getObjectGroup("GravityZones"));

    //
    // Load tiles from TMX map
    //
    loadTileLayer(context->backgroundtiles,
                  context->map->getTileset("tilesetBack"),
                  context->map->getLayer("BackgroundLayer"));

    loadTileLayer(context->foregroundtiles,
                  context->map->getTileset("tilesetFore"),
                  context->map->getLayer("ForegroundLayer"));


    //
    // Put the tiles in a quad tree
    //
    context->entitybuckets.initialize(0, 0, 2460, 2460,75, 75);
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

    loadTexture(context->backgroundtilesettexture, 
                context->map,
                "tilesetBack", 
                sf::Color::Blue);

    loadTexture(context->foregroundtilesettexture,
                context->map,
                "tilesetFore",
                sf::Color::Blue);

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
