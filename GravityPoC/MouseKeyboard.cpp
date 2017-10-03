///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "MouseKeyboard.h"

using namespace bali;

void MouseKeyboard::doKeyboard(GameContext & ctx)
{
    if (ctx.inputClock.getElapsedTime().asMilliseconds() >= 50)
    {
        ctx.inputClock.restart();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            ctx.player.moveRight = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            ctx.player.moveLeft = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            ctx.player.moveUp = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            //ctx.player.velocity.y += 0.3f;
            ctx.player.moveDown = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            ctx.player.angle = ctx.player.angle - 2.0;
            if (ctx.player.angle < 0)
                ctx.player.angle = 360;
            ctx.player.GravityOn();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            ctx.player.angle = ctx.player.angle + 2.0;
            if (ctx.player.angle >= 360)
                ctx.player.angle = 0;
            ctx.player.GravityOn();
        }
    }
}

void MouseKeyboard::doMouse(GameContext & ctx)
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        //this->done();
        ctx.screenMousePos = sf::Mouse::getPosition(ctx.window);
        ctx.worldMousePos = ctx.window.mapPixelToCoords(ctx.screenMousePos);
    }
}
