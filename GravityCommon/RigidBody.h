///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef RigidBody_H_
#define RigidBody_H_

#include <stdint.h>
#include <queue>
#include <list>
#include "Vector2.h"
#include "Command.h"
#include "Collider.h"

namespace bali {

class RigidBody
{
public:
    RigidBody()
    {
        pos = vec::Zero();
        vel = vec::Zero();
        accel = vec::Zero();
        mass = 1.0;
        angle = 0.0f;
        eid = 0;
    }
    vec::VECTOR2 impulse(vec::VECTOR2 force);

    vec::VECTOR2 pos;
    vec::VECTOR2 vel;
    vec::VECTOR2 accel;
    uint32_t     mass;
    float        angle;

    Collider     collider;
    uint32_t     eid; // This id tells us which Entity we are associated with.

    std::list<Command>  cmdHistory;
    struct CompareCommandPriority {
        bool operator()(Command const & p1, Command const & p2) {
            // return "true" if "p1" is ordered before "p2", for example:
            return p1.priority < p2.priority;
        }
    };

    std::priority_queue<Command, std::vector<Command>, CompareCommandPriority>  cmdQ;
public:
};


}

#endif
