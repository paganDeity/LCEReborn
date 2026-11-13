#ifndef P2P_P2PIDENTITY_HPP
#define P2P_P2PIDENTITY_HPP

#include <stddef.h>
#include <stdint.h>

#include "p2p/P2pPublicKey.hpp"

namespace p2p
{
class Identity
{
public:
    static const size_t kSeedSize = 32;
    static const size_t kSignatureSize = 64;

    Identity();
    ~Identity();
    Identity(const Identity& other);
    Identity& operator=(const Identity& other);

    static bool Generate(Identity& identity);
    static void FromSeed(const uint8_t seed[kSeedSize], Identity& identity);

    const PublicKey& Public() const { return m_public; }
    const uint8_t* Seed() const { return m_seed; }
    void Sign(const uint8_t* message, size_t size, uint8_t signature[kSignatureSize]) const;
    static bool Verify(const PublicKey& key, const uint8_t* message, size_t size,
                       const uint8_t signature[kSignatureSize]);

private:
    uint8_t m_seed[kSeedSize];
    uint8_t m_secret[64];
    PublicKey m_public;
};
}

#endif
