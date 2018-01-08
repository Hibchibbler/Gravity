///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#include "Physics.h"




#include "Polygons.h"
#include <list>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iostream>
#include "ConfigLoader.h"
#include "Player.h"
#include "SATAlgo/SATAlgo.h"

#include <assert.h>

using namespace bali;
using namespace bali::vec;

void initialize()
{

}

void update()
{

}

void cleanup()
{

}

// Find intersection of RAY & SEGMENT
bool physics::getIntersection(vec::VECTOR2 rayStart, vec::VECTOR2 ray, Segment segment, physics::Intersection & intersection)
{
    // RAY in parametric: Point + Delta*T1
    float r_px = rayStart.x;
    float r_py = rayStart.y;
    float r_dx = ray.x;
    float r_dy = ray.y;
    // SEGMENT in parametric: Point + Delta*T2
    float s_px = segment.start.x;// +segment.off.x;
    float s_py = segment.start.y;// +segment.off.y;
    float s_dx = segment.end.x - segment.start.x;
    float s_dy = segment.end.y - segment.start.y;
    // Are they parallel? If so, no intersect
    float r_mag = sqrt(r_dx*r_dx + r_dy*r_dy);
    float s_mag = sqrt(s_dx*s_dx + s_dy*s_dy);

    if (r_dx / r_mag == s_dx / s_mag && r_dy / r_mag == s_dy / s_mag) {
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

bool physics::RayCast(vec::VECTOR2 dir, vec::VECTOR2 start, std::vector<Segment> & segments, physics::Intersection & intersect)
{
    bool found = false;
    dir = dir / 8.0f;// This is important. is it?
                     ///PHYSPRINT("[Ang](" << angle << ")");
    float closest = 999999.9;
    for (auto seg = segments.begin(); seg != segments.end(); ++seg)
    {
        physics::Intersection intersectTemp;
        intersectTemp.expired = true;
        intersectTemp.rayPoint = start + dir * 3400.0f;
        intersectTemp.distance = intersectTemp.rayPoint - start;
        intersectTemp.ray = dir;
        physics::getIntersection(start, dir, *seg, intersectTemp);
        if (vec::mag(intersectTemp.distance) <= closest)
        {
            closest = vec::mag(intersectTemp.distance);
            intersect = intersectTemp;
            intersect.expired = false;
            found = true;
        }
    }
    return found;
}

bool physics::RayCast(float a, vec::VECTOR2 start, std::vector<Segment> & segments, physics::Intersection & intersect)
{
    float radians = a * (PI / 180.0f);

    vec::VECTOR2 dir(1.0f*cos(radians),
        1.0f*sin(radians));

    return physics::RayCast(dir, start, segments, intersect);
}

void physics::createLoSTriFan(std::vector<CONVEXSHAPE> & shapes, vec::VECTOR2 pos, sf::VertexArray & lineSegments)
{
    // We will shoot a ray for each degree of rotation around position.
    vec::VECTOR2 posOrg = pos;
    std::vector<Segment> segments;
    for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
    {
        std::vector<Segment> s = bali::SAT::Shape::getSegments(*shape);
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
            
            //pos.x -= 4;
            //pos.y += 8;

            pos.x -= 5;// ctx->playerpolygons.front().getLocalBounds().width / 1.0;//-8
            pos.y += 5;// ctx->playerpolygons.front().getLocalBounds().height / 1.0;//+5;


            RayCast(a, pos, segments, intersectMin);
            intersections.push_back(intersectMin);
        }

        lineSegments.clear();
        lineSegments = sf::VertexArray(sf::PrimitiveType::TrianglesFan);//Lines);
        vec::VECTOR2 positionFixed = posOrg;
        lineSegments.append(positionFixed);

        for (auto i = intersections.begin(); i != intersections.end(); ++i)
        {
            if (true)// !i->expired)
            {
                i->rayPoint += i->ray * 100.0f;
                lineSegments.append(i->rayPoint);
            }
        }
    }
}


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
float mindist(vec::VECTOR2 v, vec::VECTOR2 w, vec::VECTOR2 p) {
    // Return minimum distance between line segment vw and point p
    const float l2 = vec::mag(w - v);//length_squared(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return (vec::mag(p - v));//distance(p, v);   // v == w case
                                            // Consider the line extending the segment, parameterized as v + t (w - v).
                                            // We find projection of point p onto the line. 
                                            // It falls where t = [(p-v) . (w-v)] / |w-v|^2
                                            // We clamp t from [0,1] to handle points outside the segment vw.
    const float t = max(0, min(1, vec::dot(p - v, w - v) / l2));
    vec::VECTOR2 projection = v + t * (w - v);  // Projection falls on the segment
    return vec::mag(projection-vec::norm(p));
}

bool ciGreater(SAT::ContactInfo a, SAT::ContactInfo b)
{
    return a.overlap < b.overlap;
}

#define PRINT_DIAGNOSTICS
bool physics::ResolveCollisions(std::stringstream & ss, bali::CONVEXSHAPE::Vec & shapes, bali::CONVEXSHAPE & playerShape, Player & player, PhysicsConfig & pc, std::vector<Segment> & sharedEdges, OnCollisionEvent onCollision, OnNonCollisionEvent onNonCollision)
{
    //std::stringstream ss;
    ss << std::setprecision(5);
    Physical & phys = player.physical;
    int cnt = 0;
    bool collision = false;
    player.isCollidedLast = player.isCollided;
    player.isCollided = false;

    vec::VECTOR2 original_velocity = phys.vel;
    if (original_velocity == vec::Zero())
    {

    }
#ifdef PRINT_DIAGNOSTICS
    ss << "[<P " << phys.pos.x << ", " << phys.pos.y << ">";
    ss << "<V " << original_velocity.x << ", " << original_velocity.y << ">";
    ss << "<Shapes " << shapes.size() << ">, ";
#endif
    SAT::ContactInfo hitMin;
    std::list<SAT::ContactInfo> cinfos;
    for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
    {
        bool found = false;
        std::vector<SAT::ContactInfo> hitInfo;
        collision = SAT::Shape::collision(playerShape, *shape, hitInfo);
        if (collision)
        {
            collision = true;
            float minimumOverlap = 999999999.0f;
            
            //For this shape, winnow down the hit list by finding the
            // Hit that has the minimum overlap
            for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
            {
                assert(h->overlap != 0.0f);
                if (h->overlap <= minimumOverlap)
                {
                    if (vec::dot(original_velocity, h->normal) < 0.01)
                    {
                        minimumOverlap = h->overlap;
                        hitMin = *h;
                        found = true;
                    }
                }
            }
            if (found)
            {
                assert(hitMin.overlap != 0.0f);
                assert(hitMin.normal != vec::Zero());
                cinfos.push_back(hitMin);
            }
            else
            {
                // Happens when rotating?
                // A collision occured, but since we rotated, the 
                // dot product of the velocity and surface normal no
                // longer are in opposing directions, as is assumed 
                // by the response flows.
                std::cout << "XONXX ";
            }
        }
    }
    if (collision == false)
    {
        onNonCollision(player, pc);
    }
#ifdef PRINT_DIAGNOSTICS
    ss << "<Collisions " << cinfos.size() << ">";
#endif
    SAT::ContactInfo biggest;
    vec::VECTOR2  newPos(0, 0);
    float overlap = 0.0;
    // sort using a custom function object

    if (cinfos.size() > 1)
    {
        std::cout << "CONK ";
    }



    if (cinfos.size() > 0)
    {
        // Sort the hit list by overlap.
        // greatest overlap first.
        cinfos.sort(ciGreater);
        if (cinfos.size() > 1)
        {
            assert(cinfos.back().overlap >= cinfos.front().overlap);
        }
        // Remember the greatest, use it as the magnitude of MTV
        overlap = cinfos.back().overlap;
        biggest = cinfos.back();
        //newPos = cinfos.back().normal;
        // Average all the collision normals together.
        int cnt = 0;
        for (auto cinfo = cinfos.begin(); cinfo != cinfos.end(); ++cinfo)
        {
            newPos += cinfo->normal;
#ifdef PRINT_DIAGNOSTICS
            ss << "<CN " << cinfo->normal.x << ", " << cinfo->normal.y << ">";
            ss << "<OVR? " << cinfo->overlap << ">, ";
            ss << "<EDGE (" << cinfo->edge.start.x << ", " << cinfo->edge.start.y << ")";
            ss << "(" << cinfo->edge.end.x << ", " << cinfo->edge.end.y << ")>";
#endif
            cnt++;
        }

        player.isCollided = true;
        if (newPos != vec::VECTOR2(0, 0)) //TODO: don't think we need this check anymore (i.e. GetOverlap() is fixed?)
        { 
            float newMag = 0.f;
            newPos = vec::norm(newPos);
            vec::VECTOR2 newVelocity;
            vec::VECTOR2 posDelta = newPos * overlap * 1.05f;
            phys.addAddPosition(posDelta);
            newVelocity = newPos*(vec::dot(original_velocity, newPos)) * -1.f * (1 + pc.RESTITUTION) + original_velocity;

            assert(newVelocity != vec::Zero());
            assert(overlap != 0.0f);

            newMag = vec::mag(newVelocity);
            phys.addSetVelocity(newVelocity);
            //TODO: we need to reposition the polygon
            //      that playerShape is based on.
            //playerShape.move(posDelta);

            vec::VECTOR2 tangent = newVelocity - (newPos * vec::dot(newVelocity, newPos));
            tangent = vec::norm(tangent);
            float jt = vec::dot(newVelocity, tangent) * -1.0f;

            //
            // If velocity is low,
            // use static friction
            // otherwise, use dynamic
            // friction.
            if (newMag < pc.STATIC_FRICTION_VELOCITY_MAX && !player.isMoving())
            {
                phys.addAddVelocity(tangent * pc.STATIC_FRICTION * jt);
#ifdef PRINT_DIAGNOSTICS
                ss << "< --- KJT>";
#endif
            }
            else// if (!player.isMoving())
            {
                phys.addAddVelocity(tangent * pc.DYNAMIC_FRICTION * jt);
#ifdef PRINT_DIAGNOSTICS
                ss << "< !!! DJT>";
#endif
            }
#ifdef PRINT_DIAGNOSTICS
            ss << "< " << vec::mag(phys.vel) << " >";
#endif

#ifdef PRINT_DIAGNOSTICS
            ss << "<A " << phys.angle << ">";
            ss << "<PD " << posDelta.x << ", " << posDelta.y << ">";
            ss << "<NV " << newVelocity.x << ", " << newVelocity.y << ">";
            ss << "<OVR! " << overlap << ">";
            ss << "]" << std::endl;
            //std::cout << ss.str();
#endif

            onCollision(player, newPos, pc);
        }
        else
        {
            //assert(newPos != vec::VECTOR2(0, 0));
        }
    }
    return false;
}

void physics::updatePlayerMotion(Player & player, sf::Time elapsed, PhysicsConfig & pc)
{
    Physical & phys = player.physical;
    player.accumulator += elapsed;
    while (player.accumulator.asMilliseconds() > pc.FIXED_DELTA)
    {
        //std::cout << "#";
        player.accumulator -= sf::seconds(pc.FIXED_DELTA);

        player.posHist.push_back(player.physical.pos);
        if (player.posHist.size() > 125)
        {
            player.posHist.erase(player.posHist.begin());
        }



        //
        // Integrate motion
        //  Apply gravity only if player is moving more than a hair
        //  Update velocity
        Command cmd;
        vec::VECTOR2 velAcc;
        vec::VECTOR2 theVel;
        while (player.physical.getSetCommand(cmd))
        {
            if (cmd.code == Command::Code::SETVELOCITY)
            {
                phys.vel = cmd.sv.vel;
            }
            else if (cmd.code == Command::Code::SETPOSITION)
            {
                phys.pos = cmd.sp.pos;
            }
            else if (cmd.code == Command::Code::SETTARGETANGLE)
            {
                //// Update angle, and rotate velocity.
                //float angle = (player.angle - cmd.sta.targetangle) * (PI / 180.0f);
                //player.vel.x = player.vel.x * cos(angle) - player.vel.y * sin(angle);
                //player.vel.y = player.vel.x * sin(angle) + player.vel.y * cos(angle);
                phys.angle = cmd.sta.targetangle;
            }
        }
        while (phys.getAddCommand(cmd))
        {
            if (cmd.code == Command::Code::ADDVELOCITY)
            {
                phys.vel += cmd.av.delta;
            }
            else if (cmd.code == Command::Code::ADDPOSITION)
            {
                phys.pos += cmd.ap.delta;
            }
            else if (cmd.code == Command::Code::MOVE)
            {
                if (!cmd.mv.gnd)
                {
                    vec::VECTOR2 dir = vec::norm(cmd.mv.dir);
                    vec::VECTOR2 m;
                    //if (vec::dot(downVector(phys.angle), dir) > -0.01f)
                    {

                        m = dir * cmd.mv.str * pc.FREEFALL_MOVE_STRENGTH;
                        phys.vel += phys.impulse(m);
                        //std::cout << "V ";
                    }
                }
                else
                {
                    vec::VECTOR2 dir = vec::norm(cmd.mv.dir);
                    vec::VECTOR2 m = dir * cmd.mv.str * pc.MOVE_STRENGTH;
                    phys.vel += phys.impulse(m);
                    //std::cout << "- ";
                }
            }
            else if (cmd.code == Command::Code::JUMP)
            {
                vec::VECTOR2 u;
                vec::VECTOR2 up = upVector(phys.angle) * 0.0f;
                if (cmd.jmp.dir != vec::Zero())
                {
                    
                    u = vec::norm(cmd.jmp.dir + up)  * cmd.jmp.str * pc.JUMP_STRENGTH;
                    if (vec::dot(player.surfaceNormal, downVector(phys.angle)) < 0.01f)
                        phys.vel += phys.impulse(u);
                    else
                    {
                        std::cout << "CRUNCH ";
                    }
                }
                else
                {
                    std::cout << "????? ";
                }
            }
        }

        //
        // Apply drag if physical is airborne.
        // But, only apply drag in the lateral directions.
        //
        float rightStrength = vec::dot(rightVector(phys.angle), vec::norm(phys.vel));
        float leftStrength = vec::dot(leftVector(phys.angle), vec::norm(phys.vel));
        float upStrength = vec::dot(upVector(phys.angle), vec::norm(phys.vel));
        float downStrength = vec::dot(downVector(phys.angle), vec::norm(phys.vel));
        if (!player.isCollided &&
            phys.vel != vec::Zero())// &&
            //lateralStrength > 0.96f)
            //vec::dot(downVector(player.angle), vec::norm(phys.vel)) < 0.25f)
        {
            if (upStrength < downStrength)
            {
                phys.vel -= phys.vel * pc.DRAG_CONSTANT * max(rightStrength, leftStrength);
            }
        }

        //
        // semi-implicit euler
        //
        if (player.applyGravity)
        {
            phys.accel = downVector(phys.angle) * (float)pc.GRAVITY_CONSTANT;
        }
        else
        {
            phys.accel = vec::Zero();
        }
        phys.vel += (player.physical.accel * pc.FIXED_DELTA);

        //
        // Limit Velocity
        //
        if (phys.vel.x > pc.VELOCITY_MAX)
        {
            phys.vel.x = pc.VELOCITY_MAX;
        }

        if (phys.vel.x < -pc.VELOCITY_MAX)
        {
            phys.vel.x = -pc.VELOCITY_MAX;
        }

        if (phys.vel.y > pc.VELOCITY_MAX)
        {
            phys.vel.y = pc.VELOCITY_MAX;
        }

        if (phys.vel.y < -pc.VELOCITY_MAX)
        {
            phys.vel.y = -pc.VELOCITY_MAX;
        }


        phys.pos += phys.vel * pc.FIXED_DELTA;

    }
}

physics::Intersection::Intersection()
{
    time = 0.0f;
    rayPoint = vec::VECTOR2(0.0f, 0.0f);
    segPoint = vec::VECTOR2(0.0f, 0.0f);
    angle = 0;
    expired = true;
    segment = Segment(rayPoint, rayPoint, rayPoint);
}

vec::VECTOR2 physics::rotVector(float angle)
{
    float a = DEG_TO_RAD(angle);
    vec::VECTOR2 v(0, 1);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::upVector(float angle)
{
    float a = DEG_TO_RAD(angle - 90);
    vec::VECTOR2 v(0, 1);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::leftVector(float angle)
{
    //float a = DEG_TO_RAD(angle - 180 - LEFTUP);
    float a = DEG_TO_RAD(angle - 180);
    vec::VECTOR2 v(0, 1);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::downVector(float angle)
{
    float a = DEG_TO_RAD(angle - 270);
    vec::VECTOR2 v(0, 1);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::rightVector(float angle)
{
    //float a = DEG_TO_RAD(angle - 360 + LEFTUP);
    float a = DEG_TO_RAD(angle - 360);
    vec::VECTOR2 v(0, 1);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}
