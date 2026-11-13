#include "mod/ModDataFiles.hpp"

#include <stdint.h>

#include "archive/ArchiveFiles.hpp"

namespace mod
{
bool DataFiles::Safe(const std::string& relative)
{
    if (relative.empty() || relative.size() > 240 || relative[0] == '/' || relative[relative.size() - 1] == '/')
        return false;
    size_t start = 0;
    for (size_t at = 0; at <= relative.size(); ++at)
    {
        if (at < relative.size())
        {
            const unsigned char letter = static_cast<unsigned char>(relative[at]);
            if (letter < 32 || letter == 127 || letter == '\\' || letter == ':')
                return false;
        }
        if (at == relative.size() || relative[at] == '/')
        {
            const std::string part = relative.substr(start, at - start);
            if (part.empty() || part == "." || part == "..")
                return false;
            start = at + 1;
        }
    }
    return true;
}

bool DataFiles::Exists(const std::string& relative) const
{
    int64_t seconds, nanoseconds;
    uint64_t size;
    return !m_root.empty() && Safe(relative)
           && archive::Files::Stamp(archive::Files::Join(m_root, relative), &seconds, &nanoseconds, &size);
}

bool DataFiles::Read(const std::string& relative, std::string& data) const
{
    return !m_root.empty() && Safe(relative) && archive::Files::Read(archive::Files::Join(m_root, relative), data, m_maxBytes);
}

bool DataFiles::Write(const std::string& relative, const std::string& data) const
{
    if (m_root.empty() || !Safe(relative) || data.size() > m_maxBytes)
        return false;
    const std::string path = archive::Files::Join(m_root, relative);
    const size_t slash = path.rfind('/');
    if (!archive::Files::MakeDirectories(path.substr(0, slash)))
        return false;
    return archive::Files::Write(path, data);
}
}
