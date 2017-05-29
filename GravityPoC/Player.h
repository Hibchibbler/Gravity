#pragma once

namespace bali
{
    class Player
    {
    public:
        //std::list<Impulse> impulses;

#ifdef _DEBUG
        const int LEFTUP = -10;
        const int h = 10;
        const int GK = 1;
        const int PPS = 10;
#else
        const int LEFTUP = -20;
        const int h = 10;
        const int GK = 6;
        const int PPS = 10;
#endif

        Player()
        {
            applyGravity = true;
            angle = 0.0;
            moveRight = moveLeft = moveDown = moveUp = false;
            isCollided = false;
        }

        void force()
        {
            //f = m*a
            //a = f/m
            //dv/dt = a
            //dx/dt=
        }

        vec::VECTOR2 upVector()
        {
            float a = DEG_TO_RAD(angle - 90);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        vec::VECTOR2 downVector()
        {
            float a = DEG_TO_RAD(angle - 270);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        vec::VECTOR2 leftVector()
        {
            float a = DEG_TO_RAD(angle - 180 - LEFTUP);
            vec::VECTOR2 v(1, 0);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        vec::VECTOR2 rightVector()
        {
            float a = DEG_TO_RAD(angle - 360 + LEFTUP);
            vec::VECTOR2 v(1, 0);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }
        void update(sf::Time elapsed)
        {
            float dt = elapsed.asSeconds() * PPS;

            position += (velocity + (acceleration / 2) * 2) * dt;
            velocity += acceleration * dt;

            posHist.push_back(position);
            if (posHist.size() > 200)
            {
                posHist.erase(posHist.begin());
            }

            vec::VECTOR2 vel(0, 0);
            if (moveUp)
            {
                vec::VECTOR2 u = upVector()*h;
                vel += u;
                moveUp = false;
            }

            if (moveLeft)
            {
                vec::VECTOR2 l = leftVector()*h;
                vel += l;
                moveLeft = false;
            }

            if (moveDown)
            {
                vec::VECTOR2 d = downVector()*h;
                vel += d;
                moveDown = false;
            }

            if (moveRight)
            {
                vec::VECTOR2 r = rightVector()*h;
                vel += r;
                moveRight = false;
            }

            velocity += vel;

            vec::VECTOR2 acc(0, 0);
            if (applyGravity)// #1
            {
                ////std::cout << " G<" <<g.x << ", " << g.y << " >G" << std::endl;
                vec::VECTOR2 g = downVector() * GK;
                acc = g;
                //std::cout << accx << ", " << accy << std::endl;
            }

            acceleration = acc;


        }

        std::vector<vec::VECTOR2> velHist;
        std::vector<vec::VECTOR2> posHist;
        vec::VECTOR2 position;
        vec::VECTOR2 velocity;
        vec::VECTOR2 acceleration;
        float angle;

        SAT::MTV currentMTV;
        QuadLayer playerQuads;
        vec::VECTOR2 velCorrection;
        bool applyGravity;


        bool moveRight;
        bool moveLeft;
        bool moveDown;
        bool moveUp;
        bool isCollided;
    private:

        sf::Time updateTime;
    };
}