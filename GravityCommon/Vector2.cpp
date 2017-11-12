#include "Vector2.h"
//
// TODO: There were problems with dependencies when Polygon didn't have it's own
// compilation unit.
//

#include <cmath>
using namespace bali;
using namespace bali::vec;

float vec::dot(const VECTOR2 a, const VECTOR2 b)
{
    return a.x * b.x + a.y * b.y;
}
float vec::mag(const VECTOR2 a)
{
    return std::sqrt(std::pow(a.x, 2) + std::pow(a.y, 2));
}

VECTOR2 vec::norm(const VECTOR2 a)
{
    return a / bali::vec::mag(a);
}

VECTOR2 vec::normal(const VECTOR2 a)
{// WARNING: this assumes clocwise winding
    return VECTOR2(a.y, -1 * a.x);
}

float vec::cross(const VECTOR2 a, const VECTOR2 b)
{
    return ((a.x*b.y) - (a.y*b.x));
}
