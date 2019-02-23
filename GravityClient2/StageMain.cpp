///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageMain.h"
#include "ClientContext.h"
#include "GravityCommon\Physics.h"
#include "GravityCommon\Builders.h"
#include <math.h>
#include  <assert.h>

namespace bali
{
StageMain::StageMain(Context* context)
    : Stage(context)
{
}

StageMain::~StageMain()
{
}

uint32_t StageMain::initialize()
{
    //

    //
    // Last things
    //
    //context->map.reset();
    return 0;
}

uint32_t StageMain::doWindowEvent(sf::Event & event)
{
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
        if (event.key.code == sf::Keyboard::P)
        {
            this->context->paused = !this->context->paused;
            if (context->paused == true)
            {
                this->context->pausedacc = context->frameacc;
                this->context->pausedftime = context->frametime;
            }
            else
            {
                context->frameacc = this->context->pausedacc;
                context->frametime = this->context->pausedftime;
            }
        }
        //if (event.key.code == sf::Keyboard::Right)
        //{
        //    context->camera.view.move(sf::Vector2f(10,0));
        //}
        //else if (event.key.code == sf::Keyboard::Left)
        //{
        //    context->camera.view.move(sf::Vector2f(-10, 0));
        //}
        //else if (event.key.code == sf::Keyboard::Up)
        //{
        //    context->camera.view.move(sf::Vector2f(0, 10));
        //}
        //else if (event.key.code == sf::Keyboard::Down)
        //{
        //    context->camera.view.move(sf::Vector2f(0, -10));
        //}

        break;
    case sf::Event::KeyReleased:
        break;
    case sf::Event::Closed:
        break;
    }
    return 0;
}

void onCollision(void* context, Entity & entity, sf::Vector2f normal)
{
    Context* ctx = (Context*)context;
    //std::cout << "X";
    entity.collider.surfaceNormal = normal;
    entity.collider.jumpNormal = normal;
    
}

void onNonCollision(void* context, Entity & entity)
{
    Context* ctx = (Context*)context;
    //std::cout << "Y";
    entity.collider.jumpNormal = vec::Zero();
    entity.collider.surfaceNormal = vec::Zero();
}

Shape GetTransformedShape(Shape & shape)
{
    Shape transformedShape = shape;
    sf::Transform t = shape.getTransform();
    for (int w = 0; w < shape.getPointCount(); w++) {
        transformedShape.setPoint(w, t.transformPoint(shape.getPoint(w)));
    }
    return transformedShape;
}
uint32_t StageMain::doUpdate()
{
    static std::list<sf::Vector2f> poshistory;
    context->players[0].controller.mk.Update(context->frametime);





    physics::ResolveAllCollisions(context, onCollision, onNonCollision, context->physicsConfig);

    sf::Vector2f pos = context->players[0].entity->proto.body.pos;
    float angle = context->players[0].entity->proto.body.angle;

    sf::Vector2f avgpos;
    float  poscnt = 0;
    if (poshistory.size() > 25)
        poshistory.pop_back();
    poshistory.push_front(pos);
    for (auto p = poshistory.begin();
        p != poshistory.end();
        p++)
    {
        avgpos = avgpos + *p;
        poscnt++;
    }
    avgpos = avgpos / poscnt;
    context->camera.view.setCenter(pos);
    context->camera.view.setRotation(angle);
    return 0;
}


uint32_t StageMain::doDraw()
{
    //
    // And Finally
    //


    context->gameWindow.window.clear(sf::Color::Black);

    {
        sf::RenderStates states;
        ////
        //
        // Draw
        //

        // clear everything
        context->gameWindow.window.clear(sf::Color::Black);
        context->canvas.clear(sf::Color::Black);

        // Draw backgound tiles
        if (context->backgroundvertices.size() > 0)
        {
            states.texture = &context->backgroundtilesettexture.tex;
            context->canvas.draw(&context->backgroundvertices[0], context->backgroundvertices.size(), sf::Quads, states);
        }
        // Draw foreground tiles
        if (context->foregroundvertices.size() > 0)
        {
            states.texture = &context->foregroundtilesettexture.tex;
            context->canvas.draw(&context->foregroundvertices[0], context->foregroundvertices.size(), sf::Quads, states);
        }
        
        //// Draw Collision Polygons
        ////
        //// Construct all collision polygons that are visible to the player
        ////
        for (int h = 0; h < 1; h++)
        {
            for (int si = 0; si < context->entities[h].collisionShapes.size(); si++)
            {
                states.texture = NULL;
                context->collisionshapes[context->entities[h].collisionShapes[si]].setFillColor(sf::Color::Transparent);
                
                context->collisionshapes[context->entities[h].collisionShapes[si]].setOutlineThickness(1);
                context->collisionshapes[context->entities[h].collisionShapes[si]].setOutlineColor(sf::Color::Red);
                context->canvas.draw(context->collisionshapes[context->entities[h].collisionShapes[si]], states);
            }
        }
        // Draw Entities
        for (int h = 0; h < context->entities.size(); h++)
        {
            context->canvas.draw(context->entities[h].proto.shapes[0]);
        }
        for (int h = 0; h < context->entities[0].collisionEntities.size(); h++)
        {
            sf::RectangleShape rs(sf::Vector2f(4, 4));
            rs.setFillColor(sf::Color::Cyan);
            sf::Vector2f pos(context->entities[context->entities[0].collisionEntities[h]].proto.body.pos);
            //pos.x += (context->entities[context->entities[0].collisionEntities[h]].proto.shapes[0].getGlobalBounds().width / 2.0f);// Change it also in Physics.cpp
            //pos.y += (context->entities[context->entities[0].collisionEntities[h]].proto.shapes[0].getGlobalBounds().height / 2.0f);
            /////////////////
            size_t e = context->entities[0].collisionEntities[h];
            float width = context->entities[e].proto.shapes[0].getLocalBounds().width / 2.f;
            float height = context->entities[e].proto.shapes[0].getLocalBounds().height / 2.f;

            width += context->entities[e].proto.body.pos.x;
            height += context->entities[e].proto.body.pos.y;

            sf::Vector2f newpos(width, height);
            rs.setPosition(newpos);// +sf::Vector2f(10, 0));
            context->canvas.draw(rs);
        }
        //for (int e = 0; e < context->entities.size(); e++)
        //{
        //    context->entities[0].collisionEntities.clear();
        //}

        context->canvas.display();

        // Draw canvas to display
        sf::Sprite levelSprite(context->canvas.getTexture());
        
        context->gameWindow.window.setView(context->camera.view);
        context->gameWindow.window.draw(levelSprite);
        //context->gameWindow.window.display();
    }



    context->gameWindow.window.display();

    return 0;
}

uint32_t StageMain::cleanup()
{
    context->players.back().controller.mk.Cleanup();
    return 0;
}




}//end namespace bali
