///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#include "KeyboardHandlers.h"
#include "ClientContext.h"
#include "GravityCommon\Physics.h"
#include <assert.h>
#include <iostream>

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
            std::cout << "FJA ";
        }
        // UpVector DOT SurfaceNormal > 0 == Not too steep
        else if (vec::dot(physics::upVector(context->entities[0].proto.body.angle), context->entities[0].collider.surfaceNormal) > -0.01f)
        {
            // Character is on the ground
            // Jump according to the angle of the ground
            kp.nml = context->entities[0].collider.jumpNormal;
            context->entities[0].collider.jumpNormal = vec::Zero();
            context->entities[0].jumping = true;
            std::cout << "FJS ";
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
        
    }
    std::cout << "Press ";
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
    std::cout << "DblPress ";
}

void KeyHeldHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    Player & player = context->players[0];// Only local will have these handlers installed.
    Entity & entity = *player.entity;
    RigidBody & body = player.entity->proto.body;//context->entities[context->players[0].entityindex].body;

    if (kp.cc == context->keyboardConfig.ATTACK_KEY)
    {
        for (auto e = context->entities.begin(); e != context->entities.end(); e++)
        {
            e->proto.body.angle += 5.0f;
        }

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
        for (auto e = context->entities.begin(); e != context->entities.end(); e++)
        {
            e->proto.body.angle -= 5.0f;
        }
    }
    else if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        if (entity.jumping == true)
        {
            float str = 1.0f;
            assert(kp.nml != vec::Zero());
            CommandQueue::postJump(body, str, kp.nml);
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
    std::cout << ".";
}

void KeyReleasedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        context->players[0].entity->jumping = false;
        context->entities[0].collider.jumpNormal = vec::Zero();
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
    std::cout << "Released" << std::endl;
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



/*
#include "KeyboardHandlers.h"
#include "ClientContext.h"
#include "GravityCommon/Physics.h"
#include <assert.h>

namespace bali
{

void KeyPressedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    Player & player = context->entitymanager.player;
    RigidBody & phys = player.physical;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        if (player.doubleJumpCnt == context->physicsConfig.JUMP_COUNT)
        {
            if (player.surfaceNormal == vec::Zero())
            {
                // Character is in the air.
                // Jump upwards
                kp.nml = physics::upVector(player.physical.angle);
                player.jumpNormal = vec::Zero();
                player.isJumping = true;
                player.doubleJumpCnt--;
                context->frames_since_jump = 0;//TODO: rename to frames_since_jump
                std::cout << "FJA ";
            }
            else if (vec::dot(physics::upVector(phys.angle), player.jumpNormal) > -0.01f)
            {
                // Character is on the ground
                // Jump according to the angle of the ground
                kp.nml = player.jumpNormal;
                player.jumpNormal = vec::Zero();
                player.isJumping = true;
                player.doubleJumpCnt--;
                context->frames_since_jump = 0;//TODO: rename to frames_since_jump
                std::cout << "FJS ";
            }
            else
            {
                // Character is touching something "steep"
                // ignore request
            }
        }
        else if (player.doubleJumpCnt > 0)
        {
            std::cout << "SJ ";
            if (player.jumpNormal == vec::Zero())
            {
                kp.nml = physics::upVector(phys.angle);
            }
            else
            {
                kp.nml = player.jumpNormal;
                player.jumpNormal = vec::Zero();
            }
            kp.elp = sf::Time::Zero;// So subsequent jumps have full duration too
            player.doubleJumpCnt--;
            player.isJumping = true;
        }
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        player.isMovingRight = true;
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        player.isMovingLeft = true;
    }
}

void KeyDblPressedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    RigidBody & phys = context->entitymanager.player.physical;
    Player & player = context->entitymanager.player;

    if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        player.isCharging = true;
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        player.isCharging = true;
    }
    std::cout << "DblPress " << std::endl;
}

void KeyHeldHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    RigidBody & phys = context->entitymanager.player.physical;
    Player & player = context->entitymanager.player;

    if (kp.cc == context->keyboardConfig.ATTACK_KEY)
    {
        std::cout << "A";
        physics::SubmitModifyAngle(player.physical, phys.angle - 6, true);
    }
    else if (kp.cc == context->keyboardConfig.HARPOON_KEY)
    {
        std::cout << "H";
        physics::SubmitModifyAngle(player.physical, phys.angle + 6, true);
    }
    else if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        if (player.isJumping == true)
        {
            float str = 1.0f;
            assert(kp.nml != vec::Zero());
            physics::SubmitJump(player.physical, str, kp.nml);
        }
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        float str = 1.f;
        bool grounded = false;
        if (!player.isCharging)
        {

            if (player.latNormal == vec::Zero())
            {
                kp.nml = physics::rightVector(phys.angle);
            }
            else
            {
                kp.nml = vec::normal(player.latNormal) * -1.0f;
                grounded = true;
            }
            physics::SubmitMove(player.physical,
                str,
                kp.nml,
                grounded);
        }
        else
        {
            str = 2.f;
            if (player.latNormal == vec::Zero())
            {
                kp.nml = physics::rightVector(phys.angle);
            }
            else
            {
                kp.nml = vec::normal(player.latNormal) * -1.0f;
            }
            grounded = true;
            physics::SubmitCharge(player.physical,
                str,
                kp.nml,
                grounded);
        }
        player.latNormal = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        float str = 1.f;
        bool grounded = false;
        if (!player.isCharging)
        {

            if (player.latNormal == vec::Zero())
            {
                kp.nml = physics::leftVector(phys.angle);
            }
            else
            {
                kp.nml = vec::normal(player.latNormal) * 1.0f;
                grounded = true;
            }
            physics::SubmitMove(player.physical,
                str,
                kp.nml,
                grounded);
        }
        else
        {
            str = 2.f;
            if (player.latNormal == vec::Zero())
            {
                kp.nml = physics::leftVector(phys.angle);
            }
            else
            {
                kp.nml = vec::normal(player.latNormal) * 1.0f;
            }
            grounded = true;
            physics::SubmitCharge(player.physical,
                str,
                kp.nml,
                grounded);
        }
        player.latNormal = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.ROTATE_RIGHT_KEY)
    {
        physics::SubmitModifyAngle(player.physical, phys.angle + 5, true);
    }
    else if (kp.cc == context->keyboardConfig.ROTATE_LEFT_KEY)
    {
        physics::SubmitModifyAngle(player.physical, phys.angle - 5, true);
    }
}

void KeyReleasedHandler(Keypress & kp, void* ud)
{
    ClientContext::Ptr context = (ClientContext::Ptr)ud;
    RigidBody & phys = context->entitymanager.player.physical;
    Player & player = context->entitymanager.player;

    if (kp.cc == context->keyboardConfig.JUMP_KEY)
    {
        player.isJumping = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.RIGHT_KEY)
    {
        player.isMovingRight = false;
        player.isCharging = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.LEFT_KEY)
    {
        player.isMovingLeft = false;
        player.isCharging = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == context->keyboardConfig.ROTATE_RIGHT_KEY)
    {

    }
    else if (kp.cc == context->keyboardConfig.ROTATE_LEFT_KEY)
    {

    }
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
*/




