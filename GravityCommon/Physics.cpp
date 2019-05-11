///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#include "Physics.h"
#include <list>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <random>
#include "ConfigLoader.h"
#include "Player.h"
#include "Builders.h"

#include <assert.h>

#define CORRECTION_FACTOR 2.0f

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


sf::Vector2f ReflectUnitVector(sf::Vector2f d, sf::Vector2f n)
{
    sf::Vector2f v;
    v = d - 2 * (vec::dot(d, n))*n;
    return v;
}

bool CorrectPosition(
    RigidBody & rb,
    sf::Vector2f collision_normal,
    float overlap
)
{
    sf::Vector2f posDelta = collision_normal * overlap;

    CommandQueue::postModifyPosition(rb, posDelta, false);
    return true;
}


sf::Vector2f
CollisionResponseEntity(
    RigidBody & rb,
    RigidBody & orb,
    sf::Vector2f cn,
    float overlap,
    PhysicsConfig & pc
)
{
    float ma = 1.0f / rb.mass;
    float mb = 1.0f / orb.mass;
    sf::Vector2f Vrel = rb.vel - orb.vel;                // Relative velocity (N.B. - walls have 0 velocity)
    float velAlongNormal = vec::dot(Vrel, cn);  // Magnitude of velocity, at impact, along collision normal.
    float e = pc.RESTITUTION;
    // Skip response if moving away
    if (velAlongNormal > 0)
        return vec::Zero();// rb.vel;
    sf::Vector2f VR = Vrel - cn*velAlongNormal*(2.0f);
    // magnitude of velocity along collision normal
    // after restitution is taken into account
    float j = -(1 + e) * velAlongNormal;
    j = j / ma;
    sf::Vector2f jn = cn * j;
    // and to turn it into a force magnitude, account for mass

    // the force applied along the collision normal
    sf::Vector2f Vnew = (VR * j * ma);

}

sf::Vector2f
ApplyImpulse(
    sf::Vector2f v,
    float j,
    sf::Vector2f n
)
{
    return (v + (n * j));
}

sf::Vector2f
CollisionResponseWall(
    RigidBody & rb,
    float omass,
    sf::Vector2f ovel,
    sf::Vector2f cn,
    float overlap,
    bool EntityActive,
    PhysicsConfig & pc
)
{
    ////////// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table--gamedev-7756
    float ma = 1.0f / rb.mass;
    float mb = (omass == 0 ? 0 : 1.0f / omass);
    sf::Vector2f Vrel = rb.vel - ovel;          // Relative velocity (N.B. - walls have 0 velocity)
    float velAlongNormal = vec::dot(Vrel, cn);  // Magnitude of velocity, at impact, along collision normal.
    float e = pc.RESTITUTION;

    // Skip response if moving away
    if (velAlongNormal >= 0)
        return vec::Zero();

    // magnitude of velocity along collision normal
    // after restitution is taken into account
    float j = -(1 + e) * velAlongNormal;
    j = j / (ma+mb);
    sf::Vector2f jcn = cn * j;

    // the force applied along the collision normal
    sf::Vector2f totalresponse = (jcn * ma);

    // Add friction to the mix
    // IFF entity does not intend to move.
    if (!EntityActive)
    {
        
        sf::Vector2f t = Vrel - cn*velAlongNormal;
        
        if (vec::mag(t) >0)
        {
            t = vec::norm(t);
            float velAlongTangent = vec::dot(Vrel, t);
            float jt = -(1 + e) * (velAlongTangent / (ma + mb));
            float absjt = abs(jt);
            sf::Vector2f friction;
            if (absjt < j * pc.STATIC_FRICTION)
            {
                friction = t * jt;
                //std::cout << std::setprecision(3) <<"1) " << absjt << " < " << j * pc.STATIC_FRICTION << std::endl;
            }
            else
            {
                friction =  t * -j * pc.DYNAMIC_FRICTION;
                //std::cout << "2";
                //std::cout << std::setprecision(3) << "2) " << absjt << " < " << j * pc.STATIC_FRICTION << std::endl;
            }
            //if (vec::mag(friction) > 1.0f)
            totalresponse = totalresponse + (friction * ma);
        }
    }
    return totalresponse;
}

#define PRINT_DIAGNOSTICS

void
GetEntityEntityContacts(
    Vec<Entity> & entities,
    Vec<SAT::ContactInfo> & contacts
)
{
    for (int j = 0; j < entities.size(); j++)
    {
        for (int e = 0; e < entities[j].collisionentities.size(); e++)
        {
            std::vector<SAT::ContactInfo> tempContacts;
            size_t index = entities[j].collisionentities[e];
            if (j == index)
                continue;
            if (SAT::Collision::iscollided(entities[j].proto.shapes[0],
                entities[index].proto.shapes[0],
                tempContacts))
            {
                tempContacts.back().thisindex = j;
                tempContacts.back().thatindex = index;
                contacts.push_back(tempContacts.back());
            }
        }
    }
}

void
GetEntityWallContacts(
    Vec<Entity> & entities,
    Vec<Shape> & collisionshapes,
    Vec<SAT::ContactInfo> & contacts
)
{
    for (int j = 0; j < entities.size(); j++)
    {
        for (int p = 0; p < entities[j].collisionshapes.size(); p++)
        {
            std::vector<SAT::ContactInfo> tempContacts;
            size_t index = entities[j].collisionshapes[p];
            if (SAT::Collision::iscollided(entities[j].proto.shapes[0],
                                      collisionshapes[index],
                                      tempContacts))
            {
                /*sf::Vector2f seg = tempContacts.back().seg.end - tempContacts.back().seg.start;
                float mag = vec::dot(seg, downVector(entities[j].proto.body.angle));
                if (mag )*/
                //if (abs(tempContacts.back().overlap) > 0.9f)
                //{
                    tempContacts.back().thisindex = j;
                    tempContacts.back().thatindex = -1;
                    contacts.push_back(tempContacts.back());
                //}
            }
        }
    }
}

bool GreatestOverlap(SAT::ContactInfo a, SAT::ContactInfo b)
{
    // descending order, largest to smallest.
    return a.overlap < b.overlap;
}

void
ProcessAllContacts(
    Context* context,
    Vec<Entity> & entities,
    Vec<SAT::ContactInfo> & contacts,
    OnCollisionEvent onCollision,
    OnNonCollisionEvent onNonCollision,
    PhysicsConfig & pc
)
{
    std::sort(contacts.begin(),
        contacts.end(),
        GreatestOverlap);

    /*std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(context->allcontacts.begin(), context->allcontacts.end(), generator);*/

    for (int j = 0; j < contacts.size(); j++)
    {
        SAT::ContactInfo & contact = contacts[j];

        if (contact.overlap < 0.01)
            contact.overlap = 0.f;
        Entity* thisEntity = &entities[contact.thisindex];
        float thatmass = 0.f;
        sf::Vector2f thatvel;
        sf::Vector2f posDelta;
        float overlap;
        if (contact.thatindex != -1)
        {
            if (thisEntity->ignoreentitycollision) {
                continue;
            }

            Entity* thatEntity = &entities[contact.thatindex];
            thatmass = thatEntity->proto.body.mass;
            thatvel = thatEntity->proto.body.vel;
            overlap = contact.overlap;// *0.8f;
        }
        else
        {
            thatmass = 0.0f;
            overlap = contact.overlap*1.0f;
        }
        posDelta = contact.normal * overlap;
        sf::Vector2f velDelta = CollisionResponseWall(thisEntity->proto.body,
            thatmass,
            thatvel,
            contact.normal,
            overlap,
            thisEntity->isActive(),
            pc);
        //if (vec::mag(posDelta) > 0.001)
        {
            CommandQueue::postModifyPosition(thisEntity->proto.body,
                posDelta,
                false);
        }
        physics::updateRigidBodyInternal(thisEntity->proto.body, pc);
        //if (vec::mag(velDelta) > 0.001)
        {
            CommandQueue::postModifyVelocity(thisEntity->proto.body,
                velDelta,
                false);
                //thisEntity->proto.body.vel + velDelta,
                //true);
        }
       // thisEntity->avgForce = thisEntity->avgForce + velDelta;
        //thisEntity->numForce++;

        physics::updateRigidBodyInternal(thisEntity->proto.body, pc);
        thisEntity->proto.shapes[0].position = thisEntity->proto.body.pos;
        //if (contact.thatindex == -1)
            onCollision(context, *thisEntity, contact.normal);

    }
}

sf::Vector2f 
lerp(
    sf::Vector2f start,
    sf::Vector2f end,
    float t
)
{
    return start + (end * t);
}


bool
ResolveAllCollisions(
    Context* context,
    OnCollisionEvent onCollision, 
    OnNonCollisionEvent onNonCollision,
    PhysicsConfig & pc
)
{
    bool collision = false;
    bool done = false;
    bool integrated = false;
    int iter = 0;
    std::vector<SAT::ContactInfo> entityContacts;
    std::vector<SAT::ContactInfo> wallContacts;
    sf::Time framequant = context->frameacc + context->frametime;

    while (framequant >= sf::seconds(pc.FIXED_DELTA) && !context->paused)
    {
        if (framequant > sf::seconds(pc.FIXED_DELTA)*20.f)
        {
            framequant = sf::seconds(pc.FIXED_DELTA)*20.f;
            std::cout << "Death Spiral Detected!" << std::endl;
        }
        framequant -= sf::seconds(pc.FIXED_DELTA);
        integrated = true;
        for (int e = 0; e < context->entities.size(); e++)
        {
            /////////// Integrate ///////////
            updateRigidBodyInternal(context->entities[e].proto.body, pc);
            //if (e == 0)
                integrateEuler(context->entities[e].proto.body, pc);
            /////////////////////////////////

            // Reinitialize 1 pass entity structures.
            context->entities[e].collisionentities.clear();

            /////////////////
            sf::Vector2f newpos = context->entities[e].proto.body.pos;
            context->entities[e].proto.shapes[0].position = newpos;
        }
        uint32_t maxiters = 2;
        uint32_t curiters = 0;
        do {
            // Reinitialize 1 pass global structures
            context->allcontacts.clear();
            context->entitybuckets.clear();

            //
            //
            // collect the collision polygons visible to each entity
            //
            GetCPolyNeighbors(context,
                context->entities,
                context->cpolybuckets);


            //
            // Build Spatial Buckets for entities
            // We rebuild the entire thing every update
            //
            CreateEntityBucket(context->entities,
                context->entitybuckets);
            //
            // Using newly create bucket, Locate nearest neighbors for each entity
            //
            GetEntityNeighbors(context->entities,
                context->entitybuckets);

            //
            // Collect collision contacts for entity to entity
            //
            GetEntityEntityContacts(context->entities,
                context->allcontacts);

            //
            // Collect collision contacts for entity-to-wall
            //
            GetEntityWallContacts(context->entities,
                context->allcollisionshapes,
                context->allcontacts);

            ProcessAllContacts(context,
                context->entities,
                context->allcontacts,
                onCollision, onNonCollision,
                pc);
            curiters++;
        } while (context->allcontacts.size() > 0 && curiters < maxiters);
    }
    context->frameacc = framequant; // store the left over time for next iteration.
    if (integrated)
    {
        context->shadowcopy = context->entities;
    }
    else
    {
        for (Entity & e : context->shadowcopy)
        {
            /////////////////
            e.proto.body.pos = physics::lerp(e.proto.body.pos, e.proto.body.vel, context->frametime.asSeconds());
            e.proto.shapes[0].position = e.proto.body.pos;
        }
    }
    return true;
}


void
updateRigidBodyInternal(
    RigidBody & rb,
    PhysicsConfig & pc
)
{
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
            if (cmd.set)
            {
                rb.angle = cmd.ang.angle;
            }
            else {
                rb.angle += cmd.ang.angle;
            }
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
        //rb.cmdqueue.execseq.push_back(cmd);
    }

    rb.lerppos = rb.pos;
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

    //accumulator += elapsed;

    while (accumulator.asMilliseconds() > pc.FIXED_DELTA)
    {
        accumulator -= sf::seconds(pc.FIXED_DELTA);

        updateRigidBodyInternal(rb, pc);

        integrateEuler(rb, pc);

    }
}

void
integrateEuler(
    RigidBody & rb,
    PhysicsConfig & pc
)
{
    //
    // Apply drag if physical is airborne.
    // But, only apply drag in the lateral directions.
    //
    //if (rb.vel != vec::Zero())

    //if (!rb.collider.isCollided[0] && !rb.collider.isCollided[1]) {
    //    //rb.vel -= physics::ApplyDrag(rb, pc.DRAG_CONSTANT);
    //}

    //
    // semi-implicit euler
    //
    rb.accel = (physics::downVector(rb.angle) * pc.GRAVITY_CONSTANT);
    rb.vel += (rb.accel * pc.FIXED_DELTA) * (float)rb.mass;
    ClampUpperVector(rb.vel, pc.VELOCITY_MAX);

    /*int64_t vx = (int64_t )rb.vel.x;
    int64_t vy = (int64_t )rb.vel.y;
    rb.vel.x = vx;
    rb.vel.y = vy;*/
#include <math.h>
    rb.vel.x = ceilf(rb.vel.x);
    rb.vel.y = ceilf(rb.vel.y);

    rb.pos += rb.vel * pc.FIXED_DELTA;
    //rb.pos.x = roundf(rb.pos.x);
    //rb.pos.y = roundf(rb.pos.y);

}

void ClampUpperVector(sf::Vector2f & vel, float max)
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

sf::Vector2f rotVector(float angle)
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

sf::Vector2f upVector(float angle)
{
    float a = DEG_TO_RAD(angle - 90.f);
    sf::Vector2f v(0, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f leftVector(float angle)
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

sf::Vector2f downVector(float angle)
{
    float a = DEG_TO_RAD(angle - 270.f);
    sf::Vector2f v(0.f, 1.f);
    float m = vec::mag(v);
    v.x = m * cos(a);
    v.y = m * sin(a);
    v = vec::norm(v);

    return v;
}

sf::Vector2f rightVector(float angle)
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
ApplyDrag(RigidBody & phy, float drag_coefficient)
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
ApplyJump(Command::Jump & j, RigidBody & phy, float jump_strength, float jump_velocity_max)
{
    sf::Vector2f u;
    sf::Vector2f up = upVector(phy.angle) * 0.90f;
    if (j.dir != vec::Zero())
    {
        u = vec::norm(j.dir) * jump_strength;
        if (vec::dot(up, phy.vel) < 0)
        {
            u = u - phy.vel;
        }
    }
    return u;
}

sf::Vector2f
ApplyCharge(Command::Charge & chg, RigidBody & phy, float charge_strength, float charge_velocity_max)
{
    sf::Vector2f dir = vec::norm(chg.dir);
    sf::Vector2f m = dir * chg.str * charge_strength;
    //m = phy.impulse(m);
    if (vec::mag(phy.vel + m) > charge_velocity_max)
    {
        m = vec::Zero();
    }
    //phy.vel += m;
    return m;
}

sf::Vector2f
ApplyMove(Command::Move & mov, RigidBody & phy, float move_strength, float freefall_move_strength, float move_velocity_max)
{
    sf::Vector2f m;
    if (!mov.gnd)
    {
        //if (mov.dir != vec::Zero())
        {
            sf::Vector2f dir = vec::norm(mov.dir);
            m = dir * mov.str * freefall_move_strength;// *(1.0f / phy.mass);
            m = phy.impulse(m);
        }
    }
    else
    {
        sf::Vector2f dir = vec::norm(mov.dir);
        m = dir * mov.str * move_strength;// *(1.0f / phy.mass);
        m = phy.impulse(m);
    }

    //if (vec::mag(phy.vel + m) > move_velocity_max)
    //{
    //    m = vec::norm(phy.vel) * move_velocity_max;
    //}
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

//
// Find intersection of RAY & SEGMENT
bool getIntersection(sf::Vector2f rayStart, sf::Vector2f ray, Segment segment, Intersection & intersection)
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
    intersection.rayPoint = sf::Vector2f(r_px + r_dx*T1, r_py + r_dy*T1);
    intersection.segPoint = sf::Vector2f(s_px + s_dx*T2, s_py + s_dy*T2);
    intersection.segment = segment;
    intersection.distance = rayStart - intersection.segPoint;

    return true;
}

bool RayCast(sf::Vector2f dir, sf::Vector2f start, std::vector<Segment> & segments, Intersection & intersect)
{
    bool found = false;
    dir = dir / 8.0f;// This is important. is it?
                     ///PHYSPRINT("[Ang](" << angle << ")");
    float closest = 999999.9;
    for (auto seg = segments.begin(); seg != segments.end(); ++seg)
    {
        Intersection intersectTemp;
        intersectTemp.expired = true;
        intersectTemp.rayPoint = start + dir * 3400.0f;
        intersectTemp.distance = intersectTemp.rayPoint - start;
        intersectTemp.ray = dir;
        physics::getIntersection(start, dir, *seg, intersectTemp);
        if (vec::mag(intersectTemp.distance) <= closest)
        {
            closest = vec::mag(intersectTemp.distance);

            intersect = intersectTemp;
            intersect.segment = *seg;
            intersect.expired = false;
            found = true;
        }
    }
    return found;
}

//bool RayCast(float a, sf::Vector2f start, std::vector<Segment> & segments, physics::Intersection & intersect)
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












}
}

