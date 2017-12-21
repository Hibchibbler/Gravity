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
void MouseKeyboard::doKeyboard(sf::Time elapsed, void* ud, KeyPressedEvent pressed, KeyHeldEvent held, KeyReleasedEvent released)
{
    Context::Ptr ctx = (Context::Ptr)ud;
    this->totalTime += elapsed;

    updateElapsed(keyStates[sf::Keyboard::Space], elapsed);
    updateElapsed(keyStates[sf::Keyboard::D], elapsed);
    updateElapsed(keyStates[sf::Keyboard::A], elapsed);

    if (this->totalTime.asMilliseconds() > 45.f)
    {   // std::cout << totalTime.asMilliseconds() << " ";
        this->totalTime = sf::Time::Zero;
        //std::cout << elapsed.asSeconds() << std::endl;
        updateKeypress(keyStates[sf::Keyboard::Space], elapsed, ud, pressed, held, released);
        updateKeypress(keyStates[sf::Keyboard::D], elapsed, ud, pressed, held, released);
        updateKeypress(keyStates[sf::Keyboard::A], elapsed, ud, pressed, held, released);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            float ta = ctx->player.angle - 25.0f;
            uint32_t g = 100;
            ctx->player.addSetTargetAngle(ta, g);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            float ta = ctx->player.angle + 25.0f;
            uint32_t g = 100;
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
void MouseKeyboard::registerKeypress(sf::Keyboard::Key k, float duration)
{
    keyStates[k] = Keypress(k, duration);
}

void MouseKeyboard::updateElapsed(Keypress & kp, sf::Time elapsed)
{
    kp.elp += elapsed;
}

void MouseKeyboard::updateKeypress(Keypress & kp, sf::Time elapsed, void* ud, KeyPressedEvent pressed, KeyHeldEvent held, KeyReleasedEvent released)
{
    kp.pre = kp.cur;
    kp.cur = sf::Keyboard::isKeyPressed(kp.key);
    if (kp.pre == true && kp.cur == false)
    {
        // Released
        if (!kp.exp)
        {
            // We do not want to call release, if we already expired
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
            //kp.elp += elapsed;
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
                kp.elp = sf::Time::Zero;
                kp.exp = true;
                std::cout << "expired ";
                released(kp, ud);

            }
        }
    }
    else
    {
        // Still released
    }
}


