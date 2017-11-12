///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SFML\Graphics.hpp>
#include <vector>
#include <memory>
#include "quadtree\quadtree.h"
#include "tmxloader\TMX.h"
#include "SATAlgo\SATAlgo.h"
#include "TileLayer.h"

namespace bali
{
    #define DEG_TO_RAD(x)   \
        (x) * (PI / 180.0f);

    class QuadArray : public sf::VertexArray
    {
    public:
        typedef std::vector<QuadArray> Vec;
        QuadArray(size_t vcount = 0)
            :VertexArray(sf::Quads, vcount) {}
    };

    class CONVEXSHAPE : public sf::ConvexShape 
    {
    public:
        typedef std::vector<CONVEXSHAPE> Vec;
        double offsetX, offsetY;
    };

    sf::Vector2i GID2XY(int gid, int total_columns);
    bool isSharedEdge(SAT::ContactInfo & ci, std::vector<SAT::Segment> & sharedEdges);
    sf::Uint32 addQuad(bali::QuadArray & v, sf::FloatRect c, sf::IntRect t, unsigned char flip);
    sf::Uint32 addRotQuad(sf::VertexArray & v, sf::FloatRect p, sf::IntRect t, float angle);
    sf::Uint32 addRotShape(SAT::Shape & s, sf::FloatRect p, float angle);
    qt::AABB getSearchRegion(const sf::View & view, float zoom=1.0f);
    uint32_t buildTileLayer(Tile::Vec & tileLayer, const TMX::Tileset::Ptr tileset, const TMX::Layer::Ptr layer);
    uint32_t buildTileLayers(Tile::VecVec & tileLayers, const TMX::Tileset::Ptr tileset, const TMX::Layer::Vec layer);
    uint32_t buildQuadLayer(bali::QuadArray & quadLayer, Tile::Vec & tileLayer, uint32_t tileWidth, uint32_t tileHeight);
    uint32_t buildSharedEdgesLayers(SAT::Segment::Vec & sharedEdges, TMX::Objectgroup::Vec & objectGroups);
    uint32_t buildPolygonLayers(CONVEXSHAPE::Vec & polygons, TMX::Objectgroup::Vec & objectGroups);
    uint32_t buildPlayerObjectLayers(CONVEXSHAPE::Vec & polygons, TMX::Objectgroup::Vec & objectGroups);
    uint32_t buildSearchLayer(qt::QuadTree::ShPtr & searchLayer, std::vector<CONVEXSHAPE> polygons);
}
