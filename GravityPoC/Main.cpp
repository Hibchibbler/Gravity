#include <SFML/Graphics.hpp>
#include <memory>

#include "GameClient.h"


int main()
{
    bali::GameClient game;
    game.initialize();
    while (!game.isDone())
    {
        game.doProcessing();
    }
    game.cleanup();
    return 0;

}

