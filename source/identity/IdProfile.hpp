#ifndef IDENTITY_IDPROFILE_HPP
#define IDENTITY_IDPROFILE_HPP

#include <stdint.h>

#include <string>
#include <vector>

#include "p2p/P2pIdentity.hpp"

namespace identity
{
class Profile
{
public:
    static const size_t kMaxSkin = 1024 * 1024;

    Profile();

    static bool Generate(const std::string& name, Profile& profile);
    static bool ValidName(const std::string& name);
    static int DefaultSkin(const std::string& name);
    static bool Load(const std::vector<uint8_t>& bytes, Profile& profile, std::string* error);

    const std::string& Name() const { return m_name; }
    const std::string& SkinName() const { return m_skinName; }
    const std::vector<uint8_t>& SkinData() const { return m_skinData; }
    const p2p::Identity& Key() const { return m_key; }
    bool HasBundledSkin() const { return !m_skinData.empty(); }

    bool SetName(const std::string& name);
    bool SetNamedSkin(const std::string& name);
    bool SetBundledSkin(const std::vector<uint8_t>& png);
    void ClearSkin();

    std::string Uuid() const;
    std::string SkinHash() const;
    std::vector<uint8_t> Save() const;

private:
    std::string Manifest() const;

    std::string m_name;
    std::string m_skinName;
    std::vector<uint8_t> m_skinData;
    p2p::Identity m_key;
};
}

#endif
