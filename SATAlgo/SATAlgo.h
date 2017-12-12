///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>
#include <sstream>
#include "GravityCommon/Polygons.h"

namespace bali
{
namespace SAT
{

class ContactInfo 
{
public:
    typedef std::vector<ContactInfo> Vec;
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

    vec::VECTOR2 normal;
    Segment edge;
    float overlap;
};

typedef std::vector<ContactInfo> Axes;

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

class Shape
{
public:

    static Segment::Vec getSegments(bali::CONVEXSHAPE & shape)
    {
        Segment::Vec segments;
        size_t pc = shape.getPointCount();
        for (int i = 0; i < pc; ++i)
        {
            // get the current vertex
            vec::VECTOR2 p1 = shape.getPoint(i);// +shape.getPosition();

            // get the next vertex
            vec::VECTOR2 p2 = shape.getPoint(i + 1 == pc ? 0 : i + 1);// +shape.getPosition();

            segments.push_back(Segment(shape.getPosition(), p1, p2));
        }
        return segments;
    }
    static Axes getContactInfo(bali::CONVEXSHAPE & shape)
    {
        Axes axes;
        // loop over the vertices
        size_t pc = shape.getPointCount();
        for (int i = 0; i < pc; i++)
        {
            // get the current vertex
            vec::VECTOR2 p1 = shape.getPoint(i);

            // get the next vertex
            vec::VECTOR2 p2 = shape.getPoint(i + 1 == pc ? 0 : i + 1);

            // subtract the two to get the edge vector
            vec::VECTOR2 edge = p2 - p1;//.subtract(p1);

            // get either normal vector
            vec::VECTOR2 normal = vec::norm(vec::normal(edge));

            // the perp method is just (x, y) => (-y, x) or (y, -x)
            ContactInfo axis;
            axis.normal = normal;
            axis.edge.start = p1;// +shape.getPosition();
            axis.edge.end = p2;// +shape.getPosition();
                    
            axes.push_back(axis);
        }
        return axes;
    }

    static Projection project(bali::CONVEXSHAPE & shape, const ContactInfo & axis)
    {
        float min = vec::dot(axis.normal, shape.getPoint(0) + shape.getPosition() - shape.getOrigin());
        double max = min;

        for (int i = 1; i < shape.getPointCount(); i++) {
            double p = vec::dot(axis.normal, shape.getPoint(i)+(shape.getPosition() - shape.getOrigin()));
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

    static bool collision(bali::CONVEXSHAPE & shape, bali::CONVEXSHAPE & other, std::vector<ContactInfo> & hitInfo)
    {
        hitInfo.clear();
        Axes axes1 = getContactInfo(shape);
        Axes axes2 = getContactInfo(other);
        // loop over the axes1
        //cout << ">>>>>>>>>" << std::endl;
        std::stringstream ss;
        for (int i = 0; i < axes1.size(); i++) 
        {
            ContactInfo axis = axes1[i];
            // project both shapes onto the axis
            Projection p1 = project(shape, axis);
            Projection p2 = project(other, axis);
            //cout << p1.x << ", " << p1.y << " | " << p2.x << ", " << p2.y;// std::endl;
            // do the projections overlap?
            if (!p1.overlap(p2)) 
            {
                // then we can guarantee that the shapes do not overlap
                return false;
            }
            else 
            {
                // get the overlap, and store it
                double o = p1.getOverlap(p2);
                axis.overlap = o;
                //cout << " P[" << o << "],"<< (o < overlap ? "T" : "U")<<" <" << axis.x << ", " << axis.y << ">" << std::endl;
                hitInfo.push_back(ContactInfo(axis));
            }
        }
        //cout << "-------" << std::endl;
        // loop over the axes2
        for (int i = 0; i < axes2.size(); i++) 
        {
            ContactInfo axis = axes2[i];
            // project both shapes onto the axis
            Projection p1 = project(shape, axis);
            Projection p2 = project(other, axis);
            //cout << p1.x << ", " << p1.y << " | " << p2.x << ", " << p2.y;//std::endl;
            // do the projections overlap?
            if (!p1.overlap(p2)) 
            {
                // then we can guarantee that the shapes do not overlap
                return false;
            }
            else 
            {
                // get the overlap, and store it
                double o = p1.getOverlap(p2);
                axis.overlap = o;
                //cout << " W[" << o << "]," << (o < overlap ? "T" : "U") << " <" << axis.x << ", " << axis.y << ">" << std::endl;
                hitInfo.push_back(ContactInfo(axis));
            }
        }
        //cout << "<<<<<<<<<" << std::endl;
        // if we get here then we know that every axis had overlap on it
        // so we can guarantee an intersection
        std::cout << ss.str();
        return true;
    }
};
}
}