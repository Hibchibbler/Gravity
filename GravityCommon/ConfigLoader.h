///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef CONFIGLOADER_H_
#define CONFIGLOADER_H_

#include <string>


namespace bali
{
struct PhysicsConfig
{
    float MOVE_STRENGTH;// = 200;
    float GRAVITY_CONSTANT;// = 1000;
    float FIXED_DELTA;// = 1.0f / 20000.0f;
    float DRAG_CONSTANT;// = 0.00009;
    float RESTITUTION;// = 0.30f;
    float SLOW_THRESHOLD;// = 30.0f;
    float STATIC_FRICTION_VELOCITY_MAX;// = 30.0f;
    float STATIC_FRICTION;// = 1.75f;
    float DYNAMIC_FRICTION;// = 0.10f;
    float FAST_JUMP_THRESHOLD;
    float JUMP_STRENGTH;//=900
    float FREEFALL_MOVE_STRENGTH;//=25
    float VELOCITY_MAX;//=650
    float JUMP_VELOCITY_MAX;//=100
};

PhysicsConfig loadPhysicsConfig(std::string filename);

}

#endif

