#include "p2p/P2pSession.hpp"

#include <string.h>

#include "monocypher.h"

namespace p2p
{
namespace
{
void MakeNonce(uint64_t counter, uint8_t nonce[24])
{
    memset(nonce, 0, 24);
    for (int index = 0; index < 8; ++index)
        nonce[index] = static_cast<uint8_t>(counter >> (index * 8));
}
}

Session::Session() : m_ready(false), m_sendCounter(0), m_receiveCounter(0), m_receivedAny(false)
{
    memset(m_send, 0, sizeof(m_send));
    memset(m_receive, 0, sizeof(m_receive));
}

Session::~Session()
{
    crypto_wipe(m_send, sizeof(m_send));
    crypto_wipe(m_receive, sizeof(m_receive));
}

void Session::Start(const uint8_t sendKey[32], const uint8_t receiveKey[32])
{
    memcpy(m_send, sendKey, 32);
    memcpy(m_receive, receiveKey, 32);
    m_sendCounter = 0;
    m_receiveCounter = 0;
    m_receivedAny = false;
    m_ready = true;
}

bool Session::Seal(const uint8_t* plain, size_t size, std::vector<uint8_t>& out)
{
    if (!m_ready || m_sendCounter == ~static_cast<uint64_t>(0))
        return false;
    uint8_t nonce[24];
    MakeNonce(m_sendCounter, nonce);
    out.assign(kSessionOverhead + size, 0);
    memcpy(&out[0], nonce, 8);
    crypto_aead_lock(out.data() + kSessionOverhead, out.data() + 8, m_send, nonce, out.data(), 8, plain, size);
    ++m_sendCounter;
    return true;
}

bool Session::Open(const uint8_t* data, size_t size, std::vector<uint8_t>& plain)
{
    if (!m_ready || size < kSessionOverhead)
        return false;
    uint64_t counter = 0;
    for (int index = 0; index < 8; ++index)
        counter |= static_cast<uint64_t>(data[index]) << (index * 8);
    if (m_receivedAny && counter <= m_receiveCounter)
        return false;
    uint8_t nonce[24];
    MakeNonce(counter, nonce);
    const size_t length = size - kSessionOverhead;
    std::vector<uint8_t> out(length ? length : 1);
    if (crypto_aead_unlock(&out[0], data + 8, m_receive, nonce, data, 8, data + kSessionOverhead, length) != 0)
        return false;
    out.resize(length);
    plain.swap(out);
    m_receiveCounter = counter;
    m_receivedAny = true;
    return true;
}
}
