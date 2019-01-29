///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2018
///////////////////////////////////////////////////////////////////////////////
#ifndef KeyboardHandlers_h_
#define KeyboardHandlers_h_

#include "GravityCommon/MouseKeyboard.h"

namespace bali
{

void KeyPressedHandler(Keypress & kp, void* ud);
void KeyDblPressedHandler(Keypress & kp, void* ud);
void KeyHeldHandler(Keypress & kp, void* ud);
void KeyReleasedHandler(Keypress & kp, void* ud);
float logisticsFunction(float t, float L, float K, float t0);

}


#endif
