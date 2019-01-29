#include "Controller.h"
#include "Context.h"

namespace bali
{

void Controller::initialize(
    Context* context,
    KeyboardConfig config,
    KeyPressedEvent pressed,
    KeyDblPressedEvent dblpressed,
    KeyHeldEvent held,
    KeyReleasedEvent released
)
{
    mk.Initialize();
    mk.setUserData(context);
    mk.registerKeypress(context->keyboardConfig.JUMP_KEY,
        context->keyboardConfig.JUMP_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.RIGHT_KEY,
        context->keyboardConfig.RIGHT_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.LEFT_KEY,
        context->keyboardConfig.LEFT_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.DOWN_KEY,
        context->keyboardConfig.DOWN_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.UP_KEY,
        context->keyboardConfig.UP_TIME,
        pressed,
        dblpressed,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.ROTATE_RIGHT_KEY,
        context->keyboardConfig.ROTATE_RIGHT_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.ROTATE_LEFT_KEY,
        context->keyboardConfig.ROTATE_LEFT_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.ATTACK_KEY,
        context->keyboardConfig.ATTACK_TIME,
        pressed,
        nullptr,
        held,
        released);
    mk.registerKeypress(context->keyboardConfig.HARPOON_KEY,
        context->keyboardConfig.HARPOON_TIME,
        pressed,
        nullptr,
        held,
        released);
}

}


