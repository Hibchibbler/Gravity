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
        jumpcount = 0;
        autogravitated = false;
    }

    enum Condition
    {
        STABLE = 1,
        UNSTABLE = 2,
        FAST = 4
    };

    void setcollided(bool collided) {
        isCollided[1] = isCollided[0];
        isCollided[0] = collided;
    }

    bool iscollided() {
        return isCollided[1];
    }

    sf::Vector2f        surfaceNormal;  // This is set on any contact
    sf::Vector2f        jumpNormal;  // This is set on any contact that is feasible for jumping from
    uint64_t            flags;
    bool                isCollided[2];  // TODO: can't have this. there isn't a deep copy on this class..
    bool                autogravitated;
    uint32_t            jumpcount;

};

}

#endif

