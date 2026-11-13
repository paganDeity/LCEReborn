#include "identity/IdLock.hpp"

#include <windows.h>

namespace identity
{
Lock::Lock() : m_handle(-1)
{
}

Lock::~Lock()
{
    Release();
}

bool Lock::Acquire(const std::string& path)
{
    if (Held())
        return true;
    const HANDLE file = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    m_handle = reinterpret_cast<intptr_t>(file);
    return true;
}

void Lock::Release()
{
    if (!Held())
        return;
    CloseHandle(reinterpret_cast<HANDLE>(m_handle));
    m_handle = -1;
}
}
