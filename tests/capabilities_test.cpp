#include <stdio.h>

#include <string>

#include "game/data/NbtCapabilities.hpp"

namespace
{
int failures = 0;

void Check(bool ok, const char* what)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok)
        ++failures;
}
}

int main()
{
    using namespace NbtCapabilities;
    Check(Missing(0) == 0, "a world that records nothing opens anywhere");
    Check(Missing(eCapability_LongArrays | eCapability_ModBlockPalette) == 0, "this build opens what it can write");
    Check(Missing(eCapability_ModBlockPalette | 0x40) == 0x40, "a capability from a newer build is what is missing");
    Check(Missing(kLegacy) == 0 && (kKnown & kLegacy) == kLegacy, "the legacy set is known");
    Check(Describe(eCapability_LongArrays | eCapability_ModBlockPalette) == "long arrays, mod block palettes",
          "known capabilities are named");
    Check(Describe(0x40) == "unknown 0x40", "an unnamed capability is given as its number");
    Check(Describe(eCapability_LongArrays | 0x40) == "long arrays, unknown 0x40", "names and numbers together");
    Check(Describe(0).empty(), "nothing recorded says nothing");
    printf("%s\n", failures == 0 ? "all passed" : "FAILED");
    return failures == 0 ? 0 : 1;
}
