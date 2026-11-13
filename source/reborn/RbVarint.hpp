#ifndef REBORN_RBVARINT_HPP
#define REBORN_RBVARINT_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace reborn
{
class Varint
{
public:
    static const size_t kMaxBytes = 10;

    static void Write(std::vector<uint8_t>& out, uint64_t value);
    static size_t Size(uint64_t value);
    static bool Read(const uint8_t* data, size_t size, size_t* position, uint64_t* value);
};
}

#endif
