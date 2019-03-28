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

    //if (vec::dot(normal, physics::downVector(entity.proto.body.angle)) > 0.f)
    {
        entity.collider.surfaceNormal = normal;
    }
    //if (vec::dot(normal, physics::downVector(entity.proto.body.angle)) > 0.f)
    {
        entity.collider.jumpNormal = normal;
    }


    //if (&entity != &ctx->entities[0])
    //{//why isn't this being run
    //    
    //    sf::Vector2f d = physics::downVector(entity.proto.body.angle);
    //    //if (vec::dot(d, normal) < -0.4f && vec::dot(d, normal) > -0.7f)
    //    {
    //        /*float newangle = acos(vec::dot(d, normal));
    //        entity.proto.body.angle  =  (newangle * (180.f / PI)) / 25.0f;*/
    //        float newangle;
    //        newangle = atan2(normal.y, normal.x) - atan2(d.y, d.x);
    //        
    //        newangle *= (180.f / PI);
    //        if (newangle < 0) { newangle += 180.0f; }
    //        else { newangle -= 180.0f; }
    //        
    //        
    //        float oldangle = entity.proto.body.angle;
    //        //std::cout << oldangle << " --> " << newangle<< "  " << std::endl;
    //        //entity.proto.body.angle += newangle;
    //        for (size_t e = 1; e < ctx->entities.size();e++)
    //        {
    //            ctx->entities[e].proto.body.angle += newangle; // = entity.proto.body.angle;
    //        }
    //    }
    //}
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
    static std::list<float> anglehistory;
    sf::Vector2f pos = context->players[0].entity->proto.body.pos;
    float angle = context->players[0].entity->proto.body.angle;

    
    
    
    context->players[0].controller.mk.Update(context->frametime);

    context->AIDirector.update(context->frametime, context->entities, context->waypoints);

    //for (size_t e = 1; e < context->entities.size(); e++)
    //{
    //    Entity & entity = context->entities[e];
    //    Behavior & behavior = entity.behavior;
    //    entity.update(context->frametime);


    //    if (behavior.getstate() == Behavior::State::WALKING)
    //    {
    //        //// Find Closest Way point that is not `lastwaypoint`
    //        //// Move toward way point
    //        //// If have arrived at waypoint, update `lastwaypoint` to this waypoint.

    //        //sf::Vector2f minthere;
    //        //float mindist = 999999999999999999.f;
    //        //sf::Vector2f here = entity.proto.body.pos;

    //        //for (auto g = context->waypoints.begin();
    //        //    g != context->waypoints.end();
    //        //    g++)
    //        //{

    //        //    // calculate distance from here to there.
    //        //    // and go in the direction of the minimum distance
    //        //    sf::Vector2f there = sf::Vector2f(g->x, g->y);
    //        //    float dist = vec::mag(there - here);
    //        //    if (dist < mindist)
    //        //    {
    //        //        mindist = dist;
    //        //        minthere = there;
    //        //    }
    //        //}

    //        searchastar(context->waypoints, curwp, nextwp);
    //        //// 
    //        //if (entity.collider.surfaceNormal != vec::Zero())
    //        //{
    //        //    float str = 0.10f;
    //        //    sf::Vector2f nml;
    //        //    nml = minthere - here;//vec::normal(entity.collider.surfaceNormal);
    //        //    //str = vec::mag(nml);
    //        //    nml = vec::norm(nml);
    //        //    CommandQueue::postMove(entity.proto.body, str, nml, true);
    //        //    entity.collider.surfaceNormal = vec::Zero();
    //        //}
    //    }
    //}


    
    
    physics::ResolveAllCollisions(context, onCollision, onNonCollision, context->physicsConfig);


    //////
    float avgangle = 0.0f;
    size_t poscnt = 0;
    if (anglehistory.size() > 225)
        anglehistory.pop_front();
    anglehistory.push_back(angle);

    for (auto p = anglehistory.begin();
        p != anglehistory.end();
        p++)
    {
        avgangle = avgangle + *p;
        poscnt++;
    }
    avgangle /= poscnt;
    ///////

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
            for (int si = 0; si < context->entities[h].collisionshapes.size(); si++)
            {
                states.texture = NULL;
                context->allcollisionshapes[context->entities[h].collisionshapes[si]].setFillColor(sf::Color::Transparent);
                
                context->allcollisionshapes[context->entities[h].collisionshapes[si]].setOutlineThickness(2);
                context->allcollisionshapes[context->entities[h].collisionshapes[si]].setOutlineColor(sf::Color::Red);
                context->canvas.draw(context->allcollisionshapes[context->entities[h].collisionshapes[si]], states);
            }
        }
        // Draw Entities
        for (int e = 0; e < context->entities.size(); e++)
        {
            sf::Vector2f newpos = context->entities[e].proto.body.pos;
            sf::Vector2f vel = context->entities[e].proto.body.vel;
            sf::Vector2f next = physics::lerp(newpos,
                vel,
                context->frametime.asSeconds() * context->physicsConfig.FIXED_DELTA);
                

            context->entities[e].proto.shapes[0].setPosition(next);
            context->canvas.draw(context->entities[e].proto.shapes[0]);


            //sf::Vertex lines[2];
            //lines[0].position = context->entities[e].proto.body.pos;
            //lines[1].position = context->entities[e].distupper;

            //context->canvas.draw(lines, 2, sf::Lines);

            //lines[0].position = context->entities[e].proto.body.pos;
            //lines[1].position = context->entities[e].distahead;

            //context->canvas.draw(lines, 2, sf::Lines);

            //lines[0].position = context->entities[e].proto.body.pos;
            //lines[1].position = context->entities[e].distlower;

            //context->canvas.draw(lines,2, sf::Lines);
        }

        // Draw a box where we think the center of the entity is. center of gravity.
        for (int h = 0; h < context->entities[0].collisionentities.size(); h++)
        {
            sf::RectangleShape rs(sf::Vector2f(4, 4));
            rs.setFillColor(sf::Color::Red);
            sf::Vector2f pos(context->entities[context->entities[0].collisionentities[h]].proto.body.pos);
            size_t e = context->entities[0].collisionentities[h];
            sf::Vector2f newpos = GetCentroid(context->entities[e].proto.shapes[0]);
            rs.setPosition(newpos);
            context->canvas.draw(rs);
        }

        // Draw a box where we think the waypoints are.
        for (auto w = 0; w< context->waypoints.size();w++)
        {
            sf::RectangleShape rs;
            rs.setSize(sf::Vector2f(6, 6));
            rs.setPosition(sf::Vector2f(context->waypoints[w].location.x, context->waypoints[w].location.y));
            rs.setFillColor(sf::Color::Red);

            context->canvas.draw(rs);
        }

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
