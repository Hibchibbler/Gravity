#ifndef Proto_h_
#define Proto_h_

#include <stdint.h>
#include "RigidBody.h"
#include "Geometry.h"
#include "Animation.h"
#include "Texture.h"
#include <memory>

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
        MONSTER,
        STATIC
    };

    Type        type;
    std::string pid;
    Vec<Shape>  shapes;
    RigidBody   body;
    std::string spritesheetname;
    ASE::SpriteSheet spritesheet;
    std::map<std::string, Sequence> sequences;
    bali::Wardrobe      wardrobe;
    uint32_t tiledid;
    std::shared_ptr<Texture> sstex; // Sprite Sheet Texture
};




}

#endif

