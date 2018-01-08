#ifndef Input_H_
#define Input_H_

#include "GravityCommon/MouseKeyboard.h"

namespace bali
{
namespace input
{

void KeyPressedHandler(Keypress & kp, void* ud);
void KeyDblPressedHandler(Keypress & kp, void* ud);
void KeyHeldHandler(Keypress & kp, void* ud);
void KeyReleasedHandler(Keypress & kp, void* ud);
float logisticsFunction(float t, float L, float K, float t0);

}
}


#endif
