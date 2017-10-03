///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utility.h"
#include <list>
#include <sstream>
#include <iomanip>

#ifdef PHYSICS_DEBUG
#define PHYSPRINT(x) \
    std::cout << x << std::endl;
#endif
#define EPSILON 0.0000001f
namespace bali
{
    namespace physics
    {
#ifdef _DEBUG
        const int LEFTUP = 0;
        const int h = 15;
        const int GK = 20;
        const float FIXED_DELTA = 1.0f / 200.0f;
        const float DRAG_CONSTANT = 0.4;
        const int SKIP_FRAMES = 14;
        const float RESTITUTION = 0.9f;
        const float TOO_STEEP = -0.69f;
#else
        const int LEFTUP = 0;
        const int h = 75;
        const int GK = 1000;
        const float FIXED_DELTA = 1.0f / 375.0f;
        const float DRAG_CONSTANT = 0.9;
        const int SKIP_FRAMES = 25;
        const float RESTITUTION =0.05f;
        const float TOO_STEEP  = -1.0f;
#endif
        class Intersection
        {
        public:
            Intersection()
            {
                time = 0.0f;
                rayPoint = vec::VECTOR2(0.0f, 0.0f);
                segPoint = vec::VECTOR2(0.0f, 0.0f);
                angle = 0;
                expired = true;
                segment = SAT::Segment(0, 0, 0, 0);
            }
            vec::VECTOR2 distance;
            float time;
            vec::VECTOR2 ray;
            vec::VECTOR2 rayPoint;
            vec::VECTOR2 segPoint;
            float angle;
            bool expired;//did the ray get to the end without hitting anything?
            SAT::Segment segment;
        };

        class Ray
        {
        public:

            float getPoint(float time)
            {

            }

            vec::VECTOR2 offset;
            vec::VECTOR2 delta;
        };

        static vec::VECTOR2 upVector(float angle)
        {
            float a = DEG_TO_RAD(angle - 90);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        static vec::VECTOR2 leftVector(float angle)
        {
            //float a = DEG_TO_RAD(angle - 180 - LEFTUP);
            float a = DEG_TO_RAD(angle - 180);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        static vec::VECTOR2 downVector(float angle)
        {
            float a = DEG_TO_RAD(angle - 270);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        static vec::VECTOR2 rightVector(float angle)
        {
            //float a = DEG_TO_RAD(angle - 360 + LEFTUP);
            float a = DEG_TO_RAD(angle - 360);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }
        
        // Find intersection of RAY & SEGMENT
        bool getIntersection(vec::VECTOR2 rayStart, vec::VECTOR2 ray,SAT::Segment segment, Intersection & intersection)
        {
            // RAY in parametric: Point + Delta*T1
            float r_px = rayStart.x;
            float r_py = rayStart.y;
            float r_dx = ray.x;
            float r_dy = ray.y;
            // SEGMENT in parametric: Point + Delta*T2
            float s_px = segment.start.x;
            float s_py = segment.start.y;
            float s_dx = segment.end.x-segment.start.x;
            float s_dy = segment.end.y-segment.start.y;
            // Are they parallel? If so, no intersect
            float r_mag = sqrt(r_dx*r_dx+r_dy*r_dy);
            float s_mag = sqrt(s_dx*s_dx+s_dy*s_dy);

            if(r_dx/r_mag==s_dx/s_mag && r_dy/r_mag==s_dy/s_mag){
                // Unit vectors are the same.
                return false;
            }
            // SOLVE FOR T1 & T2
            float det = (s_dx*r_dy - s_dy*r_dx);
            float T2 = (r_dx*(s_py - r_py) + r_dy*(r_px - s_px)) / det;
            float T1 = (s_px + s_dx*T2 - r_px) / r_dx;

            // Must be a piece of the ray that is moving away from the origin
            if (T1 < 0)         return false;
            // Must be within the domain of the segment
            if (T2 < 0 || T2>1) return false;

            // Return Intersection information
            intersection.time = T1;
            intersection.rayPoint = vec::VECTOR2(r_px + r_dx*T1, r_py + r_dy*T1);
            intersection.segPoint = vec::VECTOR2(s_px + s_dx*T2, s_py + s_dy*T2);
            intersection.segment = segment;
            intersection.distance = rayStart - intersection.segPoint;
          
            return true;
        }

        bool RayCast(vec::VECTOR2 dir, vec::VECTOR2 start, std::vector<SAT::Segment> & segments, physics::Intersection & intersect)
        {
            bool found = false;          
            dir = dir / 8.0;// This is important. is it?
            ///PHYSPRINT("[Ang](" << angle << ")");
            float closest = 999999.9;
            for (auto seg = segments.begin(); seg != segments.end(); ++seg)
            {
                Intersection intersectTemp;
                intersectTemp.expired = true;
                intersectTemp.rayPoint = start + dir * 3400;
                intersectTemp.distance = intersectTemp.rayPoint - start;
                intersectTemp.ray = dir;
                getIntersection(start, dir, *seg, intersectTemp);
                if (intersectTemp.distance.mag() <= closest)
                {
                    closest = intersectTemp.distance.mag();
                    intersect = intersectTemp;
                    intersect.expired = false;
                    found = true;
                }
            }
            return found;
        }

        bool RayCast(float a, vec::VECTOR2 start, std::vector<SAT::Segment> & segments, physics::Intersection & intersect)
        {
            float radians = a * (PI / 180.0f);

            vec::VECTOR2 dir(1.0f*cos(radians),
                             1.0f*sin(radians));
            if (fabs(dir.x) < 0.0001)
                dir.x = 0.0f;

            if (fabs(dir.y) < 0.0001)
                dir.y = 0.0f;
            return RayCast(dir, start, segments, intersect);
        }

        void createLoSTriFan(std::vector<SAT::Shape> & shapes, GameContext* ctx)
        {
            // We will shoot a ray for each degree of rotation around position.
            std::vector<SAT::Segment> segments;
            for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
            {
                std::vector<SAT::Segment> s = shape->getSegments();
                segments.insert(segments.end(), s.begin(), s.end());
            }
            {
                //angle - starts at 
                //   3 O'  Clock = 0   degree
                //   6 O'  Clock = 90  degree
                //   9 O'  Clock = 180 degree
                //   12 O' Clock = 270 degree
                //   3 O'  Clock = 360 degree

                std::vector<physics::Intersection> intersections;
                //float a = 270;
                //for (float a = 89.9; a < 91; a = a + 0.10)
                for (float a = 0.0; a < 370.0; a = a + 0.5)
                {

                    if (a == 90)
                        continue;
                    if (a == 180)
                        continue;
                    if (a == 270)
                        continue;
                    if (a == 360)
                        continue;
                    
                    // A Ray will intersect with an obstacle or the maximum distance (that we cast)
                    // hence, we always will have an intersection..but 
                    // whether it's relevent or not is in the eye of the beholder.
                    Intersection intersectMin;
                    RayCast(a, ctx->player.pos, segments, intersectMin);
                    intersections.push_back(intersectMin);
                }

                ctx->lineSegments.clear();
                ctx->lineSegments = sf::VertexArray(sf::PrimitiveType::TrianglesFan);//Lines);
                vec::VECTOR2 positionFixed = ctx->player.pos;
                ctx->lineSegments.append(positionFixed.sf());
                
                for (auto i = intersections.begin(); i != intersections.end(); ++i)
                {
                    if (true)//!i->expired)
                    {
                        i->rayPoint += i->ray * 400.0f;
                        ctx->lineSegments.append(i->rayPoint.sf());
                    }
                }
            }
        }

        bool ResolveCollisions(std::vector<SAT::Shape> & shapes, std::vector<SAT::Shape> & playerShapes, Player & player)
        {
            bool isCollided = false;

            float a_deg = 180 + player.angle;
            float a = DEG_TO_RAD(a_deg);
            std::list<SAT::ContactInfo> cinfos;
            std::stringstream ss;

            vec::VECTOR2 original_velocity = player.vel;
            double original_magnitude = original_velocity.mag();
            
            ss << std::setprecision(5);
            ss << "[<P " << player.pos.x << ", " << player.pos.y << "> ";
            ss << "<V " << original_velocity.x << ", " << original_velocity.y << "> ";

            for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
            {
                SAT::Shape playerShape = playerShapes.back();
                std::vector<SAT::ContactInfo> hitInfo;

                bool collision = playerShape.collision(player.pos,player.vel,*shape, hitInfo);
                if (collision)
                {
                    float minimumOverlap  = 999999999.0f;
                    //SAT::ContactInfo cinfo;
                    
                    // We want to find the most smallest overlap.
                    // Unfortunately, there may be multiple answers
                    // when the collision polygon has parallel edges,
                    // and when there are multiple overlapping polygons
                    // being hit.
                    for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
                    {
                        if (h->overlap <= minimumOverlap)
                        {
                            minimumOverlap = h->overlap;
                        }
                    }

                    // now only cycle through hits with minimum overlaps
                    // probably only two.
                    // find the one we are approaching
                    for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
                    {
                        if (h->overlap != minimumOverlap)
                            continue;

                        // here, we distinguish between the parallel egdes,
                        // by using the one that is closest.
                        if (original_velocity.dot(h->normal) > 0)
                            continue;

                        cinfos.push_back(*h);
                    }
                }
            }

            //    ss << "<#ofCI " << cinfos.size() << ">, ";
            ss << "<#ofCI " << cinfos.size() << ">, ";

            vec::VECTOR2  newPos(0, 0);
            float overlap = 0.0;

            for (auto cinfo = cinfos.begin(); cinfo != cinfos.end(); ++cinfo)
            {
                // When we collided with multiple edges at once
                // we add all collision normals together and normalize
                newPos += cinfo->normal;
                // When we collided with multiple polygons at once
                // we only want to use the largest overlap
                if (cinfo->overlap > overlap)
                {
                    //overlap = cinfo->overlap/2.0;
                    overlap = cinfo->overlap;
                }

                ss << "<CN " << cinfo->normal.x << ", " << cinfo->normal.y << ">, ";
                ss << "<OVR? " << cinfo->overlap << ">, ";
                ss << "<EDGE (" << cinfo->edge.start.x << ", " << cinfo->edge.start.y << "), ";
                ss << "(" << cinfo->edge.end.x << ", " << cinfo->edge.end.y << "), ";
            }

            

            if (newPos != vec::VECTOR2(0, 0))
            {
                isCollided = true;
                vec::VECTOR2 newVelocity;
                newPos = newPos.norm();
                newVelocity = newPos*(original_velocity.dot(newPos)) * -1 * (1 + RESTITUTION) + original_velocity;

                if (newPos.x != 0)
                { 
                    player.vel.x = newVelocity.x;
                }

                if (newPos.y != 0)
                {
                    player.vel.y = newVelocity.y;
                }

                vec::VECTOR2 posDelta = newPos * overlap * 1.5;
                player.pos += posDelta;

                //float steepness = downVector(player.angle).dot(newPos);
                //if (steepness > TOO_STEEP || original_magnitude > 2000.0)
                //{
                //    ss << "< " << "HARD " << original_magnitude << ">, ";
                //}
                //else
                //{
                //    ss << "< " << "SOFT" << ">, ";
                //    player.vel = vec::VECTOR2(0, 0);
                //    player.acc = vec::VECTOR2(0, 0);
                //}

                ss << "<A " << player.angle << ">";
                ss << "<PD " << posDelta.x << ", " << posDelta.y << "> ";//aggregate new position
                ss << "<NV " << newVelocity.x << ", " << newVelocity.y << "> ";//aggregate new velocity
                ss << "<OVR! " << overlap << ">, ";
                //ss << "<STEEP " << steepness << ">, ";
            }

            if (!cinfos.empty())
            {
                ss << "]" << endl;
                cout << ss.str();
            }
            return isCollided;
        }

        static void update(Player & player, sf::Time elapsed)
        {
            player.accumulator += elapsed;

            while (player.accumulator.asSeconds() > FIXED_DELTA)
            {
                vec::VECTOR2 velAccum(0, 0);
                vec::VECTOR2 accelAccum(0, 0);
                player.accumulator -= sf::seconds(FIXED_DELTA);

                player.posHist.push_back(player.pos);
                if (player.posHist.size() > 60)
                {
                    player.posHist.erase(player.posHist.begin());
                }

                if (player.moveUp)
                {
                    vec::VECTOR2 u = upVector(player.angle)*h;
                    velAccum += player.impulse(u);
                    player.moveUp = false;
                }

                if (player.moveLeft)
                {
                    vec::VECTOR2 l;
                    l = leftVector(player.angle + LEFTUP)*h;
                    velAccum += player.impulse(l);
                    player.moveLeft = false;
                }

                if (player.moveDown)
                {
                    vec::VECTOR2 d = downVector(player.angle)*h;
                    velAccum += player.impulse(d);
                    player.moveDown = false;
                }

                if (player.moveRight)
                {
                    vec::VECTOR2 r;
                    r = rightVector(player.angle - LEFTUP)*h;
                    velAccum += player.impulse(r);
                    player.moveRight = false;
                }

                if (player.applyGravity)
                {
                    ////std::cout << " G<" <<g.x << ", " << g.y << " >G" << std::endl;
                    vec::VECTOR2 g = downVector(player.angle) * GK;
                    accelAccum += g;
                }

                // Integrate motion
                // This is something that is almost as simple as euler integration
                // but better. or something. TODO: find reference
                {
                    player.pos += (player.vel + ((player.acc / 2) * FIXED_DELTA)) * FIXED_DELTA;
                    player.vel += (player.acc * FIXED_DELTA / 2.0f) + velAccum;// 2.0f;
                    vec::VECTOR2 newAcceleration = player.impulse(accelAccum);
                    player.vel += ((newAcceleration - player.acc) / 2) * (FIXED_DELTA / 2.0f);
                }
            }
        }
    }
}
