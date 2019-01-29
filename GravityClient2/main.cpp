#include <SFML/Graphics.hpp>
#include <memory>

#include "GameClient.h"
//#include "ClientContext.h"
#include <memory>

int main()
{
    std::unique_ptr<bali::ClientContext> gcc = std::make_unique<bali::ClientContext>();
    std::unique_ptr<bali::GameClient>  game = std::make_unique<bali::GameClient>(std::move(gcc));

    game->initialize();
    while (!game->isDone())
    {
        game->doProcessing();
    }
    game->cleanup();

    return 0;

}

