#include "identity/IdUuid.hpp"

#include "monocypher.h"

namespace identity
{
void Uuid::FromKey(const p2p::PublicKey& key, uint8_t out[16])
{
    static const char label[] = "lce identity uuid v1";
    crypto_blake2b_ctx context;
    crypto_blake2b_init(&context, 16);
    crypto_blake2b_update(&context, reinterpret_cast<const uint8_t*>(label), sizeof(label) - 1);
    crypto_blake2b_update(&context, key.bytes, p2p::PublicKey::kSize);
    crypto_blake2b_final(&context, out);
    out[6] = static_cast<uint8_t>((out[6] & 0x0f) | 0x80);
    out[8] = static_cast<uint8_t>((out[8] & 0x3f) | 0x80);
}

std::string Uuid::Hex(const uint8_t bytes[16])
{
    static const char digits[] = "0123456789abcdef";
    std::string text;
    for (int index = 0; index < 16; ++index)
    {
        text += digits[bytes[index] >> 4];
        text += digits[bytes[index] & 15];
    }
    return text;
}

std::string Uuid::HexOf(const p2p::PublicKey& key)
{
    uint8_t bytes[16];
    FromKey(key, bytes);
    return Hex(bytes);
}
}
