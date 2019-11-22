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
//  It will get a copy one of the prototype instances.
// Proto geometry is specified in the TMX file.
//
class Proto
{
public:
    typedef Proto* Ptr;
    enum class ProtoType {
        PLAYER,
        CONSUMABLE,
        MONSTER,
        STATIC,
        BULLET
    };

    enum class ProtoShape {
        POLYGON,
        RECTANGLE,
        CIRCLE
    };

    RigidBody           body;
    ProtoType           type;
    std::string         pid;
    Vec<Shape>          shapes;


    std::string         spritesheetname;
    ASE::SpriteSheet    spritesheet;
    std::map<std::string, Sequence> sequences;
    bali::Wardrobe      wardrobe;
    uint32_t            tiledid;
    std::shared_ptr<Texture> sstex; // Sprite Sheet Texture
};




}

#endif

