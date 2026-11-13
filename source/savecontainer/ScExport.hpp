#ifndef SAVECONTAINER_SCEXPORT_HPP
#define SAVECONTAINER_SCEXPORT_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace savecontainer
{
class Export
{
public:
    static bool ToLegacyChunks(const uint8_t* blob, size_t size, std::vector<uint8_t>& out);
};
}

#endif
