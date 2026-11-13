#include "game/core/AudioHost.hpp"

namespace
{
class WiiUAudioHost : public AudioHost
{
public:
    virtual int Open(int, int) { return 0; }
    virtual bool Queue(int, const void*, size_t) { return false; }
    virtual size_t Queued(int) { return 0; }
    virtual void Close(int) {}
    virtual bool Decode(const std::string&, std::string*) { return false; }
};
}

AudioHost& AudioHost::Get()
{
    static WiiUAudioHost host;
    return host;
}
