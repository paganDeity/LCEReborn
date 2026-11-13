#ifndef SAVECONTAINER_SCREGION_HPP
#define SAVECONTAINER_SCREGION_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace savecontainer
{
class ChunkRef
{
public:
    ChunkRef() : slot(0), timestamp(0), header(0), rawLength(0), payload(NULL), length(0) {}
    uint16_t slot;
    int32_t timestamp;
    uint32_t header;
    uint32_t rawLength;
    const uint8_t* payload;
    size_t length;
};

class Region
{
public:
    static const size_t kSectorBytes = 4096;
    static const size_t kSlots = 1024;
    static const uint32_t kLengthMask = 0x1FFFFFFFu;

    static bool Scan(const uint8_t* file, size_t size, std::vector<ChunkRef>& chunks);
    static void Build(const std::vector<ChunkRef>& chunks, std::vector<uint8_t>& file);
};

bool IsRegionName(const std::vector<uint16_t>& name);
}

#endif
