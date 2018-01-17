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
    Physical & phys = ctx->player.physical;
    if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        if (ctx->player.doubleJumpCnt == ctx->physicsConfig.JUMP_COUNT)
        {
            if (ctx->player.surfaceNormal == vec::Zero())
            {
                // Character is in the air.
                // Jump upwards
                kp.nml = physics::upVector(ctx->player.physical.angle);
                ctx->player.jumpNormal = vec::Zero();
                ctx->player.isJumping = true;
                ctx->player.doubleJumpCnt--;
            }
            else if (vec::dot(physics::upVector(phys.angle), ctx->player.jumpNormal) > -0.01f)
            {
                // Character is on the ground
                // Jump according to the angle of the ground
                std::cout << "FJ ";
                kp.nml = ctx->player.jumpNormal;
                ctx->player.jumpNormal = vec::Zero();
                ctx->player.isJumping = true;
                ctx->player.doubleJumpCnt--;
            }
            else
            {
                // Character is touching something "steep"
                // ignore request
            }
        }
        else if (ctx->player.doubleJumpCnt > 0)
        {
            std::cout << "SJ ";
            kp.nml = physics::upVector(phys.angle);
            kp.elp = sf::Time::Zero;// So subsequent jumps have full duration too
            ctx->player.doubleJumpCnt--;
            ctx->player.isJumping = true;
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
}

void KeyDblPressedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    Physical & phys = ctx->player.physical;

    if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        //vec::VECTOR2 chargeDir;
        //chargeDir = -1.0f*vec::normal(ctx->player.jumpNormal);
        //kp.nml = chargeDir;
        ctx->player.isCharging = true;
    }else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        //vec::VECTOR2 chargeDir;
        //chargeDir = 1.0f*vec::normal(ctx->player.jumpNormal);
        //kp.nml = chargeDir;
        ctx->player.isCharging = true;
    }
    std::cout << "DblPress " << std::endl;
}

void KeyHeldHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;
    Physical & phys = ctx->player.physical;

    if (kp.cc == ctx->keyboardConfig.ATTACK_KEY)
    {
        //ctx->screenMousePos = sf::Mouse::getPosition(ctx->gameWindow.window);
        //ctx->worldMousePos = ctx->gameWindow.window.mapPixelToCoords(ctx->screenMousePos);
        //vec::VECTOR2 dir = ctx->worldMousePos - ctx->player.physical.pos;
        //physics::ApplyMove(ctx->player.physical, 4.0f, dir, false);
        std::cout << "A";
        physics::SubmitModifyAngle(ctx->player.physical, phys.angle - 6, true);
    }
    else if (kp.cc == ctx->keyboardConfig.HARPOON_KEY)
    {
        std::cout << "H";
        physics::SubmitModifyAngle(ctx->player.physical, phys.angle + 6, true);
    }
    else if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        if (ctx->player.isJumping == true)
        {
            float str = 1.0f;// cos((kp.elp.asMilliseconds() * ((0.5f * PI) / kp.dur))) + 0.0f;
            //str = abs(vec::mag(ctx->player.physical.vel)) / (ctx->physicsConfig.VELOCITY_MAX / 5.0f);
            //std::cout << str << std::endl;
            assert(kp.nml != vec::Zero());
            physics::SubmitJump(ctx->player.physical, str, kp.nml);
        }
    }
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
#define DON 700.0f
        float str = 1.f;
        //if (kp.elp.asMilliseconds() >= DON)
        //    str = 1.f;
        //else
        //    str = sin(((kp.elp.asMilliseconds()+16) * ((0.5f * PI) / DON))) + 0.1f;

        bool grounded = false;
        if (!ctx->player.isCharging)
        {

            if (ctx->player.latNormal == vec::Zero())
            {
                kp.nml = physics::rightVector(phys.angle);
            }
            else
            {
                kp.nml = vec::normal(ctx->player.latNormal) * -1.0f;
                grounded = true;
            }
            physics::SubmitMove(ctx->player.physical,
                str,
                kp.nml,
                grounded);
        }
        else
        {
            str = 2.f;
            if (ctx->player.latNormal == vec::Zero())
            {
                kp.nml = physics::rightVector(phys.angle);
            }
            else
            {
                kp.nml = vec::normal(ctx->player.latNormal) * -1.0f;
            }
            grounded = true;
            physics::SubmitCharge(ctx->player.physical,
                str,
                kp.nml,
                grounded);
        }
        ctx->player.latNormal = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        float str = 1.f;
        //if (kp.elp.asMilliseconds() >= DON)
        //    str = 1.f;
        //else
        //    str = sin((kp.elp.asMilliseconds() * ((0.5f * PI) / DON))) + 0.1f;

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
                str = 2.f;
                //kp.nml += physics::upVector(ctx->player.physical.angle) / 1.0f;
            }
            
            grounded = true;
        }
        if (!ctx->player.isCharging)
        {
            physics::SubmitMove(ctx->player.physical,
                str,
                kp.nml,
                grounded);
        }
        else
        {
            physics::SubmitCharge(ctx->player.physical,
                str,
                kp.nml,
                grounded);
        }

        ctx->player.latNormal = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_RIGHT_KEY)
    {
        physics::SubmitModifyAngle(ctx->player.physical, phys.angle + 5, true);
    }
    else if (kp.cc == ctx->keyboardConfig.ROTATE_LEFT_KEY)
    {
        physics::SubmitModifyAngle(ctx->player.physical, phys.angle - 5, true);
    }
}

void KeyReleasedHandler(Keypress & kp, void* ud)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)ud;

    if (kp.cc == ctx->keyboardConfig.JUMP_KEY)
    {
        ctx->player.isJumping = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.RIGHT_KEY)
    {
        ctx->player.isMovingRight = false;
        ctx->player.isCharging = false;
        kp.nml = vec::Zero();
    }
    else if (kp.cc == ctx->keyboardConfig.LEFT_KEY)
    {
        ctx->player.isMovingLeft = false;
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
