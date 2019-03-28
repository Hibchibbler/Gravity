#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "Aseprite.h"

namespace bali
{
class Sequence
{
public:
    std::string name;
    std::vector<ASE::Cel> cels;
};

void
LoadSequences(
    ASE::SpriteSheet sheet,
    std::vector<Sequence> & sequences
);
}
#endif
