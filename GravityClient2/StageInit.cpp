///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageInit.h"
#include "imgui.h"
#include "imgui-sfml/imgui-SFML.h"
#include "GravityCommon/ConfigLoader.h"
#include "GravityCommon/Builders.h"
#include "GravityCommon/MouseKeyboard.h"
#include "ClientContext.h"
#include "XMLLoaders/TMXReader.h"



#include <math.h>
#include  <assert.h>

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
    //
    // Phase 1: Load Level Assets
    //
    //
    // Load Fonts
    //
    context->systemfont.loadFromFile("assets\\neuropol x rg.ttf");
    //
    // Load TMX from file, into map
    //
    context->map = std::make_shared<bali::TMX::Map>();
    TMX::TMXReader::load("assets\\level_test1.tmx", context->map);
    //
    // Load entity defaults from TMX map
    //  Load entity geometry data
    //
    loadEntities(context->entities,
                 context->map->getObjectGroup("PlayerShapes"));
    loadEntities(context->entities,
                 context->map->getObjectGroup("MonsterShapes"));
    loadEntities(context->entities,
                 context->map->getObjectGroup("ConsumableShapes"));

    //
    // Load Collision Polygon geometry from TMX map
    //
    loadPolygons(context->collisionshapes,
                 context->map->getObjectGroup("CollisionPolygons"));

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


    context->canvas.create(context->map->width * context->map->tilewidth, context->map->height * context->map->tileheight);

    //
    //
    // Last things 
    //
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
        break;
    }case sf::Event::Resized: {
        break;
    }case sf::Event::MouseMoved: {
        break;
    }case sf::Event::KeyPressed:
        this->done();
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
    ImGui::SFML::Update(context->gameWindow.window, context->frametime);
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
    return 0;
}




}//end namespace bali
