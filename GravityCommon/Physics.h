///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "GravityCommon/Polygons.h"
#include <list>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iostream>
#include "GravityCommon/ConfigLoader.h"
#include "GravityCommon/Player.h"

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

vec::VECTOR2 rotVector(float angle);
vec::VECTOR2 upVector(float angle);
vec::VECTOR2 leftVector(float angle);
vec::VECTOR2 downVector(float angle);
vec::VECTOR2 rightVector(float angle);
// Find intersection of RAY & SEGMENT
bool getIntersection(vec::VECTOR2 rayStart, vec::VECTOR2 ray, Segment segment, Intersection & intersection);
bool RayCast(vec::VECTOR2 dir, vec::VECTOR2 start, std::vector<Segment> & segments, physics::Intersection & intersect);
bool RayCast(float a, vec::VECTOR2 start, std::vector<Segment> & segments, physics::Intersection & intersect);
void createLoSTriFan(std::vector<CONVEXSHAPE> & shapes, vec::VECTOR2 pos, sf::VertexArray & lineSegments);
#undef PRINT_DIAGNOSTICS
bool ResolveCollisions(bali::CONVEXSHAPE::Vec & shapes, bali::CONVEXSHAPE & playerShape, Player & player, PhysicsConfig & pc, std::vector<Segment> & sharedEdges);
void update(Player & player, sf::Time elapsed, PhysicsConfig & pc);
//vec::VECTOR2 getContactPosition(vec::VECTOR2 pos, vec::VECTOR2 vel, SAT::Shape & shape, std::vector<SAT::Shape> & playerShapes, std::vector<SAT::ContactInfo> & hitInfo, PhysicsConfig & pc)
//{
//    //
//    // It is assumed that a collision has currently been detected.
//    //
//    float step = 2.5f;
//    bool hit = true;
//    vec::VECTOR2 posAdjustment = vec::VECTOR2(0, 0);// = pos;
//    vel = vel.norm();
//    bool collision = false;
//
//    int c = 0;
//    std::vector<SAT::ContactInfo> lastHitInfo;
//    do //accumulate a constant delta, until we find we are not colliding
//    {
//        SAT::Shape pshape = playerShapes.back();
//
//        posAdjustment -= (vel * step);
//        pshape.move(posAdjustment.x, posAdjustment.y);
//        std::vector<SAT::ContactInfo> tempHitInfo;
//        collision = playerShapes.back().collision(pshape, tempHitInfo);
//
//    std:cout << "? " << c++ << " ?" << posAdjustment.x << ", " << posAdjustment.y << "???" << std::endl;
//    } while (collision);
//
//    //At this point, we are no longer colliding.
//    //so undo one increment. 
//    //it's assumed that we are one increment away from colliding again.
//    posAdjustment += (vel * step)*3.0;
//
//    std::cout << "~~~" << posAdjustment.x << ", " << posAdjustment.y << "~~~" << std::endl;
//    return posAdjustment;
//}
}
}

#endif
