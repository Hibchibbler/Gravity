///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "MouseKeyboard.h"

using namespace bali;

void MouseKeyboard::doKeyboard(GameContext & ctx)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        ctx.player.applyJump();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        ctx.player.velocity.x += 0.3f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        ctx.player.velocity.x -= 0.3f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        ctx.player.velocity.y -= 0.3f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        ctx.player.velocity.y += 0.3f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        ctx.player.angle = ctx.player.angle - 0.20;
        if (ctx.player.angle < 0)
            ctx.player.angle = 360;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        ctx.player.angle = ctx.player.angle + 0.20;
        if (ctx.player.angle >= 360)
            ctx.player.angle = 0;
    }
}

void MouseKeyboard::doMouse(GameContext & ctx)
{
    //if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    //{
    //    //this->done();
    //    sf::Vector2i mpos = sf::Mouse::getPosition(ctx.window);
    //    sf::Vector2f worldPos = ctx.window.mapPixelToCoords(mpos);
    //}
}
