#include "identity/IdFiles.hpp"

#include <windows.h>

namespace identity
{
bool Files::Read(const std::string& path, std::vector<uint8_t>& data)
{
    const HANDLE file = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    data.clear();
    uint8_t buffer[4096];
    DWORD got = 0;
    while (ReadFile(file, buffer, sizeof(buffer), &got, NULL) && got > 0)
    {
        data.insert(data.end(), buffer, buffer + got);
        if (data.size() > 16 * 1024 * 1024)
        {
            CloseHandle(file);
            return false;
        }
    }
    CloseHandle(file);
    return true;
}

bool Files::WritePrivate(const std::string& path, const std::vector<uint8_t>& data)
{
    const std::string temporary = path + ".tmp";
    const HANDLE file = CreateFileA(temporary.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    DWORD written = 0;
    const BOOL done = data.empty() || WriteFile(file, &data[0], static_cast<DWORD>(data.size()), &written, NULL);
    const BOOL closed = CloseHandle(file);
    if (!done || !closed || !MoveFileExA(temporary.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING))
    {
        DeleteFileA(temporary.c_str());
        return false;
    }
    return true;
}

bool Files::MakeDirectory(const std::string& path)
{
    return CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

bool Files::Remove(const std::string& path)
{
    return DeleteFileA(path.c_str()) != 0;
}

bool Files::List(const std::string& directory, const std::string& extension, std::vector<std::string>& names)
{
    WIN32_FIND_DATAA found;
    const HANDLE handle = FindFirstFileA((directory + "\\*" + extension).c_str(), &found);
    if (handle == INVALID_HANDLE_VALUE)
        return GetLastError() == ERROR_FILE_NOT_FOUND;
    do
    {
        const std::string name = found.cFileName;
        if (name.size() > extension.size())
            names.push_back(name.substr(0, name.size() - extension.size()));
    } while (FindNextFileA(handle, &found));
    FindClose(handle);
    return true;
}
}
