#ifndef Player_H_
#define Player_H_

#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include <queue>
#include <map>
#include "RigidBody.h"
#include "Animation.h"

namespace bali
{

class Player
{
public:

    enum class State
    {
        IDLERIGHT,
        IDLELEFT,
        RIGHTWARDS,
        LEFTWARDS,
        JUMPINGRIGHT,
        JUMPINGLEFT,
        FALLINGRIGHT,
        FALLINGLEFT,
        CHARGINGRIGHT,
        CHARGINGLEFT,
        SLIDING,
        HANGING,
        CLIMBING,
        DYING,
        SHOOTING,
        SWINGING,
        PUNCHING,
        TEETERING
    };

    Player()
        : accumulator()
    {
        isCollided = isCollidedLast = false;
        jumpNormal = vec::Zero();
        latNormal = vec::Zero();
        state = State::IDLERIGHT;
        isMovingRight = isMovingLeft = isJumping = isCharging = false;
        applyGravity = true;
        doubleJumpCnt = 3;
    }

    void initialize();

    uint32_t updateState();
    void cleanup();

    vec::VECTOR2 surfaceNormal; // This is always set
    vec::VECTOR2 jumpNormal;    // This is set when a jump is performed, and cleared when jump is done done.
    vec::VECTOR2 latNormal;     // This is set when lateral move is performed.

    sf::Time accumulator;

    bool isMovingRight;
    bool isMovingLeft;
    bool isJumping;
    bool isCharging;

    bool isCollided;
    bool isCollidedLast;

    State state;
    bali::ani::AnimationManager aniMan;
    bool applyGravity;

    uint32_t doubleJumpCnt;
    bool isMoving();
    RigidBody physical;

private:

    sf::Time updateTime;
};




}

#endif
