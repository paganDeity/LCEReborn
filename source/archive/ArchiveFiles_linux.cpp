#include "archive/ArchiveFiles.hpp"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

namespace archive
{
bool Files::Read(const std::string& path, std::string& data, size_t limit)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (file == NULL)
        return false;
    data.clear();
    char buffer[16384];
    for (size_t got = fread(buffer, 1, sizeof(buffer), file); got > 0; got = fread(buffer, 1, sizeof(buffer), file))
    {
        data.append(buffer, got);
        if (limit != 0 && data.size() > limit)
        {
            fclose(file);
            return false;
        }
    }
    fclose(file);
    return true;
}

bool Files::Write(const std::string& path, const std::string& data)
{
    FILE* file = fopen(path.c_str(), "wb");
    if (file == NULL)
        return false;
    const bool wrote = data.empty() || fwrite(data.data(), 1, data.size(), file) == data.size();
    return fclose(file) == 0 && wrote;
}

bool Files::MakeDirectories(const std::string& path)
{
    for (size_t at = path.find('/', 1); ; at = path.find('/', at + 1))
    {
        const std::string part = path.substr(0, at);
        if (mkdir(part.c_str(), 0755) != 0 && errno != EEXIST)
            return false;
        if (at == std::string::npos)
            return true;
    }
}

bool Files::List(const std::string& directory, std::vector<std::string>* directories, std::vector<std::string>* files)
{
    DIR* handle = opendir(directory.c_str());
    if (handle == NULL)
        return false;
    for (dirent* entry = readdir(handle); entry != NULL; entry = readdir(handle))
    {
        const std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        struct stat info;
        if (stat(Join(directory, name).c_str(), &info) != 0)
            continue;
        if (S_ISDIR(info.st_mode) && directories != NULL)
            directories->push_back(name);
        else if (S_ISREG(info.st_mode) && files != NULL)
            files->push_back(name);
    }
    closedir(handle);
    return true;
}

bool Files::Stamp(const std::string& path, int64_t* seconds, int64_t* nanoseconds, uint64_t* size)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        return false;
    *seconds = info.st_mtim.tv_sec;
    *nanoseconds = info.st_mtim.tv_nsec;
    *size = static_cast<uint64_t>(info.st_size);
    return true;
}
}
