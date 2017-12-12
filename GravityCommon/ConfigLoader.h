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

struct KeyboardConfig
{
    uint32_t JUMP_KEY;//='Space'
    uint32_t JUMP_TIME;//=200 ms
    uint32_t RIGHT_KEY;//='D'
    uint32_t RIGHT_TIME;//=0 ms
    uint32_t LEFT_KEY;//='A'
    uint32_t LEFT_TIME;//=0 ms
};

struct AnimationConfig
{

};

PhysicsConfig loadPhysicsConfig(std::string filename);
KeyboardConfig loadKeyboardConfig(std::string filename);

}

#endif

