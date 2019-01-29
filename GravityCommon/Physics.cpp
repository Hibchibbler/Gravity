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
//
//// Find intersection of RAY & SEGMENT
//bool physics::getIntersection(sf::Vector2f rayStart, sf::Vector2f ray, Segment segment, physics::Intersection & intersection)
//{
//    // RAY in parametric: Point + Delta*T1
//    float r_px = rayStart.x;
//    float r_py = rayStart.y;
//    float r_dx = ray.x;
//    float r_dy = ray.y;
//    // SEGMENT in parametric: Point + Delta*T2
//    float s_px = segment.start.x;// +segment.off.x;
//    float s_py = segment.start.y;// +segment.off.y;
//    float s_dx = segment.end.x - segment.start.x;
//    float s_dy = segment.end.y - segment.start.y;
//    // Are they parallel? If so, no intersect
//    float r_mag = sqrt(r_dx*r_dx + r_dy*r_dy);
//    float s_mag = sqrt(s_dx*s_dx + s_dy*s_dy);
//
//    if (r_dx / r_mag == s_dx / s_mag && r_dy / r_mag == s_dy / s_mag) {
//        // Unit vectors are the same.
//        return false;
//    }
//    // SOLVE FOR T1 & T2
//    float det = (s_dx*r_dy - s_dy*r_dx);
//    float T2 = (r_dx*(s_py - r_py) + r_dy*(r_px - s_px)) / det;
//    float T1 = (s_px + s_dx*T2 - r_px) / r_dx;
//
//    // Must be a piece of the ray that is moving away from the origin
//    if (T1 < 0)         return false;
//    // Must be within the domain of the segment
//    if (T2 < 0 || T2>1) return false;
//
//    // Return Intersection information
//    intersection.time = T1;
//    intersection.rayPoint = sf::Vector2f(r_px + r_dx*T1, r_py + r_dy*T1);
//    intersection.segPoint = sf::Vector2f(s_px + s_dx*T2, s_py + s_dy*T2);
//    intersection.segment = segment;
//    intersection.distance = rayStart - intersection.segPoint;
//
//    return true;
//}
//
//bool physics::RayCast(sf::Vector2f dir, sf::Vector2f start, std::vector<Segment> & segments, physics::Intersection & intersect)
//{
//    bool found = false;
//    dir = dir / 8.0f;// This is important. is it?
//                     ///PHYSPRINT("[Ang](" << angle << ")");
//    float closest = 999999.9;
//    for (auto seg = segments.begin(); seg != segments.end(); ++seg)
//    {
//        physics::Intersection intersectTemp;
//        intersectTemp.expired = true;
//        intersectTemp.rayPoint = start + dir * 3400.0f;
//        intersectTemp.distance = intersectTemp.rayPoint - start;
//        intersectTemp.ray = dir;
//        physics::getIntersection(start, dir, *seg, intersectTemp);
//        if (vec::mag(intersectTemp.distance) <= closest)
//        {
//            closest = vec::mag(intersectTemp.distance);
//            intersect = intersectTemp;
//            intersect.expired = false;
//            found = true;
//        }
//    }
//    return found;
//}
//
//bool physics::RayCast(float a, sf::Vector2f start, std::vector<Segment> & segments, physics::Intersection & intersect)
//{
//    float radians = a * (PI / 180.0f);
//
//    sf::Vector2f dir(1.0f*cos(radians),
//        1.0f*sin(radians));
//
//    return physics::RayCast(dir, start, segments, intersect);
//}
//
//void physics::createLoSTriFan(std::vector<CONVEXSHAPE> & shapes, sf::Vector2f pos, sf::VertexArray & lineSegments)
//{
//    // We will shoot a ray for each degree of rotation around position.
//    sf::Vector2f posOrg = pos;
//    std::vector<Segment> segments;
//    for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
//    {
//        std::vector<Segment> s = bali::SAT::Shape::getSegments(*shape);
//        segments.insert(segments.end(), s.begin(), s.end());
//    }
//    {
//        //angle - starts at 
//        //   3 O'  Clock = 0   degree
//        //   6 O'  Clock = 90  degree
//        //   9 O'  Clock = 180 degree
//        //   12 O' Clock = 270 degree
//        //   3 O'  Clock = 360 degree
//
//        std::vector<physics::Intersection> intersections;
//        for (float a = 0.0f; a < 365.0f; a = a + 1.0f)
//        {
//            if (a == 90.f)
//                continue;
//            if (a == 180.f)
//                continue;
//            if (a == 270.f)
//                continue;
//            if (a == 360.f)
//                continue;
//
//            // A Ray will intersect with an obstacle or the maximum distance (that we cast)
//            // hence, we always will have an intersection..but 
//            // whether it's relevent or not is in the eye of the beholder.
//            Intersection intersectMin;
//
//            //pos.x -= 4;
//            //pos.y += 8;
//
//            pos.x -= 5.f;// ctx->playerpolygons.front().getLocalBounds().width / 1.0;//-8
//            pos.y += 5.f;// ctx->playerpolygons.front().getLocalBounds().height / 1.0;//+5;
//
//
//            RayCast(a, pos, segments, intersectMin);
//            intersections.push_back(intersectMin);
//        }
//
//        lineSegments.clear();
//        lineSegments = sf::VertexArray(sf::PrimitiveType::TrianglesFan);//Lines);
//        sf::Vector2f positionFixed = posOrg;
//        lineSegments.append(positionFixed);
//
//        for (auto i = intersections.begin(); i != intersections.end(); ++i)
//        {
//            if (true)// !i->expired)
//            {
//                i->rayPoint += i->ray * 100.0f;
//                lineSegments.append(i->rayPoint);
//            }
//        }
//    }
//}
//
//
//#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
//#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
//float mindist(sf::Vector2f v, sf::Vector2f w, sf::Vector2f p) {
//    // Return minimum distance between line segment vw and point p
//    const float l2 = vec::mag(w - v);//length_squared(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
//    if (l2 == 0.0) return (vec::mag(p - v));//distance(p, v);   // v == w case
//                                            // Consider the line extending the segment, parameterized as v + t (w - v).
//                                            // We find projection of point p onto the line. 
//                                            // It falls where t = [(p-v) . (w-v)] / |w-v|^2
//                                            // We clamp t from [0,1] to handle points outside the segment vw.
//    const float t = max(0, min(1, vec::dot(p - v, w - v) / l2));
//    sf::Vector2f projection = v + t * (w - v);  // Projection falls on the segment
//    return vec::mag(projection - vec::norm(p));
//}
//

sf::Vector2f physics::ReflectUnitVector(sf::Vector2f d, sf::Vector2f n)
{
    sf::Vector2f v;
    v = d - 2 * (vec::dot(d, n))*n;
    return v;
}

bool physics::CollisionResponse (
    RigidBody & rb,
    sf::Vector2f collision_normal,
    float overlap,
    bool playerMoving,
    PhysicsConfig & pc
)
{
    float newMag = 0.f;
    sf::Vector2f original_velocity = rb.vel;
    collision_normal = vec::norm(collision_normal);
    sf::Vector2f newVelocity;
    sf::Vector2f posDelta = collision_normal * overlap * 1.0f;

    CommandQueue::postModifyPosition(rb, posDelta, false);
    newVelocity = collision_normal*(vec::dot(original_velocity, collision_normal)) * -1.f * (1 + pc.RESTITUTION) + original_velocity;
    assert(newVelocity != vec::Zero());
    assert(overlap != 0.0f);

    newMag = vec::mag(newVelocity);
    CommandQueue::postModifyVelocity(rb, newVelocity, true);

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
        ////if (newMag < pc.STATIC_FRICTION_VELOCITY_MAX)
        ////{
        //// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table--gamedev-7756
        //sf::Vector2f tangent = newVelocity - (collision_normal * vec::dot(newVelocity, collision_normal));
        //tangent = vec::norm(tangent);
        //float jt = vec::dot(newVelocity, tangent) * -1.0f;

        //sf::Vector2f rotated_collision_normal = rotVector(rb.angle, collision_normal);
        //sf::Vector2f frictionvector = tangent * jt * pc.STATIC_FRICTION *( (vec::dot(rotated_collision_normal, rightVector(rb.angle))) /vec::mag(rb.vel));

        ////sf::Vector2f frictionvector = tangent * jt * pc.STATIC_FRICTION;

        //CommandQueue::postModifyVelocity(rb, frictionvector, false);

    }
    //physics::ModifyAcceleration(phys, vec::Zero(), true);
    return true;
}


#define PRINT_DIAGNOSTICS
bool physics::ResolvePolygonCollisions(Entity & entity, Vec<Shape> & collisionpolygons, OnCollisionEvent onCollision, OnNonCollisionEvent onNonCollision, void* ud, PhysicsConfig & pc)
{
    bool collision = false;
    std::vector<SAT::ContactInfo> contacts;

    entity.collider.isCollided[1] = entity.collider.isCollided[0];
    entity.collider.isCollided[0] = false;

    for (auto oent = collisionpolygons.begin(); oent != collisionpolygons.end(); ++oent)
    {
        if (SAT::Shape::collision(entity.proto.shapes[0], *oent, contacts)) {
            entity.collider.isCollided[0] = collision;
            CollisionResponse(entity.proto.body, contacts.back().normal, contacts.back().overlap, entity.isActive(), pc);
            onCollision(ud, entity, contacts.back().normal);
            return true;
        }
        else {
            onNonCollision(ud, entity);
        }
    }
    return collision;
}
bool physics::ResolveProtoCollisions(Entity & entity, Vec<Entity> & otherentities, Vec<Proto> & protos, OnCollisionEvent onCollision, OnNonCollisionEvent onNonCollision, void* ud, PhysicsConfig & pc)
{
    bool collision = false;
    std::vector<SAT::ContactInfo> contacts;

    entity.collider.isCollided[1] = entity.collider.isCollided[0];
    entity.collider.isCollided[0] = false;

    for (auto oent = otherentities.begin(); oent != otherentities.end(); ++oent)
    {
        if (&entity == &(*oent)) {
            continue;
        }
        if (SAT::Shape::collision(entity.proto.shapes[0], oent->proto.shapes[0], contacts)) {
            entity.collider.isCollided[0] = collision;
            CollisionResponse(entity.proto.body, contacts.back().normal, contacts.back().overlap / 3.0f, entity.isActive(), pc);
            onCollision(ud, entity, contacts.back().normal);
            return true;
        }
        else {
            onNonCollision(ud, entity);
        }
    }
    return collision;
}


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
void
updateRigidBody(
    RigidBody & rb,
    sf::Time elapsed,
    PhysicsConfig & pc,
    sf::Time & accumulator
)
{

    accumulator += elapsed;

    while (accumulator.asMilliseconds() > pc.FIXED_DELTA)
    {
        accumulator -= sf::seconds(pc.FIXED_DELTA);

        //
        // Integrate motion
        //
        Command cmd;
        sf::Vector2f velAcc;
        sf::Vector2f theVel;
        float ta = 0.f;

        while (rb.cmdqueue.nextCommand(rb, cmd))
        {
            switch (cmd.code)
            {
            case Command::Code::ACCELERATION:
                if (cmd.set) {
                    rb.accel = cmd.accel.accel;
                }
                else {
                    rb.accel += cmd.accel.accel;
                }
                break;
            case Command::Code::VELOCITY:
                if (cmd.set) {
                    rb.vel = cmd.vel.vel;
                }
                else {
                    rb.vel += cmd.vel.vel;
                }
                break;
            case Command::Code::POSITION:
                if (cmd.set) {
                    rb.pos = cmd.pos.pos;
                }
                else {
                    rb.pos += cmd.pos.pos;
                }
                break;
            case Command::Code::ANGLE:
                rb.angle = cmd.ang.angle;
                break;
            case Command::Code::MOVE:
                rb.vel += physics::ApplyMove(cmd.mov, rb, pc.MOVE_STRENGTH, pc.FREEFALL_MOVE_STRENGTH, pc.MOVE_VELOCITY_MAX);
                break;
            case Command::Code::CHARGE:
                rb.vel += physics::ApplyCharge(cmd.chg, rb, pc.MOVE_STRENGTH, pc.CHARGE_VELOCITY_MAX);
                break;
            case Command::Code::JUMP:
                rb.vel += physics::ApplyJump(cmd.jmp, rb, pc.JUMP_STRENGTH, pc.JUMP_VELOCITY_MAX);
                break;
            }// end switch
        }
        //
        // Apply drag if physical is airborne.
        // But, only apply drag in the lateral directions.
        //
        //if (rb.vel != vec::Zero())

        if (!rb.collider.isCollided[0] && !rb.collider.isCollided[1]) {
            //rb.vel -= physics::ApplyDrag(rb, pc.DRAG_CONSTANT);
        }

        //
        // semi-implicit euler
        //
        rb.accel = physics::downVector(rb.angle) * pc.GRAVITY_CONSTANT;
        rb.vel += (rb.accel * pc.FIXED_DELTA);
        //ClampUpperVector(rb.vel, pc.VELOCITY_MAX);
        rb.pos += rb.vel * pc.FIXED_DELTA;

    }
}

void physics::ClampUpperVector(sf::Vector2f & vel, float max)
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

//physics::Intersection::Intersection()
//{
//    time = 0.0f;
//    rayPoint = sf::Vector2f(0.0f, 0.0f);
//    segPoint = sf::Vector2f(0.0f, 0.0f);
//    angle = 0;
//    expired = true;
//    segment = Segment(rayPoint, rayPoint, rayPoint);
//}

sf::Vector2f physics::rotVector(float angle)
{
    float a = DEG_TO_RAD(angle);
    sf::Vector2f v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f rotVector(float angle, sf::Vector2f v)
{
    float a = DEG_TO_RAD(angle);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f physics::upVector(float angle)
{
    float a = DEG_TO_RAD(angle - 90.f);
    sf::Vector2f v(0, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f physics::leftVector(float angle)
{
    //float a = DEG_TO_RAD(angle - 180 - LEFTUP);
    float a = DEG_TO_RAD(angle - 180.f);
    sf::Vector2f v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f physics::downVector(float angle)
{
    float a = DEG_TO_RAD(angle - 270.f);
    sf::Vector2f v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f physics::rightVector(float angle)
{
    //float a = DEG_TO_RAD(angle - 360 + LEFTUP);
    float a = DEG_TO_RAD(angle - 360.f);
    sf::Vector2f v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f
physics::ApplyDrag(RigidBody & phy, float drag_coefficient)
{
    sf::Vector2f draglateral;
    sf::Vector2f lv = leftVector(phy.angle);
    sf::Vector2f uv = upVector(phy.angle);
    sf::Vector2f dv = downVector(phy.angle);
    sf::Vector2f velnorm = vec::norm(phy.vel);
    float upStrength = vec::dot(uv, velnorm);
    float downStrength = vec::dot(dv, velnorm);

    //
    // Apply drag when in the air, and falling.
    // And, only apply to the lateral component 
    // of the velocity.
    //
    if (downStrength > upStrength)
    {
        sf::Vector2f drag = drag_coefficient * phy.vel;
        draglateral = vec::dot(drag, lv)* lv;
        //phy.vel -= draglateral;
    }
    return draglateral;
}

sf::Vector2f
physics::ApplyJump(Command::Jump & j, RigidBody & phy, float jump_strength, float jump_velocity_max)
{
    sf::Vector2f u;
    sf::Vector2f up = upVector(phy.angle) * 0.90f;
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
            //phy.vel += u;
        }
    }
    return u;
}

sf::Vector2f
physics::ApplyCharge(Command::Charge & chg, RigidBody & phy, float charge_strength, float charge_velocity_max)
{
    sf::Vector2f dir = vec::norm(chg.dir);
    sf::Vector2f m = dir * chg.str * charge_strength;
    m = phy.impulse(m);
    if (vec::mag(phy.vel + m) > charge_velocity_max)
    {
        m = vec::Zero();
    }
    //phy.vel += m;
    return m;
}

sf::Vector2f
physics::ApplyMove(Command::Move & mov, RigidBody & phy, float move_strength, float freefall_move_strength, float move_velocity_max)
{
    sf::Vector2f m;
    if (!mov.gnd)
    {
        if (mov.dir != vec::Zero())
        {
            sf::Vector2f dir = vec::norm(mov.dir);
            m = dir * mov.str * freefall_move_strength;
            m = phy.impulse(m);
        }
    }
    else
    {
        sf::Vector2f dir = vec::norm(mov.dir);
        m = dir * mov.str * move_strength;
        m = phy.impulse(m);
    }

    if (vec::mag(phy.vel + m) > move_velocity_max)
    {
        m = vec::Zero();
    }
    //phy.vel += m;
    return m;
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
