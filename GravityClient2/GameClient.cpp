//#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "GameClient.h"
#include "StageInit.h"
#include "StageMain.h"

namespace bali
{

GameClient::GameClient(std::unique_ptr<Context> c)
    : Game(std::move(c))
{

}

uint32_t GameClient::initialize()
{
    context->gameWindow.screenWidth = 1300;
    context->gameWindow.screenHeight = 1300;
    context->gameWindow.window.create(sf::VideoMode(context->gameWindow.screenWidth, context->gameWindow.screenHeight), "Gravity");
    //ctx->window.setVerticalSyncEnabled(false);

    std::unique_ptr<Stage> stage0 = std::make_unique<StageInit>(context.get());
    gameStages.push_back(std::move(stage0));

    std::unique_ptr<Stage> stage1 = std::make_unique<StageMain>(context.get());
    gameStages.push_back(std::move(stage1));

    // Last thing
    Game::initialize();
    return 0;
}

uint32_t GameClient::doProcessing()
{
    //

    // Last thing
    Game::doProcessing();
    return 0;
}

uint32_t GameClient::cleanup()
{
    //

    // Last thing
    Game::cleanup();
    return 0;
}
}
