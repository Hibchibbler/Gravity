#ifndef GameContext_h_
#define GameContext_h_

#include <SFML/Graphics.hpp>
#include <stdint.h>
#include "TMXLoader\TMXReader.h"
#include "QuadTree\quadtree.h"
#include "Utility.h"
#include "SATAlgo\SATAlgo.h"

namespace bali 
{
    class Player
    {
    public:
        void update(sf::Time elapsed)
        {
            //float timeU = elapsed.asSeconds()
            //if (sqrt(pow(velocity.x, 2) + pow(velocity.y, 2)) < 5)
            //{
            velocity.x += acceleration.x;
            velocity.y += acceleration.y;
            //velocity.x -= 0.1;
            //velocity.y -= 0.1;
            //if (velocity.x < 0)
            //    velocity.x = 0;
            //if (velocity.y < 0)
            //    velocity.y = 0;
            //}
            
            position.x += velocity.x;// *elapsed.asSeconds();
            position.y += velocity.y;// *elapsed.asSeconds();
        }
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::Vector2f acceleration;

        SAT::MTV currentMTV;
        QuadLayer playerQuads;
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
        sf::Vector2f        size;

        bali::SearchLayers        searchLayers;
        bali::QuadLayers          quadLayers;
        bali::QuadLayer           visibleQuads;
        bali::TileLayers          tileLayers;
        std::vector<ConvexShape>          polygons;

        sf::Transform  levelRotTrans;
        float angle;
        Player player;

        
        sf::Clock mainClock;
        


    };


}//end namespace bali
#endif
