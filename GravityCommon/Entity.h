#ifndef Entity_h_
#define Entity_h_

#include <stdint.h>
#include "Proto.h"
#include "SATAlgo\SATAlgo.h"//TODO: don't include this!!! jeezzuz

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
    }
    State    state;

    Proto       proto;
    Collider    collider;

    bool isActive() { return moving || jumping || charging; }
    uint8_t     moving;
    uint8_t     jumping;
    uint8_t     charging;

    // These cross reference into the list that owns them.
    Vec<size_t> collisionShapes;//cleared in update
    Vec<size_t> collisionEntities;// cleared in update

    sf::Vector2f totalForce;
    sf::Vector2f totalPos;
    uint32_t numForces;
    float totalMagnitude;

};


}

#endif
