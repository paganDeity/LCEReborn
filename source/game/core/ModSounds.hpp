#ifndef GAME_CORE_MODSOUNDS_HPP
#define GAME_CORE_MODSOUNDS_HPP

#include <string>

class SoundEvent;

class ModSounds
{
public:
    static void Open();
    static void Close();
    // Add a sound, only from Item_staticCtor_after. wav is the bytes of a 16-bit PCM WAV file, mono or stereo (read it with file.read). The sound is
    // mod:name (name is a-z 0-9 _ . up to 32); level:play_sound and /playsound play it like a game sound, and sound.decode gives the file back to a mod
    // that is the sound engine. It is heard up to 16 blocks away, quieter with distance, without direction. False when called too late, the name is
    // taken or the file is not such a WAV.
    static bool Declare(const std::string& owner, const std::wstring& name, const std::string& wav);
    static bool Play(const SoundEvent* sound, float x, float y, float z, float volume, float pitch);
    static bool Decode(const std::wstring& name, std::string* wav);
    // The sound behind a registered sound name, such as minecraft:mob.cow.say, as the bytes of a 16-bit WAV file. Names with several recordings give one of
    // them at random each time. An empty string if the sound is unknown.
    static std::string Wav(const std::wstring& name);
    static void Tick();
};

#endif
