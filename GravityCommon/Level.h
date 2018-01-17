#ifndef Level_H_
#define Level_H_

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include "TMXLoader/TMX.h"
#include "QuadTree/quadtree.h"
#include "SATAlgo/SATAlgo.h"
#include "TMXLoader/TextureAtlasLoader.h"
#include "Context.h"

namespace bali
{

struct BackgroundImage
{
    sf::Image                   img;
    sf::Texture                 tex;
    sf::Sprite                  sprite;
};

class BackgroundParallax
{
public:
    void loadBackgrounds(TMX::Imagelayer::Vec & layers);
    void updateBackgrounds(vec::VECTOR2 pos);
    BackgroundImage bgimages[5];
};


class Level
{
public:
    void initialize(
        std::string font,
        std::string tmx
    );

    void update(
        vec::VECTOR2    size,
        vec::VECTOR2    pos,
        float           angle
    );
    void draw(sf::RenderTarget & target, sf::RenderStates states) ;
    void cleanup();

    sf::RenderTexture           LevelRenderTexture;
    float                       TargetCameraAngle;
    TMX::Map::Ptr               map;
    sf::Image                   tilesetAImg;
    sf::Texture                 tilesetATex;
    sf::Image                   backgroundImg;
    sf::Texture                 backgroundTex;
    BackgroundParallax          parallaxBackground;
    qt::QuadTree::Vec           searchLayers;
    QuadArray::Vec              quadLayers;       // Full time
    QuadArray                   visibleQuads;
    CONVEXSHAPE::Vec            glowPolygons;
    Tile::VecVec                tileLayers;       // All the ones with a gid
    CONVEXSHAPE::Vec            polygons;         // All of them
    CONVEXSHAPE::Vec            polygonsVisible;  // Updated each iteration
    Segment::Vec                sharedEdges;      // 
    CONVEXSHAPE::Vec            playerPolygons;   // For Rendering (transformed SFML style)
    CONVEXSHAPE::Vec            playerCollisionPolygons;  // For collision (transformed Gravity Style)
    sf::Shader                  shader;
    sf::VertexArray             lineSegments;
    bali::tilemap::TileMap      playerAniTileMap;
    sf::Image                   textureAtlas1Image;
    sf::Texture                 textureAtlas1Tex;
    sf::Font                    font;

private:
    vec::VECTOR2    size;
    vec::VECTOR2    pos;
};



}


#endif

