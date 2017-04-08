#pragma once

#include <stdint.h>
#include <vector>

namespace bali
{
    namespace SAT
    {
        class Vector2
        {
        public:
            int32_t x;
            int32_t y;
            Vector2() {}
            Vector2(int32_t x, int32_t y)
            {
                this->x = x;
                this->y = y;
            }

            Vector2(const Vector2 & v)
            {
                this->x = v.x;
                this->y = v.y;
            }

            int32_t magnitude() const
            {
                int32_t m;
                m = sqrt(pow(x, 2) + pow(y, 2));
                return m;
            }

            Vector2 normal() const
            {
                Vector2 v(y, -x);
                return v;
            }

            Vector2 normalize() const
            {
                Vector2 n(*this);
                int32_t len = sqrt(x*x + y*y);
                if (len > 0)
                {
                    n.x /= len;
                    n.y /= len;
                }
                return n;
            }
            Vector2 subtract(Vector2 b)
            {
                Vector2 v(*this);
                v.x -= b.x;
                v.y -= b.y;
                return v;
            }

            int32_t dot(Vector2 b) const
            {
                int32_t scalar;
                scalar = (x*b.x) + (y*b.y);
                return scalar;
            }
        };

        typedef std::vector<Vector2> Axes;
        typedef Vector2 Axis;

        class MTV
        {
        public:
            MTV() {}
            MTV(const Vector2 & smallest, int32_t overlap) 
            {
                this->smallest = smallest;
                this->overlap = overlap;
            }
            MTV(const MTV & mtv)
            {
                this->smallest = mtv.smallest;
                this->overlap = mtv.overlap;
            }

            Vector2 smallest;
            int32_t overlap;
        };

        class Projection
        {
        public:
            Vector2 a;
            Vector2 b;

            Projection(int32_t min, int32_t max)
            {
                this->a.x = min;
                this->a.y = 0;
                this->b.x = max;
                this->b.y = 0;
            }
            Projection(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
            {
                this->a.x = x1;
                this->a.y = y1;
                this->b.x = x2;
                this->b.y = y2;
            }

            Projection(const Projection & proj)
            {
                this->a = proj.a;
                this->b = proj.b;
            }
            int32_t max(int32_t a, int32_t b)
            {
                return (a > b ? a : b);
            }

            int32_t min(int32_t a, int32_t b)
            {
                return (a < b ? a : b);
            }
            bool overlap(const Projection & p)
            {
                if (this->a.x > min(p.a.x, p.b.x) && this->a.x < max(p.a.x, p.b.x) &&
                    this->a.y > min(p.a.y, p.b.y) && this->a.y < max(p.a.y, p.b.y) &&
                    this->b.x > min(p.a.x, p.b.x) && this->b.x < max(p.a.x, p.b.x) &&
                    this->b.y > min(p.a.y, p.b.y) && this->b.y < max(p.a.y, p.b.y))
                {
                    return true;
                }
                return false;
            }
            double getOverlap(const Projection & p)
            {

                return 0.0;
            }
        };



        class Shape
        {
        public:
            std::vector<Vector2> vertices;

            Axes getAxes()
            {
                Axes axes;// = new Vector2[vertices.size()];
                // loop over the vertices
                for (int i = 0; i < vertices.size(); i++) 
                {
                    // get the current vertex
                    Vector2 p1 = vertices[i];

                    // get the next vertex
                    Vector2 p2 = vertices[i + 1 == vertices.size() ? 0 : i + 1];

                    // subtract the two to get the edge vector
                    Vector2 edge = p1.subtract(p2);

                    // get either normal vector
                    Vector2 normal = edge.normal();

                    // the perp method is just (x, y) => (-y, x) or (y, -x)
                    axes.push_back(normal);
                }
                return axes;
            }

            Projection project(const Axis & axis)
            {
                double min = axis.dot(vertices[0]);
                double max = min;
                for (int i = 1; i < vertices.size(); i++) {
                    // NOTE: the axis must be normalized to get accurate projections
                    double p = axis.dot(vertices[i]);
                    if (p < min) {
                        min = p;
                    }
                    else if (p > max) {
                        max = p;
                    }
                }
                Projection proj(min, max);
                return proj;
            }

            bool collision(Shape & other, MTV & mtv)
            {
                double overlap = 9999999999999.0;// really large value;
                Vector2 smallest;
                Axes axes1 = getAxes();
                Axes axes2 = other.getAxes();

                // loop over the axes1
                for (int i = 0; i < axes1.size(); i++) {
                    Axis axis = axes1[i];
                    // project both shapes onto the axis
                    Projection p1 = project(axis);
                    Projection p2 = other.project(axis);

                    // do the projections overlap?
                    if (!p1.overlap(p2)) {
                        // then we can guarantee that the shapes do not overlap
                        return false;
                    }
                    else {
                        // get the overlap
                        double o = p1.getOverlap(p2);

                        // check for minimum
                        if (o < overlap) {
                            // then set this one as the smallest
                            overlap = o;
                            smallest = axis;
                        }
                    }
                }
                // loop over the axes2
                for (int i = 0; i < axes2.size(); i++) {
                    Axis axis = axes2[i];
                    // project both shapes onto the axis
                    Projection p1 = project(axis);
                    Projection p2 = other.project(axis);

                    // do the projections overlap?
                    if (!p1.overlap(p2)) {
                        // then we can guarantee that the shapes do not overlap
                        return false;
                    }
                    else {
                        // get the overlap
                        double o = p1.getOverlap(p2);

                        // check for minimum
                        if (o < overlap) {
                            // then set this one as the smallest
                            overlap = o;
                            smallest = axis;
                        }
                    }
                }
                mtv = MTV(smallest, overlap);
                // if we get here then we know that every axis had overlap on it
                // so we can guarantee an intersection
                return true;
            }
        };
    }
}