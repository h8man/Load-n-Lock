#pragma once
#pragma once

#ifdef LOAD_AND_LOCK_USE_RAYLIB
#include "RaylibAudioPlayer.h"
#else
#include "BeepAudioPlayer.h"
#endif

namespace audio
{
#ifdef LOAD_AND_LOCK_USE_RAYLIB
    using AudioPlayer = RaylibAudioPlayer;
#else
    using AudioPlayer = BeepAudioPlayer;
#endif
}
