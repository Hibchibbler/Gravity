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
        const int GK = 20;
        const float FIXED_DELTA = 1.0f / 200.0f;
        const float DRAG_CONSTANT = 0.4;
        const int SKIP_FRAMES = 14;
        const float RESTITUTION = 0.9f;
#else
        const int LEFTUP = 0;
        const int h = 100;
        const int GK = 2000;
        const float FIXED_DELTA = 1.0f / 300.0f;
        const float DRAG_CONSTANT = 0.10;
        const int SKIP_FRAMES = 25;
        const float RESTITUTION = 0.05f;
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
            vec::VECTOR2 ray;
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
            const float epsilon = 0.0001f;
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
        bool sortHitsByAngle(physics::Intersection & i, physics::Intersection & j) { return (i.angle  > j.angle); }


        bool RayCast(vec::VECTOR2 dir, vec::VECTOR2 start, std::vector<SAT::Segment> & segments, physics::Intersection & intersect)
        {
            bool found = false;          
            dir = dir / 10.0;// This is important. is it?
            ///PHYSPRINT("[Ang](" << angle << ")");
            float closest = 999999.9;
            for (auto seg = segments.begin(); seg != segments.end(); ++seg)
            {
                physics::Intersection intersectTemp;
                intersectTemp.expired = true;
                intersectTemp.castedPoint = start + dir * 4000;
                intersectTemp.distance = intersectTemp.castedPoint - start;
                intersectTemp.ray = dir;
                getIntersection(start, dir, (*seg), intersectTemp);
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

            vec::VECTOR2 dir(cos(radians),
                sin(radians));
            return RayCast(dir, start, segments, intersect);
        }

        void doShit(std::vector<SAT::Segment> & segments, std::vector<SAT::Shape> & shapes, GameContext* ctx)
        {
            // We will shoot a ray for each degree of rotation around position.
            segments.clear();
            for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
            {
                std::vector<SAT::Segment> s = shape->getSegments();
                segments.insert(segments.end(), s.begin(), s.end());
            }
            {
                std::vector<physics::Intersection> intersections;
                for (float a = 0; a < 360; a = a + 0.25)
                {
                    //if (a == 0)
                    //    continue;
                    //if (a == 180.0)
                    //    continue;

                    // A Ray will intersect with an obstacle or the maximum distance (that we cast)
                    // hence, we always will have an intersection..but 
                    // whether it's relevent or not is in the eye of the beholder.

                    ////////////////////////////////////////////////////////////////////////////////////
                    Intersection intersectMin;
                    RayCast(a, ctx->player.position, segments, intersectMin);
                    intersections.push_back(intersectMin);
                    ////////////////////////////////////////////////////////////////////////////////////

                }
                //std::sort(intersections.begin(), intersections.end(), sortHitsByAngle);
                //ctx->lineSegments.clear();
                //ctx->lineSegments = sf::VertexArray(sf::PrimitiveType::Lines);

                //for (auto i = intersections.begin(); i != intersections.end(); ++i)
                //{
                //    vec::VECTOR2 positionFixed = ctx->player.position;
                //    ctx->lineSegments.append(positionFixed.sf());
                //    ctx->lineSegments.append(i->castedPoint.sf());
                //}
                ctx->lineSegments = sf::VertexArray(sf::PrimitiveType::TrianglesFan);//Lines);
                vec::VECTOR2 positionFixed = ctx->player.position;
                //positionFixed.x += 23;
                //positionFixed.y -= 10;
                ctx->lineSegments.append(positionFixed.sf());
                for (auto i = intersections.begin(); i != intersections.end(); ++i)
                {
                    //vec::VECTOR2 newLine = i->actualPoint - ctx->player.position;
                    //ctx->lineSegments.append(positionFixed.sf());
                    if (!i->expired)
                    {
                        i->castedPoint += i->ray * 700.0f;
                        ctx->lineSegments.append(i->castedPoint.sf());
                    }
                }
            }
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
                    {
                        if (h->overlap <= fuck)
                        {
                            fuck = h->overlap;
                        }
                    }
                    for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
                    {
                        if (h->overlap != fuck)
                            continue;

                        if (original_velocity.dot(h->smallest) > 0)
                            continue;

                        ss << "<fuck " << fuck << "> ";

                        if (false)
                        {
                            mtvA = *h;
                            if (original_velocity.dot(mtvA.smallest) > 0)
                            {
                                ss << "<> ";
                                continue;
                            }
                        }
                        if (true)
                        {
                            vec::VECTOR2 edgeLine = h->smallest.edge.end - h->smallest.edge.start;
                            edgeLine = edgeLine.norm();
                            vec::VECTOR2 edgeStart = h->smallest.edge.start;

                            float m = 9999999999.0;
                            float t = 0;
                            for (t = 0; t < 1.0; t = t=t+0.005)
                            {
                                vec::VECTOR2 edgePoint = (edgeStart * (1-t)) + (edgeLine)* t;
                                vec::VECTOR2 yang = player.position;

                                vec::VECTOR2 dist = edgePoint - yang;
                                float mag = dist.mag();
                                if (mag <= m)
                                {
                                    m = mag;
                                    //ss << "<m? " << m << "> ";
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
                //if (mtv->overlap <= overlap)
                //{
                    overlap = mtv->overlap;
                //}
                ss << "<" << zxc << ">, ";
                ss << "<CN " << collision_normal.x << ", " << collision_normal.y << ">, ";
                ss << "<OVR? " << overlap << ">, ";
                ss << "<EDGE (" << mtv->smallest.edge.start.x << ", " << mtv->smallest.edge.start.y << "), ";
                ss << "(" << mtv->smallest.edge.end.x << ", " << mtv->smallest.edge.end.y << "), ";
            }

            if (!mtvs.empty() && newPos != vec::VECTOR2(0, 0))
            {

                    
                isCollided = true;
                //overlap /= mtvs.size();
                vec::VECTOR2 newVelocity;
                newVelocity = newPos*(original_velocity.dot(newPos)) * -1 * (1 + RESTITUTION) + original_velocity;

                newPos = newPos.norm();

                

                if (newPos.x != 0)
                { 
                    player.velocity.x = newVelocity.x;
                }

                if (newPos.y != 0)
                {
                    player.velocity.y = newVelocity.y;
                }

                vec::VECTOR2 posDelta = newPos * overlap * 2.0;
                player.position += posDelta;// *overlap *;//newPos *  original_magnitude * 0.30;//  * 2.0;// overlap * 1.0;// 3.0;//

                float steepness = downVector(player.angle).dot(newPos);
                //if (steepness > -0.90 || original_magnitude > 80.0)
                if (original_magnitude > 80.0)
                {
                    ss << "< " << "HARD " << original_magnitude << ">, ";
                    //player.skipCollision = 2;
                    //player.velocity = newVelocity;
                    //player.GravityOn();
                }
                else
                {
                    ss << "< " << "SOFT" << ">, ";
                    player.velocity = vec::VECTOR2(0, 0);
                    player.acceleration = vec::VECTOR2(0, 0);
                   // player.GravityOff();
                }

                
                
                
                
                ss << "<A " << player.angle << ">";
                ss << "<PD " << posDelta.x << ", " << posDelta.y << "> ";//aggregate new position
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
                    velAccum += player.accelerate(u);
                    player.moveUp = false;
                }

                if (player.moveLeft)
                {
                    vec::VECTOR2 l;
                    l = leftVector(player.angle + LEFTUP)*h;
                    velAccum += player.accelerate(l);
                    player.moveLeft = false;
                }

                if (player.moveDown)
                {
                    vec::VECTOR2 d = downVector(player.angle)*h;
                    velAccum += player.accelerate(d);
                    player.moveDown = false;
                }

                if (player.moveRight)
                {
                    vec::VECTOR2 r;
                    r = rightVector(player.angle - LEFTUP)*h;
                    velAccum += player.accelerate(r);
                    player.moveRight = false;
                }

                //if (player.gravitySkipFrames == 0 && player.applyGravity)
                if (player.applyGravity)
                {
                    ////std::cout << " G<" <<g.x << ", " << g.y << " >G" << std::endl;
                    vec::VECTOR2 g = downVector(player.angle) * GK;
                    accelAccum += g;
                }
                else
                {
                    int g = 32;
                }

                if (player.gravitySkipFrames > 0)
                    player.gravitySkipFrames--;

                if (true)
                {
                    player.acceleration = player.accelerate(accelAccum);
                    
                    player.position += (player.velocity + ((player.acceleration / 2) * FIXED_DELTA)) * FIXED_DELTA;
                    player.velocity += (player.acceleration * FIXED_DELTA/2.0f) + velAccum;
                    vec::VECTOR2 newAcceleration = player.accelerate(accelAccum);
                    player.velocity += ((newAcceleration - player.acceleration) / 2) * (FIXED_DELTA / 2.0f);
                }
                if (false)
                {
                    vec::VECTOR2 drag = player.velocity * DRAG_CONSTANT;
                    player.acceleration = player.accelerate(accelAccum) - drag;


                    //player.position += (player.velocity + (player.acceleration / 2) * 2) * FIXED_DELTA;
                    player.position += (player.velocity + ((player.acceleration / 2)*FIXED_DELTA)) * FIXED_DELTA;
                    player.velocity += (player.acceleration * FIXED_DELTA) + velAccum;
                }
            }
        }
    }
}
