#pragma once

#include "GameContext.h"

namespace bali {
    class MouseKeyboard
    {
    public:
        static void doKeyboard(GameContext & ctx);
        static void doMouse(GameContext & ctx);
    };
}