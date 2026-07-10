#pragma once

#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include "InputHandler.h"

namespace input
{
    class RaylibInputHandler
    {
    public:
        static void CachePendingInput();
        Command ReadCommand() const;
    };
}

#endif
