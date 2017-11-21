#ifndef Command_h_
#define Command_h_
#include <stdint.h>
#include "Vector2.h"
namespace bali
{

class Command
{
public:
    enum Code
    {
        SETPOSITION,
        ADDPOSITION,
        SETVELOCITY,
        ADDVELOCITY,
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

    Command();

    uint32_t code;

    union
    {
        //Impulse      im;
        SetPosition  sp;
        AddPosition  ap;
        SetVelocity  sv;
        AddVelocity  av;
    };
};



}

#endif

