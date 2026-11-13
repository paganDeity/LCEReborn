#include "platform/pc/PcVirtualMemory.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

void* PcVirtualReserve(std::size_t size, bool commit)
{
    return ::VirtualAlloc(NULL, size, commit ? MEM_RESERVE | MEM_COMMIT : MEM_RESERVE, PAGE_READWRITE);
}

bool PcVirtualCommit(void* address, std::size_t size)
{
    return ::VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE) != NULL;
}

bool PcVirtualDecommit(void* address, std::size_t size)
{
    return ::VirtualFree(address, size, MEM_DECOMMIT) != 0;
}

bool PcVirtualRelease(void* address, std::size_t)
{
    return ::VirtualFree(address, 0, MEM_RELEASE) != 0;
}
