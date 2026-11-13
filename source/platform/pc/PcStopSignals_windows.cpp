#include "platform/pc/PcStopSignals.hpp"

#include <windows.h>

static BOOL WINAPI HandleConsoleControl(DWORD)
{
    PcRequestStop();
    return TRUE;
}

void PcInstallConsoleStop()
{
    SetConsoleCtrlHandler(HandleConsoleControl, TRUE);
}
