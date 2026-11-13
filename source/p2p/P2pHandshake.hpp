#ifndef P2P_P2PHANDSHAKE_HPP
#define P2P_P2PHANDSHAKE_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pSession.hpp"

namespace p2p
{
const size_t kFirstMessageSize = 192;

class Handshake
{
public:
    enum EStatus
    {
        eStatus_Continue,
        eStatus_Done,
        eStatus_Failed
    };

    Handshake(const Identity& identity, bool initiator);
    ~Handshake();

    void Expect(const PublicKey& peer);
    std::vector<uint8_t> Start();
    EStatus Receive(const uint8_t* data, size_t size, std::vector<uint8_t>& reply);

    bool Complete() const { return m_status == eStatus_Done; }
    const PublicKey& Peer() const { return m_peer; }
    Session& GetSession() { return m_session; }

private:
    EStatus Fail();
    bool Derive(const uint8_t* theirEphemeral);
    void Finish();

    const Identity& m_identity;
    bool m_initiator;
    bool m_expecting;
    PublicKey m_expected;
    PublicKey m_peer;
    EStatus m_status;
    int m_step;
    uint8_t m_secret[32];
    uint8_t m_ephemeral[32];
    uint8_t m_first[33];
    uint8_t m_theirEphemeral[32];
    uint8_t m_transcript[32];
    uint8_t m_prk[32];
    uint8_t m_handshakeKey[32];
    Session m_session;
};
}

#endif
