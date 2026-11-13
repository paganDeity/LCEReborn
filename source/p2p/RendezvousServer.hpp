#ifndef P2P_RENDEZVOUSSERVER_HPP
#define P2P_RENDEZVOUSSERVER_HPP

#include <stdint.h>

#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "p2p/P2pHandshake.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/RendezvousProtocol.hpp"

namespace p2p
{
namespace rendezvous
{
class Server
{
public:
    class Limits
    {
    public:
        Limits();

        size_t maxConnections;
        size_t handshakesPerAddress;
        int64_t handshakeWindowMs;
        double messagesPerSecond;
        double burst;
        int64_t pendingTimeoutMs;
        int64_t idleTimeoutMs;
        int64_t liveWindowMs;
        int64_t pairLifetimeMs;
        int64_t lobbyTtlMs;
        double relayBytesPerSecond;
        double relayBurst;
        double lookupCost;
    };

    class Outgoing
    {
    public:
        Endpoint to;
        std::vector<uint8_t> data;
    };

    Server(const Identity& identity, const Limits& limits);

    void Receive(const Endpoint& from, const uint8_t* data, size_t size, int64_t nowMs, std::vector<Outgoing>& out);
    void Tick(int64_t nowMs);
    size_t Connections() const { return m_connections.size(); }

private:
    struct Connection
    {
        Connection();

        uint64_t id;
        Endpoint address;
        boost::shared_ptr<Handshake> handshake;
        bool established;
        bool registered;
        PublicKey peer;
        int64_t created;
        int64_t lastSeen;
        double tokens;
        int64_t tokenTime;
        std::set<std::string> allowed;
        std::vector<uint8_t> lobby;
        unsigned int lobbyVisibility;
        int64_t lobbyAt;
        std::set<std::string> invited;
    };

    void OnHandshake(const Endpoint& from, uint64_t id, const uint8_t* data, size_t size, int64_t now,
                     std::vector<Outgoing>& out);
    void OnSession(const Endpoint& from, uint64_t id, const uint8_t* data, size_t size, int64_t now,
                   std::vector<Outgoing>& out);
    void Handle(Connection& from, const std::vector<uint8_t>& plain, int64_t now, std::vector<Outgoing>& out);
    bool Allow(Connection& connection, int64_t now, double cost);
    bool AllowHandshake(uint32_t ip, int64_t now);
    Connection* Find(const PublicKey& key);
    bool Visible(const Connection& owner, const Connection& requester);
    bool Reachable(const Connection& requester, const Connection& target);
    bool AllowRelay(const PublicKey& identity, int64_t now, size_t bytes);
    void Send(Connection& to, const std::vector<uint8_t>& plain, std::vector<Outgoing>& out);
    void Reply(Connection& to, unsigned int kind, unsigned int request, std::vector<Outgoing>& out);
    void Remove(uint64_t id);
    static std::string PairKey(const PublicKey& a, const PublicKey& b);

    const Identity& m_identity;
    Limits m_limits;
    std::map<uint64_t, boost::shared_ptr<Connection> > m_connections;
    std::map<std::string, uint64_t> m_byKey;
    std::map<uint32_t, std::deque<int64_t> > m_handshakes;
    std::map<std::string, int64_t> m_pairs;
    std::map<std::string, std::string> m_codes;
    class Budget
    {
    public:
        Budget() : tokens(0), at(0) {}

        double tokens;
        int64_t at;
    };
    std::map<std::string, Budget> m_relay;
};
}
}

#endif
