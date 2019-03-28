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

    Waypoint(size_t weight, size_t x, size_t y, uint32_t flags)
    {
        this->weight = weight;
        location.x = x;
        location.y = y;
        this->flags = flags;
    }
    size_t weight;
    size_t id;
    uint32_t flags;
    sf::Vector2f  location;

    // Shall be the same size
    std::vector<size_t> neighborindices;
    std::vector<sf::Vector2f> neighborlocations;
};

#define SEGMENT_FLAGS_WALK      0
#define SEGMENT_FLAGS_JUMP_UP   1
#define SEGMENT_FLAGS_JUMP_OVER 2
#define SEGMENT_FLAGS_STOP      3



class PathSegment
{
public:
    sf::Vector2f s;// Start endpoint
    sf::Vector2f e;// End endpoint
    size_t flags;
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

