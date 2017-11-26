#ifndef Player_H_
#define Player_H_

#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include <queue>
#include <map>
#include "PhysicalObject.h"

namespace bali
{





class Player : public PhysicalObject
{
public:
    //std::list<Impulse> impulses;



    Player()
        : PhysicalObject(),
        edgeContact(0, 0),
        accumulator()
    {
        applyGravity = true;
        angle = targetangle = 0.0;
        moveRight = moveLeft = moveDown = moveUp = false;
        //jumpAccum = 0;
        isCollided = false;
        posCorrection.x = posCorrection.y = 0;
        velCorrection.x = velCorrection.y = 0;
        rotated = false;
        gravitySkipFrames = 0;
        solidGround = false;
        skipCollision = 0;
        onGround = false;
        moving = 0;
    }


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
    bool rotated; //meta data
    bool solidGround;

    //sf::VertexArray playerQuads;
    vec::VECTOR2 velCorrection;
    vec::VECTOR2 posCorrection;
    vec::VECTOR2 edgeContact;
    bool applyGravity;
    bool GravityOn() {
        return (applyGravity = true);
    }
    bool GravityOff() {
        return (applyGravity = false);
    }
    const uint32_t GravitySkipFramesMax = 100;
    uint32_t gravitySkipFrames;

    sf::Time accumulator;
    uint32_t skipCollision;
    bool moveRight;
    bool moveLeft;
    bool moveDown;
    bool moveUp;
    uint32_t moving;
    bool isCollided;
    bool onGround;

private:

    sf::Time updateTime;
};
}

#endif
