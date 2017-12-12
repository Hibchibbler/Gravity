#ifndef Player_H_
#define Player_H_

#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include <queue>
#include <map>
#include "PhysicalObject.h"
#include "Animation.h"

namespace bali
{

class Player : public PhysicalObject
{
public:
    //std::list<Impulse> impulses;



    Player()
        : PhysicalObject(),
        accumulator()
    {
        angle = targetangle = 0.0;
        isCollided = isCollidedLast = false;
        jumpNormal = vec::Zero();
        latNormal = vec::Zero();
        moving = 0;
        state = State::IDLE;
        isMovingRight = isMovingLeft = isJumping = false;
    }

    enum class State
    {
        IDLE,
        RIGHTWARDS,
        LEFTWARDS,
        JUMPING,
        FALLING,
        SLIDING,
        HANGING,
        CLIMBING,
        DYING,
        SHOOTING,
        SWINGING,
        PUNCHING,
        TEETERING
    };


    vec::VECTOR2 lastPosition;
    std::vector<vec::VECTOR2> velHist;
    std::vector<vec::VECTOR2> posHist;

    //angle - starts at 
    //   3 O'  Clock = 0   degree
    //   6 O'  Clock = 90  degree
    //   9 O'  Clock = 180 degree
    //   12 O' Clock = 240 degree
    //   3 O'  Clock = 360 degree
    float angle;
    float targetangle;
    uint32_t granularity;

    vec::VECTOR2 jumpNormal;
    vec::VECTOR2 latNormal;

    sf::Time accumulator;

    uint32_t moving;
    bool isMovingRight;
    bool isMovingLeft;
    bool isJumping;

    bool isCollided;
    bool isCollidedLast;

    State state;
    bali::ani::AnimationManager aniMan;

    void doJumping();
    void doRightward();
    void doLeftward();
    void doIdle(Player::State s);

    bool isMoving();

private:

    sf::Time updateTime;
};
}

#endif
