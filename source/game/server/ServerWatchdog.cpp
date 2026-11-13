#include "game/server/ServerWatchdog.hpp"

namespace
{
volatile long long sLastBeat = 0;
int sLimit = 60;
}

void ServerWatchdog::Beat(long long nowMs)
{
    sLastBeat = nowMs;
}

void ServerWatchdog::Rest()
{
    sLastBeat = 0;
}

int ServerWatchdog::StalledSeconds(long long nowMs)
{
    const long long last = sLastBeat;
    if (last == 0 || nowMs <= last)
        return 0;
    return static_cast<int>((nowMs - last) / 1000);
}

int ServerWatchdog::Limit()
{
    return sLimit;
}

void ServerWatchdog::SetLimit(int seconds)
{
    sLimit = seconds;
}

bool ServerWatchdog::Stalled(long long nowMs)
{
    return sLimit > 0 && StalledSeconds(nowMs) >= sLimit;
}
