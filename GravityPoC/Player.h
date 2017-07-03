#pragma once

#include "PhysicalObject.h"

namespace bali
{
    class Player : public PhysicalObject
    {
    public:
        //std::list<Impulse> impulses;



        Player()
            : PhysicalObject(),
              lastPosition(0,0),
              accumulator()
        {
            applyGravity = true;
            angle = 0.0;
            moveRight = moveLeft = moveDown = moveUp = false;
            isCollided = false;
            posCorrection.x = posCorrection.y = 0;
            velCorrection.x = velCorrection.y = 0;
            rotated = false;
            gravitySkipFrames = 0;
            solidGround = false;
            skipCollision = 0;
            onGround = false;
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
        bool rotated; //meta data
        bool solidGround;

        QuadLayer playerQuads;
        vec::VECTOR2 velCorrection;
        vec::VECTOR2 posCorrection;
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
        bool isCollided;
        bool onGround;
    private:

        sf::Time updateTime;
    };
}