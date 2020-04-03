#ifndef AIDIRECTOR_H_
#define AIDIRECTOR_H_

#include "Entity.h"
#include "Player.h"
#include "Pathfinding.h"
#include "ConfigLoader.h"

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
        Vec<Player> & players,
        Vec<Entity> & entities,
        Vec<Waypoint> & waypoints,
        GeneralConfig & settings
    );

    bool
    getNearestPlayer(
        Vec<Player> & players,
        Vec<Entity> & entities,
        uint32_t eid,
        Player & player
    );

    void
    godirection(
        Entity & entity,
        Entity* otherentity,
        sf::Vector2f ngap,
        float dist
    );

    bool
    PopulateEntityPath(
        std::vector<Waypoint> & waypoints,
        Entity & entity,
        uint32_t pathid
    );

    void
        SetRailAndClickToNearestWaypoint(
        Vec<Waypoint> & waypoints,
        Entity & entity
    );

    void
    SetRail(
        Vec<Waypoint> & waypoints,
        size_t wpi,
        Entity & entity
    );

    bool
    handleHunting(
        Behavior::Disposition disp,
        Vec<Waypoint> & waypoints,
        Player & player,
        Vec<Entity> & entities,
        uint32_t eid
    );

    bool
    handleSeeking(
        Behavior::Disposition disp,
        Vec<Waypoint> & waypoints,
        GeneralConfig & settings,
        Player & player,
        Vec<Entity> & entities,
        uint32_t eid
    );

    bool
    handleWandering(
        Behavior::Disposition disp,
        Vec<Waypoint> & waypoints,
        GeneralConfig & settings,
        Entity & entity
    );

    bool
    handleDispositions(
        Vec<Waypoint> & waypoints,
        Vec<Player> & players,
        Vec<Entity> & entities,
        uint32_t eid,   // Current Entity Id
        GeneralConfig & settings
    );

    bool
    handleAlive(
        Vec<Waypoint> & waypoints,
        Vec<Player> & players,
        Vec<Entity> & entities,
        uint32_t eid,   // Current Entity Id
        GeneralConfig & settings
    );

    bool
    handleBirth(
        Vec<Waypoint> & waypoints,
        Entity & entity
    );

    bool
    handleDying(
        Entity & entity
    );


    bool
    handleDead(
        Entity & entity
    );


    std::vector<size_t> registeredentities;
    sf::Time timeaccumulator;
};

}
#endif


