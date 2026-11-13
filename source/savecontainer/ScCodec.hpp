#ifndef SAVECONTAINER_SCCODEC_HPP
#define SAVECONTAINER_SCCODEC_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace savecontainer
{
class Codec
{
public:
    static const uint32_t kRleFlag = 0x80000000u;
    static const uint32_t kCompressedFlag = 0x40000000u;
    static const uint32_t kZstdFlag = 0x20000000u;
    static const int kZstdLevel = 3;

    static bool ZstdCompress(const uint8_t* source, size_t size, uint8_t* destination, size_t capacity, size_t* written);
    static bool ZstdDecompress(const uint8_t* source, size_t size, uint8_t* destination, size_t expected);

    static bool RleCompress(const uint8_t* source, size_t size, std::vector<uint8_t>& out);
    static bool RleDecompress(const uint8_t* source, size_t size, std::vector<uint8_t>& out);
    static bool LegacyCompress(const uint8_t* source, size_t size, std::vector<uint8_t>& out, bool* rle);
    static bool LegacyDecompress(const uint8_t* source, size_t size, bool rle, size_t expected, std::vector<uint8_t>& out);
};
}

#endif
