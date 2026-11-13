#include "p2p/P2pIdentityStore.hpp"

#include <stdio.h>

#include "p2p/P2pPrivateFile.hpp"

namespace p2p
{
bool IdentityStore::LoadOrCreate(const std::string& path, Identity& identity)
{
    uint8_t seed[Identity::kSeedSize];
    FILE* file = fopen(path.c_str(), "rb");
    if (file != NULL)
    {
        const bool whole = fread(seed, 1, sizeof(seed), file) == sizeof(seed);
        fclose(file);
        if (!whole)
            return false;
        Identity::FromSeed(seed, identity);
        return true;
    }
    if (!Identity::Generate(identity))
        return false;
    return WritePrivateFile(path, identity.Seed(), Identity::kSeedSize);
}
}
