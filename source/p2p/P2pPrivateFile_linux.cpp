#include "p2p/P2pPrivateFile.hpp"

#include <fcntl.h>
#include <unistd.h>

namespace p2p
{
bool WritePrivateFile(const std::string& path, const void* data, size_t size)
{
    const int file = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (file < 0)
        return false;
    const bool written = write(file, data, size) == static_cast<ssize_t>(size);
    return close(file) == 0 && written;
}
}
