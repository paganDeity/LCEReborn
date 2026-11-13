#ifndef REBORN_RBBATCH_HPP
#define REBORN_RBBATCH_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "reborn/RbFrame.hpp"

namespace reborn
{
// several frames sent as the payload of one, so a tick's packets cost one header and compress together
class Batch
{
public:
    // frames are whole frames back to back; they are compressed with zstd when there are at least compressFrom bytes
    // and the result is smaller
    static void Pack(const std::vector<uint8_t>& frames, size_t compressFrom, std::vector<uint8_t>& payload);
    // false on a flag this build does not know, frames longer than maxSize, or compressed data that does not unpack
    static bool Unpack(const uint8_t* payload, size_t size, size_t maxSize, std::vector<uint8_t>& frames);
};

class MemorySource : public ByteSource
{
public:
    MemorySource(const uint8_t* data, size_t size) : m_data(data), m_size(size), m_at(0) {}

    virtual bool Read(uint8_t* data, size_t size);
    bool AtEnd() const { return m_at == m_size; }

private:
    const uint8_t* m_data;
    size_t m_size;
    size_t m_at;
};
}

#endif
