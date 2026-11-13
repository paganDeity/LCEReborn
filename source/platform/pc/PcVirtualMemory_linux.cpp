#include "platform/pc/PcVirtualMemory.hpp"

#include <sys/mman.h>

void* PcVirtualReserve(std::size_t size, bool commit)
{
    void* memory = mmap(NULL, size, commit ? PROT_READ | PROT_WRITE : PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return memory == MAP_FAILED ? NULL : memory;
}

bool PcVirtualCommit(void* address, std::size_t size)
{
    return mprotect(address, size, PROT_READ | PROT_WRITE) == 0;
}

bool PcVirtualDecommit(void* address, std::size_t size)
{
    madvise(address, size, MADV_DONTNEED);
    return mprotect(address, size, PROT_NONE) == 0;
}

bool PcVirtualRelease(void* address, std::size_t size)
{
    return munmap(address, size) == 0;
}
