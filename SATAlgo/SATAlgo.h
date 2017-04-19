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
            float x;
            float y;
            Vector2() {}
            Vector2(float x, float y)
            {
                this->x = x;
                this->y = y;
            }

            Vector2(const Vector2 & v)
            {
                this->x = v.x;
                this->y = v.y;
            }

            float magnitude() const
            {
                float m;
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
                float len = sqrt(x*x + y*y);
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

            float dot(Vector2 b) const
            {
                float scalar;
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
            MTV(const Vector2 & smallest, float overlap) 
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
            float overlap;
        };

        class Projection : public Vector2
        {
        public:
            Projection(float min, float max)
            {
                this->x = min;
                this->y = max;
            }


            Projection(const Projection & proj)
            {
                this->x = proj.x;// min
                this->y = proj.y;// max
            }
            float max(float a, float b)
            {
                return (a > b ? a : b);
            }

            float min(float a, float b)
            {
                return (a < b ? a : b);
            }
            bool overlap(const Projection & p)
            {
                if (x >= min(p.x, p.y) && x <= max(p.x, p.y) ||
                    y >= min(p.x, p.y) && x <= max(p.x, p.y))
                {
                    return true;
                }
                return false;
            }
            double getOverlap(const Projection & p)
            {
                // Find Max
                // Find Min
                // Of the two values remaining
                //  find Max
                //  find min
                //  subtract them

                //float pMin, pMax;
                //float tMin, tMax;
                float M, m;
                m = max(x, p.x);
                M = min(y, p.y);
                //
                /*M = max(x, max(y, max(p.x, p.y)));
                m = min(x, min(y, min(p.x, p.y)));
                
                float Mi, mi;
                mi = min(x, min(y, min(p.x, min(p.y, M))));


                if (x > min(p.x, p.y) && x < max(p.x, p.y) &&
                    y > min(p.x, p.y) && y < max(p.x, p.y))
                {
                }*/
                return (M-m);
            }
        };



        class Shape
        {
        public:
            std::vector<Vector2> vertices;
            void translate(float x, float y)
            {
                for (int i = 0; i < vertices.size(); i++)
                {
                    vertices[i].x += x;
                    vertices[i].y += y;
                }
            }

            void addVertex(float x, float y)
            {
                vertices.push_back(Vector2(x, y));
            }
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
                    normal = normal.normalize();

                    // the perp method is just (x, y) => (-y, x) or (y, -x)
                    axes.push_back(normal);
                }
                return axes;
            }

            Projection project(const Axis & axis)
            {
                Axis naxis = axis;// .normalize();
                double min = naxis.dot(vertices[0]);
                double max = min;
                for (int i = 1; i < vertices.size(); i++) {
                    // NOTE: the axis must be normalized to get accurate projections
                    double p = naxis.dot(vertices[i]);
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
                    Projection p1 = (*this).project(axis);
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