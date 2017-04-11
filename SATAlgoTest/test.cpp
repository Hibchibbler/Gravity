#include "SATAlgo.h"
#include <iostream>

int main()
{
    bali::SAT::Shape shape1;
    bali::SAT::Shape shape2;
    bali::SAT::Shape shape3;

    shape1.addVertex(0, 100);
    shape1.addVertex(100, 100);
    shape1.addVertex(100, 0);
    shape1.addVertex(0, 0);
    
    shape2.addVertex(50, 150);
    shape2.addVertex(150, 150);
    shape2.addVertex(150, 50);
    shape2.addVertex(50, 50);

    shape3.addVertex(145, 150);
    shape3.addVertex(151, 150);
    shape3.addVertex(151, 50);
    shape3.addVertex(145, 50);

    bali::SAT::MTV mtv;
    bool r = false;
    r = shape1.collision(shape2, mtv);// Yes
    std::cout << "shape1 HIT shape2 ?= " << (r == true ? "yes" : "no") << std::endl;
    r = shape1.collision(shape3, mtv);// No
    std::cout << "shape1 HIT shape3 ?= " << (r == true ? "yes" : "no") << std::endl;
    r = shape2.collision(shape3, mtv);// Yes
    std::cout << "shape2 HIT shape3 ?= " << (r == true ? "yes" : "no") << std::endl;
    return 0;
}