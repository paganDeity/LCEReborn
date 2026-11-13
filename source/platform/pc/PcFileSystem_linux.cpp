#include "platform/pc/PcFileSystem.hpp"

#include <sys/types.h>

#include <cctype>
#include <mutex>
#include <system_error>
#include <unordered_map>

#include "platform/pc/PcPaths.hpp"

namespace fs = std::filesystem;

fs::path PcGamePath(const std::string& text)
{
    return fs::path(text);
}

std::string PcGameName(const fs::path& path)
{
    return path.native();
}

static bool SameNameIgnoringCase(const std::string& first, const std::string& second)
{
    if (first.size() != second.size())
        return false;
    for (std::string::size_type index = 0; index < first.size(); ++index)
    {
        if (std::tolower(static_cast<unsigned char>(first[index]))
            != std::tolower(static_cast<unsigned char>(second[index])))
            return false;
    }
    return true;
}

fs::path PcContentPath(const fs::path& path)
{
    std::error_code error;
    if (fs::exists(path, error))
        return path;

    static std::mutex mutex;
    static std::unordered_map<std::string, fs::path> matches;
    std::lock_guard<std::mutex> lock(mutex);
    std::unordered_map<std::string, fs::path>::const_iterator known = matches.find(path.native());
    if (known != matches.end())
        return known->second;

    const fs::path root(PcContentRoot());
    fs::path resolved = root;
    bool found = true;
    for (const fs::path& part : path.lexically_relative(root))
    {
        fs::path next = resolved / part;
        if (found && !fs::exists(next, error))
        {
            found = false;
            for (fs::directory_iterator entry(resolved, error), end; !error && entry != end; entry.increment(error))
            {
                if (SameNameIgnoringCase(entry->path().filename().native(), part.native()))
                {
                    next = entry->path();
                    found = true;
                    break;
                }
            }
        }
        resolved = next;
    }
    matches.emplace(path.native(), found ? resolved : path);
    return found ? resolved : path;
}

std::FILE* PcOpenFile(const fs::path& path, const char* mode)
{
    return std::fopen(path.c_str(), mode);
}

bool PcSeekFile(std::FILE* stream, long long offset, int origin)
{
    return fseeko(stream, static_cast<off_t>(offset), origin) == 0;
}

long long PcTellFile(std::FILE* stream)
{
    return static_cast<long long>(ftello(stream));
}
