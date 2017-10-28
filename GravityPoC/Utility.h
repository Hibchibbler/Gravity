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

    typedef qt::QuadTree::Ptr          SearchLayer;
    typedef std::vector<SearchLayer>   SearchLayers;
    class QuadLayer  : public sf::VertexArray 
    {
    public:
        QuadLayer(size_t vcount=0)
            :VertexArray(sf::Quads, vcount) {}
    };
    class QuadLayers : public std::vector<QuadLayer> {};
    class CONVEXSHAPE :public sf::ConvexShape 
    {
    public:
        // add flags to indicate shared edges....
        std::vector<bool> shared;
        double offsetX, offsetY;
        sf::Vector2f normal;
    };


    /*class HitInfo
    {
    public:
        SAT::MTV;
    };*/

    sf::Vector2i GID2XY(int gid, int total_columns);
    bool isSharedEdge(SAT::ContactInfo & ci, std::vector<SAT::Segment> & sharedEdges);
    //sf::Uint32 addQuad(sf::VertexArray & v, sf::FloatRect c, sf::IntRect t, bool, bool, bool);
    sf::Uint32 addQuad(sf::VertexArray & v, sf::FloatRect c, sf::IntRect t, unsigned char flip);
    sf::Uint32 addRotQuad(sf::VertexArray & v, sf::FloatRect p, sf::IntRect t, float angle);
    //sf::Uint32 addShape(SAT::Shape & s, uint32_t ti, uint32_t gid, sf::FloatRect c);
    sf::Uint32 addRotShape(SAT::Shape & s, sf::FloatRect p, float angle);

    qt::AABB getSearchRegion(const sf::View & view, float zoom=1.0f);

    uint32_t buildTileLayer(TileLayer & tileLayer, const TMX::Tileset::Ptr tileset, const TMX::Layer::Ptr layer);
    uint32_t buildTileLayers(TileLayers & tileLayers, const TMX::Tileset::Ptr tileset, const TMX::Layer::Vec layer);
    uint32_t buildQuadLayer(QuadLayer & quadLayer, TileLayer & tileLayer, uint32_t tileWidth, uint32_t tileHeight);

    uint32_t buildSharedEdgesLayers(std::vector<SAT::Segment> & sharedEdges, TMX::Objectgroup::Vec & objectGroups);
    uint32_t buildPolygonLayers(std::vector<CONVEXSHAPE> & polygons, TMX::Objectgroup::Vec & objectGroups);
    //uint32_t buildObjectLayers(std::vector<CONVEXSHAPE> & polygons, TMX::Objectgroup::Vec & objectGroups);
    uint32_t buildPlayerObjectLayers(std::vector<CONVEXSHAPE> & polygons, TMX::Objectgroup::Vec & objectGroups);
    uint32_t buildSearchLayer(SearchLayer & searchLayer, std::vector<CONVEXSHAPE> polygons);
}
