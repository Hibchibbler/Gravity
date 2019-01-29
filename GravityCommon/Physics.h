///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "Polygons.h"
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
//class Intersection
//{
//public:
//    Intersection();
//    sf::Vector2f distance;
//    float time;
//    sf::Vector2f ray;
//    sf::Vector2f rayPoint;
//    sf::Vector2f segPoint;
//    float angle;
//    bool expired;//did the ray get to the end without hitting anything?
//    Segment segment;
//};
//
//class Ray
//{
//public:
//    sf::Vector2f offset;
//    sf::Vector2f delta;
//};


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
//bool getIntersection(sf::Vector2f rayStart, sf::Vector2f ray, Segment segment, Intersection & intersection);
//bool RayCast(sf::Vector2f dir, sf::Vector2f start, std::vector<Segment> & segments, physics::Intersection & intersect);
//bool RayCast(float a, sf::Vector2f start, std::vector<Segment> & segments, physics::Intersection & intersect);
//void createLoSTriFan(std::vector<CONVEXSHAPE> & shapes, sf::Vector2f pos, sf::VertexArray & lineSegments);

typedef void(*OnCollisionEvent) (void* context, Entity & entity, sf::Vector2f collisionNormal);
typedef void(*OnNonCollisionEvent) (void* context, Entity & entity);

void
updateRigidBody(
    RigidBody & rb,
    sf::Time elapsed,
    PhysicsConfig & pc,
    sf::Time & accumulator
);

bool CollisionResponse(
    RigidBody & rb,
    sf::Vector2f collision_normal,
    float overlap,
    bool playerMoving,
    PhysicsConfig & pc
);

bool 
ResolveProtoCollisions(
    Entity & entity,
    Vec<Entity> & otherentities,
    Vec<Proto> & protos,
    OnCollisionEvent onCollision,
    OnNonCollisionEvent onNonCollision,
    void* ud,
    PhysicsConfig & pc
);

bool
ResolvePolygonCollisions(
    Entity & entity,
    Vec<Shape> & collisionpolygons,
    OnCollisionEvent onCollision,
    OnNonCollisionEvent onNonCollision,
    void* ud,
    PhysicsConfig & pc
);
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
