#ifndef P2P_RENDEZVOUSCLIENT_HPP
#define P2P_RENDEZVOUSCLIENT_HPP

#include <stdint.h>

#include <deque>
#include <string>
#include <vector>

#include "p2p/P2pHandshake.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pLobby.hpp"
#include "p2p/RendezvousProtocol.hpp"

namespace p2p
{
namespace rendezvous
{
class Client
{
public:
    class Event
    {
    public:
        Event() : kind(0), request(0), more(false) {}

        unsigned int kind;
        unsigned int request;
        std::vector<Lobby> lobbies;
        Lobby invitation;
        bool more;
        PublicKey peer;
        Endpoint address;
        std::vector<uint8_t> data;
    };

    Client(const Identity& identity, const PublicKey& server);

    std::vector<uint8_t> Hello();
    bool Receive(const uint8_t* data, size_t size, std::vector<uint8_t>& reply);
    bool Connected() const { return m_connected; }
    bool NextEvent(Event& event);

    std::vector<uint8_t> Register(const std::vector<PublicKey>& allowed);
    std::vector<uint8_t> Publish(const Lobby& lobby);
    std::vector<uint8_t> PublishRecord(const std::vector<uint8_t>& record);
    std::vector<uint8_t> Unpublish();
    std::vector<uint8_t> List(unsigned int offset);
    std::vector<uint8_t> Connect(const PublicKey& peer);
    std::vector<uint8_t> Relay(const PublicKey& peer, const uint8_t* data, size_t size);
    std::vector<uint8_t> Lookup(const std::string& code);
    std::vector<uint8_t> Reflect();
    std::vector<uint8_t> Invite(const PublicKey& peer);

private:
    std::vector<uint8_t> Datagram(unsigned int kind, const std::vector<uint8_t>& body) const;
    std::vector<uint8_t> Request(const std::vector<uint8_t>& plain);

    const Identity& m_identity;
    Handshake m_handshake;
    uint64_t m_id;
    bool m_connected;
    std::deque<Event> m_events;
};
}
}

#endif
