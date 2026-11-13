#include "game/core/ModSounds.hpp"
#include "game/core/ModText.hpp"

#include <math.h>
#include <string.h>

#include <map>
#include <vector>

#include "game/client/audio/SoundEvent.hpp"
#include "game/client/audio/ConsoleSoundEngine.hpp"
#include "game/client/audio/SoundEvents.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/core/AudioHost.hpp"
#include "game/core/Minecraft.hpp"
#include "game/world/generation/MultiplayerLocalPlayer.hpp"
#include "platform/pc/Log.hpp"

static const Logger sLog("ModSounds");

namespace
{
const size_t kMaxSounds = 512;
const size_t kMaxBytes = 16 * 1024 * 1024;
const size_t kMaxVoices = 32;
const float kHearingRange = 16.0f;

struct Sound
{
    std::string wav;
    int sampleRate;
    int channels;
    std::vector<short> samples;
};

struct Voice
{
    int stream;
};

bool sOpen = false;
std::map<const SoundEvent*, Sound> sSounds;
std::map<std::wstring, const SoundEvent*> sByName;
std::vector<Voice> sVoices;

unsigned int Read32(const unsigned char* p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<unsigned int>(p[3]) << 24);
}

unsigned int Read16(const unsigned char* p)
{
    return p[0] | (p[1] << 8);
}

bool Parse(const std::string& wav, Sound* sound)
{
    const unsigned char* data = reinterpret_cast<const unsigned char*>(wav.data());
    if (wav.size() < 44 || memcmp(data, "RIFF", 4) != 0 || memcmp(data + 8, "WAVE", 4) != 0)
        return false;
    size_t position = 12;
    bool gotFormat = false;
    while (position + 8 <= wav.size())
    {
        const unsigned int size = Read32(data + position + 4);
        const unsigned char* body = data + position + 8;
        if (position + 8 + size > wav.size())
            return false;
        if (memcmp(data + position, "fmt ", 4) == 0 && size >= 16)
        {
            if (Read16(body) != 1 || Read16(body + 14) != 16)
                return false;
            sound->channels = static_cast<int>(Read16(body + 2));
            sound->sampleRate = static_cast<int>(Read32(body + 4));
            gotFormat = true;
        }
        else if (memcmp(data + position, "data", 4) == 0 && gotFormat)
        {
            sound->samples.resize(size / 2);
            memcpy(&sound->samples[0], body, (size / 2) * 2);
            return (sound->channels == 1 || sound->channels == 2) && sound->sampleRate >= 8000
                   && sound->sampleRate <= 192000 && !sound->samples.empty();
        }
        position += 8 + size + (size & 1);
    }
    return false;
}
}

void ModSounds::Open()
{
    sOpen = true;
}

void ModSounds::Close()
{
    sOpen = false;
}

bool ModSounds::Declare(const std::string& owner, const std::wstring& name, const std::string& wav)
{
    if (!sOpen || sSounds.size() >= kMaxSounds || wav.size() > kMaxBytes || !ModText::IsName(name, true))
        return false;
    const std::wstring full = std::wstring(owner.begin(), owner.end()) + L":" + name;
    if (sByName.count(full) != 0)
        return false;
    Sound sound;
    if (!Parse(wav, &sound))
        return false;
    sound.wav = wav;
    const SoundEvent* event = SoundEvents::registerModSound(full);
    sSounds[event] = sound;
    sByName[full] = event;
    return true;
}

bool ModSounds::Decode(const std::wstring& name, std::string* wav)
{
    std::map<std::wstring, const SoundEvent*>::const_iterator found = sByName.find(name);
    if (found == sByName.end())
        return false;
    *wav = sSounds[found->second].wav;
    return true;
}

std::string ModSounds::Wav(const std::wstring& name)
{
    std::string wav;
    if (Decode(name, &wav))
        return wav;
    ConsoleSoundEngine* engine = Minecraft::GetInstance()->mSoundEngine;
    const std::wstring plain = CommandArguments::StripNamespace(name);
    const std::string event = std::string("Minecraft/") + engine->ConvertSoundPathToName(plain, false);
    if (!AudioHost::Get().Decode(event, &wav))
        wav.clear();
    return wav;
}

bool ModSounds::Play(const SoundEvent* sound, float x, float y, float z, float volume, float pitch)
{
    std::map<const SoundEvent*, Sound>::const_iterator found = sSounds.find(sound);
    if (found == sSounds.end())
        return false;
    Tick();
    Minecraft* minecraft = Minecraft::GetInstance();
    float gain = volume;
    if (minecraft != NULL && minecraft->mPlayer != NULL)
    {
        const float dx = x - static_cast<float>(minecraft->mPlayer->mX);
        const float dy = y - static_cast<float>(minecraft->mPlayer->mY);
        const float dz = z - static_cast<float>(minecraft->mPlayer->mZ);
        const float distance = sqrtf(dx * dx + dy * dy + dz * dz);
        gain *= distance >= kHearingRange * (volume > 1.0f ? volume : 1.0f) ? 0.0f : 1.0f - distance / (kHearingRange * (volume > 1.0f ? volume : 1.0f));
    }
    if (gain <= 0.0f || sVoices.size() >= kMaxVoices)
        return true;
    if (pitch < 0.25f)
        pitch = 0.25f;
    if (pitch > 4.0f)
        pitch = 4.0f;
    const Sound& source = found->second;
    const size_t frames = source.samples.size() / source.channels;
    const size_t outFrames = static_cast<size_t>(static_cast<float>(frames) / pitch);
    std::vector<short> out(outFrames * source.channels);
    for (size_t frame = 0; frame < outFrames; ++frame)
    {
        const size_t from = static_cast<size_t>(static_cast<float>(frame) * pitch);
        for (int channel = 0; channel < source.channels; ++channel)
        {
            float value = static_cast<float>(source.samples[(from < frames ? from : frames - 1) * source.channels + channel]) * gain;
            if (value > 32767.0f)
                value = 32767.0f;
            if (value < -32768.0f)
                value = -32768.0f;
            out[frame * source.channels + channel] = static_cast<short>(value);
        }
    }
    const int stream = AudioHost::Get().Open(source.sampleRate, source.channels);
    if (stream <= 0)
        return true;
    AudioHost::Get().Queue(stream, &out[0], out.size() * 2);
    LOG_DEBUG(sLog, "Playing a mod sound: %d bytes at volume %.2f", static_cast<int>(out.size() * 2), gain);
    Voice voice = {stream};
    sVoices.push_back(voice);
    return true;
}

void ModSounds::Tick()
{
    for (size_t index = 0; index < sVoices.size();)
    {
        if (AudioHost::Get().Queued(sVoices[index].stream) == 0)
        {
            AudioHost::Get().Close(sVoices[index].stream);
            sVoices.erase(sVoices.begin() + index);
        }
        else
            ++index;
    }
}
