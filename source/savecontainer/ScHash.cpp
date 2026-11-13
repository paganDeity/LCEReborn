#include "savecontainer/ScHash.hpp"

#include <string.h>

namespace savecontainer
{
uint64_t Hash64(const uint8_t* data, size_t size, uint64_t seed)
{
    const uint64_t kMultiply = 0x9E3779B97F4A7C15ULL;
    uint64_t hash = seed ^ (size * kMultiply);
    size_t at = 0;
    for (; at + 8 <= size; at += 8)
    {
        uint64_t word;
        memcpy(&word, data + at, 8);
        hash = (hash ^ word) * kMultiply;
        hash ^= hash >> 29;
    }
    uint64_t tail = 0;
    for (size_t index = 0; at + index < size; ++index)
        tail |= static_cast<uint64_t>(data[at + index]) << (8 * index);
    hash = (hash ^ tail) * kMultiply;
    hash ^= hash >> 32;
    hash *= 0xD6E8FEB86659FD93ULL;
    hash ^= hash >> 32;
    return hash;
}
}
