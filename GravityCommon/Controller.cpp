#include "Controller.h"
#include "Context.h"

namespace bali
{

void Controller::initialize(
    Context*  context,
    KeyboardConfig config,
    KeyPressedEvent pressed,
    KeyDblPressedEvent dblpressed,
    KeyHeldEvent held,
    KeyReleasedEvent released
)
{
    mk.Initialize();
    mk.setUserData(context);
    mk.registerKeypress(config.JUMP_KEY,
        (float)config.JUMP_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(config.RIGHT_KEY,
        config.RIGHT_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(config.LEFT_KEY,
        config.LEFT_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(config.DOWN_KEY,
        config.DOWN_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(config.UP_KEY,
        config.UP_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(config.ROTATE_RIGHT_KEY,
        config.ROTATE_RIGHT_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(config.ROTATE_LEFT_KEY,
        config.ROTATE_LEFT_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(config.ATTACK_KEY,
        config.ATTACK_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(config.HARPOON_KEY,
        config.HARPOON_TIME,
        pressed,
        nullptr,
        held,
        released);
}

}


