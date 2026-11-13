#ifndef P2P_P2PNODE_HPP
#define P2P_P2PNODE_HPP

#include <stddef.h>
#include <stdint.h>

#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "p2p/P2pEndpoint.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pLobby.hpp"
#include "p2p/P2pPeerLink.hpp"
#include "p2p/RendezvousClient.hpp"

namespace p2p
{
class Node : private PeerLink::Output
{
public:
    class Admission
    {
    public:
        virtual ~Admission() {}
        virtual bool Admit(const PublicKey& peer) = 0;
    };

    class Outgoing
    {
    public:
        Endpoint to;
        std::vector<uint8_t> data;
    };

    static const int64_t kRegisterEveryMs = 15000;
    static const int64_t kPublishEveryMs = 10000;
    static const int64_t kProbeEveryMs = 300;
    static const int64_t kProbeForMs = 15000;
    static const int64_t kHelloEveryMs = 1000;

    Node(const Identity& identity, const PublicKey& server, const Endpoint& serverAddress, Admission* admission);

    void OnDatagram(const Endpoint& from, const uint8_t* data, size_t size, int64_t now);
    void Update(int64_t now);
    void TakeOutgoing(std::vector<Outgoing>& out);

    bool Online() const { return m_client->Connected() && !m_inUse; }
    bool IdentityInUse() const { return m_inUse; }
    const Endpoint& Reflected() const { return m_reflected; }
    const std::vector<Lobby>& Lobbies() const { return m_lobbies; }
    bool LobbiesMore() const { return m_lobbiesMore; }

    void SetFriends(const std::vector<PublicKey>& friends);
    void Publish(const Lobby& lobby);
    void Unpublish();
    void RefreshLobbies();
    void Invite(const PublicKey& peer);
    void LookupCode(const std::string& code);
    void Dial(const PublicKey& host);
    boost::shared_ptr<PeerLink> DialedLink(const PublicKey& host);

    bool TakeFound(PublicKey& key);
    bool TakeLookupFailure();
    bool TakeInvitation(Lobby& lobby);
    boost::shared_ptr<PeerLink> TakeAccepted();
    bool DialRefused(const PublicKey& host);

private:
    class Probe
    {
    public:
        Probe() : until(0), last(0) {}

        Endpoint address;
        int64_t until;
        int64_t last;
    };

    virtual void Direct(const Endpoint& to, const std::vector<uint8_t>& datagram);
    virtual void Relay(const PublicKey& peer, const std::vector<uint8_t>& datagram);
    void Queue(const Endpoint& to, const std::vector<uint8_t>& data);
    void HandleEvent(const rendezvous::Client::Event& event, int64_t now);
    void OnPeerDatagram(bool viaRelay, const PublicKey* relayed, const Endpoint& from, const uint8_t* data, size_t size,
                        int64_t now);
    uint64_t NewId();

    const Identity& m_identity;
    PublicKey m_serverKey;
    Endpoint m_server;
    Admission* m_admission;
    boost::shared_ptr<rendezvous::Client> m_client;
    std::vector<Outgoing> m_out;
    std::map<uint64_t, boost::shared_ptr<PeerLink> > m_links;
    std::map<std::string, uint64_t> m_dialed;
    std::deque<PublicKey> m_dialOrder;
    std::set<std::string> m_pending;
    std::set<std::string> m_refused;
    std::map<std::string, Probe> m_probes;
    std::deque<boost::shared_ptr<PeerLink> > m_accepted;
    std::deque<PublicKey> m_found;
    unsigned int m_lookupFailures;
    std::deque<Lobby> m_invitations;
    std::vector<Lobby> m_lobbies;
    bool m_lobbiesMore;
    size_t m_listing;
    Endpoint m_reflected;
    std::vector<PublicKey> m_friends;
    bool m_hasFriends;
    bool m_publishing;
    Lobby m_lobby;
    bool m_started;
    bool m_announced;
    std::vector<uint8_t> m_helloDatagram;
    int64_t m_lastServerInbound;
    bool m_registerNow;
    bool m_publishNow;
    bool m_inUse;
    int64_t m_lastHello;
    int64_t m_lastRegister;
    int64_t m_lastPublish;
    uint64_t m_idCounter;
};
}

#endif
