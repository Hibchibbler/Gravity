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
        //stowContext(&gcc);
    }

    uint32_t GameClient::initialize()
    {
        GameClientContext::Ptr ctx = (GameClientContext::Ptr)context;
        ctx->owner = this;
        ctx->screenWidth = 900;
        ctx->screenHeight = 900;
        ctx->window.create(sf::VideoMode(ctx->screenWidth, ctx->screenHeight), "Gravity");

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
