//#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "GameClient.h"
#include "StageMainClient.h"

namespace bali
{
GameClient::GameClient(Context::Ptr c)
    : Game(c)
{

}

uint32_t GameClient::initialize()
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
    ctx->gameWindow.screenWidth = 900;
    ctx->gameWindow.screenHeight = 900;
    ctx->gameWindow.window.create(sf::VideoMode(ctx->gameWindow.screenWidth, ctx->gameWindow.screenHeight), "Gravity");
    //ctx->window.setFramerateLimit(0);
    //ctx->window.setVerticalSyncEnabled(false);
    Stage::ShPtr newStage = std::make_shared<StageMainClient>();
    addStage(newStage);

    // Last thing
    Game::initialize();
    return 0;
}

uint32_t GameClient::doProcessing()
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;

    // Last thing
    Game::doProcessing();
    return 0;
}

uint32_t GameClient::cleanup()
{
    GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;

    // Last thing
    Game::cleanup();
    return 0;
}
}
