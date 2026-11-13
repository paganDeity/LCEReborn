#ifndef P2P_P2PLOBBY_HPP
#define P2P_P2PLOBBY_HPP

#include <stdint.h>

#include <string>
#include <vector>

#include "p2p/P2pIdentity.hpp"

namespace p2p
{
class Lobby
{
public:
    enum EVisibility
    {
        eVisibility_Friends = 0,
        eVisibility_FriendsOfFriends = 1,
        eVisibility_Invite = 2
    };

    enum EProtocol
    {
        eProtocol_Legacy = 1
    };

    static const size_t kMaxName = 32;
    static const size_t kMaxMode = 16;

    Lobby() : visibility(eVisibility_Friends), maxPlayers(0), players(0), protocol(eProtocol_Legacy), version(0), port(0) {}

    bool Compatible(unsigned int localProtocol, unsigned int localVersion) const
    {
        return protocol == localProtocol && version == localVersion;
    }

    static std::vector<uint8_t> Sign(const Identity& owner, Lobby lobby);
    static bool Read(const uint8_t* data, size_t size, Lobby& lobby, size_t* used);

    PublicKey owner;
    unsigned int visibility;
    unsigned int maxPlayers;
    unsigned int players;
    unsigned int protocol;
    unsigned int version;
    unsigned int port;
    std::string name;
    std::string mode;
};

}

#endif
