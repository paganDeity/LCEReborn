#include "savecontainer/ScRegion.hpp"

#include <string.h>

namespace savecontainer
{
namespace
{
const size_t kRegionHeaderBytes = Region::kSectorBytes * 2;
const size_t kRecordHeaderBytes = 8;

uint32_t Read32(const uint8_t* at)
{
    return static_cast<uint32_t>(at[0]) | (static_cast<uint32_t>(at[1]) << 8) | (static_cast<uint32_t>(at[2]) << 16)
           | (static_cast<uint32_t>(at[3]) << 24);
}

void Write32(uint8_t* at, uint32_t value)
{
    at[0] = static_cast<uint8_t>(value);
    at[1] = static_cast<uint8_t>(value >> 8);
    at[2] = static_cast<uint8_t>(value >> 16);
    at[3] = static_cast<uint8_t>(value >> 24);
}
}

bool IsRegionName(const std::vector<uint16_t>& name)
{
    static const char kSuffix[] = ".mcr";
    if (name.size() < 4)
        return false;
    for (size_t index = 0; index < 4; ++index)
        if (name[name.size() - 4 + index] != static_cast<uint16_t>(kSuffix[index]))
            return false;
    return true;
}

bool Region::Scan(const uint8_t* file, size_t size, std::vector<ChunkRef>& chunks)
{
    chunks.clear();
    if (size < kRegionHeaderBytes)
        return false;
    const size_t sectors = (size + kSectorBytes - 1) / kSectorBytes;
    std::vector<bool> taken(sectors, false);
    taken[0] = true;
    taken[1] = true;
    for (size_t slot = 0; slot < kSlots; ++slot)
    {
        const uint32_t entry = Read32(file + slot * 4);
        if (entry == 0)
            continue;
        const size_t first = entry >> 8;
        size_t count = entry & 0xFF;
        if (count == 0 || first < 2 || first >= sectors)
            return false;
        size_t room = count * kSectorBytes;
        if (first * kSectorBytes + room > size)
            room = size - first * kSectorBytes;
        if (first + count > sectors)
            count = sectors - first;
        for (size_t sector = first; sector < first + count; ++sector)
        {
            if (taken[sector])
                return false;
            taken[sector] = true;
        }
        const uint8_t* at = file + first * kSectorBytes;
        ChunkRef chunk;
        chunk.slot = static_cast<uint16_t>(slot);
        chunk.timestamp = static_cast<int32_t>(Read32(file + kSectorBytes + slot * 4));
        chunk.header = Read32(at);
        chunk.rawLength = Read32(at + 4);
        chunk.length = chunk.header & kLengthMask;
        if (chunk.length + kRecordHeaderBytes > room)
            return false;
        chunk.payload = at + kRecordHeaderBytes;
        chunks.push_back(chunk);
    }
    return true;
}

void Region::Build(const std::vector<ChunkRef>& chunks, std::vector<uint8_t>& file)
{
    size_t sectors = 2;
    for (size_t index = 0; index < chunks.size(); ++index)
        sectors += (chunks[index].length + kRecordHeaderBytes) / kSectorBytes + 1;
    file.assign(sectors * kSectorBytes, 0);
    size_t next = 2;
    for (size_t index = 0; index < chunks.size(); ++index)
    {
        const ChunkRef& chunk = chunks[index];
        const size_t count = (chunk.length + kRecordHeaderBytes) / kSectorBytes + 1;
        Write32(&file[chunk.slot * 4], static_cast<uint32_t>((next << 8) | count));
        Write32(&file[kSectorBytes + chunk.slot * 4], static_cast<uint32_t>(chunk.timestamp));
        uint8_t* at = &file[next * kSectorBytes];
        Write32(at, chunk.header);
        Write32(at + 4, chunk.rawLength);
        if (chunk.length != 0)
            memcpy(at + kRecordHeaderBytes, chunk.payload, chunk.length);
        next += count;
    }
}
}
