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

    void addAddVelocity(vec::VECTOR2 v);
    void addSetVelocity(vec::VECTOR2 v);
    void addAddPosition(vec::VECTOR2 v);
    void addSetPosition(vec::VECTOR2 v);
    void addSetTargetAngle(float ta, uint32_t g);
    void addAddImpulse(vec::VECTOR2 v, float duration_ms);
    void addMove(float str, vec::VECTOR2 dir, bool grounded);
    void addJump(float str, float dur, vec::VECTOR2 dir);

    bool getAddCommand(Command & c);
    bool getSetCommand(Command & c);



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

    //
    // 12 O' Clock => 0 degrees     (up)
    //  3 O' Clock => 90 degrees    (right)
    //  6 O' Clock => 180 degrees   (down)
    //  9 O' Clock => 240 degrees   (left)
    //
    float        angle;

    vec::VECTOR2 nextPos;
    vec::VECTOR2 velAccu;

public:
    std::queue<Command> cmdAddQueue;
    std::queue<Command> cmdSetQueue;
    std::list<Command>  cmdTimedMoves;
    std::list<Command>  cmdHistory;


};
}

#endif
