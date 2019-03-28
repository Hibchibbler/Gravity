///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>
#include <sstream>

#include <SFML\System.hpp>
#include <SFML\Graphics\ConvexShape.hpp>
#include "GravityCommon\Vector2.h"

namespace bali
{
namespace SAT
{
class Segment
{
public:
    typedef std::vector<Segment> Vec;
    Segment()
    {
        start = sf::Vector2f(0.0, 0.0);
        end = sf::Vector2f(0.0, 0.0);
        off = sf::Vector2f(0.0, 0.0);
    }

    Segment(sf::Vector2f _off, sf::Vector2f p1, sf::Vector2f p2)//float offx, float offy, float x1, float y1, float x2, float y2)
    {
        off.x = _off.x;
        off.y = _off.y;

        start.x = p1.x;
        start.y = p1.y;

        end.x = p2.x;
        end.y = p2.y;
    }
    bool isEqual(Segment & other)
    {
        if (start.x == other.start.x &&
            start.y == other.start.y &&
            end.x == other.end.x &&
            end.y == other.end.y)
        {
            return true;
        }
        return false;
    }

    float getSlope()
    {
        return (end.y - start.y) / (end.x - start.x);
    }

    sf::Vector2f off;
    sf::Vector2f start;
    sf::Vector2f end;
};

class ContactInfo 
{
public:
    typedef std::vector<ContactInfo> Vec;
    ContactInfo()
    {
        normal = sf::Vector2f(0.0f, 0.0f);
        overlap = 0.0f;
    }

    ContactInfo(float nx, float ny, float o)
    {
        normal.x = nx;
        normal.y = ny;
        overlap = o;
    }

    ContactInfo(const sf::Vector2f & n, float o)
    {
        normal = n;
        overlap = o;
    }

    ContactInfo(const ContactInfo & ci)

    {
        *this = ci;
    }

    sf::Vector2f normal;
    Segment seg;
    float overlap;
    int32_t thisindex;
    int32_t thatindex;
};

typedef std::vector<ContactInfo> Axes;

class Projection : public sf::Vector2f
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
        return (a >= b ? a : b);
    }

    float min(float a, float b)
    {
        return (a <= b ? a : b);
    }
    bool overlap(const Projection & p)
    {
        float aMin = min(x, y);
        float aMax = max(x, y);

        float bMin = min(p.x, p.y);
        float bMax = max(p.x, p.y);

        if (aMax <= bMin || bMax <= aMin)
            return false;
        return true;
    }
    float getOverlap(const Projection & p)
    {
        float M, m;
        m = max(x, p.x);
        M = min(y, p.y);
        return abs(M-m);
    }

    bool contains(const Projection & p)
    {
        bool s = false;
        //if (x > min(p.x, p.y) && x < max(p.x, p.y) &&
        //    y > min(p.x, p.y) && y < max(p.x, p.y))
        //{
        //    s = true;
        //}
        return s;
    }
};


class Shape
{
public:

    //static Segment::Vec getSegments(sf::ConvexShape & shape)
    //{
    //    Segment::Vec segments;
    //    size_t pc = shape.getPointCount();
    //    for (int i = 0; i < pc; ++i)
    //    {
    //        // get the current vertex
    //        sf::Vector2f p1 = shape.getPoint(i);// +shape.getPosition();

    //        // get the next vertex
    //        sf::Vector2f p2 = shape.getPoint(i + 1 == pc ? 0 : i + 1);// +shape.getPosition();

    //        segments.push_back(Segment(shape.getPosition(), p1, p2));
    //    }
    //    return segments;
    //}
    static Axes getSeparatingAxes(sf::ConvexShape & shape)
    {
        Axes axes;
        // loop over the vertices
        size_t pc = shape.getPointCount();
        axes.reserve(pc);
        for (int i = 0; i < pc; i++)
        {
            // get the current vertex
            sf::Vector2f p1 = shape.getPoint(i);

            // get the next vertex
            sf::Vector2f p2 = shape.getPoint(i + 1 == pc ? 0 : i + 1);

            // subtract the two to get the edge vector
            sf::Vector2f edge = p2 - p1;//.subtract(p1);

            // get either normal vector
            sf::Vector2f normal = vec::norm(vec::normal(edge));

            // the perp method is just (x, y) => (-y, x) or (y, -x)
            ContactInfo axis;
            axis.normal = normal;
            axis.seg.start = p1;// +shape.getPosition();
            axis.seg.end = p2;// +shape.getPosition();
            axis.seg.off = shape.getPosition();
            axes.push_back(axis);
        }
        return axes;
    }

    static Projection project(sf::ConvexShape & shape, const ContactInfo & axis)
    {
        sf::Vector2f spos = shape.getPosition();
        sf::Vector2f sorg = shape.getOrigin();
        size_t numpts = shape.getPointCount();
        float min = vec::dot(axis.normal, shape.getPoint(0) + (spos - sorg));
        float max = min;

        for (int i = 1; i < numpts; i++) {
            sf::Vector2f pean = shape.getPoint(i) + (spos - sorg);

            float p = vec::dot(axis.normal, pean);
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

    static bool ismtvvalid(sf::ConvexShape & shape, sf::ConvexShape & other, sf::Vector2f mtv)
    {
        vec::VECTOR2 c1, c2;
        vec::VECTOR2 dir;
        for (int u = 0; u < shape.getPointCount(); u++)
        {
            c1 = c1 + (shape.getPoint(u) + shape.getPosition());
        }
        c1 = c1 / (float)shape.getPointCount();

        for (int u = 0; u < other.getPointCount(); u++)
        {
            c2 = c2 + (other.getPoint(u) + other.getPosition());
        }
        c2 = c2 / (float)other.getPointCount();

        dir = c2 - c1;
        // HACK
        if (vec::mag(dir) < 0.001f) 
        {
            dir = vec::VECTOR2((rand() % 10) + 1.f, (rand() % 10) + 1.f);
//            std::cout << "what?";
        }
        dir = vec::norm(dir);//dir.norm();
        float jah = vec::dot(dir,mtv*3.0f);
        if (jah < 0.f)
            return true;
        return false;
    }

    static bool collision(
        sf::ConvexShape & shape,
        sf::ConvexShape & other,
        std::vector<ContactInfo> & hitInfo
    )
    {
        //hitInfo.clear();

        Axes axes1 = getSeparatingAxes(shape);
        Axes axes2 = getSeparatingAxes(other);

        // loop over the axes1
        //cout << ">>>>>>>>>" << std::endl;
        std::stringstream ss;
        float minimumOverlap1 = 999999999.0f;
        ContactInfo mtv;
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
                float o = p1.getOverlap(p2);
                if (o < minimumOverlap1)
                {
                    if (ismtvvalid(shape, other, axis.normal))
                    {
                        mtv.normal = axis.normal;
                        mtv.overlap = o;
                        mtv.seg = axis.seg;
                        minimumOverlap1 = o;
                        axis.overlap = o;
                        //if (o > 0.0f)
                        //{
                        //    //hitInfo.push_back(ContactInfo(axis));
                        //}
                        //std::cout << " W[" << o << "]," << " G[" << minimumOverlap1 << "]," << std::endl; //<< (o < overlap ? "T" : "U") << " <" << axis.x << ", " << axis.y << ">" << std::endl;
                    }
                }

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
                float o = p2.getOverlap(p1);
                if (o < minimumOverlap1)
                {
                    if (ismtvvalid(shape, other, axis.normal))
                    {
                        mtv.normal = axis.normal;
                        mtv.overlap = o;
                        mtv.seg = axis.seg;
                        minimumOverlap1 = o;
                        axis.overlap = o;
                        //if (o > 0.0f)
                        //{
                        //    //hitInfo.push_back(ContactInfo(axis));
                        //}
                        //std::cout << " W[" << o << "]," << " G[" << minimumOverlap1 << "]," << std::endl;
                    }
                }
            }
        }
        // if we get here then we know that every axis had overlap on it
        // so we can guarantee an intersection
        if (mtv.overlap > 0.0f)
        {
            hitInfo.push_back(ContactInfo(mtv));
            return true;
        }
        return false;
        //return true;
    }
};
}
}
