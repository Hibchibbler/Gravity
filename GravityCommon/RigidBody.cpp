#include "RigidBody.h"
#include "Vector2.h"
#include <iostream>
#include <iomanip>

#include "Physics.h"

namespace bali
{


sf::Vector2f RigidBody::impulse(sf::Vector2f force)
{
    //f = ma Newtons
    //a = f/m meters per meters squared or whatever
    return (force / (float)mass);
}


}
