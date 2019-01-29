///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef RigidBody_H_
#define RigidBody_H_

#include <stdint.h>
#include <SFML/System.hpp>
#include <queue>
#include <list>
#include "Vector2.h"
#include "CommandQueue.h"
#include "Collider.h"
#include "ConfigLoader.h"

namespace bali {


class RigidBody
{
public:
    RigidBody()
    {
        pos = vec::Zero();
        vel = vec::Zero();
        accel = vec::Zero();
        mass = 1.0;
        angle = 0.0f;
        eid = 0;
    }

    sf::Vector2f impulse(sf::Vector2f force);

    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Vector2f accel;
    uint32_t     mass;
    float        angle;
    Collider     collider;
    uint32_t     eid; // This id tells us which Entity we are associated with.

    CommandQueue        cmdqueue;


public:
};



}

#endif
