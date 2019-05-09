///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef BUILDERS_H_
#define BUILDERS_H_

#include <SFML\Graphics.hpp>
#include "XMLLoaders\TMX.h"
#include "quadtree\quadtree.h"
#include "Geometry.h"
#include "Entity.h"
#include "Texture.h"
#include "SpatialBuckets.h"
#include "Pathfinding.h"
#include "Context.h"
#include <stdint.h>

namespace bali
{
#define DEG_TO_RAD(x)   \
        (x) * (PI / 180.0f);

sf::Color GetRandomColor(int k);


//bool isSharedEdge(SAT::ContactInfo & ci, std::vector<SAT::Segment> & sharedEdges);
//uint32_t buildSharedEdgesLayers(SAT::Segment::Vec & sharedEdges, TMX::Objectgroup::Vec & objectGroups);
//sf::Vector2f rotatePoint(sf::Vector2f v, sf::Vector2f origin, float angle);
sf::Vector2i GID2XY(int gid, int total_columns);
//uint32_t addQuad(bali::QuadArray & v, sf::FloatRect c, sf::IntRect t, unsigned char flip);
//uint32_t addRotQuad(sf::VertexArray & v, sf::FloatRect p, sf::IntRect t, float angle);
//uint32_t addRotShape(bali::Shape & s, sf::FloatRect p, float angle);
qt::AABB getSearchRegion(sf::Vector2f center, sf::Vector2f size, float zoom = 1.0f);
//uint32_t buildTileLayer(Tile::Vec & tileLayer, const TMX::Tileset::Ptr tileset, const TMX::Layer::Ptr layer);
//uint32_t buildTileLayers(Tile::VecVec & tileLayers, const TMX::Tileset::Ptr tileset, const TMX::Layer::Vec layer);
//uint32_t buildQuadLayer(bali::QuadArray & quadLayer, Tile::Vec & tileLayer, uint32_t tileWidth, uint32_t tileHeight);
//uint32_t buildShapes(Shapes & polygons, TMX::Objectgroup::Ptr & objectGroup);
//uint32_t buildPlayerObjectLayers(Shapes & polygons, TMX::Objectgroup::Ptr & objectGroup);
//uint32_t buildSearchLayer(qt::QuadTree::ShPtr & searchLayer, Vec<Shapes> & polygons);
//uint32_t buildPlayerTransformedPolygon(RigidBody & phy, Shapes & pp, Shapes & pcp);

//
// TMX Level data starts life as a:
//   a few arrays of gids (tmx layer)
//   a few arrays of polygons (tmx objectgroup)
//  These are turned into:
//   arrays where each element is encoded with texture coordinates, and position(tilelayer)
//   arrays of shapes (Vec<Shape>)
//  This is turned into:
//   quadtrees
//   arrays of vertex
//  The Quadtrees store all the shapes
//      Shapes are retrieved, and then passed to collision detection
//  The arrays of vertex are passed to the GPU

sf::Vector2f
GetCentroid(
    Shape & shape
);

void
CreateEntityBucket(
    Vec<Entity> & entities,
    SpatialBuckets & buckets
);

void
GetEntityNeighbors(
    Vec<Entity> & entities,
    SpatialBuckets & buckets
);

void
CreateCPolyBucket(
    Vec<Shape> & cpolys,
    SpatialBuckets & buckets
);

void
GetCPolyNeighbors(
    Context* context,
    Vec<Entity> & entities,
    SpatialBuckets & buckets
);

uint32_t
addQuad(
    Vec<Vertex> & v,
    sf::FloatRect c,
    sf::IntRect t,
    unsigned char flip
);

void
loadTexture(
    Texture         &t,
    TMX::Tileset::Ptr & tsb,
    sf::Color       c
);

uint32_t
loadPrototype(
    Proto & proto,
    TMX::Objectgroup::Ptr & objectGroup
);

bool
buildRectangle(
    Shape & s,
    TMX::Object::Ptr obj
);

bool
buildPolygon(
    Shape & s,
    TMX::Object::Ptr obj
);


bool
buildWaypoints10(
    Vec<Waypoint> & waypoints,
    TMX::Objectgroup::Ptr & objectGroup
);

uint32_t 
loadPolygons(
    Vec<Shape> & shapes, 
    TMX::Objectgroup::Ptr & objectGroup
);

uint32_t 
loadTileLayer(
    Vec<Tile> & tiles, 
    TMX::Tileset::Ptr & tileset, 
    TMX::Layer::Ptr & layer
);

uint32_t
createVertexLayer(
    Vec<Vertex> & vertices,
    Vec<Tile> & tileLayer,
    uint32_t tileWidth,
    uint32_t tileHeight
);

uint32_t
addQuad(
    Vec<Vertex> & v,
    sf::FloatRect c,
    sf::IntRect t,
    unsigned char flip
);

bool
assignProto(
    std::vector<Proto> & protos,
    std::string pid,
    Proto & proto
);

bool
addEntity(
    bool registerwithaidirector,
    bool ignoreentitycollision,
    bool moving,
    Vec<Entity> & entities,
    Vec<Proto> & protos,
    TMX::Objectgroup::Ptr & ogptr
);

}

#endif
