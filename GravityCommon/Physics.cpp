///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#include "Physics.h"




#include "Polygons.h"
#include <list>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include "ConfigLoader.h"
#include "Player.h"


#include <assert.h>

using namespace bali::vec;

namespace bali
{
namespace physics
{
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
        for (float a = 0.0f; a < 365.0f; a = a + 1.0f)
        {
            if (a == 90.f)
                continue;
            if (a == 180.f)
                continue;
            if (a == 270.f)
                continue;
            if (a == 360.f)
                continue;

            // A Ray will intersect with an obstacle or the maximum distance (that we cast)
            // hence, we always will have an intersection..but 
            // whether it's relevent or not is in the eye of the beholder.
            Intersection intersectMin;

            //pos.x -= 4;
            //pos.y += 8;

            pos.x -= 5.f;// ctx->playerpolygons.front().getLocalBounds().width / 1.0;//-8
            pos.y += 5.f;// ctx->playerpolygons.front().getLocalBounds().height / 1.0;//+5;


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
    return vec::mag(projection - vec::norm(p));
}

bool ciGreater(SAT::ContactInfo a, SAT::ContactInfo b)
{
    return a.overlap < b.overlap;
}

vec::VECTOR2 physics::ReflectUnitVector(vec::VECTOR2 d, vec::VECTOR2 n)
{
    vec::VECTOR2 v;
    v = d - 2 * (vec::dot(d, n))*n;
    return v;
}

bool physics::GetMinimumTranslationVector(
    bali::CONVEXSHAPES & shapes, 
    bali::CONVEXSHAPE & playerShape,
    vec::VECTOR2 vel,
    std::list<SAT::ContactInfo> & cinfos
)
{
    bool collision = false;
    SAT::ContactInfo hitMin;    
    for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
    {
        bool found = false;
        std::vector<SAT::ContactInfo> hitInfo;

        if (SAT::Shape::collision(playerShape, *shape, hitInfo))
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
                    if (vec::dot(vel, h->normal) < 0.01)
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
        else
        {
        }
    }
    return collision;
}

bool physics::CollisionResponse (
    RigidBody & phy,
    vec::VECTOR2 collision_normal,
    float overlap,
    PhysicsConfig & pc,
    bool playerMoving
)
{
    float newMag = 0.f;
    vec::VECTOR2 original_velocity = phy.vel;
    collision_normal = vec::norm(collision_normal);
    vec::VECTOR2 newVelocity;
    vec::VECTOR2 posDelta = collision_normal * overlap * 1.25f;
    
    physics::SubmitModifyPosition(phy, posDelta, false);
    newVelocity = collision_normal*(vec::dot(original_velocity, collision_normal)) * -1.f * (1 + pc.RESTITUTION) + original_velocity;

    assert(newVelocity != vec::Zero());
    assert(overlap != 0.0f);

    newMag = vec::mag(newVelocity);
    physics::SubmitModifyVelocity(phy, newVelocity, true);

    // https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table--gamedev-7756
    vec::VECTOR2 tangent = newVelocity - (collision_normal * vec::dot(newVelocity, collision_normal));
    tangent = vec::norm(tangent);
    float jt = vec::dot(newVelocity, tangent) * -1.0f;

    //
    // If velocity is low,
    // use static friction
    // otherwise, use dynamic
    // friction.
    if (playerMoving)
    {

    }
    else
    {
        //if (newMag < pc.STATIC_FRICTION_VELOCITY_MAX)
        //{
        vec::VECTOR2 rotated_collision_normal = rotVector(phy.angle, collision_normal);
        vec::VECTOR2 frictionvector = tangent * jt * pc.STATIC_FRICTION *( (vec::dot(rotated_collision_normal, rightVector(phy.angle))) /vec::mag(phy.vel));

        //vec::VECTOR2 frictionvector = tangent * jt * pc.STATIC_FRICTION;

        physics::SubmitModifyVelocity(phy, frictionvector, false);

    }
    //physics::ModifyAcceleration(phys, vec::Zero(), true);
    return true;
}

#undef PRINT_DIAGNOSTICS
bool physics::ResolveCollisions(Context::Ptr context, bali::CONVEXSHAPES & shapes, bali::CONVEXSHAPE & playerShape, std::vector<Segment> & sharedEdges, OnCollisionEvent onCollision, OnNonCollisionEvent onNonCollision)
{
    Player & player = context->entitymanager.player;
    RigidBody & phys = player.physical;
    bool collision = false;
    player.isCollidedLast = player.isCollided;
    player.isCollided = false;
    vec::VECTOR2 original_velocity = phys.vel;
    std::list<SAT::ContactInfo> cinfos;
    collision = GetMinimumTranslationVector(shapes, playerShape, original_velocity, cinfos);
    if (collision == false)
    {
        onNonCollision(context);
    }
    else
    {
        SAT::ContactInfo biggest;
        vec::VECTOR2  newPos(0.f, 0.f);
        float overlap = 0.0f;
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

            // Average all the collision normals together.
            int cnt = 0;
            for (auto cinfo = cinfos.begin(); cinfo != cinfos.end(); ++cinfo)
            {
                newPos += cinfo->normal;
            }

            player.isCollided = true;
            if (newPos != vec::VECTOR2(0, 0))
            {
                CollisionResponse(phys, newPos, overlap, context->physicsConfig, player.isMoving());
                onCollision(context, newPos);
            }
        }
    }
    return false;
}

void physics::UpdateMotion(RigidBody & phy, sf::Time elapsed, uint32_t is_collided, PhysicsConfig & pc, sf::Time & accumulator)
/*
    The main idea here is we increment the physics at fixed time deltas.
    Mostly, we should only be performing 1 fixed delta per update.
    But if for some reason we get temporary local lag,
    our accumulator will help us get the physics engine 
    caught back up because it tells us how many fixed deltas
    we are behind by.


    Each entity has a Command queue that is populated from various sources (keyboard input, collision response, etc)
    The commands in this queue are applied to the target entity here.
*/
{

    accumulator += elapsed;

    while (accumulator.asMilliseconds() > pc.FIXED_DELTA)
    {
        accumulator -= sf::seconds(pc.FIXED_DELTA);

        //
        // Integrate motion
        //
        Command cmd;
        vec::VECTOR2 velAcc;
        vec::VECTOR2 theVel;
        float ta = 0.f;

        while (physics::getNextCommand(phy, cmd))
        {
            phy.cmdHistory.push_back(cmd);
            switch (cmd.code)
            {
            case Command::Code::ACCELERATION:
                if (cmd.set) {
                    phy.accel = cmd.accel.accel;
                } else {
                    phy.accel += cmd.accel.accel;
                }
                break;
            case Command::Code::VELOCITY:
                if (cmd.set) {
                    phy.vel = cmd.vel.vel;
                } else {
                    phy.vel += cmd.vel.vel;
                }
                break;
            case Command::Code::POSITION:
                if (cmd.set) {
                    phy.pos = cmd.pos.pos;
                } else {
                    phy.pos += cmd.pos.pos;
                }
                break;
            case Command::Code::ANGLE:
                phy.angle = cmd.ang.angle;
                break;
            case Command::Code::MOVE:
                ApplyMove(cmd.mov, phy, pc.MOVE_STRENGTH, pc.FREEFALL_MOVE_STRENGTH, pc.MOVE_VELOCITY_MAX);
                break;
            case Command::Code::CHARGE:
                ApplyCharge(cmd.chg, phy, pc.MOVE_STRENGTH, pc.CHARGE_VELOCITY_MAX);
                break;
            case Command::Code::JUMP:
                ApplyJump(cmd.jmp, phy, pc.JUMP_STRENGTH, pc.JUMP_VELOCITY_MAX);
                break;
            }// end switch
        }
        //
        // Apply drag if physical is airborne.
        // But, only apply drag in the lateral directions.
        //
        if (phy.vel != vec::Zero())
        {
            ApplyDrag(phy, is_collided, pc.DRAG_CONSTANT);
        }

        //
        // semi-implicit euler
        //
        phy.accel = downVector(phy.angle) * pc.GRAVITY_CONSTANT;
        phy.vel += (phy.accel * pc.FIXED_DELTA);
        ClampUpperVector(phy.vel, pc.VELOCITY_MAX);
        phy.pos += phy.vel * pc.FIXED_DELTA;

    }
}

void physics::ClampUpperVector(vec::VECTOR2 & vel, float max)
{
    //
    // Limit Velocity
    //
    if (vel.x > max)
    {
        vel.x = max;
    }

    if (vel.x < -max)
    {
        vel.x = -max;
    }

    if (vel.y > max)
    {
        vel.y = max;
    }

    if (vel.y < -max)
    {
        vel.y = -max;
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
    vec::VECTOR2 v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 rotVector(float angle, vec::VECTOR2 v)
{
    float a = DEG_TO_RAD(angle);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::upVector(float angle)
{
    float a = DEG_TO_RAD(angle - 90.f);
    vec::VECTOR2 v(0, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::leftVector(float angle)
{
    //float a = DEG_TO_RAD(angle - 180 - LEFTUP);
    float a = DEG_TO_RAD(angle - 180.f);
    vec::VECTOR2 v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::downVector(float angle)
{
    float a = DEG_TO_RAD(angle - 270.f);
    vec::VECTOR2 v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

vec::VECTOR2 physics::rightVector(float angle)
{
    //float a = DEG_TO_RAD(angle - 360 + LEFTUP);
    float a = DEG_TO_RAD(angle - 360.f);
    vec::VECTOR2 v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

double getTimestamp()
{
    FILETIME t;
    GetSystemTimePreciseAsFileTime(&t);
    //uint64_t timestamp = ((t.dwHighDateTime << 32) | t.dwLowDateTime) / 10000000.0f;
    uint64_t timestamp = ((uint64_t)t.dwHighDateTime << 32) | (uint64_t)t.dwLowDateTime;
    timestamp = (uint64_t)((double)timestamp / 10000.0f);
    //std::cout << timestamp << std::endl;
    return timestamp;
}

void physics::SubmitModifyAcceleration(RigidBody & ph, vec::VECTOR2 a, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::ACCELERATION;
    newCmd.priority = static_cast<uint32_t>((set > 0 ? Command::Priority::ULTRA : Command::Priority::HIGH));
    newCmd.set = set;
    newCmd.accel.accel = a;

    ph.cmdQ.push(newCmd);
}

void physics::SubmitModifyVelocity(RigidBody & ph, vec::VECTOR2 v, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::VELOCITY;
    newCmd.priority = static_cast<uint32_t>((set > 0 ? Command::Priority::ULTRA : Command::Priority::HIGH));
    newCmd.set = set;
    newCmd.vel.vel = v;

    ph.cmdQ.push(newCmd);
}

void physics::SubmitModifyPosition(RigidBody & ph, vec::VECTOR2 p, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::POSITION;
    newCmd.priority = static_cast<uint32_t>((set > 0 ? Command::Priority::ULTRA : Command::Priority::HIGH));
    newCmd.set = set;
    newCmd.pos.pos = p;

    ph.cmdQ.push(newCmd);
}

void physics::SubmitModifyAngle(RigidBody & ph, float ta, uint32_t set)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::ANGLE;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::ULTRA);
    newCmd.set = set;
    newCmd.ang.angle = ta;

    ph.cmdQ.push(newCmd);
}

void physics::SubmitMove(RigidBody & ph, float str, vec::VECTOR2 dir, bool grounded)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::MOVE;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::MEDIUM);
    newCmd.mov.str = str;
    newCmd.mov.dir = dir;
    newCmd.mov.gnd = grounded;

    ph.cmdQ.push(newCmd);
}

void physics::SubmitCharge(RigidBody & ph, float str, vec::VECTOR2 dir, bool grounded)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::CHARGE;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::MEDIUM);
    newCmd.mov.str = str;
    newCmd.mov.dir = dir;
    newCmd.mov.gnd = grounded;

    ph.cmdQ.push(newCmd);
}

void physics::SubmitJump(RigidBody & ph, float str, vec::VECTOR2 dir)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::JUMP;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::MEDIUM);
    newCmd.jmp.str = str;
    newCmd.jmp.dir = dir;

    ph.cmdQ.push(newCmd);
}


void physics::SubmitModifyGravity(RigidBody & ph, float str, vec::VECTOR2 dir)
{
    Command newCmd;
    newCmd.timestamp = getTimestamp();
    newCmd.code = (uint32_t)Command::Code::GRAVITY;
    newCmd.priority = static_cast<uint32_t>(Command::Priority::ULTRA);
    newCmd.grv.str = str;
    newCmd.grv.dir = dir;

    ph.cmdQ.push(newCmd);
}

void physics::ApplyDrag(RigidBody & phy, bool is_collided, float drag_coefficient)
{
    if (!is_collided)
    {
        vec::VECTOR2 lv = leftVector(phy.angle);
        vec::VECTOR2 uv = upVector(phy.angle);
        vec::VECTOR2 dv = downVector(phy.angle);
        vec::VECTOR2 velnorm = vec::norm(phy.vel);
        float upStrength = vec::dot(uv, velnorm);
        float downStrength = vec::dot(dv, velnorm);

        //
        // Apply drag when in the air, and falling.
        // And, only apply to the lateral component 
        // of the velocity.
        //
        if (downStrength > upStrength)
        {
            vec::VECTOR2 drag = drag_coefficient * phy.vel;
            vec::VECTOR2 draglateral = vec::dot(drag, lv)* lv;
            phy.vel -= draglateral;
        }
    }
}

void physics::ApplyJump(Command::Jump & j, RigidBody & phy, float jump_strength, float jump_velocity_max)
{
    vec::VECTOR2 u;
    vec::VECTOR2 up = upVector(phy.angle) * 0.90f;
    if (j.dir != vec::Zero())
    {
        if (vec::dot(j.dir, downVector(phy.angle)) < 0.01f)
        {
            u = vec::norm(j.dir + up)  * j.str * jump_strength;
            u = phy.impulse(u);
            if (vec::mag(phy.vel + u) > jump_velocity_max)
            {
                u = vec::Zero();
            }
            phy.vel += u;
        }
    }
}

void physics::ApplyCharge(Command::Charge & chg, RigidBody & phy, float charge_strength, float charge_velocity_max)
{
    vec::VECTOR2 dir = vec::norm(chg.dir);
    vec::VECTOR2 m = dir * chg.str * charge_strength;
    m = phy.impulse(m);
    if (vec::mag(phy.vel + m) > charge_velocity_max)
    {
        m = vec::Zero();
    }
    phy.vel += m;
}

void physics::ApplyMove(Command::Move & mov, RigidBody & phy, float move_strength, float freefall_move_strength, float move_velocity_max)
{
    vec::VECTOR2 m;
    if (!mov.gnd)
    {
        if (mov.dir != vec::Zero())
        {
            vec::VECTOR2 dir = vec::norm(mov.dir);
            m = dir * mov.str * freefall_move_strength;
            m = phy.impulse(m);
        }
    }
    else
    {
        vec::VECTOR2 dir = vec::norm(mov.dir);
        m = dir * mov.str * move_strength;
        m = phy.impulse(m);
    }

    if (vec::mag(phy.vel + m) > move_velocity_max)
    {
        m = vec::Zero();
    }
    phy.vel += m;
}

bool physics::getNextCommand(RigidBody & ph, Command & c)
{
    bool status = false;
    if (!ph.cmdQ.empty())
    {
        c = ph.cmdQ.top();
        ph.cmdQ.pop();
        status = true;
    }
    return status;
}


void SaveCommandHistory(std::string file, std::list<Command> & history)
{
    std::ofstream fout;
    fout.open(file, std::ios::binary);

    if (fout.is_open())
    {
        for (auto i = history.begin(); i != history.end(); i++)
        {
            //fout << i->timestamp;
            //fout << i->code;
            //fout << i->priority;
            //fout << i->set;
        }
    }
    else
    {
        // problem opening output file
    }
}

void LoadCommandHistory(std::string file, std::list<Command> & history)
{

}














}
}
