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

#undef PRINT_DIAGNOSTICS
bool physics::ResolveCollisions(bali::CONVEXSHAPE::Vec & shapes, bali::CONVEXSHAPE & playerShape, Player & player, PhysicsConfig & pc, std::vector<Segment> & sharedEdges)
{
    std::stringstream ss;
    ss << std::setprecision(5);

    int cnt = 0;
    bool collision = false;
    player.isCollidedLast = player.isCollided;
    player.isCollided = false;

    vec::VECTOR2 original_velocity = player.vel;
#ifdef PRINT_DIAGNOSTICS
    ss << "[<P " << player.pos.x << ", " << player.pos.y << ">";
    ss << "<V " << original_velocity.x << ", " << original_velocity.y << ">";
    ss << "<Shapes " << shapes.size() << ">, ";
#endif
    SAT::ContactInfo hitMin;
    std::list<SAT::ContactInfo> cinfos;
    for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
    {
        std::vector<SAT::ContactInfo> hitInfo;
        collision = SAT::Shape::collision(playerShape, *shape, hitInfo);
        if (collision)
        {
            float minimumOverlap = 999999999.0f;
            //For this shape, winnow down the hit list by finding the
            // Hit that has the minimum overlap
            for (auto h = hitInfo.begin(); h != hitInfo.end(); ++h)
            {
                if (h->overlap <= minimumOverlap)
                {
                    if (vec::dot(original_velocity, h->normal) <= 0)
                    {
                        minimumOverlap = h->overlap;
                        hitMin = *h;
                    }
                }
            }
            cinfos.push_back(hitMin);
        }
    }
    //std::cout  << "<C " << cinfos.size() << ">\n";
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
#ifdef PRINT_DIAGNOSTICS
            ss << "<CN " << cinfo->normal.x << ", " << cinfo->normal.y << ">";
            ss << "<OVR? " << cinfo->overlap << ">, ";
            ss << "<EDGE (" << cinfo->edge.start.x << ", " << cinfo->edge.start.y << ")";
            ss << "(" << cinfo->edge.end.x << ", " << cinfo->edge.end.y << ")>";
#endif
            cnt++;
        }

        player.isCollided = true;
        if (newPos != vec::VECTOR2(0, 0))//TODO: don't think we need this check anymore (i.e. GetOverlap() is fixed?)
        { 
            float newMag = 0.f;
            newPos = vec::norm(newPos);

            vec::VECTOR2 newVelocity;

            vec::VECTOR2 posDelta = newPos * overlap * 1.20f;
            //player.pos += posDelta;
            player.addAddPosition(posDelta);

            newVelocity = newPos*(vec::dot(original_velocity, newPos)) * -1.f * (1 + pc.RESTITUTION) + original_velocity;
            newMag = vec::mag(newVelocity);
            player.addSetVelocity(newVelocity);
            //playerShape.move(posDelta);

            vec::VECTOR2 tangent = newVelocity - (newPos * vec::dot(newVelocity, newPos));
            tangent = vec::norm(tangent);
            float jt = vec::dot(newVelocity, tangent) * -1.0f;


            if (newMag < pc.JUMP_VELOCITY_MAX)
            {
                //
                // TODO: We only want these to be registered
                // i.e. Enable jumping
                // when we are moving at a velocity less
                // than MAX_JUMP_VELOCITY
                //
                player.jumpNormal = newPos;
            }

            //
            // We can laterally move, no matter what.
            //
            {
                player.latNormal = newPos;
            }

            {
                player.surfaceNormal = newPos;
            }

            //
            // If velocity is low,
            // use static friction
            // otherwise, use dynamic
            // friction.
            if (newMag < pc.STATIC_FRICTION_VELOCITY_MAX && !player.isMoving())
            {
                player.addAddVelocity(tangent * pc.STATIC_FRICTION * jt);
#ifdef PRINT_DIAGNOSTICS
                ss << "< --- KJT>";
#endif
            }
            else
            {
                player.addAddVelocity(tangent * pc.DYNAMIC_FRICTION * jt);
#ifdef PRINT_DIAGNOSTICS
                ss << "< !!! DJT>";
#endif
            }
#ifdef PRINT_DIAGNOSTICS
            ss << "< " << vec::mag(player.vel) << " >";
#endif

#ifdef PRINT_DIAGNOSTICS
            ss << "<A " << player.angle << ">";
            ss << "<PD " << posDelta.x << ", " << posDelta.y << ">";
            ss << "<NV " << newVelocity.x << ", " << newVelocity.y << ">";
            ss << "<OVR! " << overlap << ">";
            ss << "]" << std::endl;
            std::cout << ss.str();
#endif
            }
    }
    return false;
}

void physics::update(Player & player, sf::Time elapsed, PhysicsConfig & pc)
{
    player.accumulator += elapsed;
    while (player.accumulator.asMilliseconds() > pc.FIXED_DELTA)
    {
        //std::cout << "#";
        player.accumulator -= sf::seconds(pc.FIXED_DELTA);

        player.posHist.push_back(player.pos);
        if (player.posHist.size() > 100)
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
        while (player.getSetCommand(cmd))
        {
            if (cmd.code == Command::Code::SETVELOCITY)
            {
                player.vel = cmd.sv.vel;
            }
            else if (cmd.code == Command::Code::SETPOSITION)
            {
                player.pos = cmd.sp.pos;
            }
            else if (cmd.code == Command::Code::SETTARGETANGLE)
            {
                player.targetangle = cmd.sta.targetangle;
                player.granularity = cmd.sta.granularity;
            }
        }
        while (player.getAddCommand(cmd))
        {
            if (cmd.code == Command::Code::ADDVELOCITY)
            {
                player.vel += cmd.av.delta;
            }
            else if (cmd.code == Command::Code::ADDPOSITION)
            {
                player.pos += cmd.ap.delta;
            }
            else if (cmd.code == Command::Code::MOVE)
            {
                if (!cmd.mv.gnd)
                {
                    vec::VECTOR2 dir = cmd.mv.dir;
                    vec::VECTOR2 m;
                    m = dir * cmd.mv.str * pc.FREEFALL_MOVE_STRENGTH;
                    player.vel += player.impulse(m);
                    //std::cout << "V ";
                }
                else
                {
                    vec::VECTOR2 dir = vec::norm(cmd.mv.dir);
                    vec::VECTOR2 m = dir * cmd.mv.str * pc.MOVE_STRENGTH;
                    player.vel += player.impulse(m);
                    //std::cout << "- ";
                }
            }
            else if (cmd.code == Command::Code::JUMP)
            {
                vec::VECTOR2 u;
                vec::VECTOR2 up = upVector(player.angle) * 0.0f;
                if (cmd.jmp.dir != vec::Zero())
                {
                    
                    u = vec::norm(cmd.jmp.dir + up)  * cmd.jmp.str * pc.JUMP_STRENGTH;
                    player.vel += player.impulse(u);
                }
                else
                {
                    std::cout << "? ";
                }
            }
        }

        //
        // semi-implicit euler
        //
        if (player.applyGravity)
        {
            player.accel = downVector(player.angle) * (float)pc.GRAVITY_CONSTANT;
        }
        else
        {
            player.accel = vec::Zero();
        }
        player.vel += (player.accel * pc.FIXED_DELTA);

        //
        // Limit Velocity
        //
        if (player.vel.x > pc.VELOCITY_MAX)
        {
            player.vel.x = pc.VELOCITY_MAX;
        }

        if (player.vel.x < -pc.VELOCITY_MAX)
        {
            player.vel.x = -pc.VELOCITY_MAX;
        }

        if (player.vel.y > pc.VELOCITY_MAX)
        {
            player.vel.y = pc.VELOCITY_MAX;
        }

        if (player.vel.y < -pc.VELOCITY_MAX)
        {
            player.vel.y = -pc.VELOCITY_MAX;
        }

        player.vel -= player.vel * pc.DRAG_CONSTANT;
        player.pos += player.vel * pc.FIXED_DELTA;

        if (player.targetangle != player.angle)
        {
            float delta = (player.targetangle - player.angle) / (float)player.granularity;
            player.angle += delta;
        }
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
