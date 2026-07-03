#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include "RaylibInputHandler.h"

#include <raylib.h>

namespace input
{
    Command RaylibInputHandler::ReadCommand() const
    {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        {
            return Command::MoveUp;
        }

        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        {
            return Command::MoveDown;
        }

        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
        {
            return Command::MoveLeft;
        }

        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        {
            return Command::MoveRight;
        }

        if (IsKeyPressed(KEY_P))
        {
            return Command::PreviousLevel;
        }

        if (IsKeyPressed(KEY_N))
        {
            return Command::NextLevel;
        }

        if (IsKeyPressed(KEY_R))
        {
            return Command::Reset;
        }

        if (WindowShouldClose() || IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE))
        {
            return Command::Quit;
        }

        return Command::None;
    }
}

#endif
