#include "p2p/P2pPublicKey.hpp"

#include <string.h>

#include "monocypher.h"

namespace p2p
{
PublicKey::PublicKey()
{
    memset(bytes, 0, sizeof(bytes));
}

bool PublicKey::operator==(const PublicKey& other) const
{
    return crypto_verify32(bytes, other.bytes) == 0;
}

std::string PublicKey::AsString() const
{
    return std::string(reinterpret_cast<const char*>(bytes), kSize);
}
}
