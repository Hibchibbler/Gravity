#ifndef Controller_h_
#define Controller_h_

#include "MouseKeyboard.h"


namespace bali
{

// a Player is a type of Entity that is controlled either locally, or remotely 
// by a real person.
// We need something to tie local and remote inputs together
// and submit the formed commands to the correct rigidbody
//
class Context;
class Controller
{
public:
    void initialize(
        Context* context,
        KeyboardConfig config,
        KeyPressedEvent pressed,
        KeyDblPressedEvent dblpressed,
        KeyHeldEvent held,
        KeyReleasedEvent released
    );

    MouseKeyboard   mk;
};

}

#endif

