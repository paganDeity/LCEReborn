#include "p2p/P2pPeerLink.hpp"

#include "p2p/P2pReader.hpp"
#include "p2p/P2pWriter.hpp"

namespace p2p
{
namespace
{
const unsigned int kStreamPacket = 1;
const unsigned int kKeepAlive = 2;
const unsigned int kReset = 3;
const int64_t kHelloIntervalMs = 400;
const int64_t kKeepAliveMs = 2000;
}

PeerLink::PeerLink(const Identity& identity, bool initiator, uint64_t connectionId, const PublicKey& peer,
                   const Endpoint& direct, Output* output, int64_t now)
    : m_identity(identity),
      m_initiator(initiator),
      m_connectionId(connectionId),
      m_peer(peer),
      m_direct(direct),
      m_output(output),
      m_handshake(new Handshake(identity, initiator)),
      m_state(eState_Connecting),
      m_established(false),
      m_confirmed(false),
      m_activeDirect(true),
      m_usedRelay(false),
      m_started(false),
      m_sawFirst(false),
      m_createdAt(now),
      m_lastHello(0),
      m_lastInbound(now),
      m_lastSend(now)
{
    m_handshake->Expect(peer);
}

void PeerLink::SendRaw(bool relay, unsigned int kind, const std::vector<uint8_t>& body)
{
    Writer writer;
    writer.U8(kind);
    for (int index = 7; index >= 0; --index)
        writer.U8(static_cast<unsigned int>(m_connectionId >> (index * 8)));
    writer.Bytes(body.empty() ? NULL : &body[0], body.size());
    if (relay)
    {
        m_usedRelay = true;
        m_output->Relay(m_peer, writer.Data());
    }
    else
        m_output->Direct(m_direct, writer.Data());
}

void PeerLink::SendHandshake(const std::vector<uint8_t>& body, bool allowRelay)
{
    SendRaw(false, eDatagram_Handshake, body);
    if (allowRelay)
        SendRaw(true, eDatagram_Handshake, body);
}

void PeerLink::SendSealed(unsigned int type, const std::vector<uint8_t>& body)
{
    Writer plain;
    plain.U8(type);
    plain.Bytes(body.empty() ? NULL : &body[0], body.size());
    std::vector<uint8_t> sealed;
    if (!m_handshake->GetSession().Seal(plain.Data().data(), plain.Data().size(), sealed))
        return;
    SendRaw(!m_activeDirect, eDatagram_Session, sealed);
}

void PeerLink::Reset()
{
    if (m_established && m_state == eState_Established)
        SendSealed(kReset, std::vector<uint8_t>());
    m_state = eState_Closed;
}

void PeerLink::Establish(int64_t now)
{
    m_established = true;
    m_state = eState_Established;
    m_lastInbound = now;
    m_lastSend = now;
}

void PeerLink::Update(int64_t now)
{
    if (m_state == eState_Failed || m_state == eState_Closed)
        return;
    if (m_state == eState_Connecting)
    {
        if (now - m_createdAt > kConnectTimeoutMs)
        {
            m_state = eState_Failed;
            return;
        }
        if (m_initiator && now - m_lastHello >= kHelloIntervalMs)
        {
            m_lastHello = now;
            const bool relay = now - m_createdAt >= kRelayAfterMs;
            if (!m_started)
            {
                m_started = true;
                m_cached = m_handshake->Start();
            }
            SendHandshake(m_cached, relay);
        }
        return;
    }
    if (m_initiator && !m_confirmed && now - m_lastHello >= kHelloIntervalMs)
    {
        m_lastHello = now;
        SendRaw(!m_activeDirect, eDatagram_Handshake, m_cached);
    }
    std::vector<std::vector<uint8_t> > packets;
    m_stream.Poll(now, packets);
    for (size_t index = 0; index < packets.size(); ++index)
    {
        SendSealed(kStreamPacket, packets[index]);
        m_lastSend = now;
    }
    if (m_stream.Failed() || now - m_lastInbound > kIdleTimeoutMs)
    {
        m_state = eState_Failed;
        return;
    }
    if (m_stream.Finished())
    {
        m_state = eState_Closed;
        return;
    }
    if (now - m_lastSend >= kKeepAliveMs)
    {
        SendSealed(kKeepAlive, std::vector<uint8_t>());
        m_lastSend = now;
    }
}

void PeerLink::OnDatagram(bool viaRelay, const Endpoint& from, const uint8_t* data, size_t size, int64_t now)
{
    if (size <= kHeaderSize || m_state == eState_Failed || m_state == eState_Closed)
        return;
    if (data[0] == eDatagram_Handshake)
        OnHandshake(viaRelay, from, data + kHeaderSize, size - kHeaderSize, now);
    else if (data[0] == eDatagram_Session)
        OnSession(viaRelay, from, data + kHeaderSize, size - kHeaderSize, now);
}

void PeerLink::OnHandshake(bool viaRelay, const Endpoint& from, const uint8_t* data, size_t size, int64_t now)
{
    std::vector<uint8_t> reply;
    if (m_initiator)
    {
        if (m_established || !m_started)
            return;
        const Handshake::EStatus status = m_handshake->Receive(data, size, reply);
        if (status == Handshake::eStatus_Failed)
        {
            m_state = eState_Failed;
            return;
        }
        m_activeDirect = !viaRelay;
        if (!viaRelay)
            m_direct = from;
        m_cached = reply;
        Establish(now);
        m_lastHello = now;
        SendRaw(viaRelay, eDatagram_Handshake, m_cached);
        return;
    }
    if (m_established)
        return;
    if (!m_sawFirst)
    {
        const Handshake::EStatus status = m_handshake->Receive(data, size, reply);
        if (status == Handshake::eStatus_Failed)
        {
            m_state = eState_Failed;
            return;
        }
        m_sawFirst = true;
        m_cached = reply;
        m_activeDirect = !viaRelay;
        if (!viaRelay)
            m_direct = from;
        SendRaw(viaRelay, eDatagram_Handshake, m_cached);
        return;
    }
    if (size == kFirstMessageSize)
    {
        SendRaw(viaRelay, eDatagram_Handshake, m_cached);
        return;
    }
    const Handshake::EStatus status = m_handshake->Receive(data, size, reply);
    if (status != Handshake::eStatus_Done)
    {
        m_state = eState_Failed;
        return;
    }
    m_activeDirect = !viaRelay;
    if (!viaRelay)
        m_direct = from;
    Establish(now);
    SendSealed(kKeepAlive, std::vector<uint8_t>());
}

void PeerLink::OnSession(bool viaRelay, const Endpoint& from, const uint8_t* data, size_t size, int64_t now)
{
    if (!m_established)
        return;
    std::vector<uint8_t> plain;
    if (!m_handshake->GetSession().Open(data, size, plain) || plain.empty())
        return;
    m_lastInbound = now;
    m_confirmed = true;
    m_activeDirect = !viaRelay;
    if (!viaRelay)
        m_direct = from;
    if (plain[0] == kReset)
        m_state = eState_Closed;
    else if (plain[0] == kStreamPacket && plain.size() > 1)
        m_stream.OnPacket(&plain[1], plain.size() - 1, now);
}
}
