#ifndef Collider_h_
#define Collider_h_

#include "Vector2.h"

namespace bali
{

class Collider
{
public:
    Collider()
    {
        isCollided = false;
        isCollidedLast = false;
        surfaceNormal = vec::Zero();
        jumpNormal = vec::Zero();
        latNormal = vec::Zero();
    }
    vec::VECTOR2        surfaceNormal;  // This is set on any contact
    vec::VECTOR2        jumpNormal;     // This is set when a jump can be performed.
    vec::VECTOR2        latNormal;      // This is set when a lateral movement can be performed.
    bool                isCollided;     // 
    bool                isCollidedLast; // 
};

}

#endif

