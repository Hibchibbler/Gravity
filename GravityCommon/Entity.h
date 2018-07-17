#ifndef Entity_h_
#define Entity_h_

#include <stdint.h>
#include "RigidBody.h"
//#include "Collider.h"
//#include "EntityIntent.h"
//#include "EntityState.h"
#include "Geometry.h"

namespace bali
{

class Entity
{
public:
    enum class Type {
        PLAYER,
        CONSUMABLE,
        MONSTER
    };

    Type                type;
    uint32_t            id;

    Shape               shape;
    RigidBody           body; // This is a template value
                              // We load these values as defaults

    //Collider            collider;

    //sf::Time            stepaccum;
    //AnimationManager    animan;
};

}

#endif
