///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICALOBJECT_H_
#define PHYSICAL_OBJECT_H_

#include <stdint.h>
#include "Vector2.h"
//#include "Observer.h"
namespace bali {
class GameObject
{
public:
    uint32_t id;
};
class PhysicalObject// : public Observer
{
public:
    PhysicalObject()
    {
        mass = 1.0;
    }

    void onNotify(uint32_t e)
    {

    }


    vec::VECTOR2 pos;
    vec::VECTOR2 vel;
    vec::VECTOR2 acc;
    uint32_t mass;

    vec::VECTOR2 nextPos;
    vec::VECTOR2 velAccu;


public:
    vec::VECTOR2 impulse(vec::VECTOR2 force);

};
}

#endif
