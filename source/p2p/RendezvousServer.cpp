#include "p2p/RendezvousServer.hpp"

#include <string.h>

#include <algorithm>

#include "p2p/P2pReader.hpp"
#include "p2p/P2pWriter.hpp"
#include "p2p/RendezvousMessages.hpp"
#include "p2p/P2pFriendCode.hpp"
#include "p2p/P2pLobby.hpp"

namespace p2p
{
namespace rendezvous
{
Server::Limits::Limits()
    : maxConnections(4096),
      handshakesPerAddress(5),
      handshakeWindowMs(10000),
      messagesPerSecond(20.0),
      burst(40.0),
      pendingTimeoutMs(10000),
      idleTimeoutMs(45000),
      liveWindowMs(45000),
      pairLifetimeMs(120000),
      lobbyTtlMs(30000),
      relayBytesPerSecond(32768.0),
      relayBurst(65536.0),
      lookupCost(8.0)
{
}

Server::Connection::Connection()
    : id(0),
      established(false),
      registered(false),
      created(0),
      lastSeen(0),
      tokens(0),
      tokenTime(0),
      lobbyVisibility(0),
      lobbyAt(0)
{
}

Server::Server(const Identity& identity, const Limits& limits) : m_identity(identity), m_limits(limits)
{
}

std::string Server::PairKey(const PublicKey& a, const PublicKey& b)
{
    const std::string first = a.AsString();
    const std::string second = b.AsString();
    return first < second ? first + second : second + first;
}

bool Server::AllowHandshake(uint32_t ip, int64_t now)
{
    std::deque<int64_t>& times = m_handshakes[ip];
    while (!times.empty() && now - times.front() > m_limits.handshakeWindowMs)
        times.pop_front();
    if (times.size() >= m_limits.handshakesPerAddress)
        return false;
    times.push_back(now);
    return true;
}

bool Server::Allow(Connection& connection, int64_t now, double cost)
{
    const double seconds = static_cast<double>(now - connection.tokenTime) / 1000.0;
    connection.tokens = std::min(m_limits.burst, connection.tokens + seconds * m_limits.messagesPerSecond);
    connection.tokenTime = now;
    if (connection.tokens < cost)
        return false;
    connection.tokens -= cost;
    return true;
}

Server::Connection* Server::Find(const PublicKey& key)
{
    std::map<std::string, uint64_t>::iterator found = m_byKey.find(key.AsString());
    if (found == m_byKey.end())
        return NULL;
    std::map<uint64_t, boost::shared_ptr<Connection> >::iterator connection = m_connections.find(found->second);
    return connection == m_connections.end() ? NULL : connection->second.get();
}

void Server::Remove(uint64_t id)
{
    std::map<uint64_t, boost::shared_ptr<Connection> >::iterator found = m_connections.find(id);
    if (found == m_connections.end())
        return;
    if (found->second->established)
    {
        std::map<std::string, uint64_t>::iterator key = m_byKey.find(found->second->peer.AsString());
        if (key != m_byKey.end() && key->second == id)
        {
            m_byKey.erase(key);
            m_codes.erase(FriendCode::Normalize(FriendCode::Of(found->second->peer)));
        }
    }
    m_connections.erase(found);
}

void Server::Receive(const Endpoint& from, const uint8_t* data, size_t size, int64_t nowMs,
                     std::vector<Outgoing>& out)
{
    if (size <= kHeaderSize)
        return;
    uint64_t id = 0;
    for (int index = 0; index < 8; ++index)
        id = (id << 8) | data[1 + index];
    if (data[0] == eDatagram_Handshake)
        OnHandshake(from, id, data + kHeaderSize, size - kHeaderSize, nowMs, out);
    else if (data[0] == eDatagram_Session)
        OnSession(from, id, data + kHeaderSize, size - kHeaderSize, nowMs, out);
}

void Server::OnHandshake(const Endpoint& from, uint64_t id, const uint8_t* data, size_t size, int64_t now,
                         std::vector<Outgoing>& out)
{
    std::vector<uint8_t> reply;
    std::map<uint64_t, boost::shared_ptr<Connection> >::iterator found = m_connections.find(id);
    if (found == m_connections.end())
    {
        if (size != kFirstMessageSize || m_connections.size() >= m_limits.maxConnections || !AllowHandshake(from.ip, now))
            return;
        boost::shared_ptr<Connection> connection(new Connection());
        connection->id = id;
        connection->address = from;
        connection->created = now;
        connection->lastSeen = now;
        connection->tokenTime = now;
        connection->tokens = m_limits.burst;
        connection->handshake.reset(new Handshake(m_identity, false));
        if (connection->handshake->Receive(data, size, reply) == Handshake::eStatus_Failed)
            return;
        m_connections[id] = connection;
    }
    else
    {
        Connection& connection = *found->second;
        if (connection.established || connection.address != from)
            return;
        const Handshake::EStatus status = connection.handshake->Receive(data, size, reply);
        if (status == Handshake::eStatus_Failed)
        {
            Remove(id);
            return;
        }
        if (status == Handshake::eStatus_Done)
        {
            connection.peer = connection.handshake->Peer();
            connection.lastSeen = now;
            std::map<std::string, uint64_t>::iterator previous = m_byKey.find(connection.peer.AsString());
            if (previous != m_byKey.end() && previous->second != id)
            {
                std::map<uint64_t, boost::shared_ptr<Connection> >::iterator existing = m_connections.find(previous->second);
                if (existing != m_connections.end() && now - existing->second->lastSeen < m_limits.liveWindowMs)
                {
                    connection.established = true;
                    Send(connection, Messages::Answer(eReply_InUse, 0), out);
                    connection.established = false;
                    Remove(id);
                    return;
                }
                const uint64_t old = previous->second;
                m_byKey.erase(previous);
                Remove(old);
            }
            connection.established = true;
            m_byKey[connection.peer.AsString()] = id;
            m_codes[FriendCode::Normalize(FriendCode::Of(connection.peer))] = connection.peer.AsString();
        }
    }
    if (reply.empty())
        return;
    Outgoing packet;
    packet.to = from;
    packet.data.push_back(eDatagram_Handshake);
    for (int index = 7; index >= 0; --index)
        packet.data.push_back(static_cast<uint8_t>(id >> (index * 8)));
    packet.data.insert(packet.data.end(), reply.begin(), reply.end());
    out.push_back(packet);
}

void Server::OnSession(const Endpoint& from, uint64_t id, const uint8_t* data, size_t size, int64_t now,
                       std::vector<Outgoing>& out)
{
    std::map<uint64_t, boost::shared_ptr<Connection> >::iterator found = m_connections.find(id);
    if (found == m_connections.end() || !found->second->established)
        return;
    Connection& connection = *found->second;
    std::vector<uint8_t> plain;
    if (!connection.handshake->GetSession().Open(data, size, plain))
        return;
    connection.address = from;
    connection.lastSeen = now;
    if (!Allow(connection, now, 1.0 + static_cast<double>(plain.size()) / 512.0))
        return;
    Handle(connection, plain, now, out);
}

bool Server::Visible(const Connection& owner, const Connection& requester)
{
    if (&owner == &requester || !owner.registered || owner.lobby.empty())
        return false;
    const std::string asker = requester.peer.AsString();
    if (owner.lobbyVisibility == Lobby::eVisibility_Invite)
        return owner.invited.count(asker) != 0;
    if (owner.allowed.count(asker) != 0)
        return true;
    if (owner.lobbyVisibility != Lobby::eVisibility_FriendsOfFriends)
        return false;
    for (std::set<std::string>::const_iterator it = owner.allowed.begin(); it != owner.allowed.end(); ++it)
    {
        std::map<std::string, uint64_t>::const_iterator found = m_byKey.find(*it);
        if (found == m_byKey.end())
            continue;
        std::map<uint64_t, boost::shared_ptr<Connection> >::const_iterator friendOf = m_connections.find(found->second);
        if (friendOf != m_connections.end() && friendOf->second->registered
            && friendOf->second->allowed.count(asker) != 0)
            return true;
    }
    return false;
}

bool Server::Reachable(const Connection& requester, const Connection& target)
{
    if (&requester == &target || !target.registered)
        return false;
    if (!target.lobby.empty())
        return Visible(target, requester);
    return target.allowed.count(requester.peer.AsString()) != 0;
}

bool Server::AllowRelay(const PublicKey& identity, int64_t now, size_t bytes)
{
    Budget& budget = m_relay[identity.AsString()];
    if (budget.at == 0)
        budget.tokens = m_limits.relayBurst;
    const double seconds = static_cast<double>(now - budget.at) / 1000.0;
    budget.tokens = std::min(m_limits.relayBurst, budget.tokens + seconds * m_limits.relayBytesPerSecond);
    budget.at = now;
    if (budget.tokens < static_cast<double>(bytes))
        return false;
    budget.tokens -= static_cast<double>(bytes);
    return true;
}

void Server::Send(Connection& to, const std::vector<uint8_t>& plain, std::vector<Outgoing>& out)
{
    Outgoing packet;
    packet.to = to.address;
    packet.data.push_back(eDatagram_Session);
    for (int index = 7; index >= 0; --index)
        packet.data.push_back(static_cast<uint8_t>(to.id >> (index * 8)));
    std::vector<uint8_t> sealed;
    if (!to.handshake->GetSession().Seal(plain.data(), plain.size(), sealed))
        return;
    packet.data.insert(packet.data.end(), sealed.begin(), sealed.end());
    out.push_back(packet);
}

void Server::Reply(Connection& to, unsigned int kind, unsigned int request, std::vector<Outgoing>& out)
{
    Send(to, Messages::Answer(kind, request), out);
}

void Server::Handle(Connection& from, const std::vector<uint8_t>& plain, int64_t now, std::vector<Outgoing>& out)
{
    Reader reader(plain.data(), plain.size());
    const unsigned int request = reader.U8();
    if (request == eRequest_Register)
    {
        const size_t count = reader.U16();
        std::set<std::string> allowed;
        for (size_t index = 0; index < count && reader.Ok() && count <= kMaxAllowed; ++index)
            allowed.insert(reader.Key().AsString());
        if (!reader.Ok() || count > kMaxAllowed)
        {
            Reply(from, eReply_Denied, request, out);
            return;
        }
        from.allowed.swap(allowed);
        from.registered = true;
        Reply(from, eReply_Ok, request, out);
    }
    else if (request == eRequest_Publish)
    {
        Lobby lobby;
        if (!reader.Ok() || !Lobby::Read(reader.Position(), reader.Remaining(), lobby, NULL) || lobby.owner != from.peer)
        {
            Reply(from, eReply_Denied, request, out);
            return;
        }
        from.lobby.assign(reader.Position(), reader.Position() + reader.Remaining());
        from.lobbyVisibility = lobby.visibility;
        from.lobbyAt = now;
        from.registered = true;
        Reply(from, eReply_Ok, request, out);
    }
    else if (request == eRequest_Unpublish)
    {
        from.lobby.clear();
        from.invited.clear();
        Reply(from, eReply_Ok, request, out);
    }
    else if (request == eRequest_List)
    {
        const size_t offset = reader.U16();
        std::vector<const Connection*> visible;
        const std::string me = from.peer.AsString();
        for (std::map<uint64_t, boost::shared_ptr<Connection> >::const_iterator it = m_connections.begin();
             it != m_connections.end(); ++it)
        {
            const Connection& other = *it->second;
            if (&other == &from || other.lobby.empty() || !other.registered)
                continue;
            if (Visible(other, from))
                visible.push_back(&other);
        }
        const size_t end = std::min(visible.size(), offset + kLobbiesPerReply);
        std::vector<std::vector<uint8_t> > records;
        for (size_t index = offset; index < end; ++index)
            records.push_back(visible[index]->lobby);
        Send(from, Messages::Lobbies(records, end < visible.size()), out);
    }
    else if (request == eRequest_Connect)
    {
        const PublicKey key = reader.Key();
        Connection* target = reader.Ok() ? Find(key) : NULL;
        if (target == NULL || !Reachable(from, *target))
        {
            Reply(from, eReply_Denied, request, out);
            return;
        }
        m_pairs[PairKey(from.peer, target->peer)] = now + m_limits.pairLifetimeMs;
        Send(from, Messages::Punch(target->peer, target->address), out);
        Send(*target, Messages::Punch(from.peer, from.address), out);
    }
    else if (request == eRequest_Lookup)
    {
        const std::string code = FriendCode::Normalize(reader.Text(32));
        std::map<std::string, std::string>::const_iterator found = m_codes.find(code);
        if (!reader.Ok() || code.empty() || found == m_codes.end() || !Allow(from, now, m_limits.lookupCost))
        {
            Reply(from, eReply_Denied, request, out);
            return;
        }
        PublicKey key;
        memcpy(key.bytes, found->second.data(), PublicKey::kSize);
        Send(from, Messages::Found(key), out);
    }
    else if (request == eRequest_Reflect)
    {
        Send(from, Messages::Reflected(from.address), out);
    }
    else if (request == eRequest_Invite)
    {
        const PublicKey key = reader.Key();
        if (!reader.Ok() || from.lobby.empty() || key == from.peer || from.invited.size() >= kMaxAllowed)
        {
            Reply(from, eReply_Denied, request, out);
            return;
        }
        from.invited.insert(key.AsString());
        Connection* invitee = Find(key);
        if (invitee != NULL)
        {
            Send(*invitee, Messages::Invited(from.lobby), out);
        }
        Reply(from, eReply_Ok, request, out);
    }
    else if (request == eRequest_Relay)
    {
        const PublicKey key = reader.Key();
        const size_t length = reader.U16();
        Connection* target = reader.Ok() ? Find(key) : NULL;
        std::map<std::string, int64_t>::iterator pair = m_pairs.find(PairKey(from.peer, key));
        if (target == NULL || !reader.Ok() || length == 0 || length > kMaxRelay || reader.Remaining() != length
            || pair == m_pairs.end() || pair->second < now || !AllowRelay(from.peer, now, length))
        {
            Reply(from, eReply_Denied, request, out);
            return;
        }
        pair->second = now + m_limits.pairLifetimeMs;
        Send(*target, Messages::Relayed(from.peer, reader.Position(), length), out);
    }
}

void Server::Tick(int64_t nowMs)
{
    std::vector<uint64_t> expired;
    for (std::map<uint64_t, boost::shared_ptr<Connection> >::const_iterator it = m_connections.begin();
         it != m_connections.end(); ++it)
    {
        const Connection& connection = *it->second;
        const int64_t limit = connection.established ? m_limits.idleTimeoutMs : m_limits.pendingTimeoutMs;
        if (nowMs - connection.lastSeen > limit)
            expired.push_back(it->first);
    }
    for (size_t index = 0; index < expired.size(); ++index)
        Remove(expired[index]);
    for (std::map<uint64_t, boost::shared_ptr<Connection> >::iterator it = m_connections.begin();
         it != m_connections.end(); ++it)
    {
        Connection& connection = *it->second;
        if (!connection.lobby.empty() && nowMs - connection.lobbyAt > m_limits.lobbyTtlMs)
        {
            connection.lobby.clear();
            connection.invited.clear();
        }
    }
    for (std::map<std::string, Budget>::iterator it = m_relay.begin(); it != m_relay.end();)
    {
        if (nowMs - it->second.at > 300000)
            m_relay.erase(it++);
        else
            ++it;
    }
    for (std::map<std::string, int64_t>::iterator it = m_pairs.begin(); it != m_pairs.end();)
    {
        if (it->second < nowMs)
            m_pairs.erase(it++);
        else
            ++it;
    }
    for (std::map<uint32_t, std::deque<int64_t> >::iterator it = m_handshakes.begin(); it != m_handshakes.end();)
    {
        while (!it->second.empty() && nowMs - it->second.front() > m_limits.handshakeWindowMs)
            it->second.pop_front();
        if (it->second.empty())
            m_handshakes.erase(it++);
        else
            ++it;
    }
}
}
}
