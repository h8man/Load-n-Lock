#pragma once

#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include "InputHandler.h"

namespace input
{
    class RaylibInputHandler
    {
    public:
        Command ReadCommand() const;
    };
}

#endif
