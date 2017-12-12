#include "Player.h"
#include <iostream>
namespace bali
{

void Player::doJumping()
{
    state = Player::State::JUMPING;
    isJumping = true;
}

void Player::doRightward()
{
    state = Player::State::RIGHTWARDS;
    isMovingRight = true;
}

void Player::doLeftward()
{
    state = Player::State::LEFTWARDS;
    isMovingLeft = true;
}

void Player::doIdle(Player::State s)
{
    switch (s)
    {
    case Player::State::JUMPING:
        isJumping = false;
        break;
    case Player::State::RIGHTWARDS:
        isMovingRight = false;
        break;
    case Player::State::LEFTWARDS:
        isMovingLeft = false;
        break;
    }
    if (!isJumping && (!isMovingRight && !isMovingLeft) || (isMovingRight && isMovingLeft))
    {
        std::cout << "IDLE ";
        state = Player::State::IDLE;
    }
    else if (!isJumping && isMovingRight && !isMovingLeft)
    {
        state = Player::State::RIGHTWARDS;
    }
    else if (!isJumping && !isMovingRight && isMovingLeft)
    {
        state = Player::State::LEFTWARDS;
    }
    else if (isJumping)
    {
        state = Player::State::JUMPING;
    }
}

bool Player::isMoving()
{
    return (isJumping || isMovingRight || isMovingLeft ? true : false);
}











}