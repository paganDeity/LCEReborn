#ifndef ARCHIVE_ARCHIVE_HPP
#define ARCHIVE_ARCHIVE_HPP

#include <string>
#include <vector>

namespace archive
{
class Archive
{
public:
    virtual ~Archive() {}

    virtual const std::vector<std::string>& Names() const = 0;
    virtual bool Contains(const std::string& name) const = 0;
    virtual bool Read(const std::string& name, std::string& out, std::string* error) const = 0;

    static Archive* Open(const std::string& path, std::string* error);
};
}

#endif
