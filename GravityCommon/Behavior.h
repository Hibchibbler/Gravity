#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include <SFML\System.hpp>
#include <stdint.h>
#include <vector>

namespace bali
{

class Behavior
{
public:
    enum State {
        BIRTH,
        DEATH,
        IDLE,
        WALKING,
        RUNNING,
        JUMPING,
        ATTACKING,
        FALLING,
        SEEKWAYPOINT,
        SEEKENTITY,
    };

    enum Disposition {
        PASSIVE,
        AGGRESSIVE
    };

    enum Specie {
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
