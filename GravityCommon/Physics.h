///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <list>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iostream>
#include "SATAlgo/SATAlgo.h"
#include "Context.h"
#include "ConfigLoader.h"
#include "MouseKeyboard.h"

#ifdef PHYSICS_DEBUG
#define PHYSPRINT(x) \
    std::cout << x << std::endl;
#endif
#define EPSILON 0.0000001f

#define DEG_TO_RAD(x)   \
        (x) * (PI / 180.0f);

namespace bali
{
namespace physics
{

//
// Frame of References
//
sf::Vector2f rotVector(float angle);
sf::Vector2f rotVector(float angle, sf::Vector2f v);
sf::Vector2f upVector(float angle);
sf::Vector2f leftVector(float angle);
sf::Vector2f downVector(float angle);
sf::Vector2f rightVector(float angle);

//
// Ray stuff
//
bool getIntersection(sf::Vector2f rayStart, sf::Vector2f ray, Segment segment, Intersection & intersection);
bool RayCast(sf::Vector2f dir, sf::Vector2f start, std::vector<Segment> & segments, Intersection & intersect);
//bool RayCast(float a, sf::Vector2f start, std::vector<Segment> & segments, physics::Intersection & intersect);
//void createLoSTriFan(std::vector<CONVEXSHAPE> & shapes, sf::Vector2f pos, sf::VertexArray & lineSegments);

typedef void(*OnCollisionEvent) (void* context, Entity & entity, sf::Vector2f collisionNormal);
typedef void(*OnNonCollisionEvent) (void* context, Entity & entity);

void
integrateEuler(
    RigidBody & rb,
    PhysicsConfig & pc
);

void
updateRigidBodyInternal(
    RigidBody & rb,
    PhysicsConfig & pc
);

sf::Vector2f
CollisionResponseEntity(
    RigidBody & rb,
    RigidBody & orb,
    sf::Vector2f collision_normal,
    float overlap,
    PhysicsConfig & pc
);

sf::Vector2f
CollisionResponseWall(
    RigidBody & rb,
    float omass,
    sf::Vector2f ovel,
    sf::Vector2f cn,
    float overlap,
    bool playerMoving,
    PhysicsConfig & pc
);

//bool 
//GetProtoCollisionContacts(
//    Entity & entity,
//    Entity & otherentity,
//    Vec<Proto> & protos,
//    OnCollisionEvent onCollision,
//    OnNonCollisionEvent onNonCollision,
//    void* ud,
//    PhysicsConfig & pc,
//    std::list<SAT::ContactInfo> & contacts
//);

sf::Vector2f
lerp(
    sf::Vector2f start,
    sf::Vector2f end,
    float t
);

bool
ResolveAllCollisions(
    Context* context,
    OnCollisionEvent onCollision,
    OnNonCollisionEvent onNonCollision,
    PhysicsConfig & pc
);
//
//bool
//ResolvePolygonCollisions(
//    Entity & entity,
//    Vec<Shape> & collisionpolygons,
//    OnCollisionEvent onCollision,
//    OnNonCollisionEvent onNonCollision,
//    void* ud,
//    PhysicsConfig & pc,
//    std::list<SAT::ContactInfo> & contacts
//);
//
//void UpdateMotion (
//    RigidBody & phy,
//    sf::Time elapsed, 
//    uint32_t is_collided,
//    PhysicsConfig & pc, 
//    sf::Time & accumulator
//);

void ClampUpperVector(sf::Vector2f & vel, float max);
sf::Vector2f ReflectUnitVector(sf::Vector2f d, sf::Vector2f n);

////
//// Enqueue Commands
////
//void SubmitModifyAcceleration(RigidBody & ph, sf::Vector2f a, uint32_t set);
//void SubmitModifyVelocity(RigidBody & ph, sf::Vector2f v, uint32_t set);
//void SubmitModifyPosition(RigidBody & ph, sf::Vector2f p, uint32_t set);
//void SubmitModifyAngle(RigidBody & ph, float ta, uint32_t set);
//void SubmitModifyGravity(RigidBody & ph, float str, sf::Vector2f dir);
//void SubmitMove(RigidBody & ph, float str, sf::Vector2f dir, bool grounded);
//void SubmitCharge(RigidBody & ph, float str, sf::Vector2f dir, bool grounded);
//void SubmitJump(RigidBody & ph, float str, sf::Vector2f dir);

////
//// Dequeue Commands
////
//bool getNextCommand(RigidBody & ph, Command & c);

//
// Apply commands
//
sf::Vector2f ApplyMove(Command::Move & mov, RigidBody & phy, float move_strength, float freefall_move_strength, float move_velocity_max);
sf::Vector2f ApplyCharge(Command::Charge & chg, RigidBody & phy, float charge_strength, float charge_velocity_max);
sf::Vector2f ApplyJump(Command::Jump & j, RigidBody & phy, float jump_strength, float jump_velocity_max);
sf::Vector2f ApplyDrag(RigidBody & phy, float drag_coefficient);

//
// Save Commands to File
//
void SaveCommandHistory(std::string file, std::list<Command> & history);
void LoadCommandHistory(std::string file, std::list<Command> & history);




}
}

#endif
