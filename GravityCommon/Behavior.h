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
    enum class LifeCycleState {
        DEAD,
        DYING,
        BIRTH,
        ALIVE
    };

    enum class Disposition {
        SEEKING,
        WANDERING,
        HUNTING,
    };

    enum class Specie { 
        ROCK,
        BIPED,
        CRAWLER,
        FLYER
    };

    Behavior();
    void initialize();

    LifeCycleState lifestate;
    Disposition disposition;
    Specie specie;

    //Hunting State
    size_t pid;
    float targetangle;
};

}

#endif
