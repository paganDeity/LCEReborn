#include "net/NetFrameCodec.hpp"

#include <algorithm>

namespace lcenet
{
void FrameCodec::Encode(unsigned char channel, unsigned char flags, const void* data, size_t size,
                        std::vector<unsigned char>& out)
{
    out.resize(kHeaderSize + size);
    out[0] = channel;
    out[1] = flags;
    out[2] = (unsigned char)(size >> 24);
    out[3] = (unsigned char)(size >> 16);
    out[4] = (unsigned char)(size >> 8);
    out[5] = (unsigned char)size;
    if (size > 0)
    {
        const unsigned char* bytes = static_cast<const unsigned char*>(data);
        std::copy(bytes, bytes + size, out.begin() + kHeaderSize);
    }
}

size_t FrameCodec::PayloadSize(const unsigned char header[kHeaderSize])
{
    return ((size_t)header[2] << 24) | ((size_t)header[3] << 16) | ((size_t)header[4] << 8) | header[5];
}
}
