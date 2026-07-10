#include "GameLogic.h"
#include "GameLogic.h"

#include <algorithm>

namespace game
{
    bool operator==(const Position& left, const Position& right)
    {
        return left.x == right.x && left.y == right.y;
    }

    bool GameLogic::Load(const std::vector<std::string>& rows, std::string& error)
    {
        if (rows.empty())
        {
            error = "Level file is empty.";
            return false;
        }

        width_ = 0;
        for (const std::string& row : rows)
        {
            width_ = std::max(width_, static_cast<int>(row.size()));
        }

        if (width_ == 0)
        {
            error = "Level file does not contain any playable rows.";
            return false;
        }

        height_ = static_cast<int>(rows.size());
        tiles_.assign(height_, std::string(width_, ' '));
        boxes_.clear();
        startBoxes_.clear();
        moveCount_ = 0;

        bool hasPlayer = false;
        int targetCount = 0;

        for (int y = 0; y < height_; ++y)
        {
            const std::string& row = rows[y];
            for (int x = 0; x < width_; ++x)
            {
                const char cell = x < static_cast<int>(row.size()) ? row[x] : ' ';

                switch (cell)
                {
                case '#':
                    tiles_[y][x] = '#';
                    break;
                case '.':
                    tiles_[y][x] = '.';
                    ++targetCount;
                    break;
                case '@':
                    if (hasPlayer)
                    {
                        error = "Level contains more than one player start.";
                        return false;
                    }
                    player_ = { x, y };
                    startPlayer_ = player_;
                    hasPlayer = true;
                    tiles_[y][x] = ' ';
                    break;
                case '+':
                    if (hasPlayer)
                    {
                        error = "Level contains more than one player start.";
                        return false;
                    }
                    player_ = { x, y };
                    startPlayer_ = player_;
                    hasPlayer = true;
                    tiles_[y][x] = '.';
                    ++targetCount;
                    break;
                case '$':
                    boxes_.push_back({ x, y });
                    tiles_[y][x] = ' ';
                    break;
                case '*':
                    boxes_.push_back({ x, y });
                    tiles_[y][x] = '.';
                    ++targetCount;
                    break;
                case ' ':
                    tiles_[y][x] = ' ';
                    break;
                default:
                    error = std::string("Unsupported character in level: '") + cell + "'.";
                    return false;
                }
            }
        }

        if (!hasPlayer)
        {
            error = "Level does not contain a player start.";
            return false;
        }

        if (boxes_.empty())
        {
            error = "Level does not contain any boxes.";
            return false;
        }

        if (targetCount != static_cast<int>(boxes_.size()))
        {
            error = "Level must contain the same number of targets and boxes.";
            return false;
        }

        startBoxes_ = boxes_;
        return true;
    }

    void GameLogic::Reset()
    {
        player_ = startPlayer_;
        boxes_ = startBoxes_;
        moveCount_ = 0;
    }

    bool GameLogic::Move(int dx, int dy)
    {
        const Position next = { player_.x + dx, player_.y + dy };
        if (IsWall(next))
        {
            return false;
        }

        const int boxIndex = FindBox(next);
        if (boxIndex >= 0)
        {
            const Position pushTarget = { next.x + dx, next.y + dy };
            if (IsWall(pushTarget) || FindBox(pushTarget) >= 0)
            {
                return false;
            }

            boxes_[boxIndex] = pushTarget;
        }

        player_ = next;
        ++moveCount_;
        return true;
    }

    bool GameLogic::IsComplete() const
    {
        for (const Position& box : boxes_)
        {
            if (!IsTarget(box))
            {
                return false;
            }
        }

        return true;
    }

    void GameLogic::SetLevelContext(const std::string& levelName, int levelNumber, int levelCount)
    {
        levelName_ = levelName;
        levelNumber_ = levelNumber;
        levelCount_ = levelCount;
    }

    void GameLogic::SetScoreContext(int levelScore, int totalScore, bool showCompletionScore, int completedLevelScore)
    {
        levelScore_ = levelScore;
        totalScore_ = totalScore;
        showCompletionScore_ = showCompletionScore;
        completedLevelScore_ = completedLevelScore;
    }

    void GameLogic::SetCutsceneState(const CutsceneState& cutsceneState)
    {
        cutsceneState_ = cutsceneState;
        SetCutsceneProgress(cutsceneState_, cutsceneState_.progress);
        SetCutsceneOverlay(cutsceneState_, cutsceneState_.isOverlayVisible, cutsceneState_.overlayOpacity);
    }

    void GameLogic::SetStatusMessage(const std::string& statusMessage)
    {
        statusMessage_ = statusMessage;
    }

    int GameLogic::GetWidth() const
    {
        return width_;
    }

    int GameLogic::GetHeight() const
    {
        return height_;
    }

    int GameLogic::GetMoveCount() const
    {
        return moveCount_;
    }

    char GameLogic::GetRenderTile(int x, int y) const
    {
        const Position position = { x, y };
        if (player_ == position)
        {
            return IsTarget(position) ? '+' : '@';
        }

        if (FindBox(position) >= 0)
        {
            return IsTarget(position) ? '*' : '$';
        }

        return tiles_[y][x];
    }

    Position GameLogic::GetPlayerPosition() const
    {
        return player_;
    }

    const std::string& GameLogic::GetLevelName() const
    {
        return levelName_;
    }

    int GameLogic::GetLevelNumber() const
    {
        return levelNumber_;
    }

    int GameLogic::GetLevelCount() const
    {
        return levelCount_;
    }

    int GameLogic::GetLevelScore() const
    {
        return levelScore_;
    }

    int GameLogic::GetTotalScore() const
    {
        return totalScore_;
    }

    bool GameLogic::ShouldShowCompletionScore() const
    {
        return showCompletionScore_;
    }

    int GameLogic::GetCompletedLevelScore() const
    {
        return completedLevelScore_;
    }

    const std::string& GameLogic::GetStatusMessage() const
    {
        return statusMessage_;
    }

    const CutsceneState& GameLogic::GetCutsceneState() const
    {
        return cutsceneState_;
    }

    bool GameLogic::IsInside(const Position& position) const
    {
        return position.x >= 0 && position.x < width_ && position.y >= 0 && position.y < height_;
    }

    bool GameLogic::IsWall(const Position& position) const
    {
        return !IsInside(position) || tiles_[position.y][position.x] == '#';
    }

    bool GameLogic::IsTarget(const Position& position) const
    {
        return IsInside(position) && tiles_[position.y][position.x] == '.';
    }

    int GameLogic::FindBox(const Position& position) const
    {
        for (int i = 0; i < static_cast<int>(boxes_.size()); ++i)
        {
            if (boxes_[i] == position)
            {
                return i;
            }
        }

        return -1;
    }
}
