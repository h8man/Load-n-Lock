#include "Application.h"
#include "Application.h"

#include "../audio/AudioPlayer.h"
#include "../assets/AssetManager.h"
#include "../game/GameLogic.h"
#ifdef LOAD_AND_LOCK_USE_RAYLIB
#include "../input/RaylibInputHandler.h"
#include "../renderer/RaylibRenderer.h"
#else
#include "../input/InputHandler.h"
#include "../renderer/ConsoleRenderer.h"
#endif

#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace core
{
    int Application::Run() const
    {
        using Clock = std::chrono::steady_clock;

        constexpr auto kCompletionScoreDisplayDuration = std::chrono::milliseconds(1500);

        audio::AudioPlayer audioPlayer;
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
        std::vector<int> bestLevelScores(levels.size(), 0);
        int totalScore = 0;
        int lastCompletedLevelScore = 0;
        bool hasProcessedCompletionForCurrentLevel = false;
        bool showCompletionScore = false;
        bool shouldAdvanceToNextLevel = false;
        Clock::time_point completionScoreDisplayUntil = Clock::time_point::min();
        bool wasLevelComplete = false;
        game::CutscenePlayer cutscenePlayer;

        const auto calculateLevelScore = [&](int moveCount)
        {
            return std::max(100, 2500 - (moveCount * 25));
        };

        const auto resetCompletionState = [&]()
        {
            hasProcessedCompletionForCurrentLevel = false;
            showCompletionScore = false;
            shouldAdvanceToNextLevel = false;
            lastCompletedLevelScore = 0;
            completionScoreDisplayUntil = Clock::time_point::min();
            wasLevelComplete = gameState.IsComplete();
            cutscenePlayer.Reset();
        };

        const auto loadCurrentLevel = [&]() -> bool
        {
            error.clear();
            if (!gameState.Load(levels[currentLevelIndex].rows, error))
            {
                return false;
            }

            gameState.SetLevelContext(
                levels[currentLevelIndex].name,
                static_cast<int>(currentLevelIndex + 1),
                static_cast<int>(levels.size()));
            return true;
        };

        const auto updateScoreContext = [&]()
        {
            gameState.SetScoreContext(
                bestLevelScores[currentLevelIndex],
                totalScore,
                showCompletionScore,
                lastCompletedLevelScore);
            gameState.SetCutsceneState(cutscenePlayer.GetState());
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

        while (renderer.IsOpen())
        {
            cutscenePlayer.Update(Clock::now());

            updateScoreContext();
            renderer.Render(gameState);

            if (!hasProcessedCompletionForCurrentLevel && wasLevelComplete)
            {
                const int levelScore = calculateLevelScore(gameState.GetMoveCount());
                if (levelScore > bestLevelScores[currentLevelIndex])
                {
                    totalScore += levelScore - bestLevelScores[currentLevelIndex];
                    bestLevelScores[currentLevelIndex] = levelScore;
                }

                lastCompletedLevelScore = levelScore;
                audioPlayer.PlayLevelComplete();
                showCompletionScore = true;
                shouldAdvanceToNextLevel = currentLevelIndex + 1 < levels.size();
                hasProcessedCompletionForCurrentLevel = true;
                completionScoreDisplayUntil = Clock::now() + kCompletionScoreDisplayDuration;
            }

            if (showCompletionScore)
            {
                if (Clock::now() < completionScoreDisplayUntil || audioPlayer.IsLevelCompletePlaying())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                    continue;
                }

                showCompletionScore = false;
                if (shouldAdvanceToNextLevel)
                {
                    ++currentLevelIndex;
                    if (!loadCurrentLevel())
                    {
                        std::cerr << "Failed to initialize level: " << error << '\n';
                        return 1;
                    }

                    resetCompletionState();
                }
                else
                {
                    cutscenePlayer.Start(Clock::now());
                    audioPlayer.PlayBell();
                }
            }

            const game::CutsceneState& cutsceneState = cutscenePlayer.GetState();
            if (cutsceneState.isActive || cutsceneState.isOverlayVisible)
            {
                const input::Command command = inputHandler.ReadCommand();
                if (command == input::Command::Quit)
                {
                    return 0;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(16));
                continue;
            }

            const auto moveWithFeedback = [&](int dx, int dy)
            {
                if (gameState.Move(dx, dy))
                {
                    audioPlayer.PlayMove();
                    if (!wasLevelComplete && gameState.IsComplete())
                    {
                        wasLevelComplete = gameState.IsComplete();
                    }
                }
            };

            const input::Command command = inputHandler.ReadCommand();
            switch (command)
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

                    resetCompletionState();
                    audioPlayer.PlayPreviousLevel();
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

                    resetCompletionState();
                    audioPlayer.PlayNextLevel();
                }
                break;
            case input::Command::Reset:
                gameState.Reset();
                resetCompletionState();
                break;
            case input::Command::Quit:
                return 0;
            case input::Command::None:
            default:
                break;
            }
        }
        return 0;
    }
}
