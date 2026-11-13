#ifndef REBORN_RBHELLO_HPP
#define REBORN_RBHELLO_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

namespace reborn
{
enum EFeature
{
    eFeature_Batching = 1u << 0,
    eFeature_Deltas = 1u << 1,
    eFeature_LongChat = 1u << 2,
    eFeature_LongArrays = 1u << 3,
    eFeature_Zstd = 1u << 4,
    eFeature_Resume = 1u << 5
};

class Hello
{
public:
    Hello() : minVersion(1), maxVersion(1), features(0) {}

    uint32_t minVersion;
    uint32_t maxVersion;
    uint64_t features;
    std::vector<uint8_t> registryHash;
    std::vector<std::string> mods;

    void Encode(std::vector<uint8_t>& out) const;
    static bool Decode(const uint8_t* data, size_t size, Hello& hello);
};

class Agreement
{
public:
    Agreement() : version(0), features(0) {}

    uint32_t version;
    uint64_t features;
};

class Preamble
{
public:
    enum ESniff
    {
        eSniff_NeedMore,
        eSniff_Legacy,
        eSniff_Reborn
    };

    static const uint8_t kMagic[4];

    static void Write(std::vector<uint8_t>& out);
    static ESniff Sniff(const uint8_t* data, size_t size);
};

enum ERefusal
{
    eRefusal_None,
    eRefusal_Version,
    eRefusal_Registry,
    eRefusal_Mod
};

// what both sides use, or why the server will not take the client; refusal names it for a person
ERefusal Negotiate(const Hello& client, const Hello& server, Agreement& agreement, std::string* refusal);
}

#endif
