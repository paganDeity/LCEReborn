#ifndef P2P_P2PENDPOINT_HPP
#define P2P_P2PENDPOINT_HPP

#include <stdint.h>

namespace p2p
{
class Endpoint
{
public:
    Endpoint() : ip(0), port(0) {}
    Endpoint(uint32_t address, unsigned int number) : ip(address), port(static_cast<uint16_t>(number)) {}

    bool operator==(const Endpoint& other) const { return ip == other.ip && port == other.port; }
    bool operator!=(const Endpoint& other) const { return !(*this == other); }
    bool operator<(const Endpoint& other) const { return ip != other.ip ? ip < other.ip : port < other.port; }

    uint32_t ip;
    uint16_t port;
};
}

#endif
