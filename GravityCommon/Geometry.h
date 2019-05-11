#ifndef Geometry_H_
#define Geometry_H_

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <iterator>


namespace bali {

//
// I love type aliasing
//
template <typename T>
using Iter = typename T::iterator;

template <typename T>
using Vec = std::vector<T>;

template <typename T>
using List = std::list<T>;

using Vertex = sf::Vertex;

class ContactInfo
{
public:
    sf::Vector2f p1;
    sf::Vector2f p2;
    sf::Vector2f off;
    sf::Vector2f normal;
    uint64_t     isinternal;
    int64_t      thisindex;
    int64_t      thatindex;
    float        overlap;
};

class Shape
{
public:
    enum class Type
    {
        POINT,
        LINE,
        TRIANGLE,
        QUAD,
        LINESTRIP,
        CONVEX
    };
    Type              type;
    sf::Vector2f      origin;
    sf::Vector2f      position;
    sf::FloatRect     bounds;
    Vec<sf::Vector2f> points;
    std::vector<ContactInfo> edges;
};

sf::FloatRect
GetBounds(
    Shape & shape
);

sf::Vector2f
GetCentroid(
    Shape & shape
);


class Tile
{
public:
    Tile()
        : gid(0),
          x(0),
          y(0),
          tx(0),
          ty(0),
          ti(0),
          flip(0)
    {
    }

    float    x;
    float    y;
    int      tx;
    int      ty;
    int      ti;
    int      gid;
    uint8_t  flip;
};

class Segment
{
public:
    sf::Vector2f start;
    sf::Vector2f end;
    sf::Vector2f off;
    uint32_t flags;
};

class Intersection
{
public:
    Intersection() {}
    sf::Vector2f distance;
    float time;
    sf::Vector2f ray;
    sf::Vector2f rayPoint;
    sf::Vector2f segPoint;
    float angle;
    bool expired;//did the ray get to the end without hitting anything?
    Segment segment;
};

class Ray
{
public:
    sf::Vector2f offset;
    sf::Vector2f delta;
};

//class Shape
//{
//public:
//    uint32_t id;
//    CONVEXSHAPE points;
//};

//
// class Shapes
//  an object with a collection of shapes.
//  The shapes are related.
//  may have one or more shapes
//
//class Shapes
//{
//public:
//    void SetSID(uint32_t id);
//    uint32_t GetSID();
//
//    void     AddShape(Shape & shape);
//    Shape & Last();
//    VecIter<Shape> & BeginIter();
//    VecIter<Shape> & EndIter();
//    void     RemoveShape(uint32_t index);
//
//private:
//    uint32_t        sid;
//    Vec<Shape> shapes;
//};

}

#endif

