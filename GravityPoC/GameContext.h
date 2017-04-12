#ifndef GameContext_h_
#define GameContext_h_

#include <SFML/Graphics.hpp>
#include <stdint.h>
#include "TMXReader.h"
#include "quadtree.h"
#include "Utility.h"

namespace bali 
{
    class Player
    {
    public:
        float x, y;
    };
    
    class GameClient;
    class GameContext
    {
    public:
        GameContext()
            /*:layerBack({0,0,800,600},5),
             layerFore({ 0,0,800,600 }, 5)*/
        {
            angle = 0.0;
        }
    public:
        GameClient*         owner;
        uint32_t            screenWidth;
        uint32_t            screenHeight;
        sf::RenderWindow    window;
        TMX::MapContext     mctx;

        sf::Image           tilesetAImg;
        sf::Texture         tilesetATex;

        sf::Image           backgroundImg;
        sf::Texture         backgroundTex;

        sf::View            mainView;
        sf::Uint32          centerx;
        sf::Uint32          centery;

        bali::SearchLayers        searchLayers;
        bali::QuadLayers          quadLayers;
        bali::QuadLayer           visibleQuads;
        bali::TileLayers          tileLayers;

        sf::Transform  levelRotTrans;
        float angle;
    };


}//end namespace bali
#endif
