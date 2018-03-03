///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef Collision_H_
#define Collision_H_
#include "GravityCommon/Player.h"
#include "GravityCommon/ConfigLoader.h"
#include "GravityCommon/Context.h"

namespace bali
{


void onNonCollisionHandler(Context::Ptr context);
void onCollisionHandler(Context::Ptr context, vec::VECTOR2 cn);



}

#endif

