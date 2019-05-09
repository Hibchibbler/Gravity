#ifndef Entity_h_
#define Entity_h_

#include <stdint.h>
#include "Proto.h"
#include "SATAlgo\SATAlgo.h"//TODO: don't include this!!! jeezzuz
#include "Behavior.h"
#include "Animation.h"
#include "Bullet.h"

namespace bali
{

class Entity
{
public:
    typedef Entity* Ptr;


    Entity()
    {
        moving = false;
        jumping = false;
        charging = false;
        health = stamina = 100;
    }

    void initialize(Proto & p) {
        proto = p;
        moving = false;
        jumping = false;
        charging = false;

        registerwithaidirector = false;
        ignoreentitycollision = false;
        huntable = false;
        behavior.initialize();

        health = 100;
        stamina = 100;
    }

    Behavior            behavior;
    Proto               proto;
    Collider            collider;

    bool isActive() { return moving || jumping || charging; }
    uint8_t             moving;
    uint8_t             jumping;
    uint8_t             charging;

    // These cross reference into the list that owns them.
    Vec<size_t>         collisionshapes;//cleared in update
    Vec<size_t>         collisionentities;// cleared in update
    Vec<size_t>         waypointpath;
    size_t              seekwaypoint;
    size_t              currentwaypoint;
    Vec<Segment>        visiblesegments;

    bool                registerwithaidirector;
    bool                ignoreentitycollision;

    sf::Vector2f        upper;
    sf::Vector2f        ahead;
    sf::Vector2f        lower;
    sf::Vector2f        distupper;
    sf::Vector2f        distahead;
    sf::Vector2f        distlower;

    sf::Vector2f        avgForce;
    float               numForce;

    uint32_t pathid;// Path Id
    uint32_t mode;

    bool huntable;

    uint32_t health;
    uint32_t stamina;

    Vec<Bullet> bullets;

};


}

#endif
