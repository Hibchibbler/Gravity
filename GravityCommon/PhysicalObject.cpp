#include "PhysicalObject.h"
using namespace bali;
vec::VECTOR2 PhysicalObject::impulse(vec::VECTOR2 force)
{
    //f = ma Newtons
    //a = f/m meters per meters squared or whatever
    return ((sf::Vector2f)force / (float)mass);
}