#include <SFML/Graphics.hpp>
#include <memory>

#include "GameServer.h"
#include "ServerContext.h"
#include <memory>

int main()
{
    std::unique_ptr<bali::ServerContext> gcc = std::make_unique<bali::ServerContext>();
    std::unique_ptr<bali::GameServer>  game = std::make_unique<bali::GameServer>(std::move(gcc));

    game->initialize();
    while (!game->isDone())
    {
        game->doProcessing();
    }
    game->cleanup();

    return 0;
}

