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

//template <typename T>
//using ListIter = std::list<T>::iterator;



//template <typename T>
//using VecIter = typename std::vector<T>::iterator;

using Vertex = sf::Vertex;
using Shape = sf::ConvexShape;

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

