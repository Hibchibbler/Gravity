///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "StageMain.h"
#include "ClientContext.h"
#include <math.h>
#include  <assert.h>

namespace bali
{
StageMain::StageMain(Context* context)
    : Stage(context)
{
}

StageMain::~StageMain()
{
}

uint32_t StageMain::initialize()
{
    //

    //
    // Last things
    //

    return 0;
}

uint32_t StageMain::doWindowEvent(sf::Event & event)
{
    switch (event.type) {
    case sf::Event::LostFocus:
        break;
    case sf::Event::GainedFocus:
        break;
    case sf::Event::MouseWheelScrolled: {
        break;
    }case sf::Event::Resized: {
        break;
    }case sf::Event::MouseMoved: {
        break;
    }case sf::Event::KeyPressed:
        this->done();
        break;
    case sf::Event::KeyReleased:
        break;
    case sf::Event::Closed:
        break;
    }
    return 0;
}

uint32_t StageMain::doUpdate()
{
    return 0;
}

uint32_t StageMain::doDraw()
{
    //
    // And Finally
    //


    context->gameWindow.window.clear(sf::Color::Black);

    {
        sf::RenderStates states;

        //
        // Draw Level Texture
        //
        context->gameWindow.window.clear(sf::Color::Red);
        context->canvas.clear(sf::Color::Blue);

        // Draw backgound tiles
        states.texture = &context->backgroundtilesettexture.tex;
        context->canvas.draw(&context->backgroundvertices[0], context->backgroundvertices.size(), sf::Quads, states);

        // Draw foreground tiles
        states.texture = &context->foregroundtilesettexture.tex;
        context->canvas.draw(&context->foregroundvertices[0], context->foregroundvertices.size(), sf::Quads, states);
        context->canvas.display();

        // Draw canvas to display
        sf::Sprite levelSprite(context->canvas.getTexture());
        context->gameWindow.window.draw(levelSprite);
        //context->gameWindow.window.display();
    }



    context->gameWindow.window.display();

    return 0;
}

uint32_t StageMain::cleanup()
{
    return 0;
}




}//end namespace bali
