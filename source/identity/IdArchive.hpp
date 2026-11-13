#ifndef IDENTITY_IDARCHIVE_HPP
#define IDENTITY_IDARCHIVE_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

namespace identity
{
class Archive
{
public:
    static const size_t kMaxEntries = 16;
    static const size_t kMaxName = 64;
    static const size_t kMaxTotal = 8 * 1024 * 1024;

    void Set(const std::string& name, const std::vector<uint8_t>& data);
    bool Get(const std::string& name, std::vector<uint8_t>& data) const;
    size_t Count() const { return m_names.size(); }

    std::vector<uint8_t> Write() const;
    static bool Read(const std::vector<uint8_t>& bytes, Archive& archive);
    static bool ValidName(const std::string& name);

private:
    std::vector<std::string> m_names;
    std::vector<std::vector<uint8_t> > m_data;
};
}

#endif
