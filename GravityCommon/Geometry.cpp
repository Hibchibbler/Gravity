#include "Geometry.h"

namespace bali
{

sf::FloatRect
GetBounds(
    Shape & shape
)
{
    // Calculate bounds
    // TODO //
    sf::Vector2f min(99999, 99999);
    sf::Vector2f max(-99999, -99999);

    for (auto p : shape.points)
    {
        if (p.x > max.x)
            max.x = p.x;
        if (p.y > max.y)
            max.y = p.y;

        if (p.x < min.x)
            min.x = p.x;
        if (p.y < min.y)
            min.y = p.y;
    }
    shape.bounds.left = min.x;
    shape.bounds.top = min.y;
    shape.bounds.width = max.x - min.x;
    shape.bounds.height = max.y - min.y;
    return shape.bounds;
}

sf::Vector2f
GetCentroid(
    Shape & shape
)
{
    sf::Vector2f c1;
    sf::Vector2f dir;
    for (int u = 0; u < shape.points.size(); u++)
    {
        c1 = c1 + (shape.points[u] + shape.position);
    }
    c1 = c1 / (float)shape.points.size();
    return c1;

}
//void Shapes::SetSID(uint32_t id)
//{
//    sid = id;
//}
//
//uint32_t Shapes::GetSID()
//{
//    return sid;
//}
//
//void Shapes::AddShape(Shape & shape)
//{
//    shapes.push_back(shape);
//}
//
//VecIter<Shape> & Shapes::BeginIter()
//{
//    return shapes.begin();
//}
//
//VecIter<Shape> & Shapes::EndIter()
//{
//    return shapes.end();
//}
//
//Shape & Shapes::Last()
//{
//    return shapes.back();
//}
//
//void Shapes::RemoveShape(uint32_t index)
//{
//    
//}


}

