#ifndef ARCHIVE_ZIPARCHIVE_HPP
#define ARCHIVE_ZIPARCHIVE_HPP

#include <stdint.h>

#include <map>

#include "archive/Archive.hpp"

namespace archive
{
class ZipArchive : public Archive
{
public:
    bool Open(const std::string& path, std::string* error);

    virtual const std::vector<std::string>& Names() const { return m_names; }
    virtual bool Contains(const std::string& name) const { return m_entries.count(name) != 0; }
    virtual bool Read(const std::string& name, std::string& out, std::string* error) const;

private:
    const unsigned char* At(size_t offset) const;

    struct Entry
    {
        uint16_t flags;
        uint16_t method;
        uint32_t crc;
        uint32_t compressedSize;
        uint32_t size;
        uint32_t localOffset;
    };

    std::string m_data;
    std::map<std::string, Entry> m_entries;
    std::vector<std::string> m_names;
};
}

#endif
