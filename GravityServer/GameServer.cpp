//#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include "GameServer.h"
#include "StageOne.h"


namespace bali
{

    GameServer::GameServer(std::unique_ptr<Context> c)
        : Game(std::move(c))
    {

    }

    uint32_t GameServer::initialize()
    {
        context->gameWindow.screenWidth = 700;
        context->gameWindow.screenHeight = 700;
        context->gameWindow.window.create(sf::VideoMode(context->gameWindow.screenWidth, context->gameWindow.screenHeight), "Gravity");
        //ctx->window.setVerticalSyncEnabled(false);

        std::unique_ptr<Stage> stage1 = std::make_unique<StageOne>(context.get());
        gameStages.push_back(std::move(stage1));


        // Last thing
        Game::initialize();
        return 0;
    }

    uint32_t GameServer::doProcessing()
    {
        //

        // Last thing
        Game::doProcessing();
        return 0;
    }

    uint32_t GameServer::cleanup()
    {
        //

        // Last thing
        Game::cleanup();
        return 0;
    }
}
