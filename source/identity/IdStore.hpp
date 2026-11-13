#ifndef IDENTITY_IDSTORE_HPP
#define IDENTITY_IDSTORE_HPP

#include <string>
#include <vector>

#include "identity/IdProfile.hpp"

namespace identity
{
class Store
{
public:
    explicit Store(const std::string& directory);

    bool Prepare();
    std::vector<std::string> List() const;
    bool Load(const std::string& stem, Profile& profile, std::string* error) const;
    bool Save(const std::string& stem, const Profile& profile) const;
    bool Create(const std::string& name, std::string* stem, Profile* profile);
    bool Remove(const std::string& stem) const;
    std::string Active() const;
    bool SetActive(const std::string& stem) const;
    std::string PathOf(const std::string& stem) const;
    std::string LockPathOf(const std::string& stem) const;
    const std::string& Directory() const { return m_directory; }

    static bool ValidStem(const std::string& stem);

private:
    std::string m_directory;
};
}

#endif
