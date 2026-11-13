#include "platform/pc/PcConsole.hpp"

#include <windows.h>

static unsigned int sPreviousCodePage = 0;

void PcConsoleUseUtf8()
{
    sPreviousCodePage = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
}

void PcConsoleRestore()
{
    if (sPreviousCodePage != 0)
        SetConsoleOutputCP(sPreviousCodePage);
    sPreviousCodePage = 0;
}
