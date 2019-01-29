#ifndef Proto_h_
#define Proto_h_

#include <stdint.h>
#include "RigidBody.h"
#include "Geometry.h"

namespace bali
{

//
// A Proto is used to store the default
//  values and geometry for the entities to use.
// A Proto is a prototypical entity.
//  When an Entity is realized in-game
//  It will refer to one of these prototypes.
//
class Proto
{
public:
    typedef Proto* Ptr;
    enum class Type {
        PLAYER,
        CONSUMABLE,
        MONSTER
    };

    Type        type;
    uint32_t    id;

    Vec<Shape>  shapes;
    RigidBody   body;
};




}

#endif

