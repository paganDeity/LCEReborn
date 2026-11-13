#include "archive/ArchiveFiles.hpp"

#include <windows.h>

namespace archive
{
bool Files::Read(const std::string& path, std::string& data, size_t limit)
{
    const HANDLE file = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    data.clear();
    char buffer[16384];
    DWORD got = 0;
    while (ReadFile(file, buffer, sizeof(buffer), &got, NULL) && got > 0)
    {
        data.append(buffer, got);
        if (limit != 0 && data.size() > limit)
        {
            CloseHandle(file);
            return false;
        }
    }
    CloseHandle(file);
    return true;
}

bool Files::Write(const std::string& path, const std::string& data)
{
    const HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    DWORD wrote = 0;
    const bool ok = data.empty() || (WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &wrote, NULL) && wrote == data.size());
    CloseHandle(file);
    return ok;
}

bool Files::MakeDirectories(const std::string& path)
{
    for (size_t at = path.find('/', 1); ; at = path.find('/', at + 1))
    {
        const std::string part = path.substr(0, at);
        if (!CreateDirectoryA(part.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
            return false;
        if (at == std::string::npos)
            return true;
    }
}

bool Files::List(const std::string& directory, std::vector<std::string>* directories, std::vector<std::string>* files)
{
    WIN32_FIND_DATAA found;
    const HANDLE handle = FindFirstFileA(Join(directory, "*").c_str(), &found);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    do
    {
        const std::string name = found.cFileName;
        if (name == "." || name == "..")
            continue;
        if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            if (directories != NULL)
                directories->push_back(name);
        }
        else if (files != NULL)
            files->push_back(name);
    } while (FindNextFileA(handle, &found));
    FindClose(handle);
    return true;
}

bool Files::Stamp(const std::string& path, int64_t* seconds, int64_t* nanoseconds, uint64_t* size)
{
    WIN32_FILE_ATTRIBUTE_DATA info;
    if (!GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &info))
        return false;
    const uint64_t ticks = (static_cast<uint64_t>(info.ftLastWriteTime.dwHighDateTime) << 32) | info.ftLastWriteTime.dwLowDateTime;
    *seconds = static_cast<int64_t>(ticks / 10000000ULL);
    *nanoseconds = static_cast<int64_t>((ticks % 10000000ULL) * 100);
    *size = (static_cast<uint64_t>(info.nFileSizeHigh) << 32) | info.nFileSizeLow;
    return true;
}
}
