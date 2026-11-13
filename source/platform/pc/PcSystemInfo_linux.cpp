#include "platform/pc/PcSystemInfo.hpp"

#include <pwd.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "platform/pc/PcUtf8.hpp"

bool PcMemoryUsage(unsigned long long* used, unsigned long long* total)
{
    const long pageSize = sysconf(_SC_PAGESIZE);
    const long pages = sysconf(_SC_PHYS_PAGES);
    if (pageSize <= 0 || pages <= 0)
        return false;
    std::FILE* statm = std::fopen("/proc/self/statm", "r");
    if (statm == nullptr)
        return false;
    unsigned long long size = 0;
    unsigned long long resident = 0;
    const bool read = std::fscanf(statm, "%llu %llu", &size, &resident) == 2;
    std::fclose(statm);
    if (!read)
        return false;
    *used = resident * static_cast<unsigned long long>(pageSize);
    *total = static_cast<unsigned long long>(pages) * static_cast<unsigned long long>(pageSize);
    return true;
}

std::string PcOperatingSystemCpuName()
{
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line))
    {
        const std::string::size_type colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        const std::string key = PcTrimSpaces(line.substr(0, colon));
        if (key == "model name" || key == "Model" || key == "Hardware")
            return PcTrimSpaces(line.substr(colon + 1));
    }
    return std::string();
}

std::wstring PcEnvironmentString(const char* variable)
{
    const char* value = std::getenv(variable);
    return value != nullptr ? PcUtf8ToWide(value) : std::wstring();
}

std::wstring PcUserName()
{
    std::wstring name = PcEnvironmentString("USER");
    if (name.empty())
    {
        const passwd* entry = getpwuid(geteuid());
        if (entry != nullptr && entry->pw_name != nullptr)
            name = PcUtf8ToWide(entry->pw_name);
    }
    return name;
}

int PcProcessId()
{
    return static_cast<int>(getpid());
}
