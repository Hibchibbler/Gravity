#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include <SFML\System.hpp>
#include <stdint.h>
#include <vector>

namespace bali
{
#define STATE_MASK(x)   \
    (1UL << (x))

class Behavior
{
public:
    enum class State {
        DEATH,
        BIRTH,
        IDLE,
        WALKING,
        RUNNING,
        JUMPING,
        ATTACKING,
        FALLING,
    };

    enum class Disposition {
        PASSIVE,
        AGGRESSIVE
    };

    enum class Specie { 
        ROCK,
        BIPED,
        CRAWLER,
        FLYER
    };

    Behavior();
    void initialize();

    State state;
    Disposition disposition;
    Specie specie;
};

}

#endif
