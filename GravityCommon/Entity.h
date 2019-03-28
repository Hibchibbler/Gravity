#ifndef Entity_h_
#define Entity_h_

#include <stdint.h>
#include "Proto.h"
#include "SATAlgo\SATAlgo.h"//TODO: don't include this!!! jeezzuz
#include "Behavior.h"
namespace bali
{

class Entity
{
public:
    typedef Entity* Ptr;
    enum class State
    {
        INIT,
        IDLING,    // physics based
        MOVING,   // physics basedd
        JUMPING, // button based
        FALLING, // physics based
        CHARGING  // button based
    };

    Entity()
    {
        state = State::INIT;
        moving = false;
        jumping = false;
        charging = false;
    }

    void initialize(Proto & p) {
        proto = p;
        state = State::IDLING;
        moving = false;
        jumping = false;
        charging = false;

        registerwithaidirector = false;
        ignoreentitycollision = false;

        behavior.initialize();
    }

    void update(sf::Time elapsed)
    {
        //behavior.update(elapsed);
    }
    State       state;
    Behavior    behavior;
    Proto       proto;
    Collider    collider;

    bool isActive() { return moving || jumping || charging; }
    uint8_t     moving;
    uint8_t     jumping;
    uint8_t     charging;

    // These cross reference into the list that owns them.
    Vec<size_t> collisionshapes;//cleared in update
    Vec<size_t> collisionentities;// cleared in update
    Vec<size_t> waypointpath;
    size_t seekwaypoint;
    size_t currentwaypoint;
    Vec<Segment> visiblesegments;

    bool registerwithaidirector;
    bool ignoreentitycollision;

    sf::Vector2f upper;
    sf::Vector2f ahead;
    sf::Vector2f lower;
    sf::Vector2f distupper;
    sf::Vector2f distahead;
    sf::Vector2f distlower;

    sf::Vector2f avgForce;
    float numForce;

};


}

#endif
