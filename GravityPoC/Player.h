#pragma once

namespace bali
{

#ifdef _DEBUG
    const int LEFTUP = -10;
    const int h = 10;
    const int GK = 3;
    const int PPS = 10;
    const float FIXED_DELTA = 1.0f / 30.0f;
#else
    const int LEFTUP = -10;
    const int h = 8;
    const int GK = 8;
    const int PPS = 10;
    const float FIXED_DELTA = 1.0f / 205.0f;
#endif

    class Player
    {
    public:
        //std::list<Impulse> impulses;



        Player()
        {
            applyGravity = true;
            angle = 0.0;
            moveRight = moveLeft = moveDown = moveUp = false;
            isCollided = false;
            posCorrection.x = posCorrection.y = 0;
            velCorrection.x = velCorrection.y = 0;
            rotated = false;
        }

        void force()
        {
            //f = m*a
            //a = f/m
            //dv/dt = a
            //dx/dt=
        }

        static vec::VECTOR2 upVector(float angle)
        {
            float a = DEG_TO_RAD(angle - 90);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        static vec::VECTOR2 leftVector(float angle)
        {
            float a = DEG_TO_RAD(angle - 180 - LEFTUP);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        static vec::VECTOR2 downVector(float angle)
        {
            float a = DEG_TO_RAD(angle - 270);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }

        static vec::VECTOR2 rightVector(float angle)
        {
            float a = DEG_TO_RAD(angle - 360 + LEFTUP);
            vec::VECTOR2 v(0, 1);
            v.x = v.mag() * cos(a);
            v.y = v.mag() * sin(a);
            v = v.norm();

            return v;
        }
        void update(sf::Time elapsed)
        {
            float dt = FIXED_DELTA;// elapsed.asSeconds() * PPS;

            position += (velocity + (acceleration / 2) * 2) * dt;
            velocity += acceleration * dt;

            posHist.push_back(position);
            if (posHist.size() > 50)
            {
                posHist.erase(posHist.begin());
            }

            vec::VECTOR2 vel(0, 0);
            if (moveUp)
            {
                //if (!applyGravity)
                {
                    vec::VECTOR2 u = Player::upVector(angle)*h;
                    vel += u;
                    moveUp = false;
                    //applyGravity = false;
                    //GravityOn();
                }
            }

            if (moveLeft)
            {
                vec::VECTOR2 l = Player::leftVector(angle)*h;//posCorrection.normal().norm()*h*1.0;//
                vel += l;
                moveLeft = false;
                //applyGravity = false;
                //GravityOn();
            }

            if (moveDown)
            {
                vec::VECTOR2 d = Player::downVector(angle)*h;
                vel += d;
                moveDown = false;
                //applyGravity = false;
                //GravityOn();
            }

            if (moveRight)
            {
                vec::VECTOR2 r = Player::rightVector(angle)*h;//posCorrection.normal().norm()*h*-1.0;//
                vel += r;
                moveRight = false;
                //applyGravity = false;
                //GravityOn();
            }

            if (true)
            {
                //vec::VECTOR2 r = (velocity * -0.05);
                //vel += r;
            }

            

            velocity += vel;

            vec::VECTOR2 acc(0, 0);



            if (applyGravity)// #1
            {
                ////std::cout << " G<" <<g.x << ", " << g.y << " >G" << std::endl;
                vec::VECTOR2 g = Player::downVector(angle) * GK;
                //float django = velocity.norm().dot(velCorrection);
                //if (django < 0)
                //{
                    acc += g;
                //}
                //std::cout << "--" << django <<  "--" << std::endl;
            }
            //applyGravity = true;
            acceleration = acc;

//#ifdef _DEBUG
//            if (velocity.mag() < 0.005f)
//#else
//            if (velocity.mag() < 0.01f)
//#endif
//            {
//                velocity.x = velocity.y = 0;
//                //acc+= Player::upVector(angle)  * GK;
//            }
        }

        std::vector<vec::VECTOR2> velHist;
        std::vector<vec::VECTOR2> posHist;
        vec::VECTOR2 position;
        vec::VECTOR2 velocity;
        vec::VECTOR2 acceleration;
        float angle;
        bool rotated; //meta data

        SAT::MTV currentMTV;
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

        bool moveRight;
        bool moveLeft;
        bool moveDown;
        bool moveUp;
        bool isCollided;
    private:

        sf::Time updateTime;
    };
}