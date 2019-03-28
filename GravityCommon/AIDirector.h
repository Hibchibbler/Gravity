#ifndef AIDIRECTOR_H_
#define AIDIRECTOR_H_

#include "Entity.h"
#include "Pathfinding.h"

namespace bali
{
class AIDirector
{
public:
    void
    initialize(
        Vec<Entity> & entities
    );

    void
    update(
        sf::Time elapsed,
        Vec<Entity> & entities,
        Vec<Waypoint> & waypoints
    );

    void SeekEntity(Entity & from, Entity & to);
    void SeekWaypoint(Entity & from, Waypoint & to);
    void FoundWaypoint(Entity & entity);
    void FoundEntity(Entity & entity);


    std::vector<size_t> registeredentities;
    sf::Time timeaccumulator;
};

}
#endif


