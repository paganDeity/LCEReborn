#include "identity/IdProfile.hpp"

#include <string.h>

#include "identity/IdArchive.hpp"
#include "identity/IdUuid.hpp"
#include "monocypher.h"

namespace identity
{
int Profile::DefaultSkin(const std::string& name)
{
    uint32_t hash = 2166136261u ^ 10u;
    for (size_t index = 0; index < name.size(); ++index)
    {
        char letter = name[index];
        if (letter >= 'A' && letter <= 'Z')
            letter = static_cast<char>(letter - 'A' + 'a');
        hash = (hash ^ static_cast<unsigned char>(letter)) * 16777619u;
    }
    hash ^= hash >> 16;
    hash *= 0x85ebca6bu;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35u;
    hash ^= hash >> 16;
    return 1 + static_cast<int>(hash % 18u);
}

namespace
{
const char kSignatureLabel[] = "lce-identity manifest v1\n";
const char kSkinEntry[] = "skin.png";

std::string Hex(const uint8_t* bytes, size_t size)
{
    static const char digits[] = "0123456789abcdef";
    std::string text;
    for (size_t index = 0; index < size; ++index)
    {
        text += digits[bytes[index] >> 4];
        text += digits[bytes[index] & 15];
    }
    return text;
}

bool FromHex(const std::string& text, uint8_t* out, size_t size)
{
    if (text.size() != size * 2)
        return false;
    for (size_t index = 0; index < size; ++index)
    {
        unsigned int byte = 0;
        for (size_t digit = 0; digit < 2; ++digit)
        {
            const char letter = text[index * 2 + digit];
            unsigned int value;
            if (letter >= '0' && letter <= '9')
                value = static_cast<unsigned int>(letter - '0');
            else if (letter >= 'a' && letter <= 'f')
                value = static_cast<unsigned int>(letter - 'a' + 10);
            else
                return false;
            byte = (byte << 4) | value;
        }
        out[index] = static_cast<uint8_t>(byte);
    }
    return true;
}

std::string Hash(const std::vector<uint8_t>& data)
{
    uint8_t hash[32];
    crypto_blake2b(hash, sizeof(hash), data.empty() ? NULL : &data[0], data.size());
    return Hex(hash, sizeof(hash));
}

bool Fail(std::string* error, const char* text)
{
    if (error != NULL)
        *error = text;
    return false;
}

bool ValidSkinName(const std::string& name)
{
    if (name.empty() || name.size() > 96)
        return false;
    for (size_t index = 0; index < name.size(); ++index)
        if (static_cast<unsigned char>(name[index]) < 0x20 || static_cast<unsigned char>(name[index]) == 0x7f)
            return false;
    return true;
}

bool LooksLikePng(const std::vector<uint8_t>& data)
{
    static const uint8_t signature[8] = {0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a};
    return data.size() >= 8 && memcmp(&data[0], signature, 8) == 0;
}

std::string Line(const std::string& text, const std::string& key)
{
    const std::string wanted = key + "=";
    size_t start = 0;
    while (start < text.size())
    {
        size_t end = text.find('\n', start);
        if (end == std::string::npos)
            end = text.size();
        if (text.compare(start, wanted.size(), wanted) == 0)
            return text.substr(start + wanted.size(), end - start - wanted.size());
        start = end + 1;
    }
    return std::string();
}
}

Profile::Profile()
{
}

bool Profile::ValidName(const std::string& name)
{
    if (name.size() < 3 || name.size() > 16)
        return false;
    for (size_t index = 0; index < name.size(); ++index)
    {
        const char letter = name[index];
        const bool plain = (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z')
                           || (letter >= '0' && letter <= '9') || letter == '_';
        if (!plain)
            return false;
    }
    return true;
}

bool Profile::Generate(const std::string& name, Profile& profile)
{
    if (!ValidName(name))
        return false;
    Profile created;
    if (!p2p::Identity::Generate(created.m_key))
        return false;
    created.m_name = name;
    profile = created;
    return true;
}

bool Profile::SetName(const std::string& name)
{
    if (!ValidName(name))
        return false;
    m_name = name;
    return true;
}

bool Profile::SetNamedSkin(const std::string& name)
{
    if (!ValidSkinName(name))
        return false;
    m_skinName = name;
    m_skinData.clear();
    return true;
}

bool Profile::SetBundledSkin(const std::vector<uint8_t>& png)
{
    if (!LooksLikePng(png) || png.size() > kMaxSkin)
        return false;
    m_skinData = png;
    m_skinName.clear();
    return true;
}

void Profile::ClearSkin()
{
    m_skinName.clear();
    m_skinData.clear();
}

std::string Profile::Uuid() const
{
    return identity::Uuid::HexOf(m_key.Public());
}

std::string Profile::SkinHash() const
{
    return m_skinData.empty() ? std::string() : Hash(m_skinData);
}

std::string Profile::Manifest() const
{
    std::string text = "lce-identity=1\n";
    text += "name=" + m_name + "\n";
    text += "public-key=" + Hex(m_key.Public().bytes, p2p::PublicKey::kSize) + "\n";
    text += "uuid=" + Uuid() + "\n";
    if (!m_skinData.empty())
    {
        text += std::string("skin-file=") + kSkinEntry + "\n";
        text += "skin-hash=" + Hash(m_skinData) + "\n";
    }
    else if (!m_skinName.empty())
        text += "skin=" + m_skinName + "\n";
    return text;
}

std::vector<uint8_t> Profile::Save() const
{
    const std::string manifest = Manifest();
    std::vector<uint8_t> signedBytes(kSignatureLabel, kSignatureLabel + sizeof(kSignatureLabel) - 1);
    signedBytes.insert(signedBytes.end(), manifest.begin(), manifest.end());
    uint8_t signature[p2p::Identity::kSignatureSize];
    m_key.Sign(&signedBytes[0], signedBytes.size(), signature);
    Archive archive;
    archive.Set("identity.txt", std::vector<uint8_t>(manifest.begin(), manifest.end()));
    archive.Set("identity.sig", std::vector<uint8_t>(signature, signature + sizeof(signature)));
    archive.Set("identity.key", std::vector<uint8_t>(m_key.Seed(), m_key.Seed() + p2p::Identity::kSeedSize));
    if (!m_skinData.empty())
        archive.Set(kSkinEntry, m_skinData);
    return archive.Write();
}

bool Profile::Load(const std::vector<uint8_t>& bytes, Profile& profile, std::string* error)
{
    Archive archive;
    if (!Archive::Read(bytes, archive))
        return Fail(error, "not an identity file");
    std::vector<uint8_t> manifestBytes, signature, seed, skin;
    if (!archive.Get("identity.txt", manifestBytes) || !archive.Get("identity.sig", signature)
        || !archive.Get("identity.key", seed))
        return Fail(error, "an identity file is missing a part");
    if (signature.size() != p2p::Identity::kSignatureSize || seed.size() != p2p::Identity::kSeedSize)
        return Fail(error, "an identity file has a part of the wrong size");
    const std::string manifest(manifestBytes.begin(), manifestBytes.end());
    if (Line(manifest, "lce-identity") != "1")
        return Fail(error, "an identity file from a newer version");
    Profile loaded;
    p2p::Identity::FromSeed(&seed[0], loaded.m_key);
    if (Hex(loaded.m_key.Public().bytes, p2p::PublicKey::kSize) != Line(manifest, "public-key"))
        return Fail(error, "the key does not match the identity");
    std::vector<uint8_t> signedBytes(kSignatureLabel, kSignatureLabel + sizeof(kSignatureLabel) - 1);
    signedBytes.insert(signedBytes.end(), manifestBytes.begin(), manifestBytes.end());
    if (!p2p::Identity::Verify(loaded.m_key.Public(), &signedBytes[0], signedBytes.size(), &signature[0]))
        return Fail(error, "the identity has been changed");
    if (Line(manifest, "uuid") != loaded.Uuid())
        return Fail(error, "the identity's UUID does not come from its key");
    if (!loaded.SetName(Line(manifest, "name")))
        return Fail(error, "the identity's name is not valid");
    if (!Line(manifest, "skin-file").empty())
    {
        if (Line(manifest, "skin-file") != kSkinEntry || !archive.Get(kSkinEntry, skin) || !loaded.SetBundledSkin(skin))
            return Fail(error, "the identity's skin is missing or not a PNG");
        if (loaded.SkinHash() != Line(manifest, "skin-hash"))
            return Fail(error, "the identity's skin does not match its hash");
    }
    else if (!Line(manifest, "skin").empty() && !loaded.SetNamedSkin(Line(manifest, "skin")))
        return Fail(error, "the identity's skin name is not valid");
    if (loaded.Manifest() != manifest)
        return Fail(error, "the identity file is not in its canonical form");
    profile = loaded;
    return true;
}
}
