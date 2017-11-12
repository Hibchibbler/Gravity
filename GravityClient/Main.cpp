#include <SFML/Graphics.hpp>
#include <memory>

#include "GameClient.h"
#include "GameClientContext.h"

int main()
{
    bali::GameClientContext gcc;
    bali::GameClient game(&gcc);

    bali::Game* pGame = &game;
    pGame->initialize();
    while (!pGame->isDone())
    {
        pGame->doProcessing();
    }
    pGame->cleanup();
    return 0;

}

