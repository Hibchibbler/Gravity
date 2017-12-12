///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "TMXReader.h"
#include "TextureAtlasLoader.h"

int main()
{
    bali::TMX::Map::Ptr map = make_shared<bali::TMX::Map>();
    bali::TMX::TMXReader::load("level1.tmx", map);

    //bali::TextureAtlas t;
    //bali::TextureAtlasLoader::load("TextureAtlas_test1.xml", t);

    bali::tilemap::TileMap tmap;
    bali::tilemap::TileMapLoader::load("MegaMan_v0.xml", tmap);
    int a = 42;
    return 0;
}