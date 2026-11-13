#include "identity/IdFiles.hpp"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

namespace identity
{
bool Files::Read(const std::string& path, std::vector<uint8_t>& data)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (file == NULL)
        return false;
    data.clear();
    uint8_t buffer[4096];
    for (size_t got = fread(buffer, 1, sizeof(buffer), file); got > 0; got = fread(buffer, 1, sizeof(buffer), file))
    {
        data.insert(data.end(), buffer, buffer + got);
        if (data.size() > 16 * 1024 * 1024)
        {
            fclose(file);
            return false;
        }
    }
    fclose(file);
    return true;
}

bool Files::WritePrivate(const std::string& path, const std::vector<uint8_t>& data)
{
    const std::string temporary = path + ".tmp";
    const int file = open(temporary.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (file < 0)
        return false;
    const bool written = data.empty() || write(file, &data[0], data.size()) == static_cast<ssize_t>(data.size());
    const bool closed = close(file) == 0;
    if (!written || !closed || rename(temporary.c_str(), path.c_str()) != 0)
    {
        unlink(temporary.c_str());
        return false;
    }
    return true;
}

bool Files::MakeDirectory(const std::string& path)
{
    return mkdir(path.c_str(), 0700) == 0 || errno == EEXIST;
}

bool Files::Remove(const std::string& path)
{
    return unlink(path.c_str()) == 0;
}

bool Files::List(const std::string& directory, const std::string& extension, std::vector<std::string>& names)
{
    DIR* handle = opendir(directory.c_str());
    if (handle == NULL)
        return false;
    for (dirent* entry = readdir(handle); entry != NULL; entry = readdir(handle))
    {
        const std::string name = entry->d_name;
        if (name.size() > extension.size() && name.compare(name.size() - extension.size(), extension.size(), extension) == 0)
            names.push_back(name.substr(0, name.size() - extension.size()));
    }
    closedir(handle);
    return true;
}
}
