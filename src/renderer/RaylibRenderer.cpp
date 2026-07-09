#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include "RaylibRenderer.h"

#include "../game/GameLogic.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <raylib.h>
#include <string>
#include <vector>

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

    std::string FindWindowIconPath()
    {
        const fs::path candidates[] =
        {
            "L&L.png",
            "./L&L.png",
            "../L&L.png",
            "../../L&L.png"
        };

        for (const fs::path& candidate : candidates)
        {
            if (fs::exists(candidate) && fs::is_regular_file(candidate))
            {
                return candidate.string();
            }
        }

        return std::string();
    }

    float Clamp01(float value)
    {
        return std::clamp(value, 0.0f, 1.0f);
    }

    float EaseOutCubic(float value)
    {
        const float t = 1.0f - Clamp01(value);
        return 1.0f - (t * t * t);
    }

    float EaseInOutSine(float value)
    {
        return -(std::cos(3.14159265f * Clamp01(value)) - 1.0f) * 0.5f;
    }

    Color LerpColor(Color from, Color to, float amount)
    {
        const float t = Clamp01(amount);
        return Color
        {
            static_cast<unsigned char>(from.r + static_cast<int>((to.r - from.r) * t)),
            static_cast<unsigned char>(from.g + static_cast<int>((to.g - from.g) * t)),
            static_cast<unsigned char>(from.b + static_cast<int>((to.b - from.b) * t)),
            static_cast<unsigned char>(from.a + static_cast<int>((to.a - from.a) * t))
        };
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
        bool useFallbackGraphics = false;
        std::string spriteSheetPath;
    };

    RaylibRenderer::RaylibRenderer() : impl_(std::make_unique<Impl>())
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
        InitWindow(kWindowWidth, kWindowHeight, "Load & Lock");

        const std::string iconPath = FindWindowIconPath();
        if (!iconPath.empty())
        {
            Image icon = LoadImage(iconPath.c_str());
            if (icon.data != nullptr)
            {
                SetWindowIcon(icon);
                UnloadImage(icon);
            }
        }

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

    RaylibRenderer::RenderLayout RaylibRenderer::CalculateLayout(const game::GameLogic& gameState) const
    {
        RenderLayout layout;
        layout.fieldWidth = std::max(1, gameState.GetWidth());
        layout.fieldHeight = std::max(1, gameState.GetHeight());
        layout.displayWidth = std::max(8, layout.fieldWidth);
        layout.displayHeight = std::max(8, layout.fieldHeight);

        const float usableWidth = static_cast<float>(GetScreenWidth()) - (kHorizontalPadding * 2.0f);
        const float usableHeight = static_cast<float>(GetScreenHeight()) - kTopPadding - kBottomPadding;
        layout.tileSize = std::max(12.0f, std::floor(std::min(
            usableWidth / static_cast<float>(layout.displayWidth),
            usableHeight / static_cast<float>(layout.displayHeight))));

        layout.boardWidth = layout.tileSize * static_cast<float>(layout.displayWidth);
        layout.boardHeight = layout.tileSize * static_cast<float>(layout.displayHeight);
        layout.startX = std::max(kHorizontalPadding, (static_cast<float>(GetScreenWidth()) - layout.boardWidth) * 0.5f);
        layout.startY = kTopPadding;
        layout.fieldStartX = layout.startX + (static_cast<float>(layout.displayWidth - layout.fieldWidth) * layout.tileSize * 0.5f);
        layout.fieldStartY = layout.startY + (static_cast<float>(layout.displayHeight - layout.fieldHeight) * layout.tileSize * 0.5f);
        return layout;
    }

    void RaylibRenderer::DrawTile(char tile, const Rectangle& destination, Color tint) const
    {
        DrawRectangleRec(destination, tile == ' ' ? Color{ 30, 34, 42, 255 } : Color{ 62, 67, 77, 255 });

        const int spriteIndex = GetSpriteIndex(tile);
        const bool useSpriteGraphics = impl_->hasSprites && !impl_->useFallbackGraphics;
        constexpr float spriteWidth = 225.0f;
        constexpr float spriteHeight = 225.0f;
        if (useSpriteGraphics && spriteIndex >= 0)
        {
            const int spriteColumn = spriteIndex % kSpriteColumns;
            const int spriteRow = spriteIndex / kSpriteColumns;
            const int hPadding = 25 * spriteColumn + 25;
            const int vPadding = 75 * spriteRow + 25;
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
                tint);
        }
        else if (spriteIndex >= 0)
        {
            DrawFallbackTile(tile, destination);
            if (tint.a < 255)
            {
                DrawRectangleRec(destination, Fade(Color{ 255, 255, 255, 255 }, 1.0f - (static_cast<float>(tint.a) / 255.0f)));
            }
        }
    }

    void RaylibRenderer::DrawBoard(const game::GameLogic& gameState, const RenderLayout& layout, std::vector<AnimatedCrate>& animatedCrates) const
    {
        const game::CutsceneState& cutsceneState = gameState.GetCutsceneState();
        const bool showFinalCutscene = cutsceneState.isActive;
        const game::Position playerPosition = gameState.GetPlayerPosition();

        DrawRectangleRounded(
            Rectangle{ layout.startX - 8.0f, layout.startY - 8.0f, layout.boardWidth + 16.0f, layout.boardHeight + 16.0f },
            0.02f,
            8,
            Color{ 36, 41, 50, 255 });

        for (int y = 0; y < gameState.GetHeight(); ++y)
        {
            for (int x = 0; x < gameState.GetWidth(); ++x)
            {
                char tile = gameState.GetRenderTile(x, y);
                const Rectangle destination =
                {
                    layout.fieldStartX + static_cast<float>(x) * layout.tileSize,
                    layout.fieldStartY + static_cast<float>(y) * layout.tileSize,
                    layout.tileSize,
                    layout.tileSize
                };

                if (showFinalCutscene)
                {
                    if (x == playerPosition.x && y == playerPosition.y)
                    {
                        tile = tile == '+' ? '.' : ' ';
                    }
                    else if (tile == '$' || tile == '*')
                    {
                        animatedCrates.push_back({ tile, destination });
                        tile = tile == '*' ? '.' : ' ';
                    }
                }

                DrawTile(tile, destination, WHITE);
            }
        }
    }

    void RaylibRenderer::DrawHud(const game::GameLogic& gameState) const
    {
        DrawText("Load & Lock", 24, 24, 36, RAYWHITE);
        DrawText(TextFormat("Level %d / %d: %s", gameState.GetLevelNumber(), gameState.GetLevelCount(), gameState.GetLevelName().c_str()), 24, 68, 24, LIGHTGRAY);
        DrawText(TextFormat("Moves: %d", gameState.GetMoveCount()), 24, GetScreenHeight() - 84, 24, RAYWHITE);
        DrawText(TextFormat("Level Score: %d", gameState.GetLevelScore()), 220, GetScreenHeight() - 84, 24, RAYWHITE);
        DrawText(TextFormat("Total Score: %d", gameState.GetTotalScore()), 440, GetScreenHeight() - 84, 24, RAYWHITE);
        DrawText("Controls: WASD/Arrows move, N/P level, R reset, G toggle graphics, Q/Esc quit", 24, GetScreenHeight() - 50, 20, LIGHTGRAY);

        const game::CutsceneState& cutsceneState = gameState.GetCutsceneState();
        if (!impl_->hasSprites)
        {
            DrawText(TextFormat("Sprite sheet not found: %s", impl_->spriteSheetPath.c_str()), 24, GetScreenHeight() - 114, 18, ORANGE);
        }
        else if (impl_->useFallbackGraphics)
        {
            DrawText("Fallback graphics enabled.", 24, GetScreenHeight() - 114, 20, ORANGE);
        }
        else if (gameState.IsComplete() && !cutsceneState.isOverlayVisible)
        {
            DrawText("Level complete. Press N, P, R or Q.", 24, GetScreenHeight() - 114, 20, Color{ 120, 230, 140, 255 });
        }
    }

    void RaylibRenderer::DrawCompletionScore(const game::GameLogic& gameState) const
    {
        if (!gameState.ShouldShowCompletionScore())
        {
            return;
        }

        const char* title = gameState.GetLevelNumber() == gameState.GetLevelCount() ? "Final Stage Clear!" : "Stage Clear!";
        const char* stageScoreText = TextFormat("Stage Score: %d", gameState.GetCompletedLevelScore());
        const char* totalScoreText = TextFormat("Total Score: %d", gameState.GetTotalScore());
        const int panelWidth = 420;
        const int panelHeight = 130;
        const int panelX = (GetScreenWidth() - panelWidth) / 2;
        const int panelY = 28;

        DrawRectangleRounded(
            Rectangle{ static_cast<float>(panelX), static_cast<float>(panelY), static_cast<float>(panelWidth), static_cast<float>(panelHeight) },
            0.12f,
            10,
            Fade(BLACK, 0.8f));
        DrawText(title, panelX + 30, panelY + 18, 30, Color{ 255, 230, 120, 255 });
        DrawText(stageScoreText, panelX + 30, panelY + 62, 28, RAYWHITE);
        DrawText(totalScoreText, panelX + 30, panelY + 94, 24, Color{ 120, 230, 140, 255 });
    }

    void RaylibRenderer::DrawFireworks(float progress) const
    {
        struct Firework
        {
            Vector2 center;
            float startProgress;
            float radius;
            Color color;
        };

        static constexpr Firework fireworks[] =
        {
            { { 190.0f, 170.0f }, 0.12f, 58.0f, Color{ 255, 208, 96, 255 } },
            { { 1040.0f, 155.0f }, 0.18f, 62.0f, Color{ 255, 120, 120, 255 } },
            { { 320.0f, 250.0f }, 0.24f, 66.0f, Color{ 120, 220, 255, 255 } },
            { { 880.0f, 240.0f }, 0.30f, 70.0f, Color{ 180, 255, 150, 255 } },
            { { 640.0f, 120.0f }, 0.36f, 76.0f, Color{ 255, 170, 255, 255 } },
            { { 230.0f, 110.0f }, 0.44f, 84.0f, Color{ 255, 235, 140, 255 } },
            { { 1060.0f, 120.0f }, 0.50f, 88.0f, Color{ 255, 160, 210, 255 } },
            { { 440.0f, 180.0f }, 0.56f, 94.0f, Color{ 140, 255, 215, 255 } },
            { { 830.0f, 170.0f }, 0.62f, 98.0f, Color{ 255, 190, 120, 255 } },
            { { 630.0f, 260.0f }, 0.68f, 108.0f, Color{ 180, 200, 255, 255 } },
            { { 300.0f, 320.0f }, 0.74f, 118.0f, Color{ 255, 140, 140, 255 } },
            { { 980.0f, 310.0f }, 0.78f, 124.0f, Color{ 255, 230, 120, 255 } }
        };

        constexpr float kBurstDuration = 0.16f;
        for (const Firework& firework : fireworks)
        {
            const float localProgress = Clamp01((progress - firework.startProgress) / kBurstDuration);
            if (localProgress <= 0.0f || localProgress >= 1.0f)
            {
                continue;
            }

            const float intensity = 0.55f + EaseOutCubic(progress) * 0.75f;
            const float burstAlpha = (1.0f - localProgress) * std::min(1.0f, intensity);
            const float burstRadius = firework.radius * EaseOutCubic(localProgress);
            const int sparkCount = 12 + static_cast<int>(std::floor(EaseOutCubic(progress) * 16.0f));
            for (int sparkIndex = 0; sparkIndex < sparkCount; ++sparkIndex)
            {
                const float angle = (static_cast<float>(sparkIndex) / static_cast<float>(sparkCount)) * 6.28318530718f;
                const Vector2 direction = { std::cos(angle), std::sin(angle) };
                const float sparkLength = 0.02f + localProgress * 0.35f;
                const Vector2 inner =
                {
                    firework.center.x + direction.x * burstRadius * 0.28f + sparkLength * direction.x * burstRadius,
                    firework.center.y + direction.y * burstRadius * 0.28f + sparkLength * direction.y * burstRadius
                };
                const Vector2 outer =
                {
                    firework.center.x + direction.x * burstRadius,
                    firework.center.y + direction.y * burstRadius
                };
                DrawLineEx(inner, outer, 2.5f + (1.0f - localProgress) * intensity* 1.6f, Fade(firework.color, burstAlpha));

                const float emberRadius = burstRadius * (1.22f + localProgress * 0.98f);
                const float gravity = 0.28f + localProgress * localProgress * localProgress * 98.5f;
                const Vector2 ember =
                {
                    firework.center.x + direction.x * emberRadius,
                    firework.center.y + direction.y * emberRadius + gravity
                };
                const Color emberColor = LerpColor(WHITE, firework.color, localProgress);
                DrawCircleV(ember, 1.5f + intensity * 1.8f, Fade(emberColor, burstAlpha * 0.8f));
            }

            DrawCircleV(firework.center, 5.0f + (1.0f - localProgress) * (6.0f + intensity * 3.0f), Fade(WHITE, burstAlpha * 0.9f));
        }
    }

    void RaylibRenderer::DrawFinalCutscene(const game::GameLogic& gameState, const RenderLayout& layout, const std::vector<AnimatedCrate>& animatedCrates) const
    {
        const game::CutsceneState& cutsceneState = gameState.GetCutsceneState();
        if (!cutsceneState.isActive)
        {
            return;
        }

        const float progress = Clamp01(cutsceneState.progress);
        const float warmth = 0.18f + EaseOutCubic(progress) * 0.24f;
        const game::Position playerPosition = gameState.GetPlayerPosition();
        const float bounceWindow = Clamp01((progress - 0.08f) / 0.5f);
        const float bounceOffset = -std::fabs(std::sin(progress * 22.0f)) * layout.tileSize * 0.28f * bounceWindow;
        const char playerTile = gameState.GetRenderTile(playerPosition.x, playerPosition.y);
        const Rectangle playerDestination =
        {
            layout.fieldStartX + static_cast<float>(playerPosition.x) * layout.tileSize,
            layout.fieldStartY + static_cast<float>(playerPosition.y) * layout.tileSize,
            layout.tileSize,
            layout.tileSize
        };
        DrawTile(
            playerTile,
            Rectangle{
                playerDestination.x,
                playerDestination.y + bounceOffset,
                playerDestination.width,
                playerDestination.height
            },
            WHITE);

        const float helperArrival = EaseOutCubic(Clamp01((progress - 0.28f) / 0.22f));
        const float pushProgress = EaseInOutSine(Clamp01((progress - 0.48f) / 0.22f));
        const float stackCenterX = static_cast<float>(GetScreenWidth()) * 0.5f;
        const float offscreenMargin = layout.tileSize * 2.0f;

        for (std::size_t crateIndex = 0; crateIndex < animatedCrates.size(); ++crateIndex)
        {
            const AnimatedCrate& animatedCrate = animatedCrates[crateIndex];
            const float direction = (animatedCrate.destination.x + animatedCrate.destination.width * 0.5f) < stackCenterX ? -1.0f : 1.0f;
            const float offscreenTargetX = direction < 0.0f
                ? -animatedCrate.destination.width - offscreenMargin
                : static_cast<float>(GetScreenWidth()) + offscreenMargin;
            const float crateX = animatedCrate.destination.x + (offscreenTargetX - animatedCrate.destination.x) * pushProgress;
            const Rectangle movedCrateDestination =
            {
                crateX,
                animatedCrate.destination.y,
                animatedCrate.destination.width,
                animatedCrate.destination.height
            };
            if (movedCrateDestination.x + movedCrateDestination.width > 0.0f && movedCrateDestination.x < static_cast<float>(GetScreenWidth()))
            {
                DrawTile(animatedCrate.tile, movedCrateDestination, WHITE);
            }

            const float helperTargetX = movedCrateDestination.x - direction * layout.tileSize * 0.92f;
            const float helperStartX = direction < 0.0f
                ? static_cast<float>(GetScreenWidth()) + offscreenMargin
                : -offscreenMargin;
            const float helperX = helperStartX + (helperTargetX - helperStartX) * helperArrival;
            const float helperBob = std::fabs(std::sin(progress * 18.0f + static_cast<float>(crateIndex) * 0.65f)) * layout.tileSize * 0.07f;
            DrawTile(
                '@',
                Rectangle{
                    helperX,
                    movedCrateDestination.y - helperBob,
                    layout.tileSize,
                    layout.tileSize
                },
                WHITE);
        }

        DrawFireworks(progress);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(Color{ 255, 152, 70, 255 }, warmth));
    }

    void RaylibRenderer::DrawFinalScoreOverlay(const game::GameLogic& gameState) const
    {
        const game::CutsceneState& cutsceneState = gameState.GetCutsceneState();
        if (!cutsceneState.isOverlayVisible)
        {
            return;
        }

        const float alpha = Clamp01(cutsceneState.overlayOpacity);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.35f + alpha * 0.5f));

        const int panelWidth = 460;
        const int panelHeight = 220;
        const int panelX = (GetScreenWidth() - panelWidth) / 2;
        const int panelY = (GetScreenHeight() - panelHeight) / 2;
        DrawRectangleRounded(
            Rectangle{ static_cast<float>(panelX), static_cast<float>(panelY), static_cast<float>(panelWidth), static_cast<float>(panelHeight) },
            0.12f,
            10,
            Fade(Color{ 32, 18, 14, 255 }, 0.82f * alpha));

        DrawText("Final Score", panelX + 112, panelY + 34, 38, Fade(Color{ 255, 226, 155, 255 }, alpha));
        DrawText(TextFormat("Total Score: %d", gameState.GetTotalScore()), panelX + 86, panelY + 98, 34, Fade(RAYWHITE, alpha));
        DrawText("Thanks for playing!", panelX + 98, panelY + 150, 28, Fade(Color{ 255, 202, 170, 255 }, alpha));
        DrawText("Press Q or Esc to exit", panelX + 110, panelY + 186, 22, Fade(LIGHTGRAY, alpha));
    }

    void RaylibRenderer::Render(const game::GameLogic& gameState) const
    {
        if (IsKeyPressed(KEY_G))
        {
            impl_->useFallbackGraphics = !impl_->useFallbackGraphics;
        }

        BeginDrawing();
        ClearBackground(Color{ 24, 28, 36, 255 });

        const RenderLayout layout = CalculateLayout(gameState);
        const game::CutsceneState& cutsceneState = gameState.GetCutsceneState();
        const bool shouldShowCompletionScore = gameState.ShouldShowCompletionScore();
        const bool shouldShowCutscene = cutsceneState.isActive;
        const bool shouldShowFinalScoreOverlay = cutsceneState.isOverlayVisible;
        std::vector<AnimatedCrate> animatedCrates;

        DrawBoard(gameState, layout, animatedCrates);
        DrawHud(gameState);

        if (shouldShowCompletionScore)
        {
            DrawCompletionScore(gameState);
        }

        if (shouldShowCutscene)
        {
            DrawFinalCutscene(gameState, layout, animatedCrates);

            if (shouldShowFinalScoreOverlay)
            {
                DrawFinalScoreOverlay(gameState);
            }
        }
        else if (shouldShowFinalScoreOverlay)
        {
            DrawFinalScoreOverlay(gameState);
        }

        EndDrawing();
    }
}

#endif
