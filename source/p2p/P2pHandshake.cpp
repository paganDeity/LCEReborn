#include "p2p/P2pHandshake.hpp"

#include <string.h>

#include "monocypher.h"
#include "p2p/P2pRandom.hpp"

namespace p2p
{
namespace
{
const uint8_t kVersion = 1;
const char kLabel[] = "lce-p2p handshake v1";
const size_t kProofSize = PublicKey::kSize + Identity::kSignatureSize;
const size_t kSealedProofSize = kProofSize + 16;
const size_t kSecondMessageSize = 1 + 32 + kSealedProofSize;
const size_t kThirdMessageSize = kSealedProofSize;

void Hash(uint8_t out[32], const uint8_t* a, size_t aSize, const uint8_t* b, size_t bSize)
{
    crypto_blake2b_ctx context;
    crypto_blake2b_init(&context, 32);
    crypto_blake2b_update(&context, reinterpret_cast<const uint8_t*>(kLabel), sizeof(kLabel) - 1);
    crypto_blake2b_update(&context, a, aSize);
    crypto_blake2b_update(&context, b, bSize);
    crypto_blake2b_final(&context, out);
}

void Subkey(uint8_t out[32], const uint8_t prk[32], const char* name)
{
    crypto_blake2b_keyed(out, 32, prk, 32, reinterpret_cast<const uint8_t*>(name), strlen(name));
}

void Nonce(uint64_t value, uint8_t nonce[24])
{
    memset(nonce, 0, 24);
    nonce[0] = static_cast<uint8_t>(value);
}

void SigningInput(const char* role, const uint8_t hash[32], std::vector<uint8_t>& out)
{
    out.assign(role, role + strlen(role));
    out.insert(out.end(), hash, hash + 32);
}
}

Handshake::Handshake(const Identity& identity, bool initiator)
    : m_identity(identity), m_initiator(initiator), m_expecting(false), m_status(eStatus_Continue), m_step(0)
{
    memset(m_secret, 0, sizeof(m_secret));
    memset(m_ephemeral, 0, sizeof(m_ephemeral));
    memset(m_first, 0, sizeof(m_first));
    memset(m_theirEphemeral, 0, sizeof(m_theirEphemeral));
    memset(m_transcript, 0, sizeof(m_transcript));
    memset(m_prk, 0, sizeof(m_prk));
    memset(m_handshakeKey, 0, sizeof(m_handshakeKey));
    uint8_t secret[32];
    if (!RandomBytes(secret, sizeof(secret)))
    {
        m_status = eStatus_Failed;
        return;
    }
    memcpy(m_secret, secret, sizeof(m_secret));
    crypto_x25519_public_key(m_ephemeral, secret);
    crypto_wipe(secret, sizeof(secret));
}

Handshake::~Handshake()
{
    crypto_wipe(m_secret, sizeof(m_secret));
    crypto_wipe(m_prk, sizeof(m_prk));
    crypto_wipe(m_handshakeKey, sizeof(m_handshakeKey));
}

void Handshake::Expect(const PublicKey& peer)
{
    m_expecting = true;
    m_expected = peer;
}

Handshake::EStatus Handshake::Fail()
{
    m_status = eStatus_Failed;
    return m_status;
}

std::vector<uint8_t> Handshake::Start()
{
    std::vector<uint8_t> message;
    if (!m_initiator || m_step != 0 || m_status == eStatus_Failed)
        return message;
    m_first[0] = kVersion;
    memcpy(m_first + 1, m_ephemeral, 32);
    message.assign(kFirstMessageSize, 0);
    memcpy(&message[0], m_first, sizeof(m_first));
    m_step = 1;
    return message;
}

bool Handshake::Derive(const uint8_t* theirEphemeral)
{
    uint8_t shared[32];
    crypto_x25519(shared, m_secret, theirEphemeral);
    static const uint8_t zero[32] = {0};
    if (crypto_verify32(shared, zero) == 0)
        return false;
    memcpy(m_theirEphemeral, theirEphemeral, 32);
    const uint8_t* initiatorKey = m_initiator ? m_ephemeral : m_theirEphemeral;
    const uint8_t* responderKey = m_initiator ? m_theirEphemeral : m_ephemeral;
    uint8_t keys[64];
    memcpy(keys, initiatorKey, 32);
    memcpy(keys + 32, responderKey, 32);
    Hash(m_transcript, m_first, sizeof(m_first), keys + 32, 32);
    crypto_blake2b_keyed(m_prk, 32, shared, 32, m_transcript, 32);
    Subkey(m_handshakeKey, m_prk, "handshake");
    crypto_wipe(shared, sizeof(shared));
    return true;
}

void Handshake::Finish()
{
    uint8_t finalHash[32];
    uint8_t initiatorToResponder[32];
    uint8_t responderToInitiator[32];
    Hash(finalHash, m_transcript, 32, reinterpret_cast<const uint8_t*>("final"), 5);
    uint8_t prk[32];
    crypto_blake2b_keyed(prk, 32, m_prk, 32, finalHash, 32);
    Subkey(initiatorToResponder, prk, "initiator to responder");
    Subkey(responderToInitiator, prk, "responder to initiator");
    if (m_initiator)
        m_session.Start(initiatorToResponder, responderToInitiator);
    else
        m_session.Start(responderToInitiator, initiatorToResponder);
    crypto_wipe(prk, sizeof(prk));
    crypto_wipe(initiatorToResponder, sizeof(initiatorToResponder));
    crypto_wipe(responderToInitiator, sizeof(responderToInitiator));
    m_status = eStatus_Done;
}

Handshake::EStatus Handshake::Receive(const uint8_t* data, size_t size, std::vector<uint8_t>& reply)
{
    reply.clear();
    if (m_status != eStatus_Continue)
        return Fail();

    if (!m_initiator && m_step == 0)
    {
        if (size < kFirstMessageSize || data[0] != kVersion)
            return Fail();
        memcpy(m_first, data, sizeof(m_first));
        if (!Derive(data + 1))
            return Fail();
        std::vector<uint8_t> input;
        SigningInput("responder", m_transcript, input);
        uint8_t proof[kProofSize];
        memcpy(proof, m_identity.Public().bytes, PublicKey::kSize);
        m_identity.Sign(&input[0], input.size(), proof + PublicKey::kSize);
        reply.assign(kSecondMessageSize, 0);
        reply[0] = kVersion;
        memcpy(&reply[1], m_ephemeral, 32);
        uint8_t nonce[24];
        Nonce(1, nonce);
        crypto_aead_lock(&reply[33], &reply[33 + kProofSize], m_handshakeKey, nonce, &reply[0], 33, proof,
                         kProofSize);
        Hash(m_transcript, m_transcript, 32, &reply[0], reply.size());
        m_step = 2;
        return eStatus_Continue;
    }

    if (m_initiator && m_step == 1)
    {
        if (size != kSecondMessageSize || data[0] != kVersion)
            return Fail();
        if (!Derive(data + 1))
            return Fail();
        uint8_t nonce[24];
        Nonce(1, nonce);
        uint8_t proof[kProofSize];
        if (crypto_aead_unlock(proof, data + 33 + kProofSize, m_handshakeKey, nonce, data, 33, data + 33,
                               kProofSize) != 0)
            return Fail();
        memcpy(m_peer.bytes, proof, PublicKey::kSize);
        if (m_expecting && m_peer != m_expected)
            return Fail();
        std::vector<uint8_t> input;
        SigningInput("responder", m_transcript, input);
        if (!Identity::Verify(m_peer, &input[0], input.size(), proof + PublicKey::kSize))
            return Fail();
        Hash(m_transcript, m_transcript, 32, data, size);
        SigningInput("initiator", m_transcript, input);
        uint8_t mine[kProofSize];
        memcpy(mine, m_identity.Public().bytes, PublicKey::kSize);
        m_identity.Sign(&input[0], input.size(), mine + PublicKey::kSize);
        reply.assign(kThirdMessageSize, 0);
        Nonce(2, nonce);
        crypto_aead_lock(&reply[0], &reply[kProofSize], m_handshakeKey, nonce, NULL, 0, mine, kProofSize);
        Finish();
        return eStatus_Done;
    }

    if (!m_initiator && m_step == 2)
    {
        if (size != kThirdMessageSize)
            return Fail();
        uint8_t nonce[24];
        Nonce(2, nonce);
        uint8_t proof[kProofSize];
        if (crypto_aead_unlock(proof, data + kProofSize, m_handshakeKey, nonce, NULL, 0, data, kProofSize) != 0)
            return Fail();
        memcpy(m_peer.bytes, proof, PublicKey::kSize);
        if (m_expecting && m_peer != m_expected)
            return Fail();
        std::vector<uint8_t> input;
        SigningInput("initiator", m_transcript, input);
        if (!Identity::Verify(m_peer, &input[0], input.size(), proof + PublicKey::kSize))
            return Fail();
        Finish();
        return eStatus_Done;
    }
    return Fail();
}
}
