#ifndef P2P_P2PPEERLINK_HPP
#define P2P_P2PPEERLINK_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include <boost/shared_ptr.hpp>

#include "p2p/P2pEndpoint.hpp"
#include "p2p/P2pHandshake.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pReliableStream.hpp"

namespace p2p
{
class PeerLink
{
public:
    enum EState
    {
        eState_Connecting,
        eState_Established,
        eState_Closed,
        eState_Failed
    };

    enum EDatagram
    {
        eDatagram_Handshake = 3,
        eDatagram_Session = 4,
        eDatagram_Probe = 5
    };

    class Output
    {
    public:
        virtual ~Output() {}
        virtual void Direct(const Endpoint& to, const std::vector<uint8_t>& datagram) = 0;
        virtual void Relay(const PublicKey& peer, const std::vector<uint8_t>& datagram) = 0;
    };

    static const size_t kHeaderSize = 9;
    static const int64_t kRelayAfterMs = 3000;
    static const int64_t kConnectTimeoutMs = 20000;
    static const int64_t kIdleTimeoutMs = 20000;

    PeerLink(const Identity& identity, bool initiator, uint64_t connectionId, const PublicKey& peer,
             const Endpoint& direct, Output* output, int64_t now);

    void Update(int64_t now);
    void OnDatagram(bool viaRelay, const Endpoint& from, const uint8_t* data, size_t size, int64_t now);

    EState State() const { return m_state; }
    bool UsedRelay() const { return m_usedRelay; }
    bool OnRelay() const { return !m_activeDirect; }
    uint64_t ConnectionId() const { return m_connectionId; }
    const PublicKey& Peer() const { return m_peer; }
    bool Initiator() const { return m_initiator; }

    size_t Write(const uint8_t* data, size_t size) { return m_stream.Write(data, size); }
    size_t Read(uint8_t* out, size_t size) { return m_stream.Read(out, size); }
    size_t Readable() const { return m_stream.Readable(); }
    size_t Writable() const { return m_stream.Writable(); }
    bool PeerClosed() const { return m_stream.PeerClosed(); }
    void Close() { m_stream.Close(); }
    void Reset();

private:
    void SendRaw(bool relay, unsigned int kind, const std::vector<uint8_t>& body);
    void SendHandshake(const std::vector<uint8_t>& body, bool allowRelay);
    void SendSealed(unsigned int type, const std::vector<uint8_t>& body);
    void Establish(int64_t now);
    void OnHandshake(bool viaRelay, const Endpoint& from, const uint8_t* data, size_t size, int64_t now);
    void OnSession(bool viaRelay, const Endpoint& from, const uint8_t* data, size_t size, int64_t now);

    const Identity& m_identity;
    bool m_initiator;
    uint64_t m_connectionId;
    PublicKey m_peer;
    Endpoint m_direct;
    Output* m_output;
    boost::shared_ptr<Handshake> m_handshake;
    ReliableStream m_stream;
    EState m_state;
    bool m_established;
    bool m_confirmed;
    bool m_activeDirect;
    bool m_usedRelay;
    bool m_started;
    bool m_sawFirst;
    std::vector<uint8_t> m_cached;
    int64_t m_createdAt;
    int64_t m_lastHello;
    int64_t m_lastInbound;
    int64_t m_lastSend;
};
}

#endif
