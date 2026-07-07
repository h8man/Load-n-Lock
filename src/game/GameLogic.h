#pragma once

#include <string>
#include <vector>

namespace game
{
    struct Position
    {
        int x;
        int y;
    };

    bool operator==(const Position& left, const Position& right);

    class GameLogic
    {
    public:
        bool Load(const std::vector<std::string>& rows, std::string& error);
        void Reset();
        bool Move(int dx, int dy);
        bool IsComplete() const;
        void SetLevelContext(const std::string& levelName, int levelNumber, int levelCount);
        void SetScoreContext(int levelScore, int totalScore, bool showCompletionScore, int completedLevelScore);
        int GetWidth() const;
        int GetHeight() const;
        int GetMoveCount() const;
        char GetRenderTile(int x, int y) const;
        const std::string& GetLevelName() const;
        int GetLevelNumber() const;
        int GetLevelCount() const;
        int GetLevelScore() const;
        int GetTotalScore() const;
        bool ShouldShowCompletionScore() const;
        int GetCompletedLevelScore() const;

    private:
        bool IsInside(const Position& position) const;
        bool IsWall(const Position& position) const;
        bool IsTarget(const Position& position) const;
        int FindBox(const Position& position) const;

        std::vector<std::string> tiles_;
        std::vector<Position> boxes_;
        std::vector<Position> startBoxes_;
        Position player_ = { 0, 0 };
        Position startPlayer_ = { 0, 0 };
        int width_ = 0;
        int height_ = 0;
        int moveCount_ = 0;
        std::string levelName_;
        int levelNumber_ = 0;
        int levelCount_ = 0;
        int levelScore_ = 0;
        int totalScore_ = 0;
        bool showCompletionScore_ = false;
        int completedLevelScore_ = 0;
    };
}
