#include "game/core/AudioHost.hpp"

#include <SDL3/SDL.h>

#include <map>
#include <mutex>
#include <vector>

#include "archive/ArchiveFiles.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcPaths.hpp"
#include "platform/pc/audio/BinkAudioDecoder.hpp"
#include "platform/pc/audio/PcSoundBank.hpp"

namespace
{
const Logger sLog("AudioHost");
const char kSoundBank[] = "WiiU/Sound/Minecraft.msscmp";

void Put16(std::string& out, unsigned int value)
{
    out.push_back(static_cast<char>(value & 255));
    out.push_back(static_cast<char>((value >> 8) & 255));
}

void Put32(std::string& out, unsigned int value)
{
    Put16(out, value & 0xFFFF);
    Put16(out, value >> 16);
}

class PcAudioHost : public AudioHost
{
public:
    PcAudioHost() : m_next(0), m_bankTried(false), m_subsystem(false) {}

    virtual int Open(int sampleRate, int channels);
    virtual bool Queue(int stream, const void* pcm16, size_t bytes);
    virtual size_t Queued(int stream);
    virtual void Close(int stream);
    virtual bool Decode(const std::string& eventName, std::string* wav);

private:
    bool LoadBank();

    std::mutex m_mutex;
    std::map<int, SDL_AudioStream*> m_streams;
    int m_next;
    PcSoundBank m_bank;
    bool m_bankTried;
    bool m_bankReady = false;
    bool m_subsystem;
    unsigned int m_random = 2463534242u;
};

int PcAudioHost::Open(int sampleRate, int channels)
{
    if (sampleRate < 8000 || sampleRate > 192000 || channels < 1 || channels > 2)
        return 0;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_subsystem)
    {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
        {
            LOG_WARN(sLog, "SDL audio could not be started: %s", SDL_GetError());
            return 0;
        }
        m_subsystem = true;
    }
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_S16;
    spec.channels = channels;
    spec.freq = sampleRate;
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (stream == NULL)
    {
        LOG_WARN(sLog, "The audio device could not be opened: %s", SDL_GetError());
        return 0;
    }
    SDL_ResumeAudioStreamDevice(stream);
    const int id = ++m_next;
    m_streams[id] = stream;
    return id;
}

bool PcAudioHost::Queue(int stream, const void* pcm16, size_t bytes)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<int, SDL_AudioStream*>::iterator found = m_streams.find(stream);
    return found != m_streams.end() && SDL_PutAudioStreamData(found->second, pcm16, static_cast<int>(bytes));
}

size_t PcAudioHost::Queued(int stream)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<int, SDL_AudioStream*>::iterator found = m_streams.find(stream);
    return found == m_streams.end() ? 0 : static_cast<size_t>(SDL_GetAudioStreamQueued(found->second));
}

void PcAudioHost::Close(int stream)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::map<int, SDL_AudioStream*>::iterator found = m_streams.find(stream);
    if (found == m_streams.end())
        return;
    SDL_DestroyAudioStream(found->second);
    m_streams.erase(found);
}

bool PcAudioHost::LoadBank()
{
    if (m_bankTried)
        return m_bankReady;
    m_bankTried = true;
    std::string text;
    if (!archive::Files::Read(PcGameContentFile(kSoundBank), text))
        return false;
    std::vector<unsigned char> bytes(text.begin(), text.end());
    std::string error;
    m_bankReady = m_bank.Load(bytes, &error);
    if (!m_bankReady)
        LOG_WARN(sLog, "The sound bank could not be loaded: %s", error.c_str());
    return m_bankReady;
}

bool PcAudioHost::Decode(const std::string& eventName, std::string* wav)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!LoadBank())
        return false;
    const PcSoundBank::Event* event = m_bank.FindEvent(eventName.c_str());
    if (event == NULL || event->plays.empty() || event->plays[0].choices.empty())
        return false;
    m_random = m_random * 1664525u + 1013904223u;
    const std::vector<PcSoundBank::Choice>& choices = event->plays[0].choices;
    const PcSoundBank::Sound* sound = m_bank.FindSound(choices[(m_random >> 8) % choices.size()].sound.c_str());
    BinkAudioDecoder decoder;
    if (sound == NULL || !decoder.Open(sound->data, sound->size))
        return false;

    const int channels = decoder.Channels();
    std::vector<float> block(static_cast<size_t>(decoder.MaxBlockFrames()) * channels);
    std::string pcm;
    for (int frames = decoder.DecodeBlock(&block[0]); frames > 0; frames = decoder.DecodeBlock(&block[0]))
    {
        for (int index = 0; index < frames * channels; ++index)
        {
            float value = block[index] * 32767.0f;
            value = value > 32767.0f ? 32767.0f : (value < -32768.0f ? -32768.0f : value);
            Put16(pcm, static_cast<unsigned int>(static_cast<int>(value)) & 0xFFFF);
        }
    }
    if (pcm.empty())
        return false;
    wav->clear();
    wav->append("RIFF", 4);
    Put32(*wav, static_cast<unsigned int>(36 + pcm.size()));
    wav->append("WAVEfmt ", 8);
    Put32(*wav, 16);
    Put16(*wav, 1);
    Put16(*wav, static_cast<unsigned int>(channels));
    Put32(*wav, static_cast<unsigned int>(decoder.SampleRate()));
    Put32(*wav, static_cast<unsigned int>(decoder.SampleRate() * channels * 2));
    Put16(*wav, static_cast<unsigned int>(channels * 2));
    Put16(*wav, 16);
    wav->append("data", 4);
    Put32(*wav, static_cast<unsigned int>(pcm.size()));
    wav->append(pcm);
    return true;
}

PcAudioHost sHost;
}

AudioHost& AudioHost::Get()
{
    return sHost;
}
