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

typedef void(*OnCollisionEvent) (Player & p, vec::VECTOR2 collisionNormal, PhysicsConfig & pc);
typedef void(*OnNonCollisionEvent) (Player &p, PhysicsConfig & pc);


bool ResolveCollisions (
    bali::CONVEXSHAPE::Vec & shapes,
    bali::CONVEXSHAPE & playerShape,
    Player & player,
    PhysicsConfig & pc,
    std::vector<Segment> & sharedEdges,
    OnCollisionEvent onCollision,
    OnNonCollisionEvent onNonCollision
);

void UpdateMotion (
    Physical & phy, 
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
void SubmitModifyAcceleration(Physical & ph, vec::VECTOR2 a, uint32_t set);
void SubmitModifyVelocity(Physical & ph, vec::VECTOR2 v, uint32_t set);
void SubmitModifyPosition(Physical & ph, vec::VECTOR2 p, uint32_t set);
void SubmitModifyAngle(Physical & ph, float ta, uint32_t set);
void SubmitModifyGravity(Physical & ph, float str, vec::VECTOR2 dir);
void SubmitMove(Physical & ph, float str, vec::VECTOR2 dir, bool grounded);
void SubmitCharge(Physical & ph, float str, vec::VECTOR2 dir, bool grounded);
void SubmitJump(Physical & ph, float str, vec::VECTOR2 dir);

//
// Dequeue Commands
//
bool getNextCommand(Physical & ph, Command & c);

//
// Apply commands
//
void ApplyMove(Command::Move & mov, Physical & phy, float move_strength, float freefall_move_strength, float move_velocity_max);
void ApplyCharge(Command::Charge & chg, Physical & phy, float charge_strength, float charge_velocity_max);
void ApplyJump(Command::Jump & j, Physical & phy, float jump_strength, float jump_velocity_max);
void ApplyDrag(Physical & phy, bool is_collided, float drag_coefficient);

//
// Save Commands to File
//
void SaveCommandHistory(std::string file, std::list<Command> & history);
void LoadCommandHistory(std::string file, std::list<Command> & history);




}
}

#endif
