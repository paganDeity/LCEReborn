#ifndef SAVECONTAINER_SCCONTAINER_HPP
#define SAVECONTAINER_SCCONTAINER_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace savecontainer
{
class Container
{
public:
    static bool IsContainer(const uint8_t* bytes, size_t size);

    static void Encode(const uint8_t* blob, size_t blobSize, std::vector<uint8_t>& native);
    static bool Decode(const uint8_t* native, size_t nativeSize, std::vector<uint8_t>& blob, size_t* validLength);
};
}

#endif
