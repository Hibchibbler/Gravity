#include "Sequence.h"

namespace bali {
void
LoadSequences(
    ASE::SpriteSheet sheet,
    std::vector<Sequence> & sequences
)
{
    for (auto ft : sheet.meta.frametags)
    {
        Sequence newseq;
        newseq.name = ft.name;
        for (auto i = 0; i < sheet.frames.size(); i++)
        {
            if (i < ft.from || i > ft.to)
                continue;
            newseq.cels.push_back(sheet.frames[i]);
        }
        sequences.push_back(newseq);
    }
}

}

