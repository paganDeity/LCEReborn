#include "p2p/P2pIdentity.hpp"

#include <string.h>

#include "monocypher.h"
#include "p2p/P2pRandom.hpp"

namespace p2p
{
Identity::Identity()
{
    memset(m_seed, 0, sizeof(m_seed));
    memset(m_secret, 0, sizeof(m_secret));
}

Identity::~Identity()
{
    crypto_wipe(m_seed, sizeof(m_seed));
    crypto_wipe(m_secret, sizeof(m_secret));
}

Identity::Identity(const Identity& other)
{
    *this = other;
}

Identity& Identity::operator=(const Identity& other)
{
    memcpy(m_seed, other.m_seed, sizeof(m_seed));
    memcpy(m_secret, other.m_secret, sizeof(m_secret));
    m_public = other.m_public;
    return *this;
}

bool Identity::Generate(Identity& identity)
{
    uint8_t seed[kSeedSize];
    if (!RandomBytes(seed, sizeof(seed)))
        return false;
    FromSeed(seed, identity);
    crypto_wipe(seed, sizeof(seed));
    return true;
}

void Identity::FromSeed(const uint8_t seed[kSeedSize], Identity& identity)
{
    uint8_t copy[kSeedSize];
    memcpy(copy, seed, sizeof(copy));
    memcpy(identity.m_seed, seed, sizeof(identity.m_seed));
    crypto_eddsa_key_pair(identity.m_secret, identity.m_public.bytes, copy);
}

void Identity::Sign(const uint8_t* message, size_t size, uint8_t signature[kSignatureSize]) const
{
    crypto_eddsa_sign(signature, m_secret, message, size);
}

bool Identity::Verify(const PublicKey& key, const uint8_t* message, size_t size, const uint8_t signature[kSignatureSize])
{
    return crypto_eddsa_check(signature, key.bytes, message, size) == 0;
}
}
