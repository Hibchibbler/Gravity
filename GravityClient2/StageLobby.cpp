///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2019
///////////////////////////////////////////////////////////////////////////////

#include "StageLobby.h"
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
    StageLobby::StageLobby(Context::Ptr context)
        : Stage(context)
    {
    }

    StageLobby::~StageLobby()
    {
    }



    uint32_t StageLobby::initialize()
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

  

        return 0;
    }

    uint32_t StageLobby::doWindowEvent(sf::Event & event)
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

    uint32_t StageLobby::doUpdate()
    {
        //
        //
        //
        ImGui::SFML::Update(context->gameWindow.window, context->frametime);


        return 0;
    }

    uint32_t StageLobby::doDraw()
    {
        context->gameWindow.window.clear(sf::Color::Black);
        ImGui::Begin("Lobby");
        //ImGui::Checkbox("Show Wall Poly", (bool*)&context->generalConfig.SHOW_OBSTRUCTION_POLYGON);
        //ImGui::SameLine();
        //ImGui::Checkbox("Show Entity Poly", (bool*)&context->generalConfig.SHOW_ENTITY_POLYGON);
        //ImGui::Checkbox("Show Waypoints", (bool*)&context->generalConfig.SHOW_WAYPOINTS);
        //ImGui::Checkbox("Show Centroids", (bool*)&context->generalConfig.SHOW_ENTITY_CENTROID);
        //ImGui::SameLine();
        //ImGui::SliderFloat("FIXED_DELTA", &context->physicsConfig.FIXED_DELTA, 0.001f, 0.03f);
        //ImGui::SliderFloat("GRAVITY_CONSTANT", &context->physicsConfig.GRAVITY_CONSTANT, 100.f, 10000.f);
        //ImGui::SliderFloat("MOVE_STRENGTH", &context->physicsConfig.MOVE_STRENGTH, 1.f, 500.f);
        //ImGui::SliderFloat("JUMP_STRENGTH", &context->physicsConfig.JUMP_STRENGTH, 1.f, 10000.f);
        //ImGui::SliderInt("JUMP_COUNT (Additional Jumps)", (int*)&(context->physicsConfig.JUMP_COUNT), 0.f, 10.0f);
        //ImGui::SliderFloat("FREEFALL_MOVE_STRENGTH", &context->physicsConfig.FREEFALL_MOVE_STRENGTH, 0.f, 500.0f);
        //ImGui::SliderFloat("VELOCITY_MAX", &context->physicsConfig.VELOCITY_MAX, 0.f, 5000.0f);
        //ImGui::SliderFloat("RESTITUTION", &context->physicsConfig.RESTITUTION, 0.f, 1.5f);
        //ImGui::SliderFloat("DRAG_CONSTANT", &context->physicsConfig.DRAG_CONSTANT, 0.f, 1.0f);
        //ImGui::SliderFloat("STATIC_FRICTION", &context->physicsConfig.STATIC_FRICTION, 0.f, 25.0f);
        //ImGui::SliderFloat("DYNAMIC_FRICTION", &context->physicsConfig.DYNAMIC_FRICTION, 0.f, 25.0f);
        ImGui::End();
        ImGui::SFML::Render(context->gameWindow.window);

        context->gameWindow.window.display();
        return 0;
    }

    uint32_t StageLobby::cleanup()
    {
        ImGui::SFML::Shutdown();
        return 0;
    }




}//end namespace bali
