///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
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
    uint32_t JUMP_COUNT;//=1
    float MOVE_VELOCITY_MAX;
    float CHARGE_VELOCITY_MAX;
};

struct KeyboardConfig
{
    uint32_t JUMP_KEY;//='Space'
    uint32_t JUMP_TIME;//=200 ms
    uint32_t RIGHT_KEY;//='D'
    uint32_t RIGHT_TIME;//=0 ms
    uint32_t LEFT_KEY;//='A'
    uint32_t LEFT_TIME;//=0 ms
    uint32_t ROTATE_RIGHT_KEY;//='E'
    uint32_t ROTATE_RIGHT_TIME;//=0 ms
    uint32_t ROTATE_LEFT_KEY;//='Q'
    uint32_t ROTATE_LEFT_TIME;//=0 ms
    uint32_t ATTACK_KEY;
    uint32_t ATTACK_TIME;
    uint32_t HARPOON_KEY;
    uint32_t HARPOON_TIME;
    uint32_t DOWN_KEY;//='S'
    uint32_t DOWN_TIME;
    uint32_t UP_KEY;//='W'
    uint32_t UP_TIME;
    uint32_t RESET_KEY;//='W'
    uint32_t RESET_TIME;

};

struct GeneralConfig
{
    uint32_t SHOW_OBSTRUCTION_POLYGON;
    uint32_t SHOW_ENTITY_POLYGON;
    uint32_t SHOW_WAYPOINTS;
    uint32_t SHOW_ENTITY_CENTROID;
    uint32_t SHOW_ENTITY_PATHS;
    uint32_t AUTO_GRAVITY_PLAYERS;
    uint32_t AUTO_GRAVITY_ENTITIES;
    uint32_t DISABLE_MOUSE_GRAVITY;
    uint32_t ENABLE_HEAD_BUMP_GRAVITY;
    float    HUNT_THRESHOLD;
    float    SEEK_THRESHOLD;
    float    ARRIVED_THRESHOLD;
};

struct AnimationConfig
{

};

PhysicsConfig loadPhysicsConfig(std::string filename);
KeyboardConfig loadKeyboardConfig(std::string filename);
GeneralConfig loadGeneralConfig(std::string filename);

}

#endif

