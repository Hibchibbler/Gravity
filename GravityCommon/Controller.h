#ifndef Controller_h_
#define Controller_h_

#include "MouseKeyboard.h"
#include "Entity.h"

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
    // Hook up a local keyboard
    void initialize(
        Context*  context,
        KeyboardConfig config,
        KeyPressedEvent pressed,
        KeyDblPressedEvent dblpressed,
        KeyHeldEvent held,
        KeyReleasedEvent released
    );

    // Hook up a remote connection
    void initialize(
        Context*  context,
        uint32_t uid
    );

    MouseKeyboard   mk;
};

}

#endif

