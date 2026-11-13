#include "game/data/NbtBudget.hpp"

#if LCE_PLATFORM_PC

namespace
{
#if defined(_MSC_VER)
__declspec(thread) bool sOpen = false;
__declspec(thread) bool sSpent = false;
__declspec(thread) long long sLeft = 0;
#else
__thread bool sOpen = false;
__thread bool sSpent = false;
__thread long long sLeft = 0;
#endif
}

void NbtBudget::Open(long long bytes)
{
    sOpen = true;
    sSpent = false;
    sLeft = bytes;
}

bool NbtBudget::Charge(long long bytes)
{
    if (!sOpen)
        return true;
    if (bytes < 0 || bytes > sLeft)
    {
        sSpent = true;
        sLeft = 0;
        return false;
    }
    sLeft -= bytes;
    return true;
}

bool NbtBudget::Close()
{
    const bool spent = sOpen && sSpent;
    sOpen = false;
    sSpent = false;
    return spent;
}

#endif
