#include "platform/pc/PcSystemInfo.hpp"

#include <cstring>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

static void CpuId(unsigned int leaf, unsigned int registers[4])
{
#if defined(_MSC_VER)
    int values[4] = {0, 0, 0, 0};
    __cpuid(values, static_cast<int>(leaf));
    std::memcpy(registers, values, sizeof(values));
#else
    registers[0] = registers[1] = registers[2] = registers[3] = 0;
    __cpuid(leaf, registers[0], registers[1], registers[2], registers[3]);
#endif
}

std::string PcCpuBrand()
{
    unsigned int registers[4] = {0, 0, 0, 0};
    CpuId(0x80000000u, registers);
    if (registers[0] < 0x80000004u)
        return std::string();
    char brand[49];
    std::memset(brand, 0, sizeof(brand));
    for (unsigned int leaf = 0; leaf < 3; ++leaf)
    {
        CpuId(0x80000002u + leaf, registers);
        std::memcpy(brand + leaf * 16, registers, sizeof(registers));
    }
    return PcTrimSpaces(brand);
}
