#pragma once

#include <string>

namespace game
{
    class GameLogic;
}

namespace renderer
{
    class ConsoleRenderer
    {
    public:
        void Render(const game::GameLogic& gameState) const;

    private:
        void ClearConsole() const;
    };
}
