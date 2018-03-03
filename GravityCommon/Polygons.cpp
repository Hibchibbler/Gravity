#include "Polygons.h"
//
// TODO: There were problems with dependencies when Polygon didn't have it's own
// compilation unit.
//
namespace bali
{

    void translateShape(CONVEXSHAPE & shape, vec::VECTOR2 delta)
    {
        size_t pc = shape.getPointCount();
        for (auto i = 0; i < pc; i++)
        {
            vec::VECTOR2 pt = shape.getPoint(i) + delta;
            shape.setPoint(i, pt);
        }
    }


    CONVEXSHAPE GetTransformedShape(CONVEXSHAPE & shape)
    {
        CONVEXSHAPE transformedShape = shape;
        sf::Transform t = shape.getTransform();
        for (int w = 0; w < shape.getPointCount(); w++) {
            transformedShape.setPoint(w, t.transformPoint(shape.getPoint(w)));
        }
        return transformedShape;
    }

}

