///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "Vector.h"
#include <cmath>

using namespace bali::vec;

Vector2::Vector2() 
{
}

Vector2::Vector2(double x, double y)
{
    this->x = x;
    this->y = y;
}

Vector2::Vector2(const Vector2 & v)
{
    this->x = v.x;
    this->y = v.y;
}

double Vector2::magnitude() const
{
    double m;
    m = sqrt(pow(x, 2) + pow(y, 2));
    return m;
}

Vector2 Vector2::normal() const
{
    Vector2 v(y, -1 * x);
    return v;
}

Vector2 Vector2::normalize() const
{
    Vector2 n(*this);
    double len = sqrt(x*x + y*y);
    if (len > 0)
    {
        n.x /= len;
        n.y /= len;
    }
    return n;
}
Vector2 Vector2::subtract(Vector2 b) const
{
    Vector2 v(*this);
    v.x -= b.x;
    v.y -= b.y;
    return v;
}

double Vector2::dot(Vector2 b) const
{
    double scalar;
    scalar = (x*b.x) + (y*b.y);
    return scalar;
}

Vector2 Vector2::rotate(double angle_degrees) const
{
    double ang_rad = angle_degrees * (3.14156f / 180.0f);
    Vector2 v(*this);

    v.x = (v.x * cos(ang_rad)) - (v.y * sin(ang_rad));
    v.y = (v.x * sin(ang_rad)) + (v.y * cos(ang_rad));
    return v;
}

Vector2 Vector2::translate(Vector2 t)
{
    Vector2 v(*this);
    v.x += t.x;
    v.y += t.y;
    return v;
}
