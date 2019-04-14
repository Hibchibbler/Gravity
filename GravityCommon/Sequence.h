#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "Aseprite.h"
#include <map>

namespace bali
{
class Sequence
{
public:
    std::string frametagname;
    std::vector<ASE::Cel> cels;
};

void
LoadSequences(
    ASE::SpriteSheet sheet,
    std::map<std::string, Sequence> & sequences
);
}
#endif
