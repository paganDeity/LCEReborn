#include "game/core/ModAudio.hpp"

#include "game/core/AudioHost.hpp"
#include "game/core/ModHandles.hpp"

ModAudioStream::ModAudioStream(const std::string& owner, int host) : mOwner(owner), mHost(host)
{
}

ModAudioStream::~ModAudioStream()
{
    AudioHost::Get().Close(mHost);
}

bool ModAudioStream::Queue(const std::string& pcm16) const
{
    return pcm16.size() % 2 == 0 && AudioHost::Get().Queue(mHost, pcm16.data(), pcm16.size());
}

int ModAudioStream::Queued() const
{
    return static_cast<int>(AudioHost::Get().Queued(mHost));
}

void ModAudioStream::Close()
{
    ModHandles::ReleasePersistentObject(this);
}

namespace ModAudio
{
boost::shared_ptr<ModAudioStream> Open(const std::string& owner, int sampleRate, int channels)
{
    const int host = AudioHost::Get().Open(sampleRate, channels);
    if (host == 0)
        return boost::shared_ptr<ModAudioStream>();
    return boost::shared_ptr<ModAudioStream>(new ModAudioStream(owner, host));
}
}

