#pragma once

namespace audio
{
    class RaylibAudioPlayer
    {
    public:
        void PlayMove() const;
        void PlayLevelComplete() const;
        void PlayPreviousLevel() const;
        void PlayNextLevel() const;
        bool IsLevelCompletePlaying() const;
    };
}
