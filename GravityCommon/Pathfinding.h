#ifndef PATHFINDING_H_
#define PATHFINDING_H_

#include <assert.h>
#include <vector>
#include <SFML\System.hpp>

namespace bali
{
class PolylineDescriptor
{
public:
    bool looped;
    uint32_t pathid;
    uint32_t flags;
    std::vector<sf::Vector2f> points;
};

class Waypoint
{
public:
    Waypoint()
    {
        weight = 1;
    }

    Waypoint(uint32_t pathid, uint32_t flags, bool looped, size_t weight, size_t x, size_t y)
    {
        this->weight = weight;
        location.x = (float)x;
        location.y = (float)y;
        this->flags = flags;
        this->pathid = pathid;
        this->looped = looped;
    }
    size_t weight;
    uint32_t pathid;
    size_t id;
    uint32_t flags;
    sf::Vector2f  location;
    bool looped;

    // Shall be the same size
    std::vector<size_t> neighborindices;
    std::vector<sf::Vector2f> neighborlocations;
};


class PathSegment
{
public:
    sf::Vector2f s;// Start endpoint
    sf::Vector2f e;// End endpoint
    size_t flags;
    uint32_t pathid;
    bool looped;
};
void
AddPolyline(
    PolylineDescriptor & polyline,
    std::vector<PathSegment> & segments
);

bool
AddWaypoint(
    Waypoint & waypoint,
    std::vector<Waypoint> & waypoints
);

void
AddNeighbor(
    sf::Vector2f & neighborcoord,
    std::vector<sf::Vector2f> & neighbors
);

bool
CreateGraph(
    std::vector<PathSegment> segments,
    std::vector<Waypoint> & waypoints
);

class GetClosestWaypointResult
{
public:
    size_t wpi;
    float distance;
};

GetClosestWaypointResult
GetClosestWaypoint(
    std::vector<Waypoint> & waypoints,
    sf::Vector2f pos
);

std::vector<size_t>
searchastar(
    std::vector<Waypoint> waypoints,
    size_t start,
    size_t goal
);

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



}


#endif

