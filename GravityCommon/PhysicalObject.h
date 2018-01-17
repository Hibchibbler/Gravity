///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICAL_OBJECT_H_
#define PHYSICAL_OBJECT_H_

#include <stdint.h>
#include <queue>
#include <list>
#include "Vector2.h"
#include "Command.h"

namespace bali {

class Physical
{
public:
    Physical()
    {
        mass = 1.0;
        angle = 0.0f;
    }
    vec::VECTOR2 impulse(vec::VECTOR2 force);
    vec::VECTOR2 pos;
    vec::VECTOR2 vel;
    vec::VECTOR2 accel;
    uint32_t     mass;
    //angle - starts at 
    //   3 O'  Clock = 0   degree
    //   6 O'  Clock = 90  degree
    //   9 O'  Clock = 180 degree
    //   12 O' Clock = 240 degree
    //   3 O'  Clock = 360 degree
    float        angle;
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
