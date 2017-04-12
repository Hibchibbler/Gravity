#include "SATAlgo.h"
#include <iostream>

int main()
{
    bali::SAT::Shape shape1;
    bali::SAT::Shape shape2;
    bali::SAT::Shape shape3;
    bali::SAT::Shape shape4;
    bali::SAT::Shape shape5;

    //
    // Clock wise winding
    //
    shape1.addVertex(0, 100);
    shape1.addVertex(100, 100);
    shape1.addVertex(100, 0);
    shape1.addVertex(0, 0);
    shape1.translate(0, 0);

    shape2.addVertex(0, 100);
    shape2.addVertex(100, 100);
    shape2.addVertex(100, 0);
    shape2.addVertex(0, 0);
    shape2.translate(50, 50);

    shape3.addVertex(0, 100);
    shape3.addVertex(100, 100);
    shape3.addVertex(100, 0);
    shape3.addVertex(0, 0);
    shape3.translate(100, 50);

    shape4.addVertex(0, 100);
    shape4.addVertex(50, 100);
    shape4.addVertex(100, 75);
    shape4.addVertex(100, 50);
    shape4.addVertex(50, 0);
    shape4.addVertex(10, 10);
    shape5 = shape4;
    shape4.translate(200, 100);


    

    bali::SAT::MTV mtv;
    bool r = false;
    
    r = shape1.collision(shape2, mtv);// Yes
    std::cout << "shape1 HIT shape2 ?= " << (r == true ? "yes" : "no") << std::endl;
    
    r = shape1.collision(shape3, mtv);// Yes
    std::cout << "shape1 HIT shape3 ?= " << (r == true ? "yes" : "no") << std::endl;
    
    r = shape2.collision(shape3, mtv);// Yes
    std::cout << "shape2 HIT shape3 ?= " << (r == true ? "yes" : "no") << std::endl;

    r = shape1.collision(shape4, mtv);// No
    std::cout << "shape1 HIT shape4 ?= " << (r == true ? "yes" : "no") << std::endl;

    r = shape1.collision(shape5, mtv);// Yes
    std::cout << "shape1 HIT shape5 ?= " << (r == true ? "yes" : "no") << std::endl;

    shape5.translate(-100, -70);
    r = shape1.collision(shape5, mtv);// Yes
    std::cout << "shape1 HIT shape5 ?= " << (r == true ? "yes" : "no") << std::endl;

    char s;
    std::cin.read(&s,1);
    
    return 0;
}