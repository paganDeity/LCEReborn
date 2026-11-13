#ifndef IDENTITY_IDUUID_HPP
#define IDENTITY_IDUUID_HPP

#include <stdint.h>

#include <string>

#include "p2p/P2pPublicKey.hpp"

namespace identity
{
class Uuid
{
public:
    static void FromKey(const p2p::PublicKey& key, uint8_t out[16]);
    static std::string Hex(const uint8_t bytes[16]);
    static std::string HexOf(const p2p::PublicKey& key);
};
}

#endif
