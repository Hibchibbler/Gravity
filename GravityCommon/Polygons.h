#ifndef Polygons_H_
#define Polygons_H_

#include <SFML/Graphics/ConvexShape.hpp>
#include <vector>
#include "Vector2.h"
namespace bali
{
//typedef sf::ConvexShape CONVEXSHAPE;
class CONVEXSHAPE : public sf::ConvexShape
{
public:
    typedef std::vector<CONVEXSHAPE> Vec;
};

class QuadArray : public sf::VertexArray
{
public:
    typedef std::vector<QuadArray> Vec;
    QuadArray(size_t vcount = 0)
        :VertexArray(sf::Quads, vcount) {}
};

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

class Segment
{
public:
    typedef std::vector<Segment> Vec;
    Segment()
    {
        start = vec::VECTOR2(0.0, 0.0);
        end = vec::VECTOR2(0.0, 0.0);
        off = vec::VECTOR2(0.0, 0.0);
    }

    Segment(vec::VECTOR2 _off, vec::VECTOR2 p1, vec::VECTOR2 p2)//float offx, float offy, float x1, float y1, float x2, float y2)
    {
        off.x = _off.x;
        off.y = _off.y;

        start.x = p1.x;
        start.y = p1.y;

        end.x = p2.x;
        end.y = p2.y;
    }
    bool isEqual(Segment & other)
    {
        if (start.x == other.start.x &&
            start.y == other.start.y &&
            end.x == other.end.x &&
            end.y == other.end.y)
        {
            return true;
        }
        return false;
    }

    float getSlope()
    {
        return (end.y - start.y) / (end.x - start.x);
    }

    vec::VECTOR2 off;
    vec::VECTOR2 start;
    vec::VECTOR2 end;
};

void translateShape(CONVEXSHAPE & shape, vec::VECTOR2 delta);


}

#endif

