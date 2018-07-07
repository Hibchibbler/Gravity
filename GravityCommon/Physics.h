///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
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
#include "Player.h"
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
class Intersection
{
public:
    Intersection();
    vec::VECTOR2 distance;
    float time;
    vec::VECTOR2 ray;
    vec::VECTOR2 rayPoint;
    vec::VECTOR2 segPoint;
    float angle;
    bool expired;//did the ray get to the end without hitting anything?
    Segment segment;
};

class Ray
{
public:
    vec::VECTOR2 offset;
    vec::VECTOR2 delta;
};


//
// Frame of References
//
vec::VECTOR2 rotVector(float angle);
vec::VECTOR2 rotVector(float angle, vec::VECTOR2 v);
vec::VECTOR2 upVector(float angle);
vec::VECTOR2 leftVector(float angle);
vec::VECTOR2 downVector(float angle);
vec::VECTOR2 rightVector(float angle);

//
// Ray stuff
//
bool getIntersection(vec::VECTOR2 rayStart, vec::VECTOR2 ray, Segment segment, Intersection & intersection);
bool RayCast(vec::VECTOR2 dir, vec::VECTOR2 start, std::vector<Segment> & segments, physics::Intersection & intersect);
bool RayCast(float a, vec::VECTOR2 start, std::vector<Segment> & segments, physics::Intersection & intersect);
void createLoSTriFan(std::vector<CONVEXSHAPE> & shapes, vec::VECTOR2 pos, sf::VertexArray & lineSegments);

typedef void(*OnCollisionEvent) (Context::Ptr context, vec::VECTOR2 collisionNormal);
typedef void(*OnNonCollisionEvent) (Context::Ptr context);



bool CollisionResponse(
    RigidBody & phy,
    vec::VECTOR2 collision_normal,
    float overlap,
    PhysicsConfig & pc,
    bool playerMoving
);

bool GetMinimumTranslationVector(
    bali::CONVEXSHAPES & shapes,
    bali::CONVEXSHAPE & playerShape,
    vec::VECTOR2 vel,
    std::list<SAT::ContactInfo> & cinfos
);

bool ResolveCollisions (
    Context::Ptr context,
    bali::CONVEXSHAPES & shapes,
    bali::CONVEXSHAPE & playerShape,
    std::vector<Segment> & sharedEdges,
    OnCollisionEvent onCollision,
    OnNonCollisionEvent onNonCollision
);

void UpdateMotion (
    RigidBody & phy,
    sf::Time elapsed, 
    uint32_t is_collided,
    PhysicsConfig & pc, 
    sf::Time & accumulator
);

void ClampUpperVector(vec::VECTOR2 & vel, float max);
vec::VECTOR2 ReflectUnitVector(vec::VECTOR2 d, vec::VECTOR2 n);

//
// Enqueue Commands
//
void SubmitModifyAcceleration(RigidBody & ph, vec::VECTOR2 a, uint32_t set);
void SubmitModifyVelocity(RigidBody & ph, vec::VECTOR2 v, uint32_t set);
void SubmitModifyPosition(RigidBody & ph, vec::VECTOR2 p, uint32_t set);
void SubmitModifyAngle(RigidBody & ph, float ta, uint32_t set);
void SubmitModifyGravity(RigidBody & ph, float str, vec::VECTOR2 dir);
void SubmitMove(RigidBody & ph, float str, vec::VECTOR2 dir, bool grounded);
void SubmitCharge(RigidBody & ph, float str, vec::VECTOR2 dir, bool grounded);
void SubmitJump(RigidBody & ph, float str, vec::VECTOR2 dir);

//
// Dequeue Commands
//
bool getNextCommand(RigidBody & ph, Command & c);

//
// Apply commands
//
void ApplyMove(Command::Move & mov, RigidBody & phy, float move_strength, float freefall_move_strength, float move_velocity_max);
void ApplyCharge(Command::Charge & chg, RigidBody & phy, float charge_strength, float charge_velocity_max);
void ApplyJump(Command::Jump & j, RigidBody & phy, float jump_strength, float jump_velocity_max);
void ApplyDrag(RigidBody & phy, bool is_collided, float drag_coefficient);

//
// Save Commands to File
//
void SaveCommandHistory(std::string file, std::list<Command> & history);
void LoadCommandHistory(std::string file, std::list<Command> & history);




}
}

#endif
