#include "p2p/P2pRandom.hpp"

#include <errno.h>
#include <sys/random.h>

namespace p2p
{
bool RandomBytes(void* data, size_t size)
{
    unsigned char* bytes = static_cast<unsigned char*>(data);
    while (size > 0)
    {
        const ssize_t got = getrandom(bytes, size, 0);
        if (got < 0)
        {
            if (errno == EINTR)
                continue;
            return false;
        }
        bytes += got;
        size -= static_cast<size_t>(got);
    }
    return true;
}
}
