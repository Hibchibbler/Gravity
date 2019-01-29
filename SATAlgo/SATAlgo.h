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
//#include "GravityCommon/Polygons.h"


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
    Segment edge;
    float overlap;
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
        if (x > min(p.x, p.y) && x < max(p.x, p.y) ||
            y > min(p.x, p.y) && y < max(p.x, p.y))
        {
            return true;
        }
        return false;
        //return (y >= x) ? 
        //            (((x <= p.x) && (p.x <= y)) || 
        //             ((x <= p.y) && (p.y <= y)) ) : 
        //             (((y <= p.x) && (p.x <= x)) || ((y <= p.y) && (p.y <= x)));
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

    static Segment::Vec getSegments(bali::CONVEXSHAPE & shape)
    {
        Segment::Vec segments;
        size_t pc = shape.getPointCount();
        for (int i = 0; i < pc; ++i)
        {
            // get the current vertex
            sf::Vector2f p1 = shape.getPoint(i);// +shape.getPosition();

            // get the next vertex
            sf::Vector2f p2 = shape.getPoint(i + 1 == pc ? 0 : i + 1);// +shape.getPosition();

            segments.push_back(Segment(shape.getPosition(), p1, p2));
        }
        return segments;
    }
    static Axes getSeparatingAxes(bali::CONVEXSHAPE & shape)
    {
        Axes axes;
        // loop over the vertices
        size_t pc = shape.getPointCount();
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
            axis.edge.start = p1;// +shape.getPosition();
            axis.edge.end = p2;// +shape.getPosition();
            axis.edge.off = shape.getOrigin();
            axes.push_back(axis);
        }
        return axes;
    }

    static Projection project(bali::CONVEXSHAPE & shape, const ContactInfo & axis)
    {
        float min = vec::dot(axis.normal, shape.getPoint(0) + (shape.getPosition() - shape.getOrigin()));
        //float min = vec::dot(axis.normal, shape.getPoint(0));
        float max = min;

        for (int i = 1; i < shape.getPointCount(); i++) {
            sf::Vector2f pean = shape.getPoint(i) + (shape.getPosition() - shape.getOrigin());

            float p = vec::dot(axis.normal, pean);
            //double p = vec::dot(axis.normal, shape.getPoint(i));
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

    static bool ismtvvalid(bali::CONVEXSHAPE & shape, bali::CONVEXSHAPE & other, vec::VECTOR2 mtv)
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
        dir = vec::norm(dir);//dir.norm();
        float jah = vec::dot(dir,mtv);
        if (jah < 0.f)
            return true;
        return false;
        //std::cout << dist1 << "..." << dist2<< std::endl;

        //std::cout << "[" << dist1 << ", " << dist2 << ", " << mtv.magnitude() << "] ";
        //std::cout << "(";
        //std::cout << c1.x;
        //std::cout << ", ";
        //std::cout << c1.y;
        //std::cout << ",,";
        //std::cout << c2.x;
        //std::cout << ", ";
        //std::cout << c2.y;
        //std::cout << " = ";
        //std::cout << dist1 << ", = ";
        //std::cout << dist2 << ")";

        //std::cout << "{" << dist1 << ", " << dist2 << ", " << mtv.magnitude() << "}";
        //if (dist1 < dist2) {
        //    return true;
        //}
        //return false;

    }

    static bool collision(
        bali::CONVEXSHAPE & shape,
        bali::CONVEXSHAPE & other,
        std::vector<ContactInfo> & hitInfo
    )
    {
        hitInfo.clear();

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
                        minimumOverlap1 = o;
                        axis.overlap = o;
                        //hitInfo.push_back(ContactInfo(axis));
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
                        minimumOverlap1 = o;
                        axis.overlap = o;
                        //hitInfo.push_back(ContactInfo(axis));
                        //std::cout << " W[" << o << "]," << " G[" << minimumOverlap1 << "]," << std::endl;
                    }
                }
            }
        }
        //cout << "<<<<<<<<<" << std::endl;
        // if we get here then we know that every axis had overlap on it
        // so we can guarantee an intersection
        //std::cout << ss.str();
        hitInfo.push_back(mtv);
        return true;
    }
};
}
}
