#ifndef Command_h_
#define Command_h_
#include <stdint.h>
#include <Windows.h>
#include "Vector2.h"
namespace bali
{

class Impulse
{
public:
    vec::VECTOR2 force;
    float duration;
};

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
        SETPOSITION,
        ADDPOSITION,
        SETVELOCITY,
        ADDVELOCITY,
        SETTARGETANGLE,
        ADDIMPULSE,
        MOVE,
        JUMP
    };

    class AddPosition {
    public:
        vec::VECTOR2 delta;
    };

    class SetPosition {
    public:
        vec::VECTOR2 pos;
    };

    class SetVelocity {
    public:
        vec::VECTOR2 vel;
    };

    class AddVelocity {
    public:
        vec::VECTOR2 delta;
    };

    class SetTargetAngle{
    public:
        float targetangle;
        uint32_t granularity;
    };

    class AddImpulse {
    public:
        vec::VECTOR2 force;
        float duration;
    };

    class Move {
    public:
        float        str;
        bool         gnd;
        vec::VECTOR2 dir;
    };

    class Jump{
    public:
        float str;//TODO: maybe a function
        float dur;
        float cur;
        vec::VECTOR2 dir;//initial jump vector
    };

    Command();

    uint64_t timestamp;
    uint32_t code;
    uint32_t timed;

    union
    {
        SetPosition     sp;
        AddPosition     ap;
        SetVelocity     sv;
        AddVelocity     av;
        SetTargetAngle  sta;
        AddImpulse      ai;
        Move            mv;
        Jump            jmp;
    };
};



}

#endif

