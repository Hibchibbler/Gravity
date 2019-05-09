///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#include "KeyboardHandlers.h"
#include "ClientContext.h"
#include "GravityCommon\Physics.h"
#include <assert.h>
#include <iostream>

#undef KBH_DBG_PRINTS

#ifdef KBH_DBG_PRINTS
#define KBH_DBG_PRINT(x)\
    std::cout << (x)
#else
#define KBH_DBG_PRINT(x)
#endif
namespace bali
{

void KeyPressedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    Player & player = context->players[0];// Only local will have these handlers installed.
    RigidBody & body = player.entity->proto.body;//context->entities[context->players[0].entityindex].body;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        //context->camera.view.move(sf::Vector2f(-25.f, 0.f));
        //body.pos += sf::Vector2f(25.f, 0.f);
        if (context->entities[0].collider.jumpNormal == vec::Zero())
        {
            // Character is in the air.
            // Jump upwards
            //kp.nml = physics::upVector(context->entities[0].proto.body.angle);
            //context->entities[0].jumping = true;
            KBH_DBG_PRINT("FJA");
        }
        // UpVector DOT SurfaceNormal > 0 == Not too steep
        else if (vec::dot(physics::upVector(context->entities[0].proto.body.angle), context->entities[0].collider.surfaceNormal) > -0.01f)
        {
            // Character is on the ground
            // Jump according to the angle of the ground
            kp.nml = context->entities[0].collider.jumpNormal;
            
            context->entities[0].jumping = true;
            KBH_DBG_PRINT("FJS");

            // "Double" Jump.. with limits.
            if (context->entities[0].collider.jumpcount++ == context->physicsConfig.JUMP_COUNT)
            {
                context->entities[0].collider.jumpNormal = vec::Zero();
                context->entities[0].collider.jumpcount = 0;
                //CommandQueue::postModifyVelocity(body,physics::upVector(context->entities[0].proto.body.angle) , false);
            }

            float str = 1.0f;// *(context->entities[0].collider.jumpcount / 4.0f);
            assert(kp.nml != vec::Zero());
            CommandQueue::postJump(body, str, kp.nml);


        }
        else
        {
            // Character is touching something "steep"
            // ignore request
        }
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        //context->camera.view.move(sf::Vector2f(0.f, 25.f));
        //body.pos += sf::Vector2f(0.f, 25.f);
        player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        //context->camera.view.move(sf::Vector2f(0.f, -25.f));
        //body.pos += sf::Vector2f(0.f, -25.f);
        player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.DOWN_KEY)
    {
        //player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.UP_KEY)
    {
        //player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.HARPOON_KEY)
    {
        Entity & e = context->entities[0];
        //for (auto e = context->entities.begin(); e != context->entities.end(); e++)
        if (!context->generalConfig.DISABLE_MOUSE_GRAVITY)
        {
            e.proto.body.angle -= 45.f;
        }
    }
    else if (kp.cc == context->keyboardConfig.ATTACK_KEY)
    {
        Entity & e = context->entities[0];
        //for (auto e = context->entities.begin(); e != context->entities.end(); e++)
        if (!context->generalConfig.DISABLE_MOUSE_GRAVITY)
        {
            e.proto.body.angle += 45.f;
        }
    }
    KBH_DBG_PRINT("Press ");
}

void KeyDblPressedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    Player & player = context->players[0];// Only local will have these handlers installed.
    RigidBody & body = player.entity->proto.body;//context->entities[context->players[0].entityindex].body;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        //context->camera.view.move(sf::Vector2f(-25.f, 0.f));
        //body.pos += sf::Vector2f(25.f, 0.f);
        context->entities[0].jumping = true;
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        //context->camera.view.move(sf::Vector2f(0.f, 25.f));
        //body.pos += sf::Vector2f(0.f, 25.f);
        player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        //context->camera.view.move(sf::Vector2f(0.f, -25.f));
        //body.pos += sf::Vector2f(0.f, -25.f);
        player.entity->moving = true;
    }
    KBH_DBG_PRINT("DblPress ");
}

void KeyHeldHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    Player & player = context->players[0];// Only local will have these handlers installed.
    Entity & entity = *player.entity;
    RigidBody & body = player.entity->proto.body;//context->entities[context->players[0].entityindex].body;

    if (kp.cc == context->keyboardConfig.ATTACK_KEY)
    {
        //for (auto e = context->entities.begin(); e != context->entities.end(); e++)
        //{
        //    e->proto.body.angle += 5.0f;
        //}

        //// Get mouse pixels
        //sf::Vector2i p = sf::Mouse::getPosition(context->gameWindow.window);
        ////Map Pixel to Coords:
        //sf::Vector2f mouse_world = context->gameWindow.window.mapPixelToCoords(p);
        //sf::Vector2f fin = mouse_world - context->entities[0].proto.body.pos;
        //std::cout << "[[" << fin.x << ", " << fin.y << "]]" << std::endl;
        ////CommandQueue::postModifyVelocity(context->entities[0].proto.body, fin * 8.0f, 1);
        ////context->entities[0].proto.body.vel = fin * 4.0f;

    }
    else if (kp.cc == context->keyboardConfig.HARPOON_KEY)
    {
        //for (auto e = context->entities.begin(); e != context->entities.end(); e++)
        //{
        //    e->proto.body.angle -= 5.0f;
        //}
    }
    else if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        if (entity.jumping == true)
        {
            //float str = 1.0f;
            //assert(kp.nml != vec::Zero());
            //CommandQueue::postJump(body, str, kp.nml);
        }
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        float str = 1.f;
        bool grounded = false;
        Player & localPlayer = context->players[0];
        Entity & localEntity = *localPlayer.entity;

        if (localEntity.collider.surfaceNormal == vec::Zero())
        {
            kp.nml = physics::rightVector(localEntity.proto.body.angle);
        }
        else
        {
            kp.nml = vec::normal(localEntity.collider.surfaceNormal) * -1.0f;
            grounded = true;
        }

        CommandQueue::postMove(localEntity.proto.body, str, kp.nml, grounded);

        localEntity.collider.surfaceNormal = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        float str = 1.f;
        bool grounded = false;
        Player & localPlayer = context->players[0];
        Entity & localEntity = *localPlayer.entity;

        if (localEntity.collider.surfaceNormal == vec::Zero())
        {
            kp.nml = physics::leftVector(localEntity.proto.body.angle);
        }
        else
        {
            kp.nml = vec::normal(localEntity.collider.surfaceNormal) * 1.0f;
            grounded = true;
        }

        CommandQueue::postMove(localEntity.proto.body, str, kp.nml, grounded);

        localEntity.collider.surfaceNormal = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.DOWN_KEY)
    {
    }
    else if (kp.cc == context->keyboardConfig.UP_KEY)
    {
    }
    else if (kp.cc == context->keyboardConfig.ROTATE_RIGHT_KEY)
    {
    }
    else if (kp.cc == context->keyboardConfig.ROTATE_LEFT_KEY)
    {
    }
    KBH_DBG_PRINT(".");
}

void KeyReleasedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        context->players[0].entity->jumping = false;
        //context->entities[0].collider.jumpNormal = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        context->players[0].entity->moving = false;
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        context->players[0].entity->moving = false;
    }
    else if (kp.cc == context->keyboardConfig.DOWN_KEY)
    {
        //context->players[0].entity->moving = false;
    }
    else if (kp.cc == context->keyboardConfig.UP_KEY)
    {
        //context->players[0].entity->moving = false;
    }
    else if (kp.cc == context->keyboardConfig.ROTATE_RIGHT_KEY)
    {
    }
    else if (kp.cc == context->keyboardConfig.ROTATE_LEFT_KEY)
    {
    }
    KBH_DBG_PRINT("Released\n");
}

float logisticsFunction(float t, float L, float K, float t0)
{

    //f(t) = L / (1 + e^(-k*(t-t0)))
    //t  => current elapsed time
    //L  => Max Value
    //t0 => x-value midpoint
    //K  => steepness

    float exp_denom = -K * (t - t0);
    float denom = 1 + pow(2.71828, (exp_denom));
    return L / denom;
}


}


