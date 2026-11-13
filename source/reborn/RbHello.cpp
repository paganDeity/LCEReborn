#include "reborn/RbHello.hpp"

#include <string.h>

#include "reborn/RbVarint.hpp"

namespace reborn
{
const uint8_t Preamble::kMagic[4] = {'L', 'C', 'E', 0xB1};

void Preamble::Write(std::vector<uint8_t>& out)
{
    out.insert(out.end(), kMagic, kMagic + 4);
}

Preamble::ESniff Preamble::Sniff(const uint8_t* data, size_t size)
{
    const size_t compare = size < 4 ? size : 4;
    if (memcmp(data, kMagic, compare) != 0)
        return eSniff_Legacy;
    return size >= 4 ? eSniff_Reborn : eSniff_NeedMore;
}

void Hello::Encode(std::vector<uint8_t>& out) const
{
    Varint::Write(out, minVersion);
    Varint::Write(out, maxVersion);
    Varint::Write(out, features);
    Varint::Write(out, registryHash.size());
    out.insert(out.end(), registryHash.begin(), registryHash.end());
    Varint::Write(out, mods.size());
    for (size_t index = 0; index < mods.size(); ++index)
    {
        Varint::Write(out, mods[index].size());
        out.insert(out.end(), mods[index].begin(), mods[index].end());
    }
}

bool Hello::Decode(const uint8_t* data, size_t size, Hello& hello)
{
    hello = Hello();
    size_t at = 0;
    uint64_t value = 0;
    if (!Varint::Read(data, size, &at, &value) || value > 0xFFFFFFFFu)
        return false;
    hello.minVersion = static_cast<uint32_t>(value);
    if (!Varint::Read(data, size, &at, &value) || value > 0xFFFFFFFFu)
        return false;
    hello.maxVersion = static_cast<uint32_t>(value);
    if (!Varint::Read(data, size, &at, &hello.features))
        return false;
    if (!Varint::Read(data, size, &at, &value) || value > 64 || value > size - at)
        return false;
    hello.registryHash.assign(data + at, data + at + static_cast<size_t>(value));
    at += static_cast<size_t>(value);
    uint64_t count = 0;
    if (!Varint::Read(data, size, &at, &count) || count > 4096)
        return false;
    for (uint64_t index = 0; index < count; ++index)
    {
        if (!Varint::Read(data, size, &at, &value) || value > 256 || value > size - at)
            return false;
        hello.mods.push_back(std::string(reinterpret_cast<const char*>(data) + at, static_cast<size_t>(value)));
        at += static_cast<size_t>(value);
    }
    return at == size;
}

ERefusal Negotiate(const Hello& client, const Hello& server, Agreement& agreement, std::string* refusal)
{
    agreement = Agreement();
    const uint32_t low = client.minVersion > server.minVersion ? client.minVersion : server.minVersion;
    const uint32_t high = client.maxVersion < server.maxVersion ? client.maxVersion : server.maxVersion;
    if (client.minVersion > client.maxVersion || server.minVersion > server.maxVersion || low > high)
    {
        if (refusal != NULL)
            *refusal = "no protocol version both sides speak";
        return eRefusal_Version;
    }
    if (client.registryHash != server.registryHash && !server.registryHash.empty() && !client.registryHash.empty())
    {
        if (refusal != NULL)
            *refusal = "the content registries differ";
        return eRefusal_Registry;
    }
    for (size_t index = 0; index < server.mods.size(); ++index)
    {
        bool found = false;
        for (size_t other = 0; other < client.mods.size() && !found; ++other)
            found = client.mods[other] == server.mods[index];
        if (!found)
        {
            if (refusal != NULL)
                *refusal = "the server requires the mod " + server.mods[index];
            return eRefusal_Mod;
        }
    }
    agreement.version = high;
    agreement.features = client.features & server.features;
    return eRefusal_None;
}
}
