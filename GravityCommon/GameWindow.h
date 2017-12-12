#ifndef GAMEWINDOW_H_
#define GAMEWINDOW_H_

#include <SFML/Graphics.hpp>
#include <stdint.h>

namespace bali
{



struct GameWindow
{
public:
    uint32_t                    screenWidth;
    uint32_t                    screenHeight;
    sf::RenderWindow            window;
};














}

#endif

