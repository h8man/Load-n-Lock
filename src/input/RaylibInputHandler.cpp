#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include "RaylibInputHandler.h"

#include <raylib.h>

namespace input
{
    static Command cachedCommand = Command::None;

    void RaylibInputHandler::CachePendingInput()
    {
        if (cachedCommand != Command::None)
        {
            return;
        }

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        {
            cachedCommand = Command::MoveUp;
            return;
        }

        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        {
            cachedCommand = Command::MoveDown;
            return;
        }

        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
        {
            cachedCommand = Command::MoveLeft;
            return;
        }

        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        {
            cachedCommand = Command::MoveRight;
            return;
        }

        if (IsKeyPressed(KEY_P))
        {
            cachedCommand = Command::PreviousLevel;
            return;
        }

        if (IsKeyPressed(KEY_N))
        {
            cachedCommand = Command::NextLevel;
            return;
        }

        if (IsKeyPressed(KEY_R))
        {
            cachedCommand = Command::Reset;
            return;
        }

        if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE))
        {
            cachedCommand = Command::Quit;
        }
    }

    Command RaylibInputHandler::ReadCommand() const
    {
        if (cachedCommand != Command::None)
        {
            const Command command = cachedCommand;
            cachedCommand = Command::None;
            return command;
        }

        return Command::None;
    }
}

#endif
