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
    RigidBody & body = player.entity->proto.body;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        if (context->entities[0].collider.jumpNormal == vec::Zero())
        {
            // Character is in the air.
            // Jump upwards
            KBH_DBG_PRINT("FJA");
        }
        // UpVector DOT SurfaceNormal > 0 == Not too steep
        else if (vec::dot(physics::upVector(context->entities[0].proto.body.angle), context->entities[0].collider.surfaceNormal) > -0.1f)
        {
            // Character is on the ground
            // Jump according to the angle of the ground

            kp.nml = physics::upVector(context->entities[0].proto.body.angle);//context->entities[0].collider.jumpNormal;

            context->entities[0].jumping = true;
            KBH_DBG_PRINT("FJS");

            // "Double" Jump.. with limits.
            if (context->entities[0].collider.jumpcount++ == context->physicsConfig.JUMP_COUNT)
            {
                context->entities[0].collider.jumpNormal = vec::Zero();
                context->entities[0].collider.jumpcount = 0;
            }

            float str = 1.0f;
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
        player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.DOWN_KEY)
    {
    }
    else if (kp.cc == context->keyboardConfig.UP_KEY)
    {
        player.entity->isWpressed = true;
        //player.entity->collider.autogravitated = true;
        //////////////////////////////////////////////
        //{
        //    {
        //        /sf::Vector2f d = physics::upVector(player.entity->proto.body.angle);
        //        //if (vec::dot(d, normal) < -0.4f && vec::dot(d, normal) > -0.7f)
        //        {
        //            sf::Vector2f dirUp = physics::upVector(0);
        //            sf::Vector2f dirRight = physics::upVector(90);
        //            sf::Vector2f dirLeft = physics::upVector(180);
        //            sf::Vector2f dirDown = physics::upVector(270);
        //            
        //            float angleUp;
        //            float angleRight;
        //            float angleLeft;
        //            float angleDown;
        //            float newAngle;

        //            {
        //                angleUp = atan2(dirUp.y, dirUp.x) - atan2(d.y, d.x);

        //                angleUp *= (180.f / PI);
        //                if (angleUp < 0) { angleUp += 180.0f; }
        //                else { angleUp -= 180.0f; }
        //            }
        //            {
        //                angleRight = atan2(dirRight.y, dirRight.x) - atan2(d.y, d.x);

        //                angleRight *= (180.f / PI);
        //                if (angleRight < 0) { angleRight += 180.0f; }
        //                else { angleRight -= 180.0f; }
        //            }
        //            {
        //                angleLeft = atan2(dirLeft.y, dirLeft.x) - atan2(d.y, d.x);

        //                angleLeft *= (180.f / PI);
        //                if (angleLeft < 0) { angleLeft += 180.0f; }
        //                else { angleLeft -= 180.0f; }
        //            }
        //            {
        //                angleDown = atan2(dirDown.y, dirDown.x) - atan2(d.y, d.x);

        //                angleDown *= (180.f / PI);
        //                if (angleDown < 0) { angleDown += 180.0f; }
        //                else { angleDown -= 180.0f; }
        //            }
        //                //float oldangle = entity.proto.body.angle;
        //                //std::cout << oldangle << " --> " << newangle<< "  " << std::endl;

        //            if (angleUp < angleRight &&
        //                angleUp < angleLeft &&
        //                angleUp < angleDown)
        //            { 
        //               newAngle = angleUp;
        //            }else if (angleDown < angleRight &&
        //                      angleDown < angleLeft &&
        //                      angleDown < angleUp)
        //            {
        //                newAngle = angleDown;
        //            }
        //            else if (angleRight < angleDown &&
        //                     angleRight < angleLeft &&
        //                     angleRight < angleUp)
        //            {
        //                newAngle = angleRight;
        //            }
        //            else if (angleLeft < angleRight &&
        //                     angleLeft < angleDown &&
        //                     angleLeft < angleUp)
        //            {
        //                newAngle = angleLeft;
        //            }

        //            CommandQueue::postModifyAngle(player.entity->proto.body, newAngle, false);
        //            //entity.proto.body.angle += newangle;
        //        }
        //    }
        //}

        //////////////////////////////////////////////
    }
    else if (kp.cc == context->keyboardConfig.RESET_KEY)
    {
        player.entity->proto.body.pos = sf::Vector2f(819, 480);
        player.entity->proto.body.angle = 0;
    }
    else if (kp.cc == context->keyboardConfig.HARPOON_KEY)
    {
        Entity & e = context->entities[0];
        if (!context->generalConfig.DISABLE_MOUSE_GRAVITY)
        {
            //// If the player pressed the W button,
            //// rotate 90 degrees, otherwise
            //// rotate 45 degrees.
            //if (player.entity->fastRotatePressed == true)
            //{
            //    //e.proto.body.angle -= 90.f;

            //    e.proto.body.angle -= 90;// - ((uint32_t)e.proto.body.angle % 90);
            //    
            //}
            //else
            //{
            //    e.proto.body.angle -= 45.f;
            //}
            e.proto.body.angle -= 45.f;
        }
    }
    else if (kp.cc == context->keyboardConfig.ATTACK_KEY)
    {
        Entity & e = context->entities[0];
        if (!context->generalConfig.DISABLE_MOUSE_GRAVITY)
        {
            //if (player.entity->fastRotatePressed == true)
            //{
            //    //e.proto.body.angle += 90.f;
            //    e.proto.body.angle += 90;// - ((uint32_t)e.proto.body.angle % 90);
            //}
            //else
            //{
            //    e.proto.body.angle += 45.f;
            //}
            e.proto.body.angle += 45.f;
        }
    }
    KBH_DBG_PRINT("Press ");
}

void KeyDblPressedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    Player & player = context->players[0];// Only local will have these handlers installed.
    RigidBody & body = player.entity->proto.body;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        context->entities[0].jumping = true;
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        player.entity->moving = true;
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        player.entity->moving = true;
    }
    KBH_DBG_PRINT("DblPress ");
}

void KeyHeldHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    Player & player = context->players[0];// Only local will have these handlers installed.
    Entity & entity = *player.entity;
    RigidBody & body = player.entity->proto.body;

    if (kp.cc == context->keyboardConfig.ATTACK_KEY)
    {
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

    }
    else if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        if (entity.jumping == true)
        {

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
    }
    else if (kp.cc == context->keyboardConfig.UP_KEY)
    {
        context->players[0].entity->isWpressed =  false;
        //context->players[0].entity->collider.autogravitated = false;
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


