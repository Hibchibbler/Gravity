#ifndef SPATIAL_BUCKETS_H_
#define SPATIAL_BUCKETS_H_

#include <vector>
#include <iostream>
#include <SFML\System.hpp>
#include <assert.h>
//
// SpatialBucket stores entity indexes
// that are local to this spatial region.
//
class SpatialBucket
{
public:
    std::vector<size_t> locals;
};

//
// SpatialBuckets divides an area into smaller fixed size buckets.
//
class SpatialBuckets
{
public:
    SpatialBuckets()
    {
    }

    SpatialBuckets(uint32_t minx, uint32_t miny, uint32_t maxx, uint32_t maxy, uint32_t numrows, uint32_t numcols)
    {
        initialize(minx, miny, maxx, maxy, numrows, numcols);
    }

    void initialize(uint32_t minx, uint32_t miny, uint32_t maxx, uint32_t maxy, uint32_t numrows, uint32_t numcols)
    {
        this->minx = minx;
        this->miny = miny;
        this->maxx = maxx;
        this->maxy = maxy;
        this->numrows = numrows;
        this->numcols = numcols;

        uint32_t rw = this->maxx - this->minx;
        uint32_t rh = this->maxy - this->miny;

        this->bucketwidth = rw / numcols;
        this->bucketheight = rh / numrows;

        bucketlist.assign(numcols*numrows, SpatialBucket());

    }

    void clear()
    {
        for (uint32_t c = 0; c < this->numcols; c++)
        {
            for (uint32_t r = 0; r < this->numrows; r++)
            {
                bucketlist[r + (numcols * c)].locals.clear();
            }
        }
    }
    void add(
        uint32_t x,
        uint32_t y,
        uint32_t id
    )
    {
        uint32_t bx = x;
        uint32_t by = y;
        if (bx >= 0 && bx < this->numcols * this->bucketwidth &&
            by >= 0 && by < this->numrows * this->bucketheight)
        {
            bx = x / this->bucketwidth;
            by = y / this->bucketheight;

            int index = XY2GID(bx, by, this->numcols);
            //std::cout << index << std::endl;
            //if (index != id)// TODO: why did i do this?!?1
            //{
                bucketlist[index].locals.push_back(id);
            //}
        }
    }

    sf::Vector2i GID2XY(uint32_t gid, uint32_t total_columns)
    {
        sf::Vector2i ret;
        ret.y = gid / total_columns;
        ret.x = gid % total_columns;
        return ret;
    }

    int XY2GID(int x, int y, int total_columns)
    {
        int g = (x + (total_columns * y));
        assert(g >= 0);
        return g;
    }

    void
    getneighbors(
            uint32_t x,
            uint32_t y,
            std::vector<size_t> & neighbors
        )
    {
        uint32_t bx;
        uint32_t by;
        uint32_t width = this->numcols * this->bucketwidth;
        uint32_t height = this->numrows * this->bucketheight;
        //std::cout << "#" << std::endl;
        bx = x;
        by = y;
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;

            int here = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[here].locals.begin(), bucketlist[here].locals.end());

            //std::cout << "My Index) " << here << ", " << bucketlist[here].locals.size() << std::endl;
        }

        bx = (x + bucketwidth);
        by = (y - bucketwidth);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int ne = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[ne].locals.begin(), bucketlist[ne].locals.end());

            //std::cout << "NE) " << ne << ", " << bucketlist[ne].locals.size() << std::endl;
        }

        bx = (x - 0);
        by = (y - bucketwidth);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int n = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[n].locals.begin(), bucketlist[n].locals.end());
            //std::cout << "N ) " << n << ", " << bucketlist[n].locals.size() << std::endl;
        }


        bx = (x - bucketwidth);
        by = (y - bucketwidth);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int nw = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[nw].locals.begin(), bucketlist[nw].locals.end());
            //std::cout << "NW) " << nw << ", " << bucketlist[nw].locals.size() << std::endl;
        }

        bx = (x + bucketwidth);
        by = (y - 0);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int e = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[e].locals.begin(), bucketlist[e].locals.end());
            //std::cout << "E ) " << e << ", " << bucketlist[e].locals.size() << std::endl;
        }

        bx = (x - bucketwidth);
        by = (y - 0);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int w = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[w].locals.begin(), bucketlist[w].locals.end());
            //std::cout << "W ) " << w << ", " << bucketlist[w].locals.size() << std::endl;
        }

        bx = (x + bucketwidth);
        by = (y + bucketwidth);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int se = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[se].locals.begin(), bucketlist[se].locals.end());
            //std::cout << "SE) " << se << ", " << bucketlist[se].locals.size() << std::endl;
        }

        bx = (x + 0);
        by = (y + bucketwidth);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int s = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[s].locals.begin(), bucketlist[s].locals.end());
            //std::cout << "S ) " << s << ", " << bucketlist[s].locals.size() << std::endl;
        }

        bx = (x - bucketwidth);
        by = (y + bucketwidth);
        if (bx >= 0 && bx < width &&
            by >= 0 && by < height)
        {
            bx = bx / this->bucketwidth;
            by = by / this->bucketheight;
            int sw = XY2GID(bx, by, this->numcols);
            neighbors.insert(neighbors.end(), bucketlist[sw].locals.begin(), bucketlist[sw].locals.end());
            //std::cout << "SW) " << sw << ", " << bucketlist[sw].locals.size() << std::endl;
        }
        //return neighbors;
    }


private:
    uint32_t minx;
    uint32_t miny;
    uint32_t maxx;
    uint32_t maxy;
    uint32_t numrows;
    uint32_t numcols;
    uint32_t bucketwidth;
    uint32_t bucketheight;

    std::vector<SpatialBucket> bucketlist;
};


#endif
