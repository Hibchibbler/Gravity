///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#include "vector2.h"

/* Includes - STL */
#include <cstdio>
#include <cmath>
using namespace bali::vec;

VECTOR2::VECTOR2(void) : x(0), y(0)
{
}

VECTOR2::VECTOR2(float x, float y) : x(x), y(y)
{
}

VECTOR2::~VECTOR2(void)
{
}

void VECTOR2::reassign(float x, float y)
{
    this->x = x;
    this->y = y;
}

void VECTOR2::reassign(VECTOR2 otr)
{
    reassign(otr.x, otr.y);
}

VECTOR2& VECTOR2::operator+=(const VECTOR2 otr)
{
    this->x += otr.x;
    this->y += otr.y;
    return *this;
}

VECTOR2& VECTOR2::operator-=(const VECTOR2 otr)
{
    this->x -= otr.x;
    this->y -= otr.y;
    return *this;
}

VECTOR2& VECTOR2::operator*=(float scl)
{
    this->x *= scl;
    this->y *= scl;
    return *this;
}

VECTOR2& VECTOR2::operator/=(float scl)
{
    this->x /= scl;
    this->y /= scl;
    return *this;
}

VECTOR2& VECTOR2::operator%=(float theta)
{
    float cs = std::cos(theta);
    float sn = std::sin(theta);

    float px = x * cs - y * sn;
    float py = x * sn + y * cs;

    this->x = px;
    this->y = py;
    return *this;
}

VECTOR2 VECTOR2::operator+(const VECTOR2 otr) const
{
    return VECTOR2(this->x + otr.x, this->y + otr.y);
}

VECTOR2 VECTOR2::operator-(const VECTOR2 otr) const
{
    return VECTOR2(this->x - otr.x, this->y - otr.y);
}

VECTOR2 VECTOR2::operator*(float scl) const
{
    return VECTOR2(this->x * scl, this->y * scl);
}

VECTOR2 VECTOR2::operator/(float scl) const
{
    return VECTOR2(this->x / scl, this->y / scl);
}

VECTOR2 VECTOR2::operator%(float theta) const
{
    float cs = std::cos(theta);
    float sn = std::sin(theta);

    float px = x * cs - y * sn;
    float py = x * sn + y * cs;

    return VECTOR2(px, py);
}

bool VECTOR2::operator==(const VECTOR2 otr)
{
    return this->x == otr.x && this->y == otr.y;
}

bool VECTOR2::operator!=(const VECTOR2 otr)
{
    return !(*this == otr);
}

float VECTOR2::dot(const VECTOR2 otr) const
{
    return this->x * otr.x + this->y * otr.y;
}

VECTOR2 VECTOR2::norm()
{
    return *this / this->mag();
}

VECTOR2 VECTOR2::normal()
{// WARNING: this assumes clocwise winding
    return VECTOR2(this->y, -1 * this->x);
}


VECTOR2& VECTOR2::trunc(float max)
{
    if (this->mag() > max)
        this->reassign(this->norm() * max);
    return *this;
}

float VECTOR2::ang()
{
    return std::atan2(this->x, this->y);
}

float VECTOR2::mag()
{
    return std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2));
}

const std::string VECTOR2::to_str()
{
    char tmpbuf[256];
    sprintf_s(tmpbuf, "[%f, %f] \n", x, y);
    return tmpbuf;
}




//
//using namespace bali::vec;
//
//Vector2::Vector2() 
//{
//}
//
//Vector2::Vector2(double x, double y)
//{
//    this->x = x;
//    this->y = y;
//}
//
//Vector2::Vector2(const Vector2 & v)
//{
//    this->x = v.x;
//    this->y = v.y;
//}
//
//double Vector2::magnitude() const
//{
//    double m;
//    m = sqrt(pow(x, 2) + pow(y, 2));
//    return m;
//}
//
//Vector2 Vector2::normal() const
//{
//    Vector2 v(y, -1 * x);
//    return v;
//}
//
//Vector2 Vector2::normalize() const
//{
//    Vector2 n(*this);
//    double len = sqrt(x*x + y*y);
//    if (len > 0)
//    {
//        n.x /= len;
//        n.y /= len;
//    }
//    return n;
//}
//Vector2 Vector2::subtract(Vector2 b) const
//{
//    Vector2 v(*this);
//    v.x -= b.x;
//    v.y -= b.y;
//    return v;
//}
//
//double Vector2::dot(Vector2 b) const
//{
//    double scalar;
//    scalar = (x*b.x) + (y*b.y);
//    return scalar;
//}
//
//Vector2 Vector2::rotate(double angle_degrees) const
//{
//    double ang_rad = angle_degrees * (3.14156f / 180.0f);
//    Vector2 v(*this);
//
//    v.x = (v.x * cos(ang_rad)) - (v.y * sin(ang_rad));
//    v.y = (v.x * sin(ang_rad)) + (v.y * cos(ang_rad));
//    return v;
//}
//
//Vector2 Vector2::translate(Vector2 t)
//{
//    Vector2 v(*this);
//    v.x += t.x;
//    v.y += t.y;
//    return v;
//}
