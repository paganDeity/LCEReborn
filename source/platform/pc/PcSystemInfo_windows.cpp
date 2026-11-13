#include "platform/pc/PcSystemInfo.hpp"

#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <stdlib.h>

bool PcMemoryUsage(unsigned long long* used, unsigned long long* total)
{
    PROCESS_MEMORY_COUNTERS counters;
    std::memset(&counters, 0, sizeof(counters));
    counters.cb = sizeof(counters);
    if (!K32GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters)))
        return false;
    MEMORYSTATUSEX status;
    std::memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status))
        return false;
    *used = counters.WorkingSetSize;
    *total = status.ullTotalPhys;
    return true;
}

std::string PcOperatingSystemCpuName()
{
    return std::string();
}

std::wstring PcEnvironmentString(const char* variable)
{
    const std::wstring wideVariable(variable, variable + std::strlen(variable));
    const wchar_t* value = _wgetenv(wideVariable.c_str());
    return value != nullptr ? std::wstring(value) : std::wstring();
}

std::wstring PcUserName()
{
    return PcEnvironmentString("USERNAME");
}

int PcProcessId()
{
    return static_cast<int>(GetCurrentProcessId());
}
