#include "Polygons.h"
//
// TODO: There were problems with dependencies when Polygon didn't have it's own
// compilation unit.
//
using namespace bali;

void bali::translateShape(CONVEXSHAPE & shape, vec::VECTOR2 delta)
{
    size_t pc = shape.getPointCount();
    for (auto i = 0; i < pc; i++)
    {
        vec::VECTOR2 pt = shape.getPoint(i) + delta;
        shape.setPoint(i, pt);
    }
}

//
//void bali::positionShape(CONVEXSHAPE & shape, vec::VECTOR2 delta)
//{
//    size_t pc = shape.getPointCount();
//    for (auto i = 0; i < pc; i++)
//    {
//        vec::VECTOR2 pt =  delta;
//        shape.setPoint(i, pt);
//    }
//}

