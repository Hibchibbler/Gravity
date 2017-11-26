///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////
#ifndef MYVECTOR2_H_
#define MYVECTOR2_H_

#include <string>
#include <SFML/System/Vector2.hpp>

// Defines
#define PI 3.14159265358979323846F

namespace bali
{
namespace vec
{
typedef sf::Vector2f VECTOR2;

float dot(const VECTOR2 a, const VECTOR2 b);
float mag(const VECTOR2 a);
VECTOR2 norm(const VECTOR2 a);
VECTOR2 normal(const VECTOR2 a);
float cross(const VECTOR2 a, const VECTOR2 b);

std::string stringify(const VECTOR2 & a);
}
}

#endif
