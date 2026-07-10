#pragma once

namespace audio
{
    class RaylibAudioPlayer
    {
    public:
        void Initialize() const;
        void PlayMove() const;
        void PlayLevelComplete() const;
        void PlayBell() const;
        void PlayPreviousLevel() const;
        void PlayNextLevel() const;
        bool IsLevelCompletePlaying() const;
    };
}
