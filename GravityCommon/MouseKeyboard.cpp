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
void MouseKeyboard::doKeyboard(sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released)
{
    Context::Ptr ctx = (Context::Ptr)ud;
    this->totalTime += elapsed;

    if (this->totalTime.asMilliseconds() > 5.f)
    {   // std::cout << totalTime.asMilliseconds() << " ";
        this->totalTime = sf::Time::Zero;
        //std::cout << elapsed.asSeconds() << std::endl;
        updateKeypress(keyStates[sf::Keyboard::Space], elapsed, ud, pressed, held, released);
        updateKeypress(keyStates[sf::Keyboard::D], elapsed, ud, pressed, held, released);
        updateKeypress(keyStates[sf::Keyboard::A], elapsed, ud, pressed, held, released);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            float ta = ctx->player.angle - 35.0f;
            uint32_t g = 400;
            ctx->player.addSetTargetAngle(ta, g);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            float ta = ctx->player.angle + 35.0f;
            uint32_t g = 400;
            ctx->player.addSetTargetAngle(ta, g);
        }
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
void MouseKeyboard::registerKeypress(sf::Keyboard::Key k, float range, float ampl, bool repeat)
{
    keyStates[k] = Keypress(k, range, ampl, repeat);
}


void MouseKeyboard::updateKeypress(Keypress & kp, sf::Time elapsed, void* ud, KeyPressed pressed, KeyHeld held, KeyReleased released)
{
    kp.pre = kp.cur;
    kp.cur = sf::Keyboard::isKeyPressed(kp.key);
    if (kp.pre == true && kp.cur == false)
    {
        // Released
        if (!kp.exp)
        {
            // We do not want to call release, if we already expired
            //kp.nml = vec::VECTOR2(0, 0);
            kp.str = 0.0f;
            kp.elp = sf::Time::Zero;
            kp.exp = false;
            released(kp, ud);
        }
    }
    else if (kp.pre == false && kp.cur == true)
    {
        // Pressed
        kp.elp = sf::Time::Zero;
        kp.exp = false;
        pressed(kp, ud);
        held(kp, ud);
    }
    else if (kp.pre == true && kp.cur == true)
    {
        //
        // Still held
        // We only care if we haven't expired.
        //
        if (!kp.exp)
        {
            kp.elp += elapsed;
            float ms = kp.elp.asMilliseconds();
            if (ms < kp.dur || kp.dur == 0.f)
            {
                // And holding
                held(kp, ud);
            }
            else
            {
                //
                // Held it too long. Expired.
                //
                //kp.nml = vec::VECTOR2(0, 0);
                kp.str = 0.0f;
                kp.elp = sf::Time::Zero;
                kp.exp = true;
                std::cout << "expired ";
                released(kp, ud);

            }
        }
        //std::cout << kp.str << std::endl;
    }
    else
    {
        // Still released
    }
}


