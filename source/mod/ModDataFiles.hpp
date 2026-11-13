#ifndef MOD_MODDATAFILES_HPP
#define MOD_MODDATAFILES_HPP

#include <stddef.h>

#include <string>

namespace mod
{
class DataFiles
{
public:
    DataFiles() : m_maxBytes(0) {}
    DataFiles(const std::string& root, size_t maxBytes) : m_root(root), m_maxBytes(maxBytes) {}

    static bool Safe(const std::string& relative);

    bool Exists(const std::string& relative) const;
    bool Read(const std::string& relative, std::string& data) const;
    bool Write(const std::string& relative, const std::string& data) const;

private:
    std::string m_root;
    size_t m_maxBytes;
};
}

#endif
