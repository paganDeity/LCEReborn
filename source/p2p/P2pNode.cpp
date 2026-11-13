#include "p2p/P2pNode.hpp"

#include <string.h>

#include "p2p/P2pRandom.hpp"

namespace p2p
{
Node::Node(const Identity& identity, const PublicKey& server, const Endpoint& serverAddress, Admission* admission)
    : m_identity(identity),
      m_serverKey(server),
      m_server(serverAddress),
      m_admission(admission),
      m_client(new rendezvous::Client(identity, server)),
      m_lobbiesMore(false),
      m_lookupFailures(0),
      m_listing(0),
      m_hasFriends(false),
      m_publishing(false),
      m_started(false),
      m_announced(false),
      m_lastServerInbound(0),
      m_registerNow(false),
      m_publishNow(false),
      m_inUse(false),
      m_lastHello(0),
      m_lastRegister(0),
      m_lastPublish(0),
      m_idCounter(0)
{
}

uint64_t Node::NewId()
{
    uint8_t bytes[8] = {0};
    RandomBytes(bytes, sizeof(bytes));
    uint64_t id = 0;
    for (int index = 0; index < 8; ++index)
        id = (id << 8) | bytes[index];
    return id == 0 ? ++m_idCounter : id;
}

void Node::Queue(const Endpoint& to, const std::vector<uint8_t>& data)
{
    if (data.empty())
        return;
    Outgoing packet;
    packet.to = to;
    packet.data = data;
    m_out.push_back(packet);
}

void Node::Direct(const Endpoint& to, const std::vector<uint8_t>& datagram)
{
    Queue(to, datagram);
}

void Node::Relay(const PublicKey& peer, const std::vector<uint8_t>& datagram)
{
    Queue(m_server, m_client->Relay(peer, datagram.data(), datagram.size()));
}

void Node::TakeOutgoing(std::vector<Outgoing>& out)
{
    out.insert(out.end(), m_out.begin(), m_out.end());
    m_out.clear();
}

void Node::SetFriends(const std::vector<PublicKey>& friends)
{
    m_friends = friends;
    m_hasFriends = true;
    m_registerNow = true;
}

void Node::Publish(const Lobby& lobby)
{
    m_lobby = lobby;
    m_publishing = true;
    m_publishNow = true;
}

void Node::Unpublish()
{
    m_publishing = false;
    Queue(m_server, m_client->Unpublish());
}

void Node::RefreshLobbies()
{
    m_listing = 0;
    Queue(m_server, m_client->List(0));
}

void Node::Invite(const PublicKey& peer)
{
    Queue(m_server, m_client->Invite(peer));
}

void Node::LookupCode(const std::string& code)
{
    Queue(m_server, m_client->Lookup(code));
}

void Node::Dial(const PublicKey& host)
{
    m_pending.insert(host.AsString());
    m_refused.erase(host.AsString());
    m_dialOrder.push_back(host);
    Queue(m_server, m_client->Connect(host));
}

boost::shared_ptr<PeerLink> Node::DialedLink(const PublicKey& host)
{
    std::map<std::string, uint64_t>::const_iterator found = m_dialed.find(host.AsString());
    if (found == m_dialed.end())
        return boost::shared_ptr<PeerLink>();
    std::map<uint64_t, boost::shared_ptr<PeerLink> >::const_iterator link = m_links.find(found->second);
    return link == m_links.end() ? boost::shared_ptr<PeerLink>() : link->second;
}

bool Node::DialRefused(const PublicKey& host)
{
    return m_refused.count(host.AsString()) != 0;
}

bool Node::TakeLookupFailure()
{
    if (m_lookupFailures == 0)
        return false;
    --m_lookupFailures;
    return true;
}

bool Node::TakeFound(PublicKey& key)
{
    if (m_found.empty())
        return false;
    key = m_found.front();
    m_found.pop_front();
    return true;
}

bool Node::TakeInvitation(Lobby& lobby)
{
    if (m_invitations.empty())
        return false;
    lobby = m_invitations.front();
    m_invitations.pop_front();
    return true;
}

boost::shared_ptr<PeerLink> Node::TakeAccepted()
{
    if (m_accepted.empty())
        return boost::shared_ptr<PeerLink>();
    boost::shared_ptr<PeerLink> link = m_accepted.front();
    m_accepted.pop_front();
    return link;
}

void Node::HandleEvent(const rendezvous::Client::Event& event, int64_t now)
{
    if (event.kind == rendezvous::eReply_Denied && event.request == rendezvous::eRequest_Lookup)
    {
        ++m_lookupFailures;
        return;
    }
    if (event.kind == rendezvous::eReply_InUse)
    {
        m_inUse = true;
        return;
    }
    if (event.kind == rendezvous::eReply_Denied && event.request == rendezvous::eRequest_Connect)
    {
        if (!m_dialOrder.empty())
        {
            m_refused.insert(m_dialOrder.front().AsString());
            m_pending.erase(m_dialOrder.front().AsString());
            m_dialOrder.pop_front();
        }
    }
    else if (event.kind == rendezvous::eReply_Lobbies)
    {
        if (m_listing == 0)
            m_lobbies.clear();
        m_lobbies.insert(m_lobbies.end(), event.lobbies.begin(), event.lobbies.end());
        m_lobbiesMore = event.more;
        m_listing = m_lobbies.size();
        if (event.more)
            Queue(m_server, m_client->List(static_cast<unsigned int>(m_lobbies.size())));
    }
    else if (event.kind == rendezvous::eReply_Found)
        m_found.push_back(event.peer);
    else if (event.kind == rendezvous::eReply_Reflected)
        m_reflected = event.address;
    else if (event.kind == rendezvous::eReply_Invited)
        m_invitations.push_back(event.invitation);
    else if (event.kind == rendezvous::eReply_Punch)
    {
        const std::string key = event.peer.AsString();
        if (m_pending.count(key) != 0)
        {
            m_pending.erase(key);
            for (std::deque<PublicKey>::iterator it = m_dialOrder.begin(); it != m_dialOrder.end(); ++it)
            {
                if (*it == event.peer)
                {
                    m_dialOrder.erase(it);
                    break;
                }
            }
            const uint64_t id = NewId();
            m_links[id] = boost::shared_ptr<PeerLink>(
                new PeerLink(m_identity, true, id, event.peer, event.address, this, now));
            m_dialed[key] = id;
        }
        else
        {
            Probe& probe = m_probes[key];
            probe.address = event.address;
            probe.until = now + kProbeForMs;
            probe.last = 0;
        }
    }
    else if (event.kind == rendezvous::eReply_Relayed)
        OnPeerDatagram(true, &event.peer, Endpoint(), event.data.empty() ? NULL : &event.data[0], event.data.size(), now);
}

void Node::OnDatagram(const Endpoint& from, const uint8_t* data, size_t size, int64_t now)
{
    if (size == 0)
        return;
    if (from == m_server)
    {
        std::vector<uint8_t> reply;
        if (m_client->Receive(data, size, reply))
            m_lastServerInbound = now;
        Queue(m_server, reply);
        rendezvous::Client::Event event;
        while (m_client->NextEvent(event))
            HandleEvent(event, now);
        return;
    }
    OnPeerDatagram(false, NULL, from, data, size, now);
}

void Node::OnPeerDatagram(bool viaRelay, const PublicKey* relayed, const Endpoint& from, const uint8_t* data,
                          size_t size, int64_t now)
{
    if (size <= PeerLink::kHeaderSize || data == NULL)
        return;
    if (data[0] != PeerLink::eDatagram_Handshake && data[0] != PeerLink::eDatagram_Session)
        return;
    uint64_t id = 0;
    for (int index = 0; index < 8; ++index)
        id = (id << 8) | data[1 + index];
    std::map<uint64_t, boost::shared_ptr<PeerLink> >::iterator found = m_links.find(id);
    if (found != m_links.end())
    {
        if (viaRelay && !(found->second->Peer() == *relayed))
            return;
        found->second->OnDatagram(viaRelay, from, data, size, now);
        return;
    }
    if (data[0] != PeerLink::eDatagram_Handshake)
        return;
    for (std::map<std::string, Probe>::iterator it = m_probes.begin(); it != m_probes.end(); ++it)
    {
        const bool match = viaRelay ? it->first == relayed->AsString() : it->second.address == from;
        if (!match || it->second.until < now)
            continue;
        PublicKey peer;
        memcpy(peer.bytes, it->first.data(), PublicKey::kSize);
        boost::shared_ptr<PeerLink> link(new PeerLink(m_identity, false, id, peer, it->second.address, this, now));
        m_links[id] = link;
        m_accepted.push_back(link);
        link->OnDatagram(viaRelay, from, data, size, now);
        return;
    }
}

void Node::Update(int64_t now)
{
    if (m_inUse)
        return;
    if (!m_started || (!m_client->Connected() && now - m_lastHello >= kHelloEveryMs))
    {
        if (!m_started)
        {
            m_started = true;
            m_helloDatagram = m_client->Hello();
            m_lastServerInbound = now;
        }
        m_lastHello = now;
        Queue(m_server, m_helloDatagram);
    }
    if (m_client->Connected())
    {
        if (!m_announced)
        {
            m_announced = true;
            m_registerNow = true;
            Queue(m_server, m_client->Reflect());
        }
        if (m_registerNow || now - m_lastRegister >= kRegisterEveryMs)
        {
            m_registerNow = false;
            m_lastRegister = now;
            Queue(m_server, m_client->Register(m_friends));
        }
        if (m_publishing && (m_publishNow || now - m_lastPublish >= kPublishEveryMs))
        {
            m_publishNow = false;
            m_lastPublish = now;
            Queue(m_server, m_client->Publish(m_lobby));
        }
        if (now - m_lastServerInbound > 4 * kRegisterEveryMs)
        {
            m_client.reset(new rendezvous::Client(m_identity, m_serverKey));
            m_started = false;
            m_announced = false;
            m_lastHello = 0;
        }
    }
    for (std::map<std::string, Probe>::iterator it = m_probes.begin(); it != m_probes.end();)
    {
        if (it->second.until < now)
        {
            m_probes.erase(it++);
            continue;
        }
        if (now - it->second.last >= kProbeEveryMs)
        {
            it->second.last = now;
            std::vector<uint8_t> probe(PeerLink::kHeaderSize + 1, 0);
            probe[0] = PeerLink::eDatagram_Probe;
            Queue(it->second.address, probe);
        }
        ++it;
    }
    for (std::map<uint64_t, boost::shared_ptr<PeerLink> >::iterator it = m_links.begin(); it != m_links.end();)
    {
        it->second->Update(now);
        if (it->second->State() == PeerLink::eState_Established && !it->second->Initiator()
            && m_admission != NULL && !m_admission->Admit(it->second->Peer()))
            it->second->Reset();
        if (it->second->State() == PeerLink::eState_Failed || it->second->State() == PeerLink::eState_Closed)
            m_links.erase(it++);
        else
            ++it;
    }
}
}
