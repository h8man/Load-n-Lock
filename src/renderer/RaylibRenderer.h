#pragma once

#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include <memory>
#include <raylib.h>
#include <string>
#include <vector>

namespace game
{
    class GameLogic;
}

namespace renderer
{
    class RaylibRenderer
    {
    public:
        RaylibRenderer();
        ~RaylibRenderer();

        RaylibRenderer(const RaylibRenderer&) = delete;
        RaylibRenderer& operator=(const RaylibRenderer&) = delete;

        bool IsOpen() const;
        void Render(const game::GameLogic& gameState) const;

    private:
        struct RenderLayout
        {
            int fieldWidth = 0;
            int fieldHeight = 0;
            int displayWidth = 0;
            int displayHeight = 0;
            float tileSize = 0.0f;
            float boardWidth = 0.0f;
            float boardHeight = 0.0f;
            float startX = 0.0f;
            float startY = 0.0f;
            float fieldStartX = 0.0f;
            float fieldStartY = 0.0f;
        };

        struct AnimatedCrate
        {
            char tile = ' ';
            Rectangle destination = { 0.0f, 0.0f, 0.0f, 0.0f };
        };

        RenderLayout CalculateLayout(const game::GameLogic& gameState) const;
        void DrawTile(char tile, const Rectangle& destination, Color tint) const;
        void DrawBoard(const game::GameLogic& gameState, const RenderLayout& layout, std::vector<AnimatedCrate>& animatedCrates) const;
        void DrawHud(const game::GameLogic& gameState) const;
        void DrawCompletionScore(const game::GameLogic& gameState) const;
        void DrawFireworks(float progress) const;
        void DrawFinalCutscene(const game::GameLogic& gameState, const RenderLayout& layout, const std::vector<AnimatedCrate>& animatedCrates) const;
        void DrawFinalScoreOverlay(const game::GameLogic& gameState) const;

        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}

#endif
