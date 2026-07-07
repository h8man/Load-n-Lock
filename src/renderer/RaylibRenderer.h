#pragma once

#ifdef LOAD_AND_LOCK_USE_RAYLIB

#include <memory>
#include <string>

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
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}

#endif
