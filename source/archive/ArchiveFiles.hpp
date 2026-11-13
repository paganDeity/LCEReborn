#ifndef ARCHIVE_ARCHIVEFILES_HPP
#define ARCHIVE_ARCHIVEFILES_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

namespace archive
{
class Files
{
public:
    static bool Read(const std::string& path, std::string& data, size_t limit = 0);
    static bool Write(const std::string& path, const std::string& data);
    static bool MakeDirectories(const std::string& path);
    static bool List(const std::string& directory, std::vector<std::string>* directories, std::vector<std::string>* files);
    static bool Stamp(const std::string& path, int64_t* seconds, int64_t* nanoseconds, uint64_t* size);

    static bool IsDirectory(const std::string& path);
    static bool ListDirectories(const std::string& directory, std::vector<std::string>& names);
    static bool ListFiles(const std::string& directory, std::vector<std::string>& names);
    static std::string Join(const std::string& directory, const std::string& name);
};
}

#endif
