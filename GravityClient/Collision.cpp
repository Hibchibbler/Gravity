#include "Collision.h"
#include "GravityCommon/Physics.h"

namespace bali
{


void onNonCollisionHandler(Player & p, PhysicsConfig & pc)
{
    p.surfaceNormal = vec::Zero();
}

void onCollisionHandler(Player & p, vec::VECTOR2 cn, PhysicsConfig & pc)
{
    Physical & phys = p.physical;
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
    if (vec::mag(phys.vel) < pc.JUMP_VELOCITY_MAX)
    {
        if (vec::dot(physics::upVector(phys.angle), cn) > -0.01f)
        {
            p.jumpNormal = cn;
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
    if (vec::dot(physics::upVector(phys.angle), cn) > 0.1f)// -0.25f)
    {
        p.latNormal = cn;
    }


    //
    // Get contact surface, regardless
    // of other things
    //
    p.surfaceNormal = cn;


    //
    // If the surface is not too steep
    // then reset double jump count
    //
    // N.B.     >0.01 because, we don't
    //          want double jump to reset
    //          when colliding with vertical
    //          wall.
    //
    //if (vec::dot(physics::upVector(phys.angle), cn) > 0.01f &&
    //    vec::dot(physics::downVector(phys.angle), p.physical.vel) > 0.01f)
    if (vec::dot(physics::upVector(phys.angle), cn) > 0.01f &&
        vec::dot(physics::downVector(phys.angle), p.physical.vel) > 0.01f)
    {
        if (p.doubleJumpCnt != pc.JUMP_COUNT)
        {
            std::cout << "CJ ";
        }
        p.doubleJumpCnt = pc.JUMP_COUNT;
    }

}





}

