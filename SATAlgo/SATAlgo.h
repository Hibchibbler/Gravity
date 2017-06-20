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
#include <sstream>

namespace bali
{
    namespace SAT
    {
        class Segment
        {
        public:
            Segment()
            {
                start = vec::VECTOR2(0.0, 0.0);
                end = vec::VECTOR2(0.0, 0.0);
            }

            Segment(float x1, float y1, float x2, float y2)
            {
                start.x = x1;
                start.y = y1;

                end.x = x2;
                end.y = y2;
            }
            vec::VECTOR2 start;
            vec::VECTOR2 end;
        };


        class ContactInfo 
        {
        public:
            ContactInfo()
            {
                normal = vec::VECTOR2(0.0f, 0.0f);
                overlap = 0.0f;
            }

            ContactInfo(float nx, float ny, float o)
            {
                normal.x = nx;
                normal.y = ny;
                overlap = o;
            }

            ContactInfo(const vec::VECTOR2 & n, float o)
            {
                normal = n;
                overlap = o;
            }

            ContactInfo(const ContactInfo & ci)

            {
                *this = ci;
            }

            //ContactInfo operator=(const ContactInfo & ci)
            //{
            //    if (this != &ci)
            //    {
            //        *this = ci;
            //        /*normal.x = a.x;
            //        normal.y = a.y;
            //        edge.start = a.edge.start;
            //        edge.end = a.edge.end;*/
            //    }
            //    return *this;
            //}

            vec::VECTOR2 normal;
            Segment edge;
            float overlap;
        };
        //typedef vec::Vector2 Axis;
        typedef std::vector<ContactInfo> Axes;
                

        //class MTV
        //{
        //public:
        //    MTV() {}
        //    MTV(const ContactInfo & smallest, double overlap)
        //    {
        //        this->smallest = smallest;
        //        this->overlap = overlap;
        //    }
        //    MTV(const MTV & mtv)
        //    {
        //        this->smallest = mtv.smallest;
        //        this->overlap = mtv.overlap;
        //    }

        //    Axis smallest;
        //    double overlap;
        //};

        //class HitInfo
        //{
        //public:
        //    MTV mtv;
        //};

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
                //vec::VECTOR2 min1 = VECTOR2(min(this->x, p.x), min(0,0));
                //vec::VECTOR2 max1 = VECTOR2(max(this->x, p.x), max(0, 0));

                //vec::VECTOR2 min2 = VECTOR2(min(this->y, p.y), min(0, 0));
                //vec::VECTOR2 max2 = VECTOR2(max(this->y, p.y), max(0, 0));


                //vec::VECTOR2 minInt = VECTOR2(max(min1.x,min2.x), max(0,0));
                //vec::VECTOR2 maxInt = VECTOR2(min(max1.x, max2.x), min(0,0));


                //if (minInt.x < maxInt.x && minInt.y < maxInt.y)
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

        //class Ray
        //{
        //public:
        //    Ray(float b_, float x_, float t0_=0.0f)
        //        :   b(b_),
        //            x(x_),
        //            t0(t0_)
        //    {

        //    }

        //    float f(float t)
        //    {
        //        return (b + (x * (t0+t)));
        //    }

        //    bool intersect()
        //    {
        //        // mx+b = mx+bc
        //    }
        //    float b, x, t0;
        //    // y = b + x(t0+t)
        //};

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
            std::vector<Segment> getSegments()
            {
                std::vector<Segment> segments;
                //if (vertices.size() > 1)
                ///{
                    for (int i = 0; i < vertices.size(); ++i)
                    {
                        // get the current vertex
                        vec::VECTOR2 p1 = vertices[i];

                        // get the next vertex
                        vec::VECTOR2 p2 = vertices[i + 1 == vertices.size() ? 0 : i + 1];

                        segments.push_back(Segment(p1.x, p1.y, p2.x, p2.y));
                    }
                //}
                return segments;
            }
            Axes getContactInfo()
            {
                Axes axes;
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
                    ContactInfo axis;
                    axis.normal = normal;
                    axis.edge.start = p1;
                    axis.edge.end = p2;
                    
                    axes.push_back(axis);
                }
                return axes;
            }

            Projection project(const ContactInfo & axis)
            {
                float min = axis.normal.dot(vertices[0]);
                double max = min;
                for (int i = 1; i < vertices.size(); i++) {
                    // NOTE: the axis must be normalized to get accurate projections
                    double p = axis.normal.dot(vertices[i]);
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

            bool collision(vec::VECTOR2 position, vec::VECTOR2 vel, Shape & other, ContactInfo & mtv, std::vector<ContactInfo> & hitInfo)
            {
                double overlap = 999999999.0;// really large value;
                ContactInfo smallest;
                Axes axes1 = (*this).getContactInfo();
                Axes axes2 = other.getContactInfo();
                bool collided = false;
                float smallestLen = 9999999.0;//
                // loop over the axes1
                //cout << ">>>>>>>>>" << std::endl;
                std::stringstream ss;
                for (int i = 0; i < axes1.size(); i++) 
                {
                    ContactInfo axis = axes1[i];
                    // project both shapes onto the axis
                    Projection p1 = (*this).project(axis);
                    Projection p2 = other.project(axis);
                    //cout << p1.x << ", " << p1.y << " | " << p2.x << ", " << p2.y;// std::endl;
                    // do the projections overlap?
                    if (!p1.overlap(p2)) 
                    {
                        // then we can guarantee that the shapes do not overlap
                        return false;
                    }
                    else 
                    {
                        // if this collision normal points to an adjacent tile, ignore it.
                        // get the overlap
                        double o = p1.getOverlap(p2);
                        axis.overlap = o;
                        //cout << " P[" << o << "],"<< (o < overlap ? "T" : "U")<<" <" << axis.x << ", " << axis.y << ">" << std::endl;
                        // check for minimum
                        hitInfo.push_back(ContactInfo(axis));

                        if (o <= overlap) 
                        {
                            // then set this one as the smallest
                            smallest = axis;
                        }
                    }
                }
                //cout << "-------" << std::endl;
                // loop over the axes2
                for (int i = 0; i < axes2.size(); i++) 
                {
                    ContactInfo axis = axes2[i];
                    // project both shapes onto the axis
                    Projection p1 = (*this).project(axis);
                    Projection p2 = other.project(axis);
                    //cout << p1.x << ", " << p1.y << " | " << p2.x << ", " << p2.y;//std::endl;
                    // do the projections overlap?
                    if (!p1.overlap(p2)) 
                    {
                        // then we can guarantee that the shapes do not overlap
                        return false;
                    }
                    else 
                    {
                        // get the overlap
                        double o = p1.getOverlap(p2);
                        axis.overlap = o;
                        // check for minimum
                        //cout << " W[" << o << "]," << (o < overlap ? "T" : "U") << " <" << axis.x << ", " << axis.y << ">" << std::endl;
                        hitInfo.push_back(ContactInfo(axis));

                        if (o <= overlap) 
                        {
                            smallest = axis;
                        }
                    }
                }
                //cout << "<<<<<<<<<" << std::endl;
                mtv = smallest;
                // if we get here then we know that every axis had overlap on it
                // so we can guarantee an intersection
                std::cout << ss.str();
                return true;
            }
        };
    }
}