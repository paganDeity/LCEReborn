#ifndef P2P_P2PSESSION_HPP
#define P2P_P2PSESSION_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace p2p
{
const size_t kSessionOverhead = 8 + 16;

class Session
{
public:
    Session();
    ~Session();

    bool Ready() const { return m_ready; }
    void Start(const uint8_t sendKey[32], const uint8_t receiveKey[32]);
    bool Seal(const uint8_t* plain, size_t size, std::vector<uint8_t>& out);
    bool Open(const uint8_t* data, size_t size, std::vector<uint8_t>& plain);

private:
    bool m_ready;
    uint8_t m_send[32];
    uint8_t m_receive[32];
    uint64_t m_sendCounter;
    uint64_t m_receiveCounter;
    bool m_receivedAny;
};
}

#endif
