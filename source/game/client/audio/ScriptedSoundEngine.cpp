#include "game/client/audio/ScriptedSoundEngine.hpp"

#include <cmath>

#include "game/core/Minecraft.hpp"
#include "game/core/ModEvents_generated.hpp"
#include "game/entity/mob/Mob.hpp"

namespace
{
enum ESoundKind
{
    eSoundKind_World,
    eSoundKind_UI,
    eSoundKind_Music
};
}

void ScriptedSoundEngine::tick(boost::shared_ptr<Mob>*, float)
{
}

void ScriptedSoundEngine::play(const SoundEvent* sound, float x, float y, float z, float volume, float pitch,
                               SoundInstance*)
{
    ModEvents::SoundStart(sound, x, y, z, volume, pitch, eSoundKind_World);
}

void ScriptedSoundEngine::playStreaming(const SoundEvent* sound, float x, float y, float z, float volume, float pitch,
                                        bool, bool, eAudioDelayType)
{
    ModEvents::SoundStart(sound, x, y, z, volume, pitch, eSoundKind_Music);
}

void ScriptedSoundEngine::playUI(const SoundEvent* sound, float volume, float pitch)
{
    ModEvents::SoundStart(sound, 0.0f, 0.0f, 0.0f, volume, pitch, eSoundKind_UI);
}

void ScriptedSoundEngine::updateMusicVolume(float volume)
{
    setMasterMusicVolume(volume);
}

void ScriptedSoundEngine::updateSoundEffectVolume(float volume)
{
    setMasterSoundEffectVolume(volume);
}

void ScriptedSoundEngine::destroy() {}
void ScriptedSoundEngine::shutdown() {}
void ScriptedSoundEngine::init(Options*) {}
void ScriptedSoundEngine::add(const std::wstring&, File*) {}
void ScriptedSoundEngine::addMusic(const std::wstring&, File*) {}
void ScriptedSoundEngine::addStreaming(const std::wstring&, File*) {}
void ScriptedSoundEngine::playMusicTick() {}
void ScriptedSoundEngine::SetStreamingSounds(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int,
                                             int, int, int)
{
}
void ScriptedSoundEngine::FlagMusicEventStart() {}
