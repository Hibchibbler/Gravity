///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "SATAlgo/SATAlgo.h"
#include "GravityCommon/Polygons.h"
#include <iostream>

int main()
{
    bali::CONVEXSHAPE shape1;
    bali::CONVEXSHAPE shape2;

    //
    // Clock wise winding
    //
    shape1.setPointCount(4);
    shape1.setPoint(0, sf::Vector2f(0, 100));
    shape1.setPoint(1, sf::Vector2f(100, 100));
    shape1.setPoint(2, sf::Vector2f(100, 0));
    shape1.setPoint(3, sf::Vector2f(0, 0));
    shape1.setPosition(0, 0);

    shape2.setPointCount(4);
    shape2.setPoint(0, sf::Vector2f(0, 100));
    shape2.setPoint(1, sf::Vector2f(100, 100));
    shape2.setPoint(2, sf::Vector2f(100, 0));
    shape2.setPoint(3, sf::Vector2f(0, 0));
    shape2.setPosition(0, 0);

    //

    bali::SAT::ContactInfo mtv;
    bool r = false;
    bali::SAT::Axes axes;
    bali::vec::VECTOR2 pos(0, 0);
    bali::vec::VECTOR2 dir = bali::vec::VECTOR2(4, 4);
    for (int i = 0; i < 27; i++)
    {
        bali::translateShape(shape2, dir);
        shape2.move(dir);

        bali::SAT::ContactInfo::Vec hitInfo;
        r = bali::SAT::Shape::collision(shape1, shape2, hitInfo);
        std::cout << "shape1 HIT shape2? "
            << (r == true ? "yes" : "no") << " @ ("
            << shape2.getPosition().x << ", " << shape2.getPosition().y << ")" << std::endl;
    }

    char s;
    std::cin.read(&s,1);
    
    return 0;
}