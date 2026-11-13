#include "p2p/P2pClock.hpp"

#include <windows.h>

namespace p2p
{
int64_t NowMs()
{
    return static_cast<int64_t>(GetTickCount64());
}
}
