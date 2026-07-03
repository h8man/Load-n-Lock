#include "Application.h"
#include "Application.h"

#include "../assets/AssetManager.h"
#include "../game/GameLogic.h"
#ifdef LOAD_AND_LOCK_USE_RAYLIB
#include "../input/RaylibInputHandler.h"
#include "../renderer/RaylibRenderer.h"
#else
#include "../input/InputHandler.h"
#include "../renderer/ConsoleRenderer.h"
#endif

#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

namespace core
{
    int Application::Run() const
    {
        assets::AssetManager assetManager;
        std::vector<assets::LevelData> levels;
        std::string error;
        if (!assetManager.LoadLevels(levels, error))
        {
            std::cerr << "Failed to load levels: " << error << '\n';
            return 1;
        }

        game::GameLogic gameState;
        std::size_t currentLevelIndex = 0;

        const auto loadCurrentLevel = [&]() -> bool
        {
            error.clear();
            return gameState.Load(levels[currentLevelIndex].rows, error);
        };

        if (!loadCurrentLevel())
        {
            std::cerr << "Failed to initialize level: " << error << '\n';
            return 1;
        }

#ifdef LOAD_AND_LOCK_USE_RAYLIB
        input::RaylibInputHandler inputHandler;
        renderer::RaylibRenderer renderer;
#else
        input::InputHandler inputHandler;
        renderer::ConsoleRenderer renderer;
#endif
        bool wasLevelComplete = gameState.IsComplete();

#ifdef LOAD_AND_LOCK_USE_RAYLIB
        while (renderer.IsOpen())
#else
        while (true)
#endif
        {
            renderer.Render(
                gameState,
                levels[currentLevelIndex].name,
                static_cast<int>(currentLevelIndex + 1),
                static_cast<int>(levels.size()));

            const auto moveWithFeedback = [&](int dx, int dy)
            {
                if (gameState.Move(dx, dy))
                {
                    Beep(880, 35);

                    if (!wasLevelComplete && gameState.IsComplete())
                    {
                        Beep(784, 70);
                        Beep(988, 70);
                        Beep(1175, 90);
                        Beep(1568, 140);
                        Beep(1175, 90);
                        Beep(1568, 180);
                    }

                    wasLevelComplete = gameState.IsComplete();
                }
            };

            switch (inputHandler.ReadCommand())
            {
            case input::Command::MoveUp:
                if (!gameState.IsComplete())
                {
                    moveWithFeedback(0, -1);
                }
                break;
            case input::Command::MoveDown:
                if (!gameState.IsComplete())
                {
                    moveWithFeedback(0, 1);
                }
                break;
            case input::Command::MoveLeft:
                if (!gameState.IsComplete())
                {
                    moveWithFeedback(-1, 0);
                }
                break;
            case input::Command::MoveRight:
                if (!gameState.IsComplete())
                {
                    moveWithFeedback(1, 0);
                }
                break;
            case input::Command::PreviousLevel:
                if (currentLevelIndex > 0)
                {
                    --currentLevelIndex;
                    if (!loadCurrentLevel())
                    {
                        std::cerr << "Failed to initialize level: " << error << '\n';
                        return 1;
                    }
                    Beep(1319, 50);
                    Beep(988, 70);
                    wasLevelComplete = gameState.IsComplete();
                }
                break;
            case input::Command::NextLevel:
                if (currentLevelIndex + 1 < levels.size())
                {
                    ++currentLevelIndex;
                    if (!loadCurrentLevel())
                    {
                        std::cerr << "Failed to initialize level: " << error << '\n';
                        return 1;
                    }

                    Beep(988, 50);
                    Beep(1319, 70);
                    wasLevelComplete = gameState.IsComplete();
                }
                break;
            case input::Command::Reset:
                gameState.Reset();
                wasLevelComplete = gameState.IsComplete();
                break;
            case input::Command::Quit:
                return 0;
            case input::Command::None:
            default:
                break;
            }
        }
    }
}
