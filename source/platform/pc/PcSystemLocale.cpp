#include "platform/pc/PcSystemLocale.hpp"

#include "platform/pc/PcLaunchOptions.hpp"

std::string PcSystemLocaleName()
{
    const std::string& configured = PcGetLaunchOptions().language;
    if (!configured.empty())
        return configured;
    return PcOperatingSystemLocaleName();
}
