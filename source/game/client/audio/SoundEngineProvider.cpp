#include "game/client/audio/SoundEngineProvider.hpp"

#include "game/client/audio/ScriptedSoundEngine.hpp"
#include "game/client/audio/SoundEngine.hpp"
#include "game/core/ModHooks.hpp"

namespace SoundEngineProvider
{
ConsoleSoundEngine* Create()
{
    if (ModHooks::IsActive(kModHook_sound_start))
        return new ScriptedSoundEngine();
    return new SoundEngine();
}
}
