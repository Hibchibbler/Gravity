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
        isCollided[0] = false;
        isCollided[1] = false;
        surfaceNormal = vec::Zero();
        flags = 0;
    }

    enum Condition
    {
        STABLE = 1,
        UNSTABLE = 2,
        FAST = 4
    };


    sf::Vector2f        surfaceNormal;  // This is set on any contact
    uint64_t            flags;
    bool                isCollided[2];  // 

};

}

#endif

