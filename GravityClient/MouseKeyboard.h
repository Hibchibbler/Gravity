#pragma once

#include "GravityCommon/Context.h"

namespace bali {
    class MouseKeyboard
    {
    public:
        static void doKeyboard(Context::Ptr ctx);
        static void doMouse(Context::Ptr ctx);
    };
}