#include "savecontainer/ScCrc32.hpp"

namespace savecontainer
{
namespace
{
struct Table
{
    Table()
    {
        for (uint32_t index = 0; index < 256; ++index)
        {
            uint32_t value = index;
            for (int bit = 0; bit < 8; ++bit)
                value = (value & 1) != 0 ? (value >> 1) ^ 0xEDB88320u : value >> 1;
            entries[index] = value;
        }
    }
    uint32_t entries[256];
};

const Table kTable;
}

uint32_t Crc32(const uint8_t* data, size_t size, uint32_t seed)
{
    uint32_t value = ~seed;
    for (size_t index = 0; index < size; ++index)
        value = kTable.entries[(value ^ data[index]) & 0xFF] ^ (value >> 8);
    return ~value;
}
}
