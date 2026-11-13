#include "platform/pc/PcStopSignals.hpp"

#include <atomic>
#include <csignal>

static std::atomic<bool> s_stopSignalled(false);

static void HandleSignal(int)
{
    PcRequestStop();
}

void PcInstallStopSignals()
{
    std::signal(SIGINT, HandleSignal);
    std::signal(SIGTERM, HandleSignal);
    PcInstallConsoleStop();
}

bool PcStopSignalled()
{
    return s_stopSignalled;
}

void PcRequestStop()
{
    s_stopSignalled = true;
}
