#pragma once

#include <SFML\Graphics.hpp>
#include <vector>
#include <memory>
#include "quadtree.h"
#include "TMX.h"

namespace bali
{
    typedef qt::QuadTree::Ptr          SearchLayer;
    typedef std::vector<SearchLayer>   SearchLayers;
    class QuadLayer  : public sf::VertexArray 
    {
    public:
        QuadLayer(size_t vcount=0)
            :VertexArray(sf::Quads, vcount) {}
    };
    class QuadLayers : public std::vector<QuadLayer> {};

    class Tile
    {
    public:
        typedef std::vector<Tile> Vec;
        float x, y;
        int tx, ty;
        int ti;
        int tw;
        int th;
    };

    class TileLayer  : public std::vector<Tile> {};
    class TileLayers : public std::vector<TileLayer> {};

    sf::Vector2i GID2XY(int gid, int total_columns);
    sf::Uint32 addQuad(sf::VertexArray & v, sf::FloatRect c, sf::IntRect t);

    uint32_t buildTileLayer(TileLayer & tileLayer, const TMX::Tileset::Ptr tileset, const TMX::Layer::Ptr layer);
    uint32_t buildTileLayers(TileLayers & tileLayers, const TMX::Tileset::Ptr tileset, const TMX::Layer::Vec layer);
    uint32_t buildQuadLayer(QuadLayer & quadLayer, TileLayer & tileLayer);
    uint32_t buildQuadLayers(QuadLayers & quadLayers, TileLayers & tileLayers);
    uint32_t buildSearchLayer(SearchLayer & searchLayer, TileLayer & tileLayer);
    uint32_t buildSearchLayers(SearchLayers & searchLayers, TileLayers & tileLayers);


    TMX::Tileset::Ptr getTileset(std::string name, TMX::Tileset::Vec & tilesets);
}
