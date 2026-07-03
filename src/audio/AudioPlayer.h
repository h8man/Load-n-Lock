#pragma once
#pragma once

namespace audio
{
    class AudioPlayer
    {
    public:
        void PlayMove() const;
        void PlayLevelComplete() const;
        void PlayPreviousLevel() const;
        void PlayNextLevel() const;
    };
}
