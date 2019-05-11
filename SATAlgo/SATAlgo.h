///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////

#ifndef SATAlgo_H_
#define SATAlgo_H_

#include <stdint.h>
#include <vector>
#include <iostream>
#include <sstream>

#include <SFML\System.hpp>
#include "GravityCommon\Geometry.h"
#include "GravityCommon\Vector2.h"

namespace bali
{
namespace SAT
{

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


class Collision
{
public:
    static Projection project(Shape & shape, const ContactInfo & axis)
    {
        sf::Vector2f spos = shape.position;
        sf::Vector2f sorg = shape.origin;
        size_t numpts = shape.points.size();
        float min = vec::dot(axis.normal, shape.points[0] + (spos - sorg));
        float max = min;

        for (int i = 1; i < numpts; i++) {
            sf::Vector2f pean = shape.points[i] + (spos - sorg);

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

    static bool ismtvvalid(Shape & shape, Shape & other, sf::Vector2f mtv)
    {
        vec::VECTOR2 c1, c2;
        vec::VECTOR2 dir;
        for (int u = 0; u < shape.points.size(); u++)
        {
            c1 = c1 + (shape.points[u] + shape.position);
        }
        c1 = c1 / (float)shape.points.size();

        for (int u = 0; u < other.points.size(); u++)
        {
            c2 = c2 + (other.points[u] + other.position);
        }
        c2 = c2 / (float)other.points.size();

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


    static bool iscollided(
        Shape & shape,
        Shape & other,
        std::vector<ContactInfo> & hitInfo
    )
    {
        //hitInfo.clear();

        std::vector<ContactInfo> axes1 = shape.edges;
        std::vector<ContactInfo> axes2 = other.edges;

        // loop over the axes1
        //cout << ">>>>>>>>>" << std::endl;
        std::stringstream ss;
        float minimumOverlap1 = 999999999.0f;
        ContactInfo mtv;
        for (int i = 0; i < shape.edges.size(); i++)
        {
            ContactInfo axis = shape.edges[i];

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
                        mtv.isinternal = axis.isinternal;
                        mtv.normal = axis.normal;
                        mtv.overlap = o;
                        mtv.p1 = axis.p1;
                        mtv.p2 = axis.p2;
                        minimumOverlap1 = o;
                        //std::cout << " W[" << o << "]," << " G[" << minimumOverlap1 << "]," << std::endl; //<< (o < overlap ? "T" : "U") << " <" << axis.x << ", " << axis.y << ">" << std::endl;
                    }
                }

            }
        }
        //cout << "-------" << std::endl;
        // loop over the axes2
        for (int i = 0; i < other.edges.size(); i++)
        {
            ContactInfo axis = other.edges[i];

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
                        mtv.isinternal = axis.isinternal;
                        mtv.normal = axis.normal;
                        mtv.overlap = o;
                        mtv.p1 = axis.p1;
                        mtv.p2 = axis.p2;
                        minimumOverlap1 = o;
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
    }
};
}
}

#endif
