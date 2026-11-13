#ifndef NET_NETFRAMECODEC_HPP
#define NET_NETFRAMECODEC_HPP

#include <stddef.h>

#include <vector>

namespace lcenet
{
class FrameCodec
{
public:
    static const size_t kHeaderSize = 6;
    static const size_t kMaxPayload = 16 * 1024 * 1024;

    static void Encode(unsigned char channel, unsigned char flags, const void* data, size_t size,
                       std::vector<unsigned char>& out);
    static size_t PayloadSize(const unsigned char header[kHeaderSize]);
};
}

#endif
