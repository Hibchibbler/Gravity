///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#include "Input.h"
#include "GameClientContext.h"
#include "GravityCommon/Physics.h"
#include <assert.h>

namespace bali
{
namespace input
{
/*
\mathrm{Ref}_a(v) = v - 2\frac{v\cdot a}{a\cdot a}a,
Refa(v) = v - 2*(dot(v,a)*a

*/
void KeyPressedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    Player & player = ctx->entitymanager.player;
    RigidBody & phys = player.physical;

    if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        if (player.doubleJumpCnt == ctx->physicsConfig.JUMP_COUNT)
        {
            if (player.surfaceNormal == vec::Zero())
            {
                // Character is in the air.
                // Jump upwards
                kp.nml = physics::upVector(player.physical.angle);
                player.jumpNormal = vec::Zero();
                player.isJumping = true;
                player.doubleJumpCnt--;
                ctx->frames_since_jump = 0;//TODO: rename to frames_since_jump
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
                ctx->frames_since_jump = 0;//TODO: rename to frames_since_jump
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
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        player.isMovingRight = true;
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        player.isMovingLeft = true;
    }
}

void KeyDblPressedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    RigidBody & phys = ctx->entitymanager.player.physical;
    Player & player = ctx->entitymanager.player;

    if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        player.isCharging = true;
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        player.isCharging = true;
    }
    std::cout << "DblPress " << std::endl;
}

void KeyHeldHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    RigidBody & phys = ctx->entitymanager.player.physical;
    Player & player = ctx->entitymanager.player;

    if (kp.cc == ctx->keyboardConfig.ATTACK_KEY)
    {
        std::cout << "A";
        physics::SubmitModifyAngle(player.physical, phys.angle - 6, true);
    }
    else if (kp.cc == ctx->keyboardConfig.HARPOON_KEY)
    {
        std::cout << "H";
        physics::SubmitModifyAngle(player.physical, phys.angle + 6, true);
    }
    else if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        if (player.isJumping == true)
        {
            float str = 1.0f;
            assert(kp.nml != vec::Zero());
            physics::SubmitJump(player.physical, str, kp.nml);
        }
    }
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
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
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
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
    else if (kp.cc == ctx->keyboardConfig.ROTATE_RIGHT_KEY)
    {
        physics::SubmitModifyAngle(player.physical, phys.angle + 5, true);
    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_LEFT_KEY)
    {
        physics::SubmitModifyAngle(player.physical, phys.angle - 5, true);
    }
}

void KeyReleasedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    RigidBody & phys = ctx->entitymanager.player.physical;
    Player & player = ctx->entitymanager.player;

    if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        player.isJumping = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        player.isMovingRight = false;
        player.isCharging = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        player.isMovingLeft = false;
        player.isCharging = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_RIGHT_KEY)
    {

    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_LEFT_KEY)
    {

    }
}

float logisticsFunction(float t, float L, float K, float t0)
{
    /*
    f(t) = L / (1 + e^(-k*(t-t0)))
    t  => current elapsed time
    L  => Max Value
    t0 => x-value midpoint
    K  => steepness
    */
    float exp_denom = -K * (t - t0);
    float denom = 1 + pow(2.71828, (exp_denom));
    return L / denom;
}











}
}
