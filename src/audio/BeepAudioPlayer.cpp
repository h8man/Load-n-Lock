#include "BeepAudioPlayer.h"

#include <Windows.h>

namespace
{
    void PlayToneSequence(const int (*tones)[2], int count)
    {
        for (int i = 0; i < count; ++i)
        {
            Beep(tones[i][0], tones[i][1]);
        }
    }
}

namespace audio
{
    void BeepAudioPlayer::PlayMove() const
    {
        Beep(880, 35);
    }

    void BeepAudioPlayer::PlayLevelComplete() const
    {
        static constexpr int tones[][2] =
        {
            { 784, 70 },
            { 988, 70 },
            { 1175, 90 },
            { 1568, 140 },
            { 1175, 90 },
            { 1568, 180 }
        };

        PlayToneSequence(tones, static_cast<int>(sizeof(tones) / sizeof(tones[0])));
    }

    void BeepAudioPlayer::PlayPreviousLevel() const
    {
        static constexpr int tones[][2] =
        {
            { 1319, 50 },
            { 988, 70 }
        };

        PlayToneSequence(tones, static_cast<int>(sizeof(tones) / sizeof(tones[0])));
    }

    void BeepAudioPlayer::PlayNextLevel() const
    {
        static constexpr int tones[][2] =
        {
            { 988, 50 },
            { 1319, 70 }
        };

        PlayToneSequence(tones, static_cast<int>(sizeof(tones) / sizeof(tones[0])));
    }

    bool BeepAudioPlayer::IsLevelCompletePlaying() const
    {
        return false;
    }
}
