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
        };
    }
}