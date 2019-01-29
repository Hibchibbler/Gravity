#ifndef Entity_h_
#define Entity_h_

#include <stdint.h>
#include "Proto.h"

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

    Vec<Shape> collisionShapes;

};


}

#endif
