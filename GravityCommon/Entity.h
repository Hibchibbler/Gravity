#ifndef Entity_h_
#define Entity_h_

#include <stdint.h>
#include "Physical.h"
#include "Collider.h"
#include "EntityIntent.h"
#include "EntityState.h"
#include "Renderable.h"

namespace bali
{

class Entity
{
public:
    enum class EntityType {
        PLAYER,
        CONSUMABLE,
        MONSTER
    };

    EntityType          type;
    uint32_t            id;

    Physical            physical;
    Collider            collider;
    EntityIntent        intent;
    EntityState         state;
    Renderable          geometry;

    //sf::Time            stepaccum;
    //AnimationManager    animan;
};

}

#endif
