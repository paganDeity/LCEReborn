#include "p2p/P2pFriendCode.hpp"

#include <string.h>

#include "monocypher.h"

namespace p2p
{
namespace
{
const char kAlphabet[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
const size_t kCodeBytes = 10;
const char kCodeLabel[] = "lce-p2p friend code v1";

int AlphabetIndex(char letter)
{
    if (letter >= 'a' && letter <= 'z')
        letter = static_cast<char>(letter - 'a' + 'A');
    if (letter == 'O')
        letter = '0';
    if (letter == 'I' || letter == 'L')
        letter = '1';
    for (int index = 0; index < 32; ++index)
        if (kAlphabet[index] == letter)
            return index;
    return -1;
}

void Fingerprint(const PublicKey& key, uint8_t out[kCodeBytes])
{
    uint8_t hash[32];
    crypto_blake2b_ctx context;
    crypto_blake2b_init(&context, sizeof(hash));
    crypto_blake2b_update(&context, reinterpret_cast<const uint8_t*>(kCodeLabel), sizeof(kCodeLabel) - 1);
    crypto_blake2b_update(&context, key.bytes, sizeof(key.bytes));
    crypto_blake2b_final(&context, hash);
    memcpy(out, hash, kCodeBytes);
}
}

std::string FriendCode::Of(const PublicKey& key)
{
    uint8_t print[kCodeBytes];
    Fingerprint(key, print);
    std::string code;
    unsigned int buffer = 0;
    int bits = 0;
    for (size_t index = 0; index < kCodeBytes; ++index)
    {
        buffer = (buffer << 8) | print[index];
        bits += 8;
        while (bits >= 5)
        {
            if (!code.empty() && code.size() % 5 == 4)
                code += '-';
            code += kAlphabet[(buffer >> (bits - 5)) & 31];
            bits -= 5;
        }
        buffer &= (1u << bits) - 1;
    }
    return code;
}

std::string FriendCode::Normalize(const std::string& code)
{
    std::string clean;
    for (size_t index = 0; index < code.size(); ++index)
    {
        if (code[index] == '-' || code[index] == ' ')
            continue;
        if (AlphabetIndex(code[index]) < 0)
            return std::string();
        clean += kAlphabet[AlphabetIndex(code[index])];
    }
    return clean;
}

bool FriendCode::Matches(const std::string& code, const PublicKey& key)
{
    const std::string clean = Normalize(code);
    std::string expected;
    const std::string formatted = Of(key);
    for (size_t index = 0; index < formatted.size(); ++index)
        if (formatted[index] != '-')
            expected += formatted[index];
    return !clean.empty() && clean == expected;
}
}
