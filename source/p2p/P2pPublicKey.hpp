#ifndef P2P_P2PPUBLICKEY_HPP
#define P2P_P2PPUBLICKEY_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>

namespace p2p
{
class PublicKey
{
public:
    static const size_t kSize = 32;

    PublicKey();

    bool operator==(const PublicKey& other) const;
    bool operator!=(const PublicKey& other) const { return !(*this == other); }
    std::string AsString() const;

    uint8_t bytes[kSize];
};
}

#endif
