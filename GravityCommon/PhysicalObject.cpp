#include "PhysicalObject.h"
#include "Vector2.h"
#include <iostream>
#include <iomanip>



namespace bali
{


vec::VECTOR2 Physical::impulse(vec::VECTOR2 force)
{
    //f = ma Newtons
    //a = f/m meters per meters squared or whatever
    return (force / (float)mass);
}



}
