#pragma once

#include <vector>

namespace bali
{
    class Tile
    {
    public:
        Tile()
        {
            n = e = s = w = false;
            gid = 0;
        }
        typedef std::vector<Tile> Vec;
        float x, y;
        int tx, ty;
        int ti;
        bool n, e, s, w;
        int gid;
        unsigned char flip;
    };

    class TileLayer : public std::vector<Tile> {};
    class TileLayers : public std::vector<TileLayer> {};
}