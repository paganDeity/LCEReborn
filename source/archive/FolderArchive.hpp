#ifndef ARCHIVE_FOLDERARCHIVE_HPP
#define ARCHIVE_FOLDERARCHIVE_HPP

#include <set>

#include "archive/Archive.hpp"

namespace archive
{
class FolderArchive : public Archive
{
public:
    bool Open(const std::string& path, std::string* error);

    virtual const std::vector<std::string>& Names() const { return m_names; }
    virtual bool Contains(const std::string& name) const { return m_set.count(name) != 0; }
    virtual bool Read(const std::string& name, std::string& out, std::string* error) const;

private:
    std::string m_root;
    std::vector<std::string> m_names;
    std::set<std::string> m_set;
};
}

#endif
