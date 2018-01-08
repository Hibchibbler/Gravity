#include "Input.h"
#include "GameClientContext.h"
#include "GravityCommon/Physics.h"
#include <assert.h>

namespace bali
{
namespace input
{

void KeyPressedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    Physical & phys = ctx->player.physical;
    if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        if (ctx->player.doubleJumpCnt == ctx->physicsConfig.JUMP_COUNT)
        {
            if (vec::dot(physics::upVector(phys.angle), ctx->player.jumpNormal) > -0.001f)
            {
                std::cout << "FJ ";
                //// When you jump, you are flicked in the direction of the
                //// jump normal you are standing on.
                //// If, however, you are running up a ramp
                //// to jump off,  the ramp inclination is in the opposite direction of 
                //// travel...so, we ________
                //// to propel the player forward, instead of backwards.
                //
                // Jump along surface when high velocity,
                // and jump along normal when low velocity
                //
                //if (abs(vec::mag(ctx->player.physical.vel)) > ctx->physicsConfig.JUMP_VELOCITY_MAX)
                //{
                //    vec::VECTOR2 chargeDir;
                //    if (vec::dot(physics::rightVector(ctx->player.angle), ctx->player.physical.vel) > 0)
                //    {
                //        chargeDir = -1.0f*vec::normal(ctx->player.jumpNormal);
                //        //kp.nml = vec::norm((kp.nml + 0.75f*physics::upVector(ctx->player.angle)) / 2.0f);
                //    }
                //    else
                //    {
                //        chargeDir = 1.0f*vec::normal(ctx->player.jumpNormal);
                //        //kp.nml = vec::norm((kp.nml + 0.75f*physics::upVector(ctx->player.angle)) / 2.0f);
                //    }
                //    kp.nml = chargeDir;
                //    ctx->player.isCharging = true;
                //    ctx->player.doubleJumpCnt = 0; //
                //}
                //else
                {
                    kp.nml = ctx->player.jumpNormal;
                    ctx->player.isJumping = true;
                    ctx->player.doubleJumpCnt--;
                }
            }
        }
        else if (ctx->player.doubleJumpCnt > 0)
        {
            if (ctx->player.isJumping == true)
            {
                std::cout << "SJ ";
                kp.nml = physics::upVector(phys.angle);
                kp.elp = sf::Time::Zero;// So subsequent jumps have full duration too
                ctx->player.doubleJumpCnt--;
            }
        }
        else
        {
            std::cout << "LJ ";
            ctx->player.jumpNormal = vec::Zero();
        }
    }
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        ctx->player.isMovingRight = true;
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        ctx->player.isMovingLeft = true;
    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_RIGHT_KEY)
    {

    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_LEFT_KEY)
    {

    }
}

void KeyDblPressedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    Physical & phys = ctx->player.physical;

    if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        vec::VECTOR2 chargeDir;
        chargeDir = -1.0f*vec::normal(ctx->player.jumpNormal);
        kp.nml = chargeDir;
        ctx->player.isCharging = true;
    }else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        vec::VECTOR2 chargeDir;
        chargeDir = 1.0f*vec::normal(ctx->player.jumpNormal);
        kp.nml = chargeDir;
        ctx->player.isCharging = true;
    }
    std::cout << "DblPress " << std::endl;
}

void KeyHeldHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    Physical & phys = ctx->player.physical;

    if (kp.cc == ctx->keyboardConfig.ATTACK_BUTTON)
    {
        ctx->screenMousePos = sf::Mouse::getPosition(ctx->gameWindow.window);
        ctx->worldMousePos = ctx->gameWindow.window.mapPixelToCoords(ctx->screenMousePos);
        vec::VECTOR2 dir = ctx->worldMousePos - ctx->player.physical.pos;
        ctx->player.physical.addMove(4.0f, dir, false);
            
        /*{
            ctx->player.isMovingRight = true;
            std::cout << "L ";
        }*/
        std::cout << "R ";
    }
    else if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {

        if (ctx->player.isJumping == true)
        {
            float str = 1.0f;// cos((kp.elp.asMilliseconds() * ((0.5f * PI) / kp.dur))) + 0.0f;
            //str = abs(vec::mag(ctx->player.physical.vel)) / (ctx->physicsConfig.VELOCITY_MAX / 5.0f);
            //std::cout << str << std::endl;
            assert(kp.nml != vec::Zero());
            ctx->player.physical.addJump(str, kp.dur, kp.nml);
        }
    }
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
#define DON 700.0f
        float str = 1.f;
        //if (kp.elp.asMilliseconds() >= DON)
        //    str = 1.f;
        //else
        //    str = sin(((kp.elp.asMilliseconds()+16) * ((0.5f * PI) / DON))) + 0.0f;

        bool grounded = false;
        if (ctx->player.latNormal == vec::Zero())
        {
            kp.nml = physics::rightVector(phys.angle);
        }
        else
        {
            kp.nml = vec::normal(ctx->player.latNormal) * -1.0f;
            if (ctx->player.isCharging == true)
            {
                str = 4.f;
                kp.nml += physics::upVector(ctx->player.physical.angle) / 1.0f;
            }
            
            grounded = true;
        }
        ctx->player.physical.addMove(str,
            kp.nml,
            grounded);

        ctx->player.latNormal = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        float str = 1.f;
        //if (kp.elp.asMilliseconds() >= DON)
        //    str = 1.f;
        //else
        //    str = sin((kp.elp.asMilliseconds() * ((0.5f * PI) / DON))) + 0.0f;

        bool grounded = false;
        if (ctx->player.latNormal == vec::Zero())
        {
            kp.nml = physics::leftVector(phys.angle);
        }
        else
        {
            kp.nml = vec::normal(ctx->player.latNormal) * 1.0f;
            if (ctx->player.isCharging == true)
            {
                str = 4.f;
                kp.nml += physics::upVector(ctx->player.physical.angle) / 1.0f;
            }
            
            grounded = true;
        }
        ctx->player.physical.addMove(str,
            kp.nml,
            grounded);

        ctx->player.latNormal = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_RIGHT_KEY)
    {
        ctx->player.physical.addSetTargetAngle(phys.angle + 5, 1);
    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_LEFT_KEY)
    {
        ctx->player.physical.addSetTargetAngle(phys.angle - 5, 1);
    }
}

void KeyReleasedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;

    if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        if (ctx->player.isJumping)
            ctx->player.isJumping = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        if (ctx->player.isMovingRight)
            ctx->player.isMovingRight = false;
        if (ctx->player.isCharging)
            ctx->player.isCharging = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        if (ctx->player.isMovingLeft)
            ctx->player.isMovingLeft = false;
        if (ctx->player.isCharging)
            ctx->player.isCharging = false;
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
