#ifndef SAVECONTAINER_SCCRC32_HPP
#define SAVECONTAINER_SCCRC32_HPP

#include <stddef.h>
#include <stdint.h>

namespace savecontainer
{
uint32_t Crc32(const uint8_t* data, size_t size, uint32_t seed);
}

#endif
