#pragma once

#include <stdint.h>
#include <vector>
#include "Vector\Vector.h"
namespace bali
{
    namespace SAT
    {
        typedef std::vector<vec::Vector2> Axes;
        typedef vec::Vector2 Axis;

        class MTV
        {
        public:
            MTV() {}
            MTV(const vec::Vector2 & smallest, double overlap)
            {
                this->smallest = smallest;
                this->overlap = overlap;
            }
            MTV(const MTV & mtv)
            {
                this->smallest = mtv.smallest;
                this->overlap = mtv.overlap;
            }

            vec::Vector2 smallest;
            double overlap;
        };

        class Projection : public vec::Vector2
        {
        public:
            Projection(double min, double max)
            {
                this->x = min;
                this->y = max;
            }


            Projection(const Projection & proj)
            {
                this->x = proj.x;// min
                this->y = proj.y;// max
            }
            double max(double a, double b)
            {
                return (a >= b ? a : b);
            }

            double min(double a, double b)
            {
                return (a <= b ? a : b);
            }
            bool overlap(const Projection & p)
            {
                if (x >= min(p.x, p.y) && x <= max(p.x, p.y) ||
                    y >= min(p.x, p.y) && y <= max(p.x, p.y))
                {
                    return true;
                }
                return false;

            }
            double getOverlap(const Projection & p)
            {
                double M, m;
                m = max(x, p.x);
                M = min(y, p.y);
                //m = max(max(x, p.x), max(y,p.y));
                //M = min(min(y, p.y), min(x, p.x));
                return abs(M-m);
            }
        };



        class Shape
        {
        public:
            std::vector<vec::Vector2> vertices;
            void translate(double x, double y)
            {
                for (int i = 0; i < vertices.size(); i++)
                {
                    vertices[i].x += x;
                    vertices[i].y += y;
                }
            }

            void addVertex(double x, double y)
            {
                vertices.push_back(vec::Vector2(x, y));
            }
            Axes getAxes()
            {
                Axes axes;// = new Vector2[vertices.size()];
                // loop over the vertices
                for (int i = 0; i < vertices.size(); i++) 
                {
                    // get the current vertex
                    vec::Vector2 p1 = vertices[i];

                    // get the next vertex
                    vec::Vector2 p2 = vertices[i + 1 == vertices.size() ? 0 : i + 1];

                    // subtract the two to get the edge vector
                    vec::Vector2 edge = p2.subtract(p1);

                    // get either normal vector
                    vec::Vector2 normal = edge.normal();
                    normal = normal.normalize();

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
                    if (p <= min) {
                        min = p;
                    }
                    else 
                    if (p >= max) {
                        max = p;
                    }
                }
                Projection proj(min, max);
                return proj;
            }

            bool collision(Shape & other, MTV & mtv)
            {
                double overlap = 999999999.0;// really large value;
                vec::Vector2 smallest;
                Axes axes1 = (*this).getAxes();
                Axes axes2 = other.getAxes();

                // loop over the axes1
                //cout << ">>>>>>>>>" << std::endl;
                for (int i = 0; i < axes1.size(); i++) {
                    Axis axis = axes1[i];
                    // project both shapes onto the axis
                    Projection p1 = (*this).project(axis);
                    Projection p2 = other.project(axis);

                    //cout << p1.x << ", " << p1.y << " | " << p2.x << ", " << p2.y;// std::endl;

                    // do the projections overlap?
                    if (!p1.overlap(p2)) {
                        // then we can guarantee that the shapes do not overlap
                        return false;
                    }
                    else {
                        // get the overlap
                        double o = p1.getOverlap(p2);
//                        cout << " P[" << o << "],"<< (o < overlap ? "T" : "U")<<" <" << axis.x << ", " << axis.y << ">" << std::endl;
                        // check for minimum
                        if (o < overlap) {
                            // then set this one as the smallest
                            overlap = o;                            
                            smallest = axis;
                            //smallest.x *= -1;
                            //smallest.y *= -1;
                        }
                    }
                }
//                cout << "-------" << std::endl;
                // loop over the axes2
                for (int i = 0; i < axes2.size(); i++) {
                    Axis axis = axes2[i];
                    // project both shapes onto the axis
                    Projection p1 = (*this).project(axis);
                    Projection p2 = other.project(axis);
                    //cout << p1.x << ", " << p1.y << " | " << p2.x << ", " << p2.y;//std::endl;
                    // do the projections overlap?
                    if (!p1.overlap(p2)) {
                        // then we can guarantee that the shapes do not overlap
                        return false;
                    }
                    else {
                        // get the overlap
                        double o = p1.getOverlap(p2);
                        // check for minimum
//                        cout << " W[" << o << "]," << (o < overlap ? "T" : "U") << " <" << axis.x << ", " << axis.y << ">" << std::endl;
                        if (o < overlap) {
                            // then set this one as the smallest
                            overlap = o;
                            smallest = axis;

                        }
                    }
                }
                //cout << "<<<<<<<<<" << std::endl;
                mtv = MTV(smallest, overlap);
                // if we get here then we know that every axis had overlap on it
                // so we can guarantee an intersection
                return true;
            }
        };
    }
}