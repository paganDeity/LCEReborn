#include "p2p/P2pPrivateFile.hpp"

#include <windows.h>

namespace p2p
{
bool WritePrivateFile(const std::string& path, const void* data, size_t size)
{
    const HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    DWORD written = 0;
    const BOOL done = WriteFile(file, data, static_cast<DWORD>(size), &written, NULL);
    return CloseHandle(file) && done && written == size;
}
}
