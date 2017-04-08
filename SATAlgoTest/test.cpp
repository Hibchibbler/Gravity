#include "SATAlgo.h"

int main()
{
    bali::SAT::Shape shape1;
    bali::SAT::Shape shape2;
    bali::SAT::Shape shape3;
    bali::SAT::Shape shape4;

    shape1.vertices.push_back(bali::SAT::Vector2(0, 0));
    shape1.vertices.push_back(bali::SAT::Vector2(10, 0));
    shape1.vertices.push_back(bali::SAT::Vector2(10, 10));
    shape1.vertices.push_back(bali::SAT::Vector2(0, 10));
    
    shape2.vertices.push_back(bali::SAT::Vector2(1, 1));
    shape2.vertices.push_back(bali::SAT::Vector2(11, 1));
    shape2.vertices.push_back(bali::SAT::Vector2(11, 11));
    shape2.vertices.push_back(bali::SAT::Vector2(1, 11));

    shape3.vertices.push_back(bali::SAT::Vector2(8, 8));
    shape3.vertices.push_back(bali::SAT::Vector2(18, 8));
    shape3.vertices.push_back(bali::SAT::Vector2(18, 18));
    shape3.vertices.push_back(bali::SAT::Vector2(8, 8));

    shape4.vertices.push_back(bali::SAT::Vector2(20, 20));
    shape4.vertices.push_back(bali::SAT::Vector2(30, 20));
    shape4.vertices.push_back(bali::SAT::Vector2(30, 30));
    shape4.vertices.push_back(bali::SAT::Vector2(20, 30));

    bali::SAT::MTV mtv;
    bool r = false;
    r = shape1.collision(shape2, mtv);
    return 0;
}