///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef GameClientContext_h_
#define GameClientContext_h_

#include "GravityCommon/Context.h"
#include "TMXLoader/TMX.h"
#include "QuadTree/quadtree.h"
#include "SATAlgo/SATAlgo.h"
#include "GravityCommon/ConfigLoader.h"
#include "GravityCommon/Player.h"
#include "GravityCommon/MouseKeyboard.h"
#include "TMXLoader/TextureAtlasLoader.h"

namespace bali 
{

struct SingleSprite
{
    sf::Image                   img;
    sf::Texture                 tex;
    sf::Sprite                  sprite;
};

class GameClientContext : public Context
{
public:
    typedef GameClientContext*  Ptr;

    TMX::Map::Ptr               map;

    sf::RenderTexture           renderTextures[2];
    sf::RenderTexture*          pRenderTexture0;
    sf::RenderTexture*          pRenderTexture1;

    sf::Image                   tilesetAImg;
    sf::Texture                 tilesetATex;

    sf::Image                   backgroundImg;
    sf::Texture                 backgroundTex;

    SingleSprite                parallaxBackgrounds[5];

    sf::View                    mainView;
    sf::Uint32                  centerx;
    sf::Uint32                  centery;
    sf::Vector2f                size;
    sf::Vector2f                worldMousePos;
    sf::Vector2i                screenMousePos;

    qt::QuadTree::Vec           searchLayers;
    QuadArray::Vec              quadLayers;       // Full time
    QuadArray                   visibleQuads;

    CONVEXSHAPE::Vec            glowPolygons;

    Tile::VecVec                tileLayers;       // All the ones with a gid
    CONVEXSHAPE::Vec            polygons;         // All of them
    CONVEXSHAPE::Vec            polygonsVisible;  // Updated each iteration
    CONVEXSHAPE::Vec            playerpolygons;   // 
    Segment::Vec                sharedEdges;      // 

    sf::VertexArray             lineSegments;

    sf::Clock                   mainClock;
    sf::Clock                   inputClock;
    sf::Shader                  shader;

    //TextureAtlas                textureAtlas1;
    bali::tilemap::TileMap      playerAniTileMap;
    sf::Image                   textureAtlas1Image;
    sf::Texture                 textureAtlas1Tex;
};


}//end namespace bali
#endif
