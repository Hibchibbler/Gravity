#ifndef TextureAtlas_H_
#define TextureAtlas_H_

#include "XMLReader.h"
#include <string>
#include <vector>
//#include "GravityCommon/Vector2.h"

namespace bali
{
namespace tilemap
{
struct Tile
{
    uint32_t x;
    uint32_t y;
    int32_t  tile;
    uint32_t rot;
    bool     flipX;
};

struct Layer
{
    uint32_t number;
    std::string name;
    std::vector<Tile> tiles;
};

struct TileMap
{
    uint32_t tilesswide;
    uint32_t tilesshigh;
    uint32_t tilewidth;
    uint32_t tileheight;
    std::vector<Layer> layers;
};

class TileMapLoader : public XMLReader
{
public:
    static bool load(const char *fn, TileMap & tmap) {
        return XMLReader::LoadXMLFile(fn, (void*)&tmap, TileMapLoader::start_element_handler, TileMapLoader::end_element_handler, TileMapLoader::character_data_handler);
    }

    static void character_data_handler(void *data, const char *content, int length)
    {

    }

    static void end_element_handler(void *data, const char *element)
    {

    }

    static void start_element_handler(void *data, const char *element, const char **attribute)
    {
        std::string elementName(element);
        TileMap* tmap = (TileMap*)data;
        if (elementName == "tilemap")
        {
            for (size_t i = 0; attribute[i]; i += 2)
            {
                if (std::string(attribute[i]) == "tileswide")
                {
                    tmap->tilesswide = std::strtol(attribute[i + 1], NULL, 10);
                }
                else if (std::string(attribute[i]) == "tileshigh")
                {
                    tmap->tilesshigh = std::strtol(attribute[i + 1], NULL, 10);
                }
                else if (std::string(attribute[i]) == "tilewidth")
                {
                    tmap->tilewidth = std::strtol(attribute[i + 1], NULL, 10);
                }
                else if (std::string(attribute[i]) == "tileheight")
                {
                    tmap->tileheight = std::strtol(attribute[i + 1], NULL, 10);
                }
            }
        }
        else if (elementName == "layer")
        {
            Layer layer;
            for (size_t i = 0; attribute[i]; i += 2)
            {
                if (std::string(attribute[i]) == "number")
                {
                    layer.number = std::strtol(attribute[i + 1], NULL, 10);
                }
                else if (std::string(attribute[i]) == "name")
                {
                    layer.name = std::string(attribute[i + 1], NULL, 10);
                }
            }

            tmap->layers.push_back(layer);
        }
        else if (elementName == "tile")
        {
            Tile tile;
            for (size_t i = 0; attribute[i]; i += 2)
            {
                if (std::string(attribute[i]) == "x")
                {
                    tile.x = std::strtol(attribute[i + 1], NULL, 10);
                }
                else if (std::string(attribute[i]) == "y")
                {
                    tile.y = std::strtol(attribute[i + 1], NULL, 10);
                }
                if (std::string(attribute[i]) == "tile")
                {
                    tile.tile = std::strtol(attribute[i + 1], NULL, 10);
                }
                else if (std::string(attribute[i]) == "rot")
                {
                    tile.rot = std::strtol(attribute[i + 1], NULL, 10);
                }
                else if (std::string(attribute[i]) == "flipX")
                {
                    tile.flipX = (std::string(attribute[i + 1], NULL, 10) == "false" ? false : true);
                }
            }
            tmap->layers.back().tiles.push_back(tile);
        }
    }
};

//class Frame
//{
//public:
//    vec::INTRECT getIntRect(bool flipX, bool flipY)
//    {
//        vec::INTRECT r;
//        r.left = x;
//        r.top = y;
//        r.width = w;
//        r.height = h;
//        if (flipY)
//        {
//            uint32_t temp = r.left;
//            r.left = r.left + r.width;
//            r.width = -1.0f * r.width;
//        }
//
//        if (flipX)
//        {
//            uint32_t temp = r.top;
//            r.top = r.top + r.height;
//            r.height = -1.0f * r.height;
//        }
//        return r;
//    }
//    std::string n;
//    uint32_t x;
//    uint32_t y;
//    uint32_t w;
//    uint32_t h;
//    float pX;
//    float pY;
//};
//
//class TextureAtlas
//{
//public:
//    std::string imagePath;
//    uint32_t width;
//    uint32_t height;
//    std::vector<Frame> frames;
//};
//
//class TextureAtlasLoader : public XMLReader
//{
//public:
//    static bool load(const char *fn, TextureAtlas & texture) {
//        return XMLReader::LoadXMLFile(fn, (void*)&texture, TextureAtlasLoader::start_element_handler, TextureAtlasLoader::end_element_handler, TextureAtlasLoader::character_data_handler);
//    }
//
//    static void character_data_handler(void *data, const char *content, int length)
//    {
//
//    }
//
//    static void end_element_handler(void *data, const char *element)
//    {
//
//    }
//
//    static void start_element_handler(void *data, const char *element, const char **attribute)
//    {
//        std::string elementName(element);
//        bali::TextureAtlas* texture = (bali::TextureAtlas*)data;
//        if (elementName == "TextureAtlas")
//        {
//            for (size_t i = 0; attribute[i]; i += 2)
//            {
//                if (std::string(attribute[i]) == "imagePath")
//                {
//                    texture->imagePath = std::string(attribute[i + 1]);
//                }
//                else if (std::string(attribute[i]) == "width")
//                {
//                    texture->width = std::strtol(attribute[i + 1], NULL, 10);
//                }
//                else if (std::string(attribute[i]) == "height")
//                {
//                    texture->height = std::strtol(attribute[i + 1], NULL, 10);
//                }
//            }
//        }
//        else if (elementName == "sprite")
//        {
//            texture->frames.push_back(Frame());
//            for (size_t i = 0; attribute[i]; i += 2)
//            {
//                if (std::string(attribute[i]) == "n")
//                {
//                    texture->frames.back().n = std::string(attribute[i + 1]);
//                }
//                else if (std::string(attribute[i]) == "x")
//                {
//                    texture->frames.back().x = std::strtol(attribute[i + 1], NULL, 10);
//                }
//                else if (std::string(attribute[i]) == "y")
//                {
//                    texture->frames.back().y = std::strtol(attribute[i + 1], NULL, 10);
//                }
//                else if (std::string(attribute[i]) == "w")
//                {
//                    texture->frames.back().w = std::strtol(attribute[i + 1], NULL, 10);
//                }
//                else if (std::string(attribute[i]) == "h")
//                {
//                    texture->frames.back().h = std::strtol(attribute[i + 1], NULL, 10);
//                }
//                else if (std::string(attribute[i]) == "pX")
//                {
//                    texture->frames.back().pX = std::strtof(attribute[i + 1], NULL);
//                }
//                else if (std::string(attribute[i]) == "pY")
//                {
//                    texture->frames.back().pY = std::strtof(attribute[i + 1], NULL);
//                }
//            }
//        }
//
//
//    }
//};


}



}

#endif

