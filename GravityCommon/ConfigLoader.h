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
    int   HORIZONTAL_MOVE_ANGLE_OFFSET;// = 0;
    int   MOVE_STRENGTH;// = 50;
    int   GRAVITY_CONSTANT;// = 1000;
    float FIXED_DELTA;// = 1.0f / 20000.0f;
    float DRAG_CONSTANT;// = 0.00009;
    float RESTITUTION;// = 0.30f;
    float FAST_THRESHOLD;// = 30.0f;
    float SLOW_THRESHOLD;// = 30.0f;
    float KINETIC_FRICTION;// = 1.75f;
    float DYNAMIC_FRICTION;// = 0.10f;
    float FAST_JUMP_THRESHOLD;
    float JUMP_STRENGTH;
    float MAX_VELOCITY;
};

PhysicsConfig loadPhysicsConfig(std::string filename);

}

#endif

