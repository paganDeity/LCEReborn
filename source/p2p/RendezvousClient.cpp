#include "p2p/RendezvousClient.hpp"

#include "p2p/P2pReader.hpp"
#include "p2p/RendezvousMessages.hpp"
#include "p2p/P2pRandom.hpp"

namespace p2p
{
namespace rendezvous
{
Client::Client(const Identity& identity, const PublicKey& server)
    : m_identity(identity), m_handshake(identity, true), m_id(0), m_connected(false)
{
    m_handshake.Expect(server);
    uint8_t bytes[8] = {0};
    RandomBytes(bytes, sizeof(bytes));
    for (int index = 0; index < 8; ++index)
        m_id = (m_id << 8) | bytes[index];
}

std::vector<uint8_t> Client::Datagram(unsigned int kind, const std::vector<uint8_t>& body) const
{
    std::vector<uint8_t> packet;
    packet.push_back(static_cast<uint8_t>(kind));
    for (int index = 7; index >= 0; --index)
        packet.push_back(static_cast<uint8_t>(m_id >> (index * 8)));
    packet.insert(packet.end(), body.begin(), body.end());
    return packet;
}

std::vector<uint8_t> Client::Hello()
{
    return Datagram(eDatagram_Handshake, m_handshake.Start());
}

std::vector<uint8_t> Client::Request(const std::vector<uint8_t>& plain)
{
    std::vector<uint8_t> sealed;
    if (!m_connected || !m_handshake.GetSession().Seal(plain.data(), plain.size(), sealed))
        return std::vector<uint8_t>();
    return Datagram(eDatagram_Session, sealed);
}

bool Client::Receive(const uint8_t* data, size_t size, std::vector<uint8_t>& reply)
{
    reply.clear();
    if (size <= kHeaderSize)
        return false;
    uint64_t id = 0;
    for (int index = 0; index < 8; ++index)
        id = (id << 8) | data[1 + index];
    if (id != m_id)
        return false;
    if (data[0] == eDatagram_Handshake && !m_connected)
    {
        std::vector<uint8_t> next;
        const Handshake::EStatus status = m_handshake.Receive(data + kHeaderSize, size - kHeaderSize, next);
        if (status == Handshake::eStatus_Failed)
            return false;
        if (!next.empty())
            reply = Datagram(eDatagram_Handshake, next);
        m_connected = status == Handshake::eStatus_Done;
        return true;
    }
    if (data[0] != eDatagram_Session || !m_connected)
        return false;
    std::vector<uint8_t> plain;
    if (!m_handshake.GetSession().Open(data + kHeaderSize, size - kHeaderSize, plain) || plain.empty())
        return false;
    Reader reader(plain.data(), plain.size());
    Event event;
    event.kind = reader.U8();
    if (event.kind == eReply_Ok || event.kind == eReply_Denied || event.kind == eReply_InUse)
        event.request = reader.U8();
    else if (event.kind == eReply_Lobbies)
    {
        const unsigned int count = reader.U8();
        event.more = reader.U8() != 0;
        for (unsigned int index = 0; index < count && reader.Ok(); ++index)
        {
            const size_t length = reader.U16();
            Lobby lobby;
            if (length > reader.Remaining() || !Lobby::Read(reader.Position(), length, lobby, NULL))
                return false;
            event.lobbies.push_back(lobby);
            std::vector<uint8_t> skip(length);
            reader.Bytes(&skip[0], length);
        }
    }
    else if (event.kind == eReply_Punch)
    {
        event.peer = reader.Key();
        event.address.ip = reader.U32();
        event.address.port = static_cast<uint16_t>(reader.U16());
    }
    else if (event.kind == eReply_Found)
        event.peer = reader.Key();
    else if (event.kind == eReply_Reflected)
    {
        event.address.ip = reader.U32();
        event.address.port = static_cast<uint16_t>(reader.U16());
    }
    else if (event.kind == eReply_Invited)
    {
        if (!Lobby::Read(reader.Position(), reader.Remaining(), event.invitation, NULL))
            return false;
        std::vector<uint8_t> skip(reader.Remaining());
        reader.Bytes(&skip[0], skip.size());
    }
    else if (event.kind == eReply_Relayed)
    {
        event.peer = reader.Key();
        event.data.assign(reader.Position(), reader.Position() + reader.Remaining());
    }
    else
        return false;
    if (!reader.Ok())
        return false;
    m_events.push_back(event);
    return true;
}

bool Client::NextEvent(Event& event)
{
    if (m_events.empty())
        return false;
    event = m_events.front();
    m_events.pop_front();
    return true;
}

std::vector<uint8_t> Client::Register(const std::vector<PublicKey>& allowed)
{
    return Request(Messages::Register(allowed));
}

std::vector<uint8_t> Client::Publish(const Lobby& lobby)
{
    return PublishRecord(Lobby::Sign(m_identity, lobby));
}

std::vector<uint8_t> Client::PublishRecord(const std::vector<uint8_t>& record)
{
    return Request(Messages::Publish(record));
}

std::vector<uint8_t> Client::Unpublish()
{
    return Request(Messages::Unpublish());
}

std::vector<uint8_t> Client::List(unsigned int offset)
{
    return Request(Messages::List(offset));
}

std::vector<uint8_t> Client::Connect(const PublicKey& peer)
{
    return Request(Messages::Connect(peer));
}

std::vector<uint8_t> Client::Relay(const PublicKey& peer, const uint8_t* data, size_t size)
{
    return Request(Messages::Relay(peer, data, size));
}

std::vector<uint8_t> Client::Lookup(const std::string& code)
{
    return Request(Messages::Lookup(code));
}

std::vector<uint8_t> Client::Reflect()
{
    return Request(Messages::Reflect());
}

std::vector<uint8_t> Client::Invite(const PublicKey& peer)
{
    return Request(Messages::Invite(peer));
}
}
}
