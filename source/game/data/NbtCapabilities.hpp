#ifndef GAME_DATA_NBTCAPABILITIES_HPP
#define GAME_DATA_NBTCAPABILITIES_HPP

#include <stdio.h>

#include <string>

class CompoundTag;
class Tag;

// what a world's data needs beyond the original format, recorded in level.dat; the NBT checks below cover the one
// that lives in tags, a chunk records the other when it is saved
namespace NbtCapabilities
{
enum ECapability
{
    eCapability_LongArrays = 0x1,
    // a chunk saved with a mod block palette (record version 13)
    eCapability_ModBlockPalette = 0x2
};

const unsigned int kKnown = eCapability_LongArrays | eCapability_ModBlockPalette;
const unsigned int kLegacy = 0;

inline unsigned int Missing(unsigned int recorded)
{
    return recorded & ~kKnown;
}

// the capabilities by name, and as a number the ones this build has no name for, so a refusal can say which
inline std::string Describe(unsigned int capabilities)
{
    std::string text;
    if ((capabilities & eCapability_LongArrays) != 0)
        text += "long arrays";
    if ((capabilities & eCapability_ModBlockPalette) != 0)
        text += std::string(text.empty() ? "" : ", ") + "mod block palettes";
    const unsigned int unnamed = capabilities & ~kKnown;
    if (unnamed != 0)
    {
        char number[32];
        snprintf(number, sizeof(number), "0x%x", unnamed);
        text += std::string(text.empty() ? "" : ", ") + "unknown " + number;
    }
    return text;
}

unsigned int Required(Tag* tag);
CompoundTag* Restricted(CompoundTag* tag, unsigned int allowed);
}

#endif
