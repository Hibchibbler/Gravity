#pragma once

#include <vector>

namespace bali
{
    class Tile
    {
    public:
        typedef std::vector<Tile> Vec;
        typedef std::vector<Vec> VecVec;
        Tile()
        {
            gid = 0;
        }
        float x, y;
        int tx, ty;
        int ti;
        int gid;
        unsigned char flip;
    };


}