#ifndef IDENTITY_IDFILES_HPP
#define IDENTITY_IDFILES_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

namespace identity
{
class Files
{
public:
    static bool Read(const std::string& path, std::vector<uint8_t>& data);
    static bool WritePrivate(const std::string& path, const std::vector<uint8_t>& data);
    static bool MakeDirectory(const std::string& path);
    static bool Remove(const std::string& path);
    static bool List(const std::string& directory, const std::string& extension, std::vector<std::string>& names);
};
}

#endif
