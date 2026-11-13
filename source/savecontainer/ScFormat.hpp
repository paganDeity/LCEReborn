#ifndef SAVECONTAINER_SCFORMAT_HPP
#define SAVECONTAINER_SCFORMAT_HPP

#include <stdint.h>

namespace savecontainer
{
namespace format
{
const uint32_t kMagic = 0x5345434Cu;
const uint16_t kVersion = 2;
const uint32_t kSegmentMagic = 0x31474553u;
const unsigned int kFileHeaderBytes = 8;
const unsigned int kSegmentOverhead = 12;
const uint8_t kFlagReset = 1;
const uint8_t kKindRaw = 0;
const uint8_t kKindRegion = 1;
const uint8_t kItemRaw = 1;
const uint8_t kItemChunks = 2;
const uint8_t kItemRemoved = 3;
const unsigned int kChunkOverhead = 22;
}
}

#endif
