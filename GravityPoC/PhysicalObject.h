#pragma once
#include <stdint.h>
#include "Vector\Vector.h"
namespace bali {
    class PhysicalObject
    {
    public:
        vec::Vector2 pos;
        vec::Vector2 vel;
        vec::Vector2 acc;
        uint32_t mass;
    protected:

    private:

    };
}