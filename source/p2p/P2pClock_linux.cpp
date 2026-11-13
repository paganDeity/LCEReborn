#include "p2p/P2pClock.hpp"

#include <time.h>

namespace p2p
{
int64_t NowMs()
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<int64_t>(now.tv_sec) * 1000 + now.tv_nsec / 1000000;
}
}
