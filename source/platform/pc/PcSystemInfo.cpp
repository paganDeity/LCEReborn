#include "platform/pc/PcSystemInfo.hpp"

std::string PcTrimSpaces(const std::string& name)
{
    const std::string::size_type first = name.find_first_not_of(" \t");
    const std::string::size_type last = name.find_last_not_of(" \t");
    return first == std::string::npos ? std::string() : name.substr(first, last - first + 1);
}

std::string PcCpuName()
{
    const std::string brand = PcCpuBrand();
    return brand.empty() ? PcOperatingSystemCpuName() : brand;
}
