#include "RaylibAudioPlayer.h"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <vector>

namespace
{
    constexpr int kSampleRate = 44100;
    constexpr float kAmplitude = 0.24f;
    constexpr float kGapDurationSeconds = 0.012f;
    constexpr float kTwoPi = 6.28318530717958647692f;

    struct Tone
    {
        int frequency;
        int durationMs;
    };

    Wave CreateWaveFromTones(const Tone* tones, int count)
    {
        std::vector<short> samples;

        for (int toneIndex = 0; toneIndex < count; ++toneIndex)
        {
            const int sampleCount = std::max(1, (kSampleRate * tones[toneIndex].durationMs) / 1000);
            const int fadeSampleCount = std::max(1, std::min(sampleCount / 4, kSampleRate / 200));
            const int gapSampleCount = toneIndex + 1 < count ? static_cast<int>(kSampleRate * kGapDurationSeconds) : 0;

            for (int sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
            {
                const float time = static_cast<float>(sampleIndex) / static_cast<float>(kSampleRate);
                const float phase = std::sinf(kTwoPi * static_cast<float>(tones[toneIndex].frequency) * time);
                const float wave = phase >= 0.0f ? 1.0f : -1.0f;

                float envelope = 1.0f;
                if (sampleIndex < fadeSampleCount)
                {
                    envelope = std::min(envelope, static_cast<float>(sampleIndex) / static_cast<float>(fadeSampleCount));
                }

                const int samplesToEnd = sampleCount - sampleIndex - 1;
                if (samplesToEnd < fadeSampleCount)
                {
                    envelope = std::min(envelope, static_cast<float>(samplesToEnd) / static_cast<float>(fadeSampleCount));
                }

                const float value = wave * envelope * kAmplitude;
                samples.push_back(static_cast<short>(value * 32767.0f));
            }

            samples.insert(samples.end(), gapSampleCount, 0);
        }

        Wave wave = { 0 };
        wave.frameCount = static_cast<unsigned int>(samples.size());
        wave.sampleRate = kSampleRate;
        wave.sampleSize = 16;
        wave.channels = 1;
        wave.data = MemAlloc(static_cast<int>(samples.size() * sizeof(short)));
        std::copy(samples.begin(), samples.end(), static_cast<short*>(wave.data));
        return wave;
    }

    Sound CreateSoundFromTones(const Tone* tones, int count)
    {
        Wave wave = CreateWaveFromTones(tones, count);
        Sound sound = LoadSoundFromWave(wave);
        UnloadWave(wave);
        return sound;
    }

    struct AudioState
    {
        bool isReady = false;
        Sound move = { 0 };
        Sound levelComplete = { 0 };
        Sound previousLevel = { 0 };
        Sound nextLevel = { 0 };

        AudioState()
        {
            InitAudioDevice();
            isReady = IsAudioDeviceReady();
            if (!isReady)
            {
                return;
            }

            static constexpr Tone moveTones[] = { { 880, 35 } };
            static constexpr Tone levelCompleteTones[] =
            {
                { 784, 70 },
                { 988, 70 },
                { 1175, 90 },
                { 1568, 140 },
                { 1175, 90 },
                { 1568, 180 }
            };
            static constexpr Tone previousLevelTones[] =
            {
                { 1319, 50 },
                { 988, 70 }
            };
            static constexpr Tone nextLevelTones[] =
            {
                { 988, 50 },
                { 1319, 70 }
            };

            move = CreateSoundFromTones(moveTones, static_cast<int>(sizeof(moveTones) / sizeof(moveTones[0])));
            levelComplete = CreateSoundFromTones(levelCompleteTones, static_cast<int>(sizeof(levelCompleteTones) / sizeof(levelCompleteTones[0])));
            previousLevel = CreateSoundFromTones(previousLevelTones, static_cast<int>(sizeof(previousLevelTones) / sizeof(previousLevelTones[0])));
            nextLevel = CreateSoundFromTones(nextLevelTones, static_cast<int>(sizeof(nextLevelTones) / sizeof(nextLevelTones[0])));
        }

        ~AudioState()
        {
            if (!isReady)
            {
                return;
            }

            if (move.frameCount > 0)
            {
                UnloadSound(move);
            }
            if (levelComplete.frameCount > 0)
            {
                UnloadSound(levelComplete);
            }
            if (previousLevel.frameCount > 0)
            {
                UnloadSound(previousLevel);
            }
            if (nextLevel.frameCount > 0)
            {
                UnloadSound(nextLevel);
            }

            if (IsAudioDeviceReady())
            {
                CloseAudioDevice();
            }
        }
    };

    AudioState& GetAudioState()
    {
        static AudioState state;
        return state;
    }

    void PlaySoundEffect(Sound& sound)
    {
        AudioState& state = GetAudioState();
        if (!state.isReady || sound.frameCount == 0)
        {
            return;
        }

        StopSound(sound);
        PlaySound(sound);
    }
}

namespace audio
{
    void RaylibAudioPlayer::PlayMove() const
    {
        AudioState& state = GetAudioState();
        PlaySoundEffect(state.move);
    }

    void RaylibAudioPlayer::PlayLevelComplete() const
    {
        AudioState& state = GetAudioState();
        PlaySoundEffect(state.levelComplete);
    }

    void RaylibAudioPlayer::PlayPreviousLevel() const
    {
        AudioState& state = GetAudioState();
        PlaySoundEffect(state.previousLevel);
    }

    void RaylibAudioPlayer::PlayNextLevel() const
    {
        AudioState& state = GetAudioState();
        PlaySoundEffect(state.nextLevel);
    }
}
