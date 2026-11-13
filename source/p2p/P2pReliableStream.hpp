#ifndef P2P_P2PRELIABLESTREAM_HPP
#define P2P_P2PRELIABLESTREAM_HPP

#include <stddef.h>
#include <stdint.h>

#include <deque>
#include <map>
#include <vector>

namespace p2p
{
class ReliableStream
{
public:
    static const size_t kPayload = 900;
    static const size_t kWindow = 128;
    static const size_t kSendBuffer = 256 * 1024;
    static const size_t kReceiveBuffer = 256 * 1024;
    static const unsigned int kMaxRetries = 14;

    ReliableStream();

    size_t Write(const uint8_t* data, size_t size);
    size_t Read(uint8_t* out, size_t size);
    size_t Readable() const { return m_readable.size(); }
    size_t Writable() const { return kSendBuffer > m_queued.size() ? kSendBuffer - m_queued.size() : 0; }
    void Close();
    bool PeerClosed() const { return m_peerFinished && m_readable.empty(); }
    bool Failed() const { return m_failed; }
    bool Finished() const { return m_failed || (m_finSent && m_inflight.empty() && m_peerFinished); }

    void OnPacket(const uint8_t* data, size_t size, int64_t now);
    void Poll(int64_t now, std::vector<std::vector<uint8_t> >& out);

private:
    class Pending
    {
    public:
        Pending() : seq(0), fin(false), sentAt(0), retries(0), retransmitted(false) {}

        uint32_t seq;
        bool fin;
        std::vector<uint8_t> payload;
        int64_t sentAt;
        unsigned int retries;
        bool retransmitted;
    };

    static bool Before(uint32_t a, uint32_t b);
    void Acknowledge(uint32_t cumulative, uint32_t bitmap, int64_t now);
    void Sample(int64_t rtt);
    std::vector<uint8_t> DataPacket(const Pending& pending);
    std::vector<uint8_t> AckPacket() const;

    std::deque<uint8_t> m_queued;
    std::deque<uint8_t> m_readable;
    std::map<uint32_t, Pending> m_inflight;
    std::map<uint32_t, Pending> m_outOfOrder;
    uint32_t m_nextSeq;
    uint32_t m_expected;
    bool m_closing;
    bool m_finSent;
    bool m_peerFinished;
    bool m_failed;
    bool m_ackDue;
    double m_srtt;
    double m_rttvar;
    int64_t m_rto;
    bool m_haveSample;
};
}

#endif
