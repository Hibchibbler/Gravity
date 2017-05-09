///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace bali
{
    namespace vec
    {
        class Vector2
        {
        public:
            double x;
            double y;
            Vector2();
            Vector2(double x, double y);

            Vector2(const Vector2 & v);

            double magnitude() const;

            Vector2 normal() const;

            Vector2 normalize() const;
            Vector2 subtract(Vector2 b) const;

            double dot(Vector2 b) const;

            Vector2 rotate(double angle_degrees) const;
            Vector2 translate(Vector2 t);

            Vector2 operator-(const Vector2 & other)
            {
                return vec::Vector2(this->x - other.x,
                                    this->y - other.y);
            }

            Vector2 operator+(const Vector2 & other)
            {
                return vec::Vector2(this->x + other.x,
                                    this->y + other.y);
            }

            Vector2 operator*(const Vector2 & other)
            {
                return vec::Vector2(this->x * other.x,
                                    this->y * other.y);
            }

            Vector2 operator-(const double & s)
            {
                return vec::Vector2(this->x - s,
                                    this->y - s);
            }

            Vector2 operator+(const double & s)
            {
                return vec::Vector2(this->x + s,
                                    this->y + s);
            }

            Vector2 operator*(const double & s)
            {
                return vec::Vector2(this->x * s,
                                    this->y * s);
            }

            Vector2 operator/(const double & s)
            {
                return vec::Vector2(this->x / s,
                    this->y / s);
            }
        };
    }
}