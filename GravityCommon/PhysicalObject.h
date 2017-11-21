///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICALOBJECT_H_
#define PHYSICAL_OBJECT_H_

#include <stdint.h>
#include <queue>
#include "Vector2.h"
#include "Command.h"
//#include "Observer.h"
namespace bali {
class GameObject
{
public:
    uint32_t id;
};

class Impulse
{
public:

};

class PhysicalObject
{
public:
    PhysicalObject()
    {
        mass = 1.0;
    }

    vec::VECTOR2 impulse(vec::VECTOR2 force);
    //void addImpulse(vec::VECTOR2 v);
    //bool  getImpulse(vec::VECTOR2 & v);

    void addAddVelocity(vec::VECTOR2 v);

    void addSetVelocity(vec::VECTOR2 v);

    void addAddPosition(vec::VECTOR2 v);


    void addSetPosition(vec::VECTOR2 v);
    bool getCommand(Command & c);



    vec::VECTOR2 pos;
    vec::VECTOR2 vel;
    vec::VECTOR2 accel;
    uint32_t mass;

    vec::VECTOR2 nextPos;
    vec::VECTOR2 velAccu;


public:
    std::queue<Command> cmdQueue;


};
}

#endif
