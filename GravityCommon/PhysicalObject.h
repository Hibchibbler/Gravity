///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICALOBJECT_H_
#define PHYSICAL_OBJECT_H_

#include <stdint.h>
#include <queue>
#include <list>
#include "Vector2.h"
#include "Command.h"

namespace bali {
class GameObject
{
public:
    uint32_t id;
};

class PhysicalObject
{
public:
    PhysicalObject()
    {
        mass = 1.0;
    }

    vec::VECTOR2 impulse(vec::VECTOR2 force);

    void addAddVelocity(vec::VECTOR2 v);
    void addSetVelocity(vec::VECTOR2 v);
    void addAddPosition(vec::VECTOR2 v);
    void addSetPosition(vec::VECTOR2 v);
    void addSetTargetAngle(float ta, uint32_t g);
    void addAddImpulse(vec::VECTOR2 v, float duration_ms);
    void addMove(float str, vec::VECTOR2 dir, bool right, bool grounde);
    void addJump(float str, float dur, vec::VECTOR2 dir);

    bool getAddCommand(Command & c);
    bool getSetCommand(Command & c);



    vec::VECTOR2 pos;
    vec::VECTOR2 vel;
    vec::VECTOR2 accel;
    uint32_t mass;

    vec::VECTOR2 nextPos;
    vec::VECTOR2 velAccu;


public:
    std::queue<Command> cmdAddQueue;
    std::queue<Command> cmdSetQueue;
    std::list<Command>  cmdTimedMoves;


};
}

#endif
