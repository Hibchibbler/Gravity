#include "Pathfinding.h"

#include <queue>
#include <unordered_map>
#include <functional>
#include "GravityCommon\Vector2.h"
namespace bali
{

size_t
heuristic(
    std::vector<Waypoint> & waypoints,
    size_t start,
    size_t goal
)
{
    size_t dx = waypoints[goal].location.x - waypoints[start].location.x;
    size_t dy = waypoints[goal].location.y - waypoints[start].location.y;
    return std::abs((long)dx) + std::abs((long)dy);
}

std::vector<size_t>
searchastar(
    std::vector<Waypoint> waypoints,
    size_t start,
    size_t goal
)
{
    typedef std::pair<size_t, size_t> Element;
    std::priority_queue<Element, std::vector<Element>, std::greater<Element>> frontier;
    frontier.emplace(0, start);

    std::unordered_map<size_t, size_t> camefrom;
    camefrom[start] = start;

    std::unordered_map<size_t, size_t> costsofar;
    costsofar[start] = 0;

    // Visit nodes, figure out most efficient path.
    while (!frontier.empty()) {
        size_t current = frontier.top().second;
        frontier.pop();

        if (current == goal)
        {
            break;
        }

        for (size_t next : waypoints[current].neighborindices)
        {
            size_t newcost = costsofar[current] + waypoints[next].weight;
            if (costsofar.find(next) == costsofar.end() ||
                newcost < costsofar[next])
            {
                costsofar[next] = newcost;
                camefrom[next] = current;
                size_t pri = newcost + heuristic(waypoints, next, goal);
                frontier.emplace(pri, next);
            }
        }
    }

    // Reverse the path list to get the path from here to there.
    std::vector<size_t> path;
    size_t current = goal;
    while (current != start) {
        path.push_back(current);
        current = camefrom[current];
    }
    path.push_back(start); // optional
    std::reverse(path.begin(), path.end());
    return path;
}

void
AddPolyline(PolylineDescriptor & polyline, std::vector<PathSegment> & segments)
{
    assert(polyline.points.size() > 1);
    for (size_t i = 0; i < polyline.points.size() - 1; i++)
    {
        PathSegment seg;
        seg.flags = polyline.flags;
        seg.s = polyline.points[i];
        seg.e = polyline.points[i + 1];
        segments.push_back(seg);
    }
}
bool
AddWaypoint(Waypoint & waypoint, std::vector<Waypoint> & waypoints)
{
    bool found = false;
    for (auto w : waypoints)
    {
        if (waypoint.location == w.location)
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        waypoint.id = waypoints.size();
        waypoints.push_back(waypoint);
    }
    return !found;
}



void
AddNeighbor(sf::Vector2f & neighborcoord, std::vector<sf::Vector2f> & neighbors)
{
    bool found = false;
    for (auto n : neighbors)
    {
        if (neighborcoord == n)
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        neighbors.push_back(neighborcoord);
    }
}

bool
CreateGraph(
    std::vector<PathSegment> segments,
    std::vector<Waypoint> & waypoints
)
{
    bool status = true;
    size_t id = 0;
    for (auto seg : segments)
    {
        Waypoint wp1(1, seg.s.x, seg.s.y, seg.flags);
        Waypoint wp2(1, seg.e.x, seg.e.y, seg.flags);

        for (auto oseg : segments)
        {
            if (seg.s == oseg.s)
            {
                AddNeighbor(oseg.e, wp1.neighborlocations);
            }
            else if (seg.s == oseg.e)
            {
                AddNeighbor(oseg.s, wp1.neighborlocations);
            }

            if (seg.e == oseg.e)
            {
                AddNeighbor(oseg.s, wp2.neighborlocations);
            }
            else if (seg.e == oseg.s)
            {
                AddNeighbor(oseg.e, wp2.neighborlocations);
            }
        }
        AddWaypoint(wp1, waypoints);
        AddWaypoint(wp2, waypoints);
    }

    for (auto & w : waypoints)
    {
        for (auto n : w.neighborlocations)
        {
            bool nope = false;
            for (auto w2 : waypoints)
            {
                if (n == w2.location)
                {
                    w.neighborindices.push_back(w2.id);
                    nope = true;
                    break;
                }
            }
        }
    }
    return status;
}

GetClosestWaypointResult
GetClosestWaypoint(
    std::vector<Waypoint> & waypoints,
    sf::Vector2f pos
)
{
    size_t thereid;
    float mindist = 999999999999999999.f;
    //        //sf::Vector2f here = entity.proto.body.pos;
    for (auto w : waypoints)
    {
        sf::Vector2f gap = w.location - pos;
        float dist = vec::mag(gap);
        if (dist < mindist)
        {
            mindist = dist;
            thereid = w.id;
        }
    }
    GetClosestWaypointResult gcwr;
    gcwr.distance = mindist;
    gcwr.wpi = thereid;
    return gcwr;
}

}
