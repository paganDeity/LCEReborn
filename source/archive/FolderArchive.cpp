#include "archive/FolderArchive.hpp"

#include <algorithm>

#include "archive/ArchiveFiles.hpp"

namespace archive
{
bool FolderArchive::Open(const std::string& path, std::string* error)
{
    m_root = path;
    m_names.clear();
    m_set.clear();
    if (!Files::ListFiles(path, m_names))
    {
        *error = "the folder could not be listed";
        return false;
    }
    std::sort(m_names.begin(), m_names.end());
    m_set.insert(m_names.begin(), m_names.end());
    return true;
}

bool FolderArchive::Read(const std::string& name, std::string& out, std::string* error) const
{
    if (!Contains(name))
    {
        *error = "missing from the folder";
        return false;
    }
    if (!Files::Read(Files::Join(m_root, name), out))
    {
        *error = "the file could not be read";
        return false;
    }
    return true;
}
}
