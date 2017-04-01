#include "quadtree.h"
#include <iostream>
using namespace bali;

int main(int argc, char**argv)
{
    qt::AABB a;
    const int max_depth = 5;

    a.min = { 0,0,0 };
    a.max = { 500,500,0 };
    qt::QuadTree qt(a, max_depth);

    qt.insert({10,10 });
    qt.insert({290,10 });
    qt.insert({300,10 });
    qt.insert({10,300 });
    qt.insert({300,300 });

    float x = 0;
    float y = 0;
    float w = 295;
    float h = 200;

    std::cout << "Searching in boundary = {" << x << ", " << y << ", " << w << ", " << h << "}" << std::endl;
    qt::AABB aabb;
    aabb.min.x = x;
    aabb.min.y = y;
    aabb.max.x = w;
    aabb.max.y = h;
   
    std::vector<qt::XY> points = qt.search(aabb);

    if (!points.empty())
    {
        std::cout << "Search found points:" << std::endl;
        for (auto i = points.begin(); i != points.end(); i++)
        {
            std::cout << "\t(" << i->x << ", " << i->y << ")" << std::endl;
        }
    }
    return 0;
}