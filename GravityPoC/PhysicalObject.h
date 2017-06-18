#pragma once
#include <stdint.h>
#include "Vector\Vector2.h"
namespace bali {
    class PhysicalObject
    {
    public:
        PhysicalObject()
        {
            mass = 1.0;
        }
        vec::VECTOR2 pos;
        vec::VECTOR2 vel;
        vec::VECTOR2 acc;
        uint32_t mass;
        vec::VECTOR2 downVector;
    public:
        vec::VECTOR2 accelerate(vec::VECTOR2 force)
        {
            //f = ma Newtons
            //a = f/m meters per meters squared or whatever
            return (force / mass);
        }
    };
}