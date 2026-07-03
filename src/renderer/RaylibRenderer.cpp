#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include "RaylibRenderer.h"

#include "../game/GameLogic.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <raylib.h>
#include <string>

namespace fs = std::filesystem;

namespace
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 900;
    constexpr int kSpriteRows = 3;
    constexpr int kSpriteColumns = 6;
    constexpr float kHorizontalPadding = 32.0f;
    constexpr float kTopPadding = 120.0f;
    constexpr float kBottomPadding = 120.0f;

    std::string FindSpriteSheetPath()
    {
        const fs::path candidates[] =
        {
            "sprites.png",
            "./sprites.png",
            "../sprites.png",
            "../../sprites.png"
        };

        for (const fs::path& candidate : candidates)
        {
            if (fs::exists(candidate) && fs::is_regular_file(candidate))
            {
                return candidate.string();
            }
        }

        return candidates[0].string();
    }

    int GetSpriteIndex(char tile)
    {
        switch (tile)
        {
        case '#': return 0;
        case '.': return 5;
        case '@': return 12;
        case '+': return 12;
        case '$': return 3;
        case '*': return 4;
        case ' ': return 1;
        case 'D': return 2;
        default: return 1;
        }
    }

    void DrawFallbackTile(char tile, const Rectangle& destination)
    {
        DrawRectangleRounded(destination, 0.1f, 8, Color{ 80, 86, 98, 255 });

        switch (tile)
        {
        case '#':
            DrawRectangleRounded(destination, 0.12f, 8, Color{ 92, 118, 148, 255 });
            break;
        case '.':
            DrawCircle(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.12f, Color{ 255, 215, 90, 255 });
            break;
        case '@':
            DrawCircle(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.28f, Color{ 110, 220, 255, 255 });
            break;
        case '+':
            DrawCircle(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.28f, Color{ 110, 220, 255, 255 });
            DrawCircleLines(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.18f, Color{ 255, 215, 90, 255 });
            break;
        case '$':
            DrawRectangleRounded(
                Rectangle{
                    destination.x + destination.width * 0.2f,
                    destination.y + destination.height * 0.2f,
                    destination.width * 0.6f,
                    destination.height * 0.6f
                },
                0.1f,
                8,
                Color{ 181, 120, 65, 255 });
            break;
        case '*':
            DrawRectangleRounded(
                Rectangle{
                    destination.x + destination.width * 0.2f,
                    destination.y + destination.height * 0.2f,
                    destination.width * 0.6f,
                    destination.height * 0.6f
                },
                0.1f,
                8,
                Color{ 181, 120, 65, 255 });
            DrawCircleLines(static_cast<int>(destination.x + destination.width * 0.5f), static_cast<int>(destination.y + destination.height * 0.5f), destination.width * 0.18f, Color{ 255, 215, 90, 255 });
            break;
        default:
            break;
        }
    }
}

namespace renderer
{
    struct RaylibRenderer::Impl
    {
        Texture2D sprites = { 0 };
        bool hasSprites = false;
        std::string spriteSheetPath;
    };

    RaylibRenderer::RaylibRenderer() : impl_(std::make_unique<Impl>())
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
        InitWindow(kWindowWidth, kWindowHeight, "Load & Lock");
        SetTargetFPS(60);

        impl_->spriteSheetPath = FindSpriteSheetPath();
        if (fs::exists(impl_->spriteSheetPath))
        {
            impl_->sprites = LoadTexture(impl_->spriteSheetPath.c_str());
            impl_->hasSprites = impl_->sprites.id != 0;
        }
    }

    RaylibRenderer::~RaylibRenderer()
    {
        if (impl_->hasSprites)
        {
            UnloadTexture(impl_->sprites);
        }

        if (IsWindowReady())
        {
            CloseWindow();
        }
    }

    bool RaylibRenderer::IsOpen() const
    {
        return IsWindowReady() && !WindowShouldClose();
    }

    void RaylibRenderer::Render(const game::GameLogic& gameState, const std::string& levelName, int levelNumber, int levelCount) const
    {
        BeginDrawing();
        ClearBackground(Color{ 24, 28, 36, 255 });

        DrawText("Load & Lock", 24, 24, 36, RAYWHITE);
        DrawText(TextFormat("Level %d / %d: %s", levelNumber, levelCount, levelName.c_str()), 24, 68, 24, LIGHTGRAY);

        const float usableWidth = static_cast<float>(GetScreenWidth()) - (kHorizontalPadding * 2.0f);
        const float usableHeight = static_cast<float>(GetScreenHeight()) - kTopPadding - kBottomPadding;
        const float tileSize = std::max(12.0f, std::floor(std::min(
            usableWidth / static_cast<float>(std::max(1, gameState.GetWidth())),
            usableHeight / static_cast<float>(std::max(1, gameState.GetHeight())))));

        const float boardWidth = tileSize * static_cast<float>(gameState.GetWidth());
        const float boardHeight = tileSize * static_cast<float>(gameState.GetHeight());
        const float startX = std::max(kHorizontalPadding, (static_cast<float>(GetScreenWidth()) - boardWidth) * 0.5f);
        const float startY = kTopPadding;

        DrawRectangleRounded(
            Rectangle{ startX - 8.0f, startY - 8.0f, boardWidth + 16.0f, boardHeight + 16.0f },
            0.02f,
            8,
            Color{ 36, 41, 50, 255 });

        const float spriteWidth = 225;//impl_->hasSprites ? static_cast<float>(impl_->sprites.width) / static_cast<float>(kSpriteColumns) : 0.0f;
        const float spriteHeight = 225;//impl_->hasSprites ? static_cast<float>(impl_->sprites.height) / static_cast<float>(kSpriteRows) : 0.0f;

        for (int y = 0; y < gameState.GetHeight(); ++y)
        {
            for (int x = 0; x < gameState.GetWidth(); ++x)
            {
                const char tile = gameState.GetRenderTile(x, y);
                const Rectangle destination =
                {
                    startX + static_cast<float>(x) * tileSize,
                    startY + static_cast<float>(y) * tileSize,
                    tileSize,
                    tileSize
                };

                DrawRectangleRec(destination, tile == ' ' ? Color{ 30, 34, 42, 255 } : Color{ 62, 67, 77, 255 });

                const int spriteIndex = GetSpriteIndex(tile);
                if (impl_->hasSprites && spriteIndex >= 0)
                {
                    const int spriteColumn = spriteIndex % kSpriteColumns;
                    const int spriteRow = spriteIndex / kSpriteColumns;
                    const int hPadding = 25 * (spriteColumn) + 25;
                    const int vPadding = 75 * (spriteRow) + 25;
                    DrawTexturePro(
                        impl_->sprites,
                        Rectangle{
                            spriteWidth * static_cast<float>(spriteColumn) + hPadding,
                            spriteHeight * static_cast<float>(spriteRow) + vPadding,
                            spriteWidth,
                            spriteHeight
                        },
                        destination,
                        Vector2{ 0.0f, 0.0f },
                        0.0f,
                        WHITE);
                }
                else if (spriteIndex >= 0)
                {
                    DrawFallbackTile(tile, destination);
                }
            }
        }

        DrawText(TextFormat("Moves: %d", gameState.GetMoveCount()), 24, GetScreenHeight() - 84, 24, RAYWHITE);
        DrawText("Controls: WASD/Arrows move, N/P level, R reset, Q/Esc quit", 24, GetScreenHeight() - 50, 20, LIGHTGRAY);

        if (!impl_->hasSprites)
        {
            DrawText(TextFormat("Sprite sheet not found: %s", impl_->spriteSheetPath.c_str()), 24, GetScreenHeight() - 114, 18, ORANGE);
        }
        else if (gameState.IsComplete())
        {
            DrawText("Level complete. Press N, P, R or Q.", 24, GetScreenHeight() - 114, 20, Color{ 120, 230, 140, 255 });
        }

        EndDrawing();
    }
}

#endif
