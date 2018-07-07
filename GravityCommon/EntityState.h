#ifndef EntityState_H_
#define EntityState_H_

namespace bali
{

class EntityState
{
public:
    enum class State
    {
        IDLERIGHT,
        IDLELEFT,
        WALKINGRIGHT,
        WALKINGLEFT,
        JUMPINGRIGHT,
        JUMPINGLEFT,
        FALLINGRIGHT,
        FALLINGLEFT,
        CHARGINGRIGHT,
        CHARGINGLEFT
    };
    EntityState()
    {
        state = State::IDLELEFT;
    }

    State state;
};

}

#endif