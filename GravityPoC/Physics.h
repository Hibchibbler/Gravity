///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utility.h"
#include <list>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include "ConfigLoader.h"

#ifdef PHYSICS_DEBUG
#define PHYSPRINT(x) \
    std::cout << x << std::endl;
#endif
#define EPSILON 0.0000001f
namespace bali
{
    namespace physics
    {
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

        static vec::VECTOR2 rotVector(float angle)
        {
            float a = DEG_TO_RAD(angle);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

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
            //if (fabs(dir.x) < 0.0001)
            //    dir.x = 0.0f;

            //if (fabs(dir.y) < 0.0001)
            //    dir.y = 0.0f;
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
                for (float a = 0.0; a < 365.0; a = a + 1.0)
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
                    vec::VECTOR2 pos = ctx->player.pos;
                    //pos.x -= 4;
                    //pos.y += 8;

                    pos.x -= 5;// ctx->playerpolygons.front().getLocalBounds().width / 1.0;//-8
                    pos.y += 5;// ctx->playerpolygons.front().getLocalBounds().height / 1.0;//+5;
                    

                    RayCast(a, pos, segments, intersectMin);
                    intersections.push_back(intersectMin);
                }

                ctx->lineSegments.clear();
                ctx->lineSegments = sf::VertexArray(sf::PrimitiveType::TrianglesFan);//Lines);
                vec::VECTOR2 positionFixed = ctx->player.pos;
                ctx->lineSegments.append(positionFixed.sf());

                for (auto i = intersections.begin(); i != intersections.end(); ++i)
                {
                    if (true)// !i->expired)
                    {
                        i->rayPoint += i->ray * 100.0f;
                        ctx->lineSegments.append(i->rayPoint.sf());
                    }
                }
            }
        }


        vec::VECTOR2 getContactPosition(vec::VECTOR2 pos, vec::VECTOR2 vel, SAT::Shape & shape, std::vector<SAT::Shape> & playerShapes, std::vector<SAT::ContactInfo> & hitInfo, PhysicsConfig & pc)
        {
            //
            // It is assumed that a collision has currently been detected.
            //
            float step = 2.5f;
            bool hit = true;
            vec::VECTOR2 posAdjustment = vec::VECTOR2(0,0);// = pos;
            vel = vel.norm();
            bool collision=false;
            
            int c = 0;
            std::vector<SAT::ContactInfo> lastHitInfo;
            do //accumulate a constant delta, until we find we are not colliding
            {
                SAT::Shape pshape = playerShapes.back();

                posAdjustment -= (vel * step);
                pshape.translate(posAdjustment.x, posAdjustment.y);
                std::vector<SAT::ContactInfo> tempHitInfo;
                collision = playerShapes.back().collision(pshape, tempHitInfo);

                std:cout << "? " << c++ << " ?" << posAdjustment.x << ", " << posAdjustment.y << "???" << std::endl;
            } while (collision);

            //At this point, we are no longer colliding.
            //so undo one increment. 
            //it's assumed that we are one increment away from colliding again.
            posAdjustment += (vel * step)*3.0;

            std::cout << "~~~" << posAdjustment.x << ", " << posAdjustment.y << "~~~" << std::endl;
            return posAdjustment;
        }


        bool ciGreater(SAT::ContactInfo a, SAT::ContactInfo b)
        {
            return a.overlap < b.overlap;
        }
        #undef PRINT_DIAGNOSTICS
        bool ResolveCollisions(std::vector<SAT::Shape> & shapes, std::vector<SAT::Shape> & playerShapes, Player & player, PhysicsConfig & pc, std::vector<SAT::Segment> & sharedEdges )
        {
            float a_deg = 180 + player.angle;
            float a = DEG_TO_RAD(a_deg);
            std::stringstream ss;
            ss << std::setprecision(5);
            int cnt = 0;
            bool collision = false;

            collision = false;
            player.edgeContact = vec::VECTOR2(0, 0);
            player.posCorrection = vec::VECTOR2(0, 0);
            player.isCollided = false;

            vec::VECTOR2 original_velocity = player.vel;
#ifdef PRINT_DIAGNOSTICS
            ss << "[<P " << player.pos.x << ", " << player.pos.y << ">";
            ss << "<V " << original_velocity.x << ", " << original_velocity.y << ">";
            ss << "<Shapes " << shapes.size() << ">, ";
#endif
            SAT::ContactInfo hitMin;
            std::list<SAT::ContactInfo> cinfos;
            for (auto shape = shapes.begin();shape != shapes.end(); ++shape)
            {
                SAT::Shape playerShape = playerShapes.back();
                std::vector<SAT::ContactInfo> hitInfo;

                collision = playerShape.collision(*shape, hitInfo);

                if (collision)
                {
                    ////vec::VECTOR2 posAdjustment = vec::VECTOR2(0, 0);
                    //vec::VECTOR2 posAdjustment = getContactPosition(player.pos, player.vel, *shape, playerShapes, hitInfo, pc);

                    //std::cout << "<pos " << player.pos.x << ", " << player.pos.y << ">";
                    //std::cout << "<adjust " << posAdjustment.x << ", " << posAdjustment.y << ">" << std::endl;

                    //player.pos += posAdjustment * 0.10;

                    float minimumOverlap = 999999999.0f;
                    //For this shape, winnow down the hit list by finding the
                    // Hit that has the minimum overlap
                    for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
                    {
                        if (h->overlap <= minimumOverlap)
                        {
                            if (original_velocity.dot(h->normal) <= 0)
                            {
                                minimumOverlap = h->overlap;
                                hitMin = *h;
                            }
                        }
                    }
                    if (!isSharedEdge(hitMin, sharedEdges))
                    {
                        //std::cout << "<Keep Edge>";
                        cinfos.push_back(hitMin);
                    }
                    else
                    {
                        std::cout << "<DscrdEdge>" << std::endl;
                    }
                }
            }

#ifdef PRINT_DIAGNOSTICS
            ss << "<Collisions " << cinfos.size() << ">";
#endif
            SAT::ContactInfo biggest;
            vec::VECTOR2  newPos(0, 0);
            float overlap = 0.0;
            // sort using a custom function object

            if (cinfos.size() > 0)
            {
                // Sort the hit list by overlap.
                // greatest overlap first.
                cinfos.sort(ciGreater);

                // Remember the greatest, use it as the magnitude of MTV
                overlap = cinfos.back().overlap;
                biggest = cinfos.back();

                // Average all the collision normals together.
                int cnt = 0;
                for (auto cinfo = cinfos.begin(); cinfo != cinfos.end(); ++cinfo)
                {
                    newPos += cinfo->normal;
                    ///overlap += cinfo->overlap;
#ifdef PRINT_DIAGNOSTICS
                    ss << "<CN " << cinfo->normal.x << ", " << cinfo->normal.y << ">";
                    ss << "<OVR? " << cinfo->overlap << ">, ";
                    ss << "<EDGE (" << cinfo->edge.start.x << ", " << cinfo->edge.start.y << ")";
                    ss <<       "(" << cinfo->edge.end.x   << ", " << cinfo->edge.end.y   << ")>";
#endif
                    cnt++;
                }

                if (newPos != vec::VECTOR2(0, 0))
                {
                    newPos = newPos.norm();
                    player.isCollided = true;
                    vec::VECTOR2 newVelocity;

                    vec::VECTOR2 posDelta = newPos * overlap * 1.50;
                    player.pos += posDelta;

                    player.posCorrection = newPos;
                    player.edgeContact = biggest.edge.end - biggest.edge.start;
                    newVelocity = newPos*(original_velocity.dot(newPos)) * -1 * (1 + pc.RESTITUTION) + original_velocity;
                    player.vel = newVelocity;

                    playerShapes.back().translate(posDelta.x, posDelta.y);// Yea ... Important

                    vec::VECTOR2 tangent = newVelocity - (newPos * newVelocity.dot(newPos));
                    tangent = tangent.norm();
                    float jt = newVelocity.dot(tangent) * -1.0;

                    // If velocity is low,
                    // use kinetic friction
                    //
                    // otherwise, use dynamic
                    // friction.
                    if (player.vel.mag() < pc.FAST_THRESHOLD)
                    {
                        player.vel += tangent * pc.KINETIC_FRICTION * jt;
#ifdef PRINT_DIAGNOSTICS
                        ss << "< --- KJT>";
#endif
                    }
                    else
                    {
                        player.vel += tangent * pc.DYNAMIC_FRICTION * jt;
#ifdef PRINT_DIAGNOSTICS
                        ss << "< !!! DJT>";
#endif
                    }

#ifdef PRINT_DIAGNOSTICS
                    ss << "< " << player.vel.mag() << " >";
#endif

#ifdef PRINT_DIAGNOSTICS
                    ss << "<A " << player.angle << ">";
                    ss << "<PD " << posDelta.x << ", " << posDelta.y << ">";
                    ss << "<NV " << newVelocity.x << ", " << newVelocity.y << ">";
                    ss << "<OVR! " << overlap << ">";
                    ss << "]" << endl;
                    cout << ss.str();
#endif
                }
            }
            return false;
        }

        static void update(Player & player, sf::Time elapsed, PhysicsConfig & pc)
        {
            player.accumulator += elapsed;

            while (player.accumulator.asSeconds() > pc.FIXED_DELTA)
            {
                player.accumulator -= sf::seconds(pc.FIXED_DELTA);

                player.posHist.push_back(player.pos);
                if (player.posHist.size() > 80)
                {
                    player.posHist.erase(player.posHist.begin());
                }

                if (player.moveUp)
                {
                    // If player is moving slow enough
                    // allow a jump with a trajectory
                    // based on the last surface the player was standing on.
                    //if (player.vel.mag() < 900.0 && player.posCorrection.mag() > 0)
                    vec::VECTOR2 u;
                    if (player.posCorrection.mag() > 0)
                    {
                        u = ((player.posCorrection + upVector(player.angle)) / 2.0) * pc.JUMP_STRENGTH;
                    }
                    else
                    {
                        u = upVector(player.angle) * pc.JUMP_STRENGTH * 0.05;
                    }
                    player.velAccu += player.impulse(u);
                    player.moveUp = false;
                }

                if (player.moveLeft)
                {
                    vec::VECTOR2 l;
                    if (player.edgeContact.mag() > 0.0)
                    {// Move the player along the surface upon which the player stands.
                        
                        l = player.edgeContact.norm() * pc.MOVE_STRENGTH * -2.5;
                    }
                    else
                    {// otherwise, move left (because we're probably in the middle of a jump or fall)
                        l = leftVector(player.angle + pc.HORIZONTAL_MOVE_ANGLE_OFFSET) * pc.MOVE_STRENGTH * 0.75;
                    }
                    player.velAccu += player.impulse(l);
                    player.moveLeft = false;
                }

                if (player.moveDown)
                {
                    vec::VECTOR2 d = downVector(player.angle)  * pc.MOVE_STRENGTH;
                    player.velAccu += player.impulse(d);
                    player.moveDown = false;
                }

                if (player.moveRight)
                {
                    vec::VECTOR2 r;
                    if (player.edgeContact.mag() > 0.0)
                    {// Move the player along the surface upon which the player stands.

                        r = player.edgeContact.norm() * pc.MOVE_STRENGTH * 2.5;
                    }
                    else
                    {// otherwise, move right (because we're probably in the middle of a jump or fall)
                        r = rightVector(player.angle + pc.HORIZONTAL_MOVE_ANGLE_OFFSET) * pc.MOVE_STRENGTH * 0.75;
                    }
                    player.velAccu += player.impulse(r);
                    player.moveRight = false;
                }

                if (player.nextangle != player.angle)
                {
                    float delta = (player.nextangle - player.angle) / 500.0;
                    player.angle += delta;
                }

                // Integrate motion
                // google: "2d game physics motion integration symplectic verlet euler rk rk4"
                {
                    player.acc = downVector(player.angle) * pc.GRAVITY_CONSTANT;

                    if (player.vel.mag() < pc.MAX_VELOCITY)
                        player.vel += player.velAccu + (player.acc * pc.FIXED_DELTA);
                    player.vel = player.vel - (player.vel * pc.DRAG_CONSTANT);

                    player.pos += player.vel * pc.FIXED_DELTA;


                    player.velAccu = vec::VECTOR2(0.f, 0.f);
                }
            }
        }
    }
}
