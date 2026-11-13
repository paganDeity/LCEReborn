#ifndef GAME_CORE_MODAUDIO_HPP
#define GAME_CORE_MODAUDIO_HPP

#include <stdint.h>

#include <string>

#include <boost/shared_ptr.hpp>

class ModAudioStream
{
public:
    ModAudioStream(const std::string& owner, int host);
    ~ModAudioStream();

    const std::string& Owner() const { return mOwner; }
    // Add sound to the end of the channel: 16-bit little-endian samples, interleaved when there are two channels.
    bool Queue(const std::string& pcm16) const;
    // How many bytes are waiting to be played. Keep it from running dry and from growing without bound.
    int Queued() const;
    // Close the channel now.
    void Close();

private:
    std::string mOwner;
    int mHost;
};

namespace ModAudio
{
// A channel to the speakers that you feed with sound. Sound you queue plays in order. It lives until you close it or your mod is unloaded.
// sampleRate is 8000 to 192000 Hz and channels 1 or 2. At most 8 per mod; nil if the audio device cannot be opened.
boost::shared_ptr<ModAudioStream> Open(const std::string& owner, int sampleRate, int channels);
}

#endif
