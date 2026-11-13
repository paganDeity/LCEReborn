#include "p2p/P2pRandom.hpp"

#include <windows.h>
#include <bcrypt.h>

namespace p2p
{
bool RandomBytes(void* data, size_t size)
{
    return BCryptGenRandom(NULL, static_cast<PUCHAR>(data), static_cast<ULONG>(size),
                           BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0;
}
}
