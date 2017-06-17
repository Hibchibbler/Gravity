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

namespace bali
{
    namespace physics
    {
#ifdef _DEBUG
        const int LEFTUP = 0;
        const int h = 15;
        const int GK = 50;
        const float FIXED_DELTA = 1.0f / 100.0f;
        const float DRAG_CONSTANT = 0.4;
        const int SKIP_FRAMES = 14;
        const float RESTITUTION = 0.80f;
#else
        const int LEFTUP = 0;
        const int h = 15;
        const int GK = 50;
        const float FIXED_DELTA = 1.0f / 200.0f;
        const float DRAG_CONSTANT = 0.10;
        const int SKIP_FRAMES = 25;
        const float RESTITUTION = 0.60f;
#endif
        /*
        Ray X = r_px+r_dx*T1
        Ray Y = r_py+r_dy*T1
        Segment X = s_px+s_dx*T2
        Segment Y = s_py+s_dy*T2
        */
        class Intersection
        {
        public:
            Intersection()
            {
                time = 0.0f;
                castedPoint = vec::VECTOR2(0.0f, 0.0f);
                actualPoint = vec::VECTOR2(0.0f, 0.0f);
                angle = 0;
                expired = true;
            }
            vec::VECTOR2 distance;
            float time;
            vec::VECTOR2 castedPoint;
            vec::VECTOR2 actualPoint;
            float angle;
            bool expired;//did the ray get to the end without hitting anything?
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

        bool
        MoreOrLessEquivalent(vec::VECTOR2 ray, vec::VECTOR2 edge)
        {
            // Adds a fuzz factor..because floating point is never perfect..
            // i don't really know what that means though.
            // but i kinda know what they're sayin'
            const float epsilon = 5.0f;
            if ((ray.x > edge.x - epsilon && ray.x < edge.x + epsilon) &&
                (ray.y > edge.y - epsilon && ray.y < edge.y + epsilon))
            {
                return true;
            }
            return false;
        }

        bool
            MoreOrLessEquivalent(float a, float b)
        {
            // Adds a fuzz factor..because floating point is never perfect..
            // i don't really know what that means though.
            // but i kinda know what they're sayin'
            const float epsilon = 0.005f;
            if (a > b-epsilon && a < b+epsilon)
            {
                return true;
            }
            return false;
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
           
            if (MoreOrLessEquivalent(r_dx / r_mag, s_dx / s_mag) &&
                MoreOrLessEquivalent(r_dy / r_mag, s_dy / s_mag))
            {
                return false;
            }
            //if(r_dx/r_mag==s_dx/s_mag && r_dy/r_mag==s_dy/s_mag){
            //    // Unit vectors are the same.
            //    return false;
            //}
            
            // SOLVE FOR T1 & T2
            // r_px+r_dx*T1 = s_px+s_dx*T2 && r_py+r_dy*T1 = s_py+s_dy*T2
            // ==> T1 = (s_px+s_dx*T2-r_px)/r_dx = (s_py+s_dy*T2-r_py)/r_dy
            // ==> s_px*r_dy + s_dx*T2*r_dy - r_px*r_dy = s_py*r_dx + s_dy*T2*r_dx - r_py*r_dx
            // ==> T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx)
            float det = (s_dx*r_dy - s_dy*r_dx);
            if (det == 0)
                return false;
            float T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/det;
            float T1 = (s_px+s_dx*T2-r_px)/r_dx;
            // Must be within parametic whatevers for RAY/SEGMENT

            if (T1 < 0)
            {
                return false;
            }
            if (T2<0 || T2>1)
            {
                return false;
            }
            // Return the POINT OF INTERSECTION
            intersection.time = T1;
            intersection.castedPoint = vec::VECTOR2(r_px + r_dx*T1, r_py + r_dy*T1);
            intersection.actualPoint = vec::VECTOR2(s_px + s_dx*T2, s_py + s_dy*T2);
            intersection.distance = rayStart - intersection.actualPoint;
            return true;
        }
        


        bool ResolveCollisions(std::vector<SAT::Segment> & segments,std::vector<SAT::Shape> & shapes, std::vector<SAT::Shape> & playerShapes, Player & player, bali::TileLayer & tileLayer, GameContext* ctx, bool ignoreDP)
        {
            // compare player shape to all other shapes for collision
            bool isCollided = false;

            float a_deg = 180 + player.angle;
            float a = DEG_TO_RAD(a_deg);
            std::list<SAT::MTV> mtvs;
            std::stringstream ss;

            vec::VECTOR2 original_velocity = player.velocity;
            double original_magnitude = original_velocity.mag();
            int sh1 = 0;
            
            ss << std::setprecision(5);
            ss << "[<P " << player.position.x << ", " << player.position.y << "> ";
            ss << "<V " << original_velocity.x << ", " << original_velocity.y << "> ";

            for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
            {
                SAT::MTV mtv1;
                SAT::Shape playerShape = playerShapes.back();
                std::vector<SAT::MTV> hitInfo;
                std::vector<SAT::MTV> reducedHitInfo;
                bool collision = playerShape.collision(player.position,player.velocity,*shape, mtv1, hitInfo);
                if (collision)
                {
                    if (player.skipCollision > 0)
                    {
                        player.skipCollision--;
                        continue;
                    }

                    ss << std::setprecision(5);

                    float minnyman = 999999999.0;
                    float fuck = 999999999.9;
                    SAT::MTV mtvA;
                    /// Try only using the last two or three or four hitInfo.those will be the smallest overlap, which gets us closer to the truth.
                    //float overlapTarget = hitInfo.back().overlap;//Since this is on the back()==size()-1, it is the smallest overlap.
                    // now only cycle through hits with equally small overlap
                    for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
                    {;
                        if (h->overlap <= fuck)
                        {
                            fuck = h->overlap;
                        }
                    }
                    for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
                    {
                        if (h->overlap != fuck)
                            continue;
                        if (false)
                        {
                            mtvA = *h;
                            if (original_velocity.dot(mtvA.smallest) > 0)
                                mtvA.smallest *= -1.0;
                        }
                        if (true)
                        {
                            vec::VECTOR2 edgeLine = h->smallest.edge.end - h->smallest.edge.start;
                            edgeLine.norm();
                            vec::VECTOR2 edgeStart = h->smallest.edge.start;

                            float m = 9999999999.0;
                            float t = 0;
                            for (t = 0; t < 50.0; t = ++t)
                            {
                                vec::VECTOR2 edgePoint = edgeStart + (edgeLine)* t;
                                vec::VECTOR2 yang = player.position;

                                vec::VECTOR2 dist = edgePoint - yang;
                                float mag = dist.mag();
                                if (mag <= m)
                                {
                                    m = mag;
                                    ss << "<m? " << m << "> ";
                                }
                            }

                            if (m <= minnyman)
                            {
                                minnyman = m;
                                mtvA = *h;
                                ss << "<m! " << m << "> ";
                            }

                            segments.push_back(h->smallest.edge);
                        }
                    }
                    mtvs.push_back(mtvA);
                    break;
                }
            }

            if (!mtvs.empty())
                ss << "<#ofMTV " << mtvs.size() << ">, ";

            vec::VECTOR2  newPos(0, 0);
            float overlap = 9999999.0;
            int zxc = 0;

            for (auto mtv = mtvs.begin(); mtv != mtvs.end(); ++mtv)
            {
                zxc++;
                SAT::Axis collision_normal = mtv->smallest;
                ////  R = -2*(V dot N)*N + V
                newPos += mtv->smallest;
                //overlap += mtv->overlap;
                 if (mtv->overlap <= overlap)
                {
                    overlap = mtv->overlap;
                }
                ss << "<" << zxc << ">, ";
                ss << "<CN " << collision_normal.x << ", " << collision_normal.y << ">, ";
                ss << "<OVR? " << overlap << ">, ";
                ss << "<EDGE (" << mtv->smallest.edge.start.x << ", " << mtv->smallest.edge.start.y << "), ";
                ss << "(" << mtv->smallest.edge.end.x << ", " << mtv->smallest.edge.end.y << "), ";
            }

            if (!mtvs.empty())
            {
                isCollided = true;
                //overlap /= mtvs.size();
                vec::VECTOR2 newVelocity;
                newPos = newPos.norm();
                float steepness = downVector(player.angle).dot(newPos);
                if (steepness > -0.90  || original_magnitude > 50.0)
                {
                    ss << "< " << "HARD" << ">, ";
                }
                else
                {
                    ss << "< " << "SOFT" << ">, ";
                }

                newVelocity = newPos*(original_velocity.dot(newPos)) * -1 * (1 + RESTITUTION) + original_velocity;

                vec::VECTOR2 posDelta = newPos * overlap * 2.0;
                player.position += posDelta;// *overlap *;//newPos *  original_magnitude * 0.30;//  * 2.0;// overlap * 1.0;// 3.0;//
                player.velocity = newVelocity;

                ss << "<A " << player.angle << ">";
                ss << "<NP " << posDelta.x << ", " << posDelta.y << "> ";//aggregate new position
                ss << "<NV " << newVelocity.x << ", " << newVelocity.y << "> ";//aggregate new velocity
                ss << "<OVR! " << overlap << ">, ";
                ss << "<STEEP " << steepness << ">, ";
            }
exit:
            //ss << "<<< " << std::endl;
            if (!mtvs.empty()) 
            {
                ss << "]" << endl;
                cout << std::setprecision(5) << ss.str();
            }
            return isCollided;
        }

        static void update(Player & player, sf::Time elapsed)
        {
            player.accumulator += elapsed;

            while (player.accumulator.asSeconds() > FIXED_DELTA)
            {
                player.accumulator -= sf::seconds(FIXED_DELTA);

                vec::VECTOR2 velAccum(0, 0);
                vec::VECTOR2 accelAccum(0, 0);



                player.posHist.push_back(player.position);
                if (player.posHist.size() > 50)
                {
                    player.posHist.erase(player.posHist.begin());
                }

                if (player.moveUp)
                {
                    vec::VECTOR2 u = upVector(player.angle)*h;
                    velAccum += u;
                    player.moveUp = false;
                    player.GravityOn();
                    player.gravitySkipFrames = SKIP_FRAMES+15;
                }

                if (player.moveLeft)
                {
                    vec::VECTOR2 l;
                    l = leftVector(player.angle + LEFTUP)*h;

                    velAccum += l;
                    //vel += upVector(player.angle) * 3.0;
                    player.moveLeft = false;
                    player.GravityOn();
                   // if (player.solidGround)
                    player.gravitySkipFrames = SKIP_FRAMES;
                }

                if (player.moveDown)
                {
                    vec::VECTOR2 d = downVector(player.angle)*h;
                    velAccum += d;
                    player.moveDown = false;
                    player.GravityOn();
                    player.gravitySkipFrames = SKIP_FRAMES;
                }

                if (player.moveRight)
                {
                    vec::VECTOR2 r;
                    r = rightVector(player.angle - LEFTUP)*h;
                    velAccum += r;
                    //vel += upVector(player.angle) * 3.0;
                    player.moveRight = false;
                    player.GravityOn();

                    //if (player.solidGround)
                    player.gravitySkipFrames = SKIP_FRAMES;
                }

                if (player.gravitySkipFrames == 0 && player.applyGravity)
                {
                    ////std::cout << " G<" <<g.x << ", " << g.y << " >G" << std::endl;
                    vec::VECTOR2 g = downVector(player.angle) * GK;
                    //accelAccum += g;
                }

                if (player.gravitySkipFrames > 0)
                    player.gravitySkipFrames--;


                player.position += (player.velocity + (player.acceleration / 2) * 2) * FIXED_DELTA;

                player.velocity += (player.acceleration * FIXED_DELTA) + velAccum;

                vec::VECTOR2 drag = player.velocity * DRAG_CONSTANT;
                player.acceleration = accelAccum - drag;
                
                //player.velocity += velAccum;
            }
        }
    }
}
