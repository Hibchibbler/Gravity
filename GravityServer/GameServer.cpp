#include "GameServer.h"
#include "Stage00Server.h"
#include <memory>

namespace bali
{
GameServer::GameServer(Context::Ptr c)
    : Game(c)
{

}

uint32_t GameServer::initialize()
{
    GameServerContext::Ptr ctx = (GameServerContext::Ptr)context;
    //ctx->owner = this;
    //ctx->screenWidth = 900;
    //ctx->screenHeight = 900;
    //ctx->window.create(sf::VideoMode(ctx->screenWidth, ctx->screenHeight), "Gravity");

    Stage::ShPtr newStage = std::make_shared<Stage00Server>();
    addStage(newStage);

    // Last thing
    Game::initialize();
    return 0;
}

uint32_t GameServer::doProcessing()
{
    GameServerContext::Ptr ctx = (GameServerContext::Ptr)context;

    // Last thing
    Game::doProcessing();
    return 0;
}

uint32_t GameServer::cleanup()
{
    GameServerContext::Ptr ctx = (GameServerContext::Ptr)context;

    // Last thing
    Game::cleanup();
    return 0;
}

}
