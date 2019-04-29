///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageMain.h"
#include "ClientContext.h"
#include "GravityCommon\Physics.h"
#include "GravityCommon\Builders.h"

#include "imgui.h"
#include "imgui-sfml/imgui-SFML.h"

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
    ImGui::SFML::Init(context->gameWindow.window);
    //
    // Last things
    //
    //context->map.reset();
    return 0;
}

uint32_t StageMain::doWindowEvent(sf::Event & event)
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

void onCollision(void* context_, Entity & entity, sf::Vector2f normal)
{
    Context* context = (Context*)context_;
    //std::cout << "X";

    //if (vec::dot(normal, physics::downVector(entity.proto.body.angle)) > 0.f)
    {
        entity.collider.surfaceNormal = normal;
    }
    //if (vec::dot(normal, physics::downVector(entity.proto.body.angle)) > 0.f)
    {
        entity.collider.jumpNormal = normal;
    }

    if (vec::dot(physics::downVector(entity.proto.body.angle), entity.collider.surfaceNormal) < 0)
        entity.collider.jumpcount = 0;

    //Entity & entity = context->entities[e];
    

        if (&entity == &context->entities[0] && !context->settings.AUTO_GRAVITY_PLAYERS)
            return;
        if (!context->settings.AUTO_GRAVITY_ENTITIES)
            return;

        if (entity.collider.surfaceNormal != vec::Zero())
        {
            if (context->settings.ENABLE_HEAD_BUMP_GRAVITY || 
                vec::dot(physics::upVector(entity.proto.body.angle), entity.collider.surfaceNormal) > -0.1)
                //if (&entity != &context->entities[0])
            {
                sf::Vector2f d = physics::downVector(entity.proto.body.angle);
                //if (vec::dot(d, normal) < -0.4f && vec::dot(d, normal) > -0.7f)
                {
                    /*float newangle = acos(vec::dot(d, normal));
                    entity.proto.body.angle  =  (newangle * (180.f / PI)) / 25.0f;*/
                    float newangle;
                    newangle = atan2(entity.collider.surfaceNormal.y, entity.collider.surfaceNormal.x) - atan2(d.y, d.x);

                    newangle *= (180.f / PI);
                    if (newangle < 0) { newangle += 180.0f; }
                    else { newangle -= 180.0f; }


                    float oldangle = entity.proto.body.angle;
                    //std::cout << oldangle << " --> " << newangle<< "  " << std::endl;
                    entity.proto.body.angle += newangle;
                    //for (size_t e = 0; e < context->entities.size(); e++)
                    //{
                    //    context->entities[e].proto.body.angle += newangle; // = entity.proto.body.angle;
                    //}
                }
            }
        }
        

}

//float dog(sf::Vector2f source, sf::Vector2f target)
//{
//    float deltarad;
//    float deltadeg;
//
//
//    deltarad = atan2(target.y, target.x) - atan2(source.y, source.x);
//    if (deltarad < 0) { deltarad += PI; }
//    else { deltarad -= PI;}
//    //return deltarad;
//
//    deltadeg = deltarad * (180.f / PI);
//    /*if (deltadeg < 0) { deltadeg += 180.0f; }
//    else { deltadeg -= 180.0f; }*/
//    return deltadeg;
//}
//
//void frog(Entity & entity)
//{
//    if (entity.collider.surfaceNormal != vec::Zero())
//    {
//        sf::Vector2f d = physics::downVector(entity.proto.body.angle);
//        float deltadeg = dog(d, entity.collider.surfaceNormal);
//
//        float oldangle = entity.proto.body.angle;
//        entity.proto.body.angle += deltadeg;
//        //std::cout << oldangle << " --> " << entity.proto.body.angle << "  " << std::endl;
//    }
//}

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

    ImGui::SFML::Update(context->gameWindow.window, context->frametime);
    //
    //
    //
    context->players[0].controller.mk.Update(context->frametime);

    //
    //
    //
    context->AIDirector.update(context->frametime, context->players, context->entities, context->waypoints);

    //
    //
    //
    physics::ResolveAllCollisions(context, onCollision, onNonCollision, context->physicsConfig);

    for (auto & ent : context->entities)
    {
        for (auto & a : ent.proto.wardrobe.animations)
        {
            a.second.update(context->frametime);
        }
    }
    //////
    float avgangle = 0.0f;
    sf::Vector2f avgpos;
    size_t poscnt = 0;
    if (anglehistory.size() > 2000)
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
    if (poshistory.size() > 5)
        poshistory.pop_front();
    poshistory.push_back(GetCentroid(context->entities[0].proto.shapes[0]));

    poscnt = 0;
    for (auto p = poshistory.begin();
        p != poshistory.end();
        p++)
    {
        avgpos = avgpos + *p;
        poscnt++;
    }
    avgpos = avgpos / (float)poscnt;
    ///////

    context->zaxpos = avgpos;
    context->camera.view.setCenter(avgpos);
    context->camera.view.setRotation(avgangle);
    context->camera.view.setSize(2500, 2500);

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
        sf::Color c;
        /*c.r = 20;
        c.g = 20;
        c.b = 20;*/
        c.r = 0;
        c.g = 0;
        c.b = 0;
        context->gameWindow.window.clear(c);
        context->canvas.clear(c);

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
        // Construct all collision polygons that are visible to the player
        //
        if (context->settings.SHOW_OBSTRUCTION_POLYGON)
        {
            //for (int h = 0; h < 1; h++)
            int h = 0;
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
            //for (int si = 0; si < context->allcollisionshapes.size(); si++)
            //{
            //    states.texture = NULL;
            //    context->allcollisionshapes[si].setFillColor(sf::Color::Transparent);
            //    context->allcollisionshapes[si].setOutlineThickness(2);
            //    context->allcollisionshapes[si].setOutlineColor(sf::Color::Red);
            //    context->canvas.draw(context->allcollisionshapes[si], states);
            //}
        }

        if (context->settings.SHOW_ENTITY_POLYGON)
        {
            // Draw Entities Shapes
            for (int e = 0; e < context->entities.size(); e++)
            {
                //rs.setTexture(context->player0spritesheet);
                context->canvas.draw(context->entities[e].proto.shapes[0]);
            }
        }

        for (int e = 0; e < context->entities.size(); e++)
        //int e = 0;
        {
            Entity & entity = context->entities[e];
            if (entity.proto.wardrobe.animations.size() == 0)
                continue;

            //sf::Vector2f newpos = context->entities[e].proto.body.pos;
            //sf::Vector2f vel = context->entities[e].proto.body.vel;
            //sf::Vector2f next;
            ////if (conteaxt->frameacc < sf::seconds(context->physicsConfig.FIXED_DELTA))
            ////assert(context->frametime < sf::seconds(context->physicsConfig.FIXED_DELTA));
            //newpos = physics::lerp(newpos, vel, context->physicsConfig.FIXED_DELTA);// context->frametime.asSeconds());// *context->physicsConfig.FIXED_DELTA);
            ////else
            //    next = newpos;
            //    //(context->frameacc.asSeconds() + context->frametime.asSeconds()) * context->physicsConfig.FIXED_DELTA);
            //    


            //context->entities[e].proto.shapes[0].setPosition(next);
            sf::RectangleShape rs;
            /*rs.setSize(sf::Vector2f(48,48));*/
            
            //
            //rs.move(-24, -24);
            /*rs.rotate(entity.proto.body.angle);
            rs.setOrigin(24, 24);
            if (e > 0)
                rs.setPosition(GetCentroid(entity.proto.shapes[0]));
            else
                rs.setPosition(context->zaxpos);
            rs.setTexture(&entity.proto.sstex->tex);
*/
            sf::IntRect subrect;
            Animation* animation;
            Wardrobe & wardrobe = entity.proto.wardrobe;
            if (entity.moving)
            {
                if (entity.proto.wardrobe.getAnimation("Running", animation))
                {
                    ASE::Cel & cell = animation->sequence.cels[animation->celid];
                    
                    ////
                    //wardrobe.getSubRect(cell, subrect);
                    if (vec::dot(entity.proto.body.vel, physics::rightVector(entity.proto.body.angle)) > 0.0f)
                    {
                        wardrobe.getSubRect(cell, subrect, false);
                    }
                    else
                    {
                        wardrobe.getSubRect(cell, subrect, true);
                    }
                    rs.setSize(sf::Vector2f(subrect.width, subrect.height));
                    rs.rotate(entity.proto.body.angle);
                    rs.setOrigin(subrect.width / 2.0f, subrect.height / 2.0f);
                    if (e > 0)
                        rs.setPosition(GetCentroid(entity.proto.shapes[0]));
                    else
                        rs.setPosition(context->zaxpos);
                    rs.setTexture(&entity.proto.sstex->tex);
                }
            }
            else
            {

                if (entity.proto.wardrobe.getAnimation("Idling", animation))
                {
                    ASE::Cel & cell = animation->sequence.cels[animation->celid];
                    wardrobe.getSubRect(cell, subrect, false);
                    
                    rs.setSize(sf::Vector2f(subrect.width, subrect.height));
                    rs.rotate(entity.proto.body.angle);
                    rs.setOrigin(subrect.width/2.0f, subrect.height/2.0f);
                    if (e > 0)
                        rs.setPosition(GetCentroid(entity.proto.shapes[0]));
                    else
                        rs.setPosition(context->zaxpos);
                    rs.setTexture(&entity.proto.sstex->tex);

                    //std::cout << animation->celid  << ", ["<< subrect.top << ", " << subrect.left << ", " << subrect.width << ", " << subrect.height << "] \n";
                }
            }
            sf::RenderStates rendsta;
            rs.setTextureRect(subrect);
            rendsta.texture = &entity.proto.sstex->tex;
            context->canvas.draw(rs);
        }



        if (context->settings.SHOW_ENTITY_CENTROID)
        {
            // Draw a box where we think the center of the entity is. center of gravity.
            for (int h = 0; h < context->entities[0].collisionentities.size(); h++)
            {
                sf::RectangleShape rs(sf::Vector2f(8, 8));
                rs.setFillColor(sf::Color::Red);
                sf::Vector2f pos(context->entities[context->entities[0].collisionentities[h]].proto.body.pos);
                size_t e = context->entities[0].collisionentities[h];
                sf::Vector2f newpos = GetCentroid(context->entities[e].proto.shapes[0]);
                rs.setPosition(newpos);
                rs.move(-4, -4);
                context->canvas.draw(rs);
            }
        }
        if (context->settings.SHOW_WAYPOINTS)
        {
            // Draw a box where we think the waypoints are.
            for (auto w = 0; w < context->waypoints.size(); w++)
            {
                sf::RectangleShape rs;
                rs.setSize(sf::Vector2f(6, 6));
                rs.setPosition(sf::Vector2f(context->waypoints[w].location.x, context->waypoints[w].location.y));
                rs.setFillColor(sf::Color::Yellow);

                context->canvas.draw(rs);
            }

            //for (auto entity : context->entities)


        }
        context->canvas.display();

        // Draw canvas to display
        sf::Sprite levelSprite(context->canvas.getTexture());
        
        context->gameWindow.window.setView(context->camera.view);
        context->gameWindow.window.draw(levelSprite);

        for (size_t e = 1; e < context->entities.size();e++)
        {
            Entity & entity = context->entities[e];
            if (entity.waypointpath.size() > 0)
            {
                sf::VertexArray va(sf::PrimitiveType::LineStrip);// , entity.waypointpath.size());
                
                for (size_t wpi = entity.currentwaypoint;
                    wpi < entity.waypointpath.size();
                    wpi++)
                {
                    Waypoint & wp = context->waypoints[entity.waypointpath[wpi]];
                    //if (wp.looped && wpi == entity.waypointpath.size() - 1)
                    //    continue;
                    //Waypoint & wp2 = context->waypoints[entity.waypointpath[wpi+1]];
                    va.append(wp.location);
                    //va.append(wp2.location);
                }
                context->gameWindow.window.draw(va);
            }
        }
        //context->gameWindow.window.display();
    }

    ///////////////////////////

    ImGui::Begin("Options");
        ImGui::Checkbox("Show Wall Poly", &context->settings.SHOW_OBSTRUCTION_POLYGON);
        ImGui::SameLine();
        ImGui::Checkbox("Show Entity Poly", &context->settings.SHOW_ENTITY_POLYGON);

        ImGui::Checkbox("Show Waypoints", &context->settings.SHOW_WAYPOINTS);
        ImGui::Checkbox("Show Centroids", &context->settings.SHOW_ENTITY_CENTROID);
        //ImGui::Checkbox("Auto Gravity Entities", &context->settings.AUTO_GRAVITY_ENTITIES);
        //ImGui::SameLine();
        ImGui::Checkbox("Enable Head Bump Gravity", &context->settings.ENABLE_HEAD_BUMP_GRAVITY);
        ImGui::Checkbox("Auto Gravity Player", &context->settings.AUTO_GRAVITY_PLAYERS);
        ImGui::Checkbox("Disable Mouse Gravity", &context->settings.DISABLE_MOUSE_GRAVITY);
        ImGui::Checkbox("R U Huntable?", &context->players[0].entity->huntable);
        //ImGui::SliderFloat("FIXED_DELTA", &context->physicsConfig.FIXED_DELTA, 0.001f, 0.03f);
        //ImGui::SliderFloat("GRAVITY_CONSTANT", &context->physicsConfig.GRAVITY_CONSTANT, 100.f, 10000.f);
        //ImGui::SliderFloat("MOVE_STRENGTH", &context->physicsConfig.MOVE_STRENGTH, 1.f, 500.f);
        //ImGui::SliderFloat("JUMP_STRENGTH", &context->physicsConfig.JUMP_STRENGTH, 1.f, 10000.f);
        //ImGui::SliderInt("JUMP_COUNT (Additional Jumps)", (int*)&(context->physicsConfig.JUMP_COUNT), 0.f, 10.0f);
        //ImGui::SliderFloat("FREEFALL_MOVE_STRENGTH", &context->physicsConfig.FREEFALL_MOVE_STRENGTH, 0.f, 500.0f);
        //ImGui::SliderFloat("VELOCITY_MAX", &context->physicsConfig.VELOCITY_MAX, 0.f, 2500.0f);
        //ImGui::SliderFloat("RESTITUTION", &context->physicsConfig.RESTITUTION, 0.f, 1.0f);
        //ImGui::SliderFloat("DRAG_CONSTANT", &context->physicsConfig.DRAG_CONSTANT, 0.f, 1.0f);
        //ImGui::SliderFloat("STATIC_FRICTION", &context->physicsConfig.STATIC_FRICTION, 0.f, 2500.0f);
        //ImGui::SliderFloat("DYNAMIC_FRICTION", &context->physicsConfig.DYNAMIC_FRICTION, 0.f, 1.0f);
    ImGui::End();
    ImGui::SFML::Render(context->gameWindow.window);
    //////////////////////////

    context->gameWindow.window.display();

    return 0;
}

uint32_t StageMain::cleanup()
{
    context->players.back().controller.mk.Cleanup();
    return 0;
}




}//end namespace bali
