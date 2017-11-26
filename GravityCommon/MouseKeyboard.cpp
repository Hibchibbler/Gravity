///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "MouseKeyboard.h"
#include "SFML/Window/Keyboard.hpp"
#include "Vector2.h"
#include "Physics.h"
#include "Context.h"
using namespace bali;

Keypress & MouseKeyboard::getKeypress(sf::Keyboard::Key k)
{
    return keyStates[k];
}
void MouseKeyboard::doKeyboard(sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released, KeyExpired expired)
{
    Context::Ptr ctx = (Context::Ptr)ud;

    //std::cout << elapsed.asSeconds() << std::endl;
    updateJumpKeypress(keyStates[sf::Keyboard::Space], elapsed, ud, pressed, held, released, expired);
    updateRightLeftKeypress(keyStates[sf::Keyboard::D], elapsed, ud, pressed, held, released, expired);
    updateRightLeftKeypress(keyStates[sf::Keyboard::A], elapsed, ud, pressed, held, released, expired);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {

    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        //ctx->player.velocity.y += 0.3f;
        //ctx->player.moveDown = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        float ta = ctx->player.angle - 25.0f;
        uint32_t g = 500;
        ctx->player.addSetTargetAngle(ta, g);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        float ta = ctx->player.angle + 25.0f;
        uint32_t g = 500;
        ctx->player.addSetTargetAngle(ta, g);
    }

}

void MouseKeyboard::doMouse(sf::Time elapsed)
{
    //GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    //if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    //{
    //    //this->done();
    //    ctx->screenMousePos = sf::Mouse::getPosition(ctx->window);
    //    ctx->worldMousePos = ctx->window.mapPixelToCoords(ctx->screenMousePos);
    //}
}
void MouseKeyboard::registerKeypress(sf::Keyboard::Key k, float range, float ampl)
{
    keyStates[k] = Keypress(k, range, ampl);
}

void MouseKeyboard::updateRightLeftKeypress(Keypress & kp, sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released, KeyExpired expired)
{
    kp.pre = kp.cur;
    kp.cur = sf::Keyboard::isKeyPressed(kp.key);

    if (kp.pre == true && kp.cur == false)
    {
        // Released
        released(kp, ud);
    }
    else if (kp.pre == false && kp.cur == true)
    {
        // Pressed
        pressed(kp, ud);
    }
    else
    {
        // Still held, or still released.
    }

    if (sf::Keyboard::isKeyPressed(kp.key))
    {
        kp.elp += elapsed;
        held(kp, ud);
    }
    else
    {
    }
}

void MouseKeyboard::updateJumpKeypress(Keypress & kp, sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released, KeyExpired expired)
{
#define MY_PI 3.14159265359f
    kp.pre = kp.cur;
    kp.cur = sf::Keyboard::isKeyPressed(kp.key);
    if (kp.pre == true && kp.cur == false)
    {
        // Released
        released(kp, ud);
    }
    else if (kp.pre == false && kp.cur == true)
    {
        // Pressed
        pressed(kp, ud);
    }
    else
    {
        // Still held, or still released.
    }

    if (kp.cur)
    {
        kp.elp += elapsed;
        float ms = kp.elp.asMilliseconds();
        if (ms < kp.rng)
        {
            held(kp, ud);
        }
        else
        {
            expired(kp, ud);
        }
        //std::cout << kp.str << std::endl;
    }
    else
    {
        kp.str = 0.0f;
        kp.elp = sf::Time::Zero;
    }
}


