///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameContext_h_
#define GameContext_h_

#include <SFML/Graphics.hpp>
#include <stdint.h>
#include "TMXLoader\TMXReader.h"
#include "QuadTree\quadtree.h"
#include "Utility.h"
#include "Vector\Vector2.h"
#include "SATAlgo\SATAlgo.h"
#include "Player.h"
#include "ConfigLoader.h"

namespace bali 
{

    class GameClient;
    struct GameContext
    {
        enum TargetTexture{
            FRONT=0,
            BACK=0,

        };
        GameClient*                 owner;

        uint32_t                    screenWidth;
        uint32_t                    screenHeight;
        sf::RenderWindow            window;
        TMX::Map::Ptr               map;

        sf::RenderTexture           renderTextures[2];
        sf::RenderTexture*          pRenderTexture0;
        sf::RenderTexture*          pRenderTexture1;

        sf::Image                   tilesetAImg;
        sf::Texture                 tilesetATex;

        sf::Image                   backgroundImg;
        sf::Texture                 backgroundTex;

        sf::View                    mainView;
        sf::Uint32                  centerx;
        sf::Uint32                  centery;
        sf::Vector2f                size;
        sf::Vector2f                worldMousePos;
        sf::Vector2i                screenMousePos;

        bali::SearchLayers          searchLayers;
        bali::QuadLayers            quadLayers;       // Full time
        bali::QuadLayer             visibleQuads;

        bali::TileLayers            tileLayers;       // All the ones with a gid
        std::vector<CONVEXSHAPE>    polygons;         // All of them
        std::vector<CONVEXSHAPE>    polygonsVisible;  // Updated each iteration
        std::vector<CONVEXSHAPE>    playerpolygons;   // 
        std::vector<SAT::Segment>   sharedEdges;      // 

        sf::VertexArray             lineSegments;

        sf::Transform               levelRotTrans;
        
        Player                      player;

        sf::Clock                   mainClock;
        sf::Clock                   inputClock;
        sf::Shader                  shader;

        PhysicsConfig               physicsConfig;
    };


}//end namespace bali
#endif
