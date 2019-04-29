#ifndef AnimationBehaviorMap_H_
#define AnimationBehaviorMap_H_

#include "Behavior.h"
#include "Animation.h"

/*
    We are mapping a physical behavior to a particular animation sequence.
    There are mappings for each monster, player, and consumable type.
*/

namespace bali
{

struct ABMapElement
{
    Behavior::State behaviorstate;
    std::string sequencename;
};

//struct ABMap
//{
//    ABMapElement map[] = {
//        { bali::Behavior::State::IDLE, "Idle"},
//        { bali::Behavior::State::RUNNING, "Run"},
//        { bali::Behavior::State::WALKING, "Walk" },
//        { bali::Behavior::State::JUMPING, "Jumping" },
//        { bali::Behavior::State::FALLING, "Falling" },
//        { bali::Behavior::State::ATTACKING, "Attacking" },
//    };
//};

}

#endif

