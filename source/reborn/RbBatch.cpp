#include "reborn/RbBatch.hpp"

#include <string.h>

#include <zstd.h>

#include "reborn/RbVarint.hpp"

namespace reborn
{
namespace
{
enum EFlag
{
    eFlag_Zstd = 1
};

// the fastest level; a batch is small and sent at once, so time matters more than the last few bytes
const int kZstdLevel = 1;
}

void Batch::Pack(const std::vector<uint8_t>& frames, size_t compressFrom, std::vector<uint8_t>& payload)
{
    payload.clear();
    if (frames.size() >= compressFrom && !frames.empty())
    {
        std::vector<uint8_t> packed(ZSTD_compressBound(frames.size()));
        const size_t written = ZSTD_compress(&packed[0], packed.size(), &frames[0], frames.size(), kZstdLevel);
        if (!ZSTD_isError(written) && written < frames.size())
        {
            Varint::Write(payload, eFlag_Zstd);
            Varint::Write(payload, frames.size());
            payload.insert(payload.end(), packed.begin(), packed.begin() + written);
            return;
        }
    }
    Varint::Write(payload, 0);
    Varint::Write(payload, frames.size());
    payload.insert(payload.end(), frames.begin(), frames.end());
}

bool Batch::Unpack(const uint8_t* payload, size_t size, size_t maxSize, std::vector<uint8_t>& frames)
{
    size_t at = 0;
    uint64_t flags = 0;
    uint64_t length = 0;
    if (!Varint::Read(payload, size, &at, &flags) || !Varint::Read(payload, size, &at, &length) || length > maxSize
        || (flags & ~static_cast<uint64_t>(eFlag_Zstd)) != 0)
        return false;
    frames.resize(static_cast<size_t>(length));
    if ((flags & eFlag_Zstd) == 0)
    {
        if (size - at != length)
            return false;
        if (length != 0)
            memcpy(&frames[0], payload + at, static_cast<size_t>(length));
        return true;
    }
    if (length == 0 || at == size)
        return false;
    const size_t written = ZSTD_decompress(&frames[0], frames.size(), payload + at, size - at);
    return !ZSTD_isError(written) && written == length;
}

bool MemorySource::Read(uint8_t* data, size_t size)
{
    if (size > m_size - m_at)
        return false;
    if (size != 0)
        memcpy(data, m_data + m_at, size);
    m_at += size;
    return true;
}
}
