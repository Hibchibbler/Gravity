#ifndef Command_h_
#define Command_h_
#include <stdint.h>
#include <Windows.h>
#include "Vector2.h"
namespace bali
{

class Timestamp
{
public:
    void start();
    void end();
    void update();
    LARGE_INTEGER getTime();

    LARGE_INTEGER f;
    LARGE_INTEGER s;
    LARGE_INTEGER e;
};

class Command
{
public:
    enum Code
    {
        POSITION,
        VELOCITY,
        ACCELERATION,
        ANGLE,
        MOVE,
        JUMP,
        CHARGE,
        GRAVITY,
    };

    enum Priority
    {
        LOW,
        MEDIUM,
        HIGH,
        ULTRA
    };

    class Position {
    public:
        vec::VECTOR2 pos;
    };

    class Velocity {
    public:
        vec::VECTOR2 vel;
    };

    class Acceleration {
    public:
        vec::VECTOR2 accel;
    };

    class Angle{
    public:
        float angle;
    };

    class Move {
    public:
        float        str;
        bool         gnd;
        vec::VECTOR2 dir;
    };

    class Jump{
    public:
        float str;
        vec::VECTOR2 dir;
    };

    class Charge
    {
    public:
        float        str;
        bool         gnd;
        vec::VECTOR2 dir;
    };

    class Gravity
    {
    public:                         
        float        str;
        vec::VECTOR2 dir;
    };

    Command();

    uint64_t timestamp;
    uint32_t code;
    uint32_t priority;
    uint32_t set;//or add

    union
    {
        Position     pos;
        Velocity     vel;
        Acceleration accel;
        Angle        ang;
        Move         mov;
        Jump         jmp;
        Gravity      grv;
        Charge       chg;
    };
};



}

#endif

