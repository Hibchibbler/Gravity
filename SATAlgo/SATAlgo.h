///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>
#include <vector>
#include "Vector\Vector2.h"
#include "GravityPoC\TileLayer.h"
#include <iostream>

namespace bali
{
    namespace SAT
    {
        class Axis : public vec::VECTOR2
        {
        public:
            Axis()
                : VECTOR2()
            {

            }

            Axis(float x, float y)
                : VECTOR2(x,y)
            {

            }

            Axis(const vec::VECTOR2 & vec)
                : VECTOR2(vec)
            {

            }

            Axis operator=(const Axis & a)
            {
                if (this != &a)
                {
                    x = a.x;
                    y = a.y;
                    edge = a.edge;
                }
                return *this;
            }


            vec::VECTOR2 edge;
        };
        //typedef vec::Vector2 Axis;
        typedef std::vector<Axis> Axes;


        

        class MTV
        {
        public:
            MTV() {}
            MTV(const Axis & smallest, double overlap)
            {
                this->smallest = smallest;
                this->overlap = overlap;
            }
            MTV(const MTV & mtv)
            {
                this->smallest = mtv.smallest;
                this->overlap = mtv.overlap;
            }

            Axis smallest;
            double overlap;
        };

        class Projection : public vec::VECTOR2
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
                return abs(M-m);
            }
        };

        #define TI_PLAYER -1


        class Shape
        {
        public:
            std::vector<vec::VECTOR2> vertices;
            uint32_t ti;
            double offsetX, offsetY;
            float mass;

            Shape()
            {
                ti = UINT32_MAX;
                offsetX = offsetY = 0;
                mass = 1;
            }
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
                vertices.push_back(vec::VECTOR2(x, y));
            }
            Axes getAxes()
            {
                Axes axes;// = new Vector2[vertices.size()];
                // loop over the vertices
                for (int i = 0; i < vertices.size(); i++) 
                {
                    // get the current vertex
                    vec::VECTOR2 p1 = vertices[i];

                    // get the next vertex
                    vec::VECTOR2 p2 = vertices[i + 1 == vertices.size() ? 0 : i + 1];

                    // subtract the two to get the edge vector
                    vec::VECTOR2 edge = p2 - p1;//.subtract(p1);

                    // get either normal vector
                    vec::VECTOR2 normal = edge.normal();//.normal();
                    normal = normal.norm();

                    // the perp method is just (x, y) => (-y, x) or (y, -x)
                    Axis axis;
                    axis = normal;
                    //axis.edge = edge.translate(vec::Vector2(offsetX, offsetY));
                    axis.edge += vec::VECTOR2(offsetX, offsetY);
                    
                    axes.push_back(axis);
                }
                return axes;
            }

            Projection project(const Axis & axis)
            {
                float min = axis.dot(vertices[0]);
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
            bool MootPoint(vec::VECTOR2 pVel, Axis axis, bali::Tile & tile)
            {
                //std::cout << " ,.-" << axis.x << ", " << axis.y << "-.," << std::endl;


                if (axis.x == 1)
                {
                    if (tile.w)
                    {
                       std::cout << "-w-";
                        return true;
                    }
                }
                if (axis.x == -1)
                {
                    if (tile.e)
                    {
                        std::cout << "-e-";
                        return true;
                    }
                }
                if (axis.y == 1)
                {
                    if (tile.n)
                    {
                        std::cout << "-n-";
                        return true;
                    }
                }
                if (axis.y == -1)
                {
                    if (tile.s)
                    {
                        std::cout << "-s-";
                        return true;
                    }
                }
                return false;
            }

            bool collision(Shape & other, MTV & mtv, bali::TileLayer & tileLayer)
            {
                double overlap = 999999999.0;// really large value;
                Axis smallest;
                Axes axes1 = (*this).getAxes();
                Axes axes2 = other.getAxes();
                bool collided = false;
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
                        // if this collision normal points to an adjacent tile, ignore it.
                        
                        
                        //if (tileLayer[other.ti].
                        // get the overlap
                        double o = p1.getOverlap(p2);
                        //cout << " P[" << o << "],"<< (o < overlap ? "T" : "U")<<" <" << axis.x << ", " << axis.y << ">" << std::endl;
                        // check for minimum
                        if (o <= overlap) {
                            //if (velocity.norm().dot(vec::VECTOR2(axis.x, axis.y)) > 0.0)
                            //    continue;
                            //if (MootPoint(axis, tileLayer[other.ti]))
                            //    continue;


                            // then set this one as the smallest
                            overlap = o;
                            smallest = axis;
                            smallest.edge = axis.edge;
                        }
                    }
                }
                //cout << "-------" << std::endl;
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
                        //if (velocity.norm().dot(vec::VECTOR2(axis.x, axis.y)) > 0.0)
                        //    continue;
                        // get the overlap
                        double o = p1.getOverlap(p2);
                        // check for minimum
                        //cout << " W[" << o << "]," << (o < overlap ? "T" : "U") << " <" << axis.x << ", " << axis.y << ">" << std::endl;
                        if (o <= overlap) {
                            //if (MootPoint(axis, tileLayer[other.ti]))
                            //    continue;
                            // then set this one as the smallest
                            overlap = o;
                            smallest = axis;
                            smallest.edge = axis.edge;
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