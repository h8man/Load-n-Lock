#pragma once

namespace audio
{
    class BeepAudioPlayer
    {
    public:
        void PlayMove() const;
        void PlayLevelComplete() const;
        void PlayPreviousLevel() const;
        void PlayNextLevel() const;
        bool IsLevelCompletePlaying() const;
    };
}
