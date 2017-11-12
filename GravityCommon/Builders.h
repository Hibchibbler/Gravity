///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef BUILDERS_H_
#define BUILDERS_H_

#include <SFML\Graphics.hpp>
#include "tmxloader\TMX.h"
#include "quadtree\quadtree.h"
#include "GravityCommon/Polygons.h"
#include <stdint.h>

namespace bali
{
#define DEG_TO_RAD(x)   \
        (x) * (PI / 180.0f);

//bool isSharedEdge(SAT::ContactInfo & ci, std::vector<SAT::Segment> & sharedEdges);
//uint32_t buildSharedEdgesLayers(SAT::Segment::Vec & sharedEdges, TMX::Objectgroup::Vec & objectGroups);
sf::Vector2i GID2XY(int gid, int total_columns);
uint32_t addQuad(bali::QuadArray & v, sf::FloatRect c, sf::IntRect t, unsigned char flip);
uint32_t addRotQuad(sf::VertexArray & v, sf::FloatRect p, sf::IntRect t, float angle);
uint32_t addRotShape(bali::CONVEXSHAPE & s, sf::FloatRect p, float angle);
qt::AABB getSearchRegion(const sf::View & view, float zoom = 1.0f);
uint32_t buildTileLayer(Tile::Vec & tileLayer, const TMX::Tileset::Ptr tileset, const TMX::Layer::Ptr layer);
uint32_t buildTileLayers(Tile::VecVec & tileLayers, const TMX::Tileset::Ptr tileset, const TMX::Layer::Vec layer);
uint32_t buildQuadLayer(bali::QuadArray & quadLayer, Tile::Vec & tileLayer, uint32_t tileWidth, uint32_t tileHeight);
uint32_t buildPolygonLayers(CONVEXSHAPE::Vec & polygons, TMX::Objectgroup::Vec & objectGroups);
uint32_t buildPlayerObjectLayers(CONVEXSHAPE::Vec & polygons, TMX::Objectgroup::Vec & objectGroups);
uint32_t buildSearchLayer(qt::QuadTree::ShPtr & searchLayer, std::vector<CONVEXSHAPE> polygons);
}

#endif
