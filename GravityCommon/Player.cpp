#include "GravityCommon/Builders.h"
#include "Player.h"
#include "Physics.h"
#include "Level.h"
#include <iostream>

namespace bali
{

void Player::initialize()
{

}

void Player::updatePolygon(Level* l)
{
}

void Player::cleanup()
{

}

bool Player::isMoving()
{
    //return (isJumping || isMovingRight || isMovingLeft || isCharging ? true : false);
    return (isMovingRight || isMovingLeft || isCharging);
}


uint32_t Player::updateState()
{
    Player::State oldState = state;

    if (physical.vel == vec::Zero())
        return 0;

    vec::VECTOR2 velN = vec::norm(physical.vel);
    float right = vec::dot(physics::rightVector(physical.angle), velN);
    float left = vec::dot(physics::leftVector(physical.angle), velN);
    float up = vec::dot(physics::upVector(physical.angle), velN);
    float down = vec::dot(physics::downVector(physical.angle), velN);
    float mag = vec::mag(physical.vel);

    if (down  > 0.1f &&
        right < 0.8f &&
        left  < 0.8f &&
        mag   > 50.0f)
    {
        if (state != Player::State::FALLINGRIGHT || state != Player::State::FALLINGLEFT)
        {
            if (right > left)
                state = Player::State::FALLINGRIGHT;
            else
                state = Player::State::FALLINGLEFT;
        }
    }
    //else if (vec::dot(physics::upVector(physical.angle), vec::norm(physical.vel)) > 0.2f &&
    //    vec::mag(physical.vel) > 40.0f)
    else if (isJumping)
    {
        if (state != Player::State::JUMPINGRIGHT &&
            state != Player::State::JUMPINGLEFT)
        {
            if (right > left)
                state = Player::State::JUMPINGRIGHT;
            else
                state = Player::State::JUMPINGLEFT;
        }
    }
    else if (isCharging)
    {
        if (state != Player::State::CHARGINGRIGHT ||
            state != Player::State::CHARGINGLEFT)
        {
            if (right > left)
                state = Player::State::CHARGINGRIGHT;
            else
                state = Player::State::CHARGINGLEFT;
        }
    }
    else if (mag < 20.0f)
    {
        if (state != Player::State::IDLERIGHT &&
            state != Player::State::IDLELEFT)
        {
            //std::cout << "IDLE ";
            if (right > left)
            {
                state = Player::State::IDLERIGHT;
            }
            else
            {
                state = Player::State::IDLELEFT;
            }
        }
    }
    else if (right > 0.5f)
    {
        if (state != Player::State::RIGHTWARDS)
        {
            state = Player::State::RIGHTWARDS;
        }
    }
    else if (left > 0.5f)
    {
        if (state != Player::State::LEFTWARDS)
        {
            state = Player::State::LEFTWARDS;
        }
    }
    return (uint32_t)oldState;
}








}
