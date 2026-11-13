#ifndef GAME_CORE_AUDIOHOST_HPP
#define GAME_CORE_AUDIOHOST_HPP

#include <stddef.h>

#include <string>

class AudioHost
{
public:
    virtual ~AudioHost() {}

    static AudioHost& Get();

    virtual int Open(int sampleRate, int channels) = 0;
    virtual bool Queue(int stream, const void* pcm16, size_t bytes) = 0;
    virtual size_t Queued(int stream) = 0;
    virtual void Close(int stream) = 0;
    virtual bool Decode(const std::string& eventName, std::string* wav) = 0;
};

#endif
