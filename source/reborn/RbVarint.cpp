#include "reborn/RbVarint.hpp"

namespace reborn
{
void Varint::Write(std::vector<uint8_t>& out, uint64_t value)
{
    while (value >= 0x80)
    {
        out.push_back(static_cast<uint8_t>(value | 0x80));
        value >>= 7;
    }
    out.push_back(static_cast<uint8_t>(value));
}

size_t Varint::Size(uint64_t value)
{
    size_t size = 1;
    while (value >= 0x80)
    {
        value >>= 7;
        ++size;
    }
    return size;
}

bool Varint::Read(const uint8_t* data, size_t size, size_t* position, uint64_t* value)
{
    uint64_t result = 0;
    size_t at = *position;
    for (size_t index = 0; index < kMaxBytes; ++index)
    {
        if (at >= size)
            return false;
        const uint8_t byte = data[at++];
        if (index == kMaxBytes - 1 && byte > 1)
            return false;
        result |= static_cast<uint64_t>(byte & 0x7F) << (7 * index);
        if ((byte & 0x80) == 0)
        {
            *value = result;
            *position = at;
            return true;
        }
    }
    return false;
}
}
