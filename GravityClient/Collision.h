#ifndef Collision_H_
#define Collision_H_
#include "GravityCommon/Player.h"
#include "GravityCommon/ConfigLoader.h"

namespace bali
{


void onNonCollisionHandler(Player & p, PhysicsConfig & pc);
void onCollisionHandler(Player & p, vec::VECTOR2 cn, PhysicsConfig & pc);



}

#endif

