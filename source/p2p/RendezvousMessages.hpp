#ifndef P2P_RENDEZVOUSMESSAGES_HPP
#define P2P_RENDEZVOUSMESSAGES_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "p2p/P2pEndpoint.hpp"
#include "p2p/P2pPublicKey.hpp"

namespace p2p
{
namespace rendezvous
{
class Messages
{
public:
    static std::vector<uint8_t> Register(const std::vector<PublicKey>& allowed);
    static std::vector<uint8_t> Publish(const std::vector<uint8_t>& record);
    static std::vector<uint8_t> Unpublish();
    static std::vector<uint8_t> List(unsigned int offset);
    static std::vector<uint8_t> Connect(const PublicKey& peer);
    static std::vector<uint8_t> Relay(const PublicKey& peer, const uint8_t* data, size_t size);
    static std::vector<uint8_t> Lookup(const std::string& code);
    static std::vector<uint8_t> Reflect();
    static std::vector<uint8_t> Invite(const PublicKey& peer);

    static std::vector<uint8_t> Answer(unsigned int kind, unsigned int request);
    static std::vector<uint8_t> Lobbies(const std::vector<std::vector<uint8_t> >& records, bool more);
    static std::vector<uint8_t> Punch(const PublicKey& peer, const Endpoint& address);
    static std::vector<uint8_t> Found(const PublicKey& peer);
    static std::vector<uint8_t> Reflected(const Endpoint& address);
    static std::vector<uint8_t> Invited(const std::vector<uint8_t>& record);
    static std::vector<uint8_t> Relayed(const PublicKey& peer, const uint8_t* data, size_t size);
};
}
}

#endif
