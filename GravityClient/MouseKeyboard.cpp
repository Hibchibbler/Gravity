///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "MouseKeyboard.h"
#include "GameClientContext.h"
using namespace bali;


KeyPressWatcher::KeyPressWatcher()
{
    KeyPress keyPress;
    keyPress.active = false;
    keyPress.duration = sf::Time::Zero;
    keyPress.key = sf::Keyboard::Space;
    keyPressMap[keyPress.key] = keyPress;
}
void KeyPressWatcher::update(std::vector<KeyPress> & keyPressReport)
{
    sf::Time elapsed = keyClock.restart();
    // For all those active key presses, add to their pressed time
    for (auto k = keyPressMap.begin(); k != keyPressMap.end(); k++)
    {
        if (sf::Keyboard::isKeyPressed(k->first))
        {
            k->second.duration += elapsed;
            k->second.active = true;
        }
        else
        {
            if (k->second.active == true)
            {
                // Report out
            }
            k->second.active = false;
        }
    }

    // For all those that are inactive, yet their duration is greater than 0
    // report out.
    // then 0 it.
    //for (auto k = keyPressMap.begin(); k != keyPressMap.end(); k++)
    //{
    //    if (k->second.active == false)
    //    {
    //        if (k->second.duration > sf::Time::Zero)// TODO: Hmmm
    //        {
    //            // Report out
    //            keyPressReport.push_back(k->second);
    //            k->second.duration = sf::Time::Zero;
    //        }
    //    }
    //}
}


void MouseKeyboard::doKeyboard(Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    if (ctx->inputClock.getElapsedTime().asMilliseconds() >= 50)
    {
        ctx->inputClock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            ctx->player.moveRight = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            ctx->player.moveLeft = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            //ctx->player.velocity.y += 0.3f;
            ctx->player.moveDown = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            ctx->player.nextangle = ctx->player.nextangle - 15.0;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            ctx->player.nextangle = ctx->player.nextangle + 15.0;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {// Jump
            ctx->player.moveUp = true;

        }
    }
}

void MouseKeyboard::doMouse(bali::Context::Ptr context)
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        //this->done();
        ctx->screenMousePos = sf::Mouse::getPosition(ctx->window);
        ctx->worldMousePos = ctx->window.mapPixelToCoords(ctx->screenMousePos);
    }
}
