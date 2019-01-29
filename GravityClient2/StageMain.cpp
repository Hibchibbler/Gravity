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
    context->map.reset();
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
        if (event.key.code == sf::Keyboard::Right)
        {
            context->camera.view.move(sf::Vector2f(10,0));
        }
        else if (event.key.code == sf::Keyboard::Left)
        {
            context->camera.view.move(sf::Vector2f(-10, 0));
        }
        else if (event.key.code == sf::Keyboard::Up)
        {
            context->camera.view.move(sf::Vector2f(0, 10));
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            context->camera.view.move(sf::Vector2f(0, -10));
        }

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
}

void onNonCollision(void* context, Entity & entity)
{
    Context* ctx = (Context*)context;
    //std::cout << "Y";
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
    context->players[0].controller.mk.Update(context->frametime);


    sf::Vector2f pos = context->players[0].entity->proto.body.pos;
    float angle = context->players[0].entity->proto.body.angle;
    sf::Vector2f size(800.f, 800.f);

    context->camera.view.setCenter(pos);
    context->camera.view.setRotation(angle);

    qt::AABB searchRegion = getSearchRegion(pos, size, 1.0f);
    std::vector<qt::XY> sr;
    sr = context->foregroundQuadTree->search(searchRegion);
    context->foregroundvertices.clear();
    context->foregroundvertices.reserve(16);
    for (auto p = sr.begin(); p != sr.end(); p++)
    {
        float x, y;
        x = p->x;
        y = p->y;
        sf::FloatRect fr(context->foregroundtiles[p->ti].x, context->foregroundtiles[p->ti].y, 32.f, 32.f);
        sf::IntRect ir(context->foregroundtiles[p->ti].tx, context->foregroundtiles[p->ti].ty, 32,32);
        addQuad(context->foregroundvertices,
            fr,
            ir,
            context->foregroundtiles[p->ti].flip);
    }

    sr = context->backgroundQuadTree->search(searchRegion);
    context->backgroundvertices.clear();
    context->backgroundvertices.reserve(16);
    for (auto p = sr.begin(); p != sr.end(); p++)
    {
        float x, y;
        x = p->x;
        y = p->y;
        sf::FloatRect fr(context->backgroundtiles[p->ti].x, context->backgroundtiles[p->ti].y, 32.f, 32.f);
        sf::IntRect ir(context->backgroundtiles[p->ti].tx, context->backgroundtiles[p->ti].ty, 32, 32);
        addQuad(context->backgroundvertices,
            fr,
            ir,
            context->backgroundtiles[p->ti].flip);
    }

    //
    // collect the collision polygons visible to each entity
    //
    for (int e = 0; e < context->entities.size(); e++) {
        searchRegion = getSearchRegion(context->entities[e].proto.body.pos, size, 1.0f);
        sr = context->collisionQuadTree->search(searchRegion);
        context->entities[e].collisionShapes.clear();
        for (auto p = sr.begin(); p != sr.end(); p++)
        {
            //context->collisionshapesvisible.push_back(GetTransformedShape(context->collisionshapes[p->ti]));
            context->entities[e].collisionShapes.push_back(context->collisionshapes[p->ti]);
        }
    }

    //
    // Until entities have a qt, compare each entity to each other entity
    //
    for (int j = 0; j < context->entities.size(); j++) {
        physics::ResolveProtoCollisions(context->entities[j], context->entities, context->protos, onCollision, onNonCollision, context, context->physicsConfig);
        physics::ResolvePolygonCollisions(context->entities[j], context->entities[j].collisionShapes, onCollision, onNonCollision, context, context->physicsConfig);

        sf::Time acc = context->frameacc;
        physics::updateRigidBody(context->entities[j].proto.body, context->frametime, context->physicsConfig, acc);
    }
    
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
        //for (int h = 0; h < context->entities.size(); h++)
        //{
        //    for (int si = 0; si < context->entities[h].collisionShapes.size(); si++)
        //    {
        //        states.texture = NULL;
        //        context->entities[h].collisionShapes[si].setFillColor(sf::Color::Transparent);
        //        context->entities[h].collisionShapes[si].setOutlineThickness(1);
        //        //context->entities[h].collisionShapes[si].setOutlineColor(GetRandomColor(si % 6));
        //        context->canvas.draw(context->entities[h].collisionShapes[si], states);
        //    }
        //}
        // Draw Entities
        for (int h = 0; h < context->entities.size(); h++)
        {
            context->entities[h].proto.shapes[0].setPosition(context->entities[h].proto.body.pos);
            context->canvas.draw(context->entities[h].proto.shapes[0]);
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
