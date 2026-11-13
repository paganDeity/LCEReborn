#ifndef P2P_P2PPRIVATEFILE_HPP
#define P2P_P2PPRIVATEFILE_HPP

#include <stddef.h>

#include <string>

namespace p2p
{
bool WritePrivateFile(const std::string& path, const void* data, size_t size);
}

#endif
