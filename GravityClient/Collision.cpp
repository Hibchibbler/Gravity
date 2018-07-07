///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#include "Collision.h"
#include "GravityCommon/Physics.h"

namespace bali
{


void onNonCollisionHandler(Context::Ptr context)
{
    Player & player = context->entitymanager.player;
    player.surfaceNormal = vec::Zero();
}

void onCollisionHandler(Context::Ptr context, vec::VECTOR2 cn)
{
    RigidBody & phys = context->entitymanager.player.physical;
    Player & player = context->entitymanager.player;
    //
    // If not moving too fast, and
    // surface is not too steep
    // then Jumpable
    //
    // N.B.     >-0.01 because, we do
    //          want jump to reset
    //          when colliding with vertical
    //          wall.
    //
    //if (vec::mag(phys.vel) < context->physicsConfig.JUMP_VELOCITY_MAX)
    {
        if (vec::dot(physics::upVector(phys.angle), cn) > -0.01f)
        {
            player.jumpNormal = cn;
        }
    }

    //
    // If the surface is not too steep
    // then Walkable
    //
    // N.B.     >-0.01 because, we do
    //          want to walk when colliding 
    //          with vertical walls.
    //
    if (vec::dot(physics::upVector(phys.angle), cn) > 0.01)// -0.25f)
    {
        player.latNormal = cn;
    }

    //
    // Get contact surface, regardless
    // of other things
    //
    player.surfaceNormal = cn;

    //
    // If the surface is not too steep
    // then reset double jump count.
    // And, ignore collision if we jumped recently.
    // N.B.     >0.01 because, we don't
    //          want double jump to reset
    //          when colliding with vertical
    //          wall.
    //
    if (vec::dot(physics::upVector(phys.angle), cn) > 0.01f &&
        context->frames_since_jump > 5)
    {
        if (player.doubleJumpCnt != context->physicsConfig.JUMP_COUNT)
        {
            std::cout << "CJ ";
        }
        player.doubleJumpCnt = context->physicsConfig.JUMP_COUNT;
    }
    else
    {
        if (context->frames_since_jump <= 5)
        {
            std::cout << "*";
        }
    }
}





}

