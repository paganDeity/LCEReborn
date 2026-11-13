#include "p2p/P2pReliableStream.hpp"

#include <algorithm>

#include "p2p/P2pReader.hpp"
#include "p2p/P2pWriter.hpp"

namespace p2p
{
namespace
{
const unsigned int kData = 1;
const unsigned int kAck = 2;
const unsigned int kFinFlag = 1;
const int64_t kMinRto = 60;
const int64_t kMaxRto = 2000;
const int64_t kInitialRto = 300;
}

ReliableStream::ReliableStream()
    : m_nextSeq(0),
      m_expected(0),
      m_closing(false),
      m_finSent(false),
      m_peerFinished(false),
      m_failed(false),
      m_ackDue(false),
      m_srtt(0),
      m_rttvar(0),
      m_rto(kInitialRto),
      m_haveSample(false)
{
}

bool ReliableStream::Before(uint32_t a, uint32_t b)
{
    return static_cast<int32_t>(a - b) < 0;
}

size_t ReliableStream::Write(const uint8_t* data, size_t size)
{
    if (m_closing || m_failed)
        return 0;
    const size_t accepted = std::min(size, Writable());
    m_queued.insert(m_queued.end(), data, data + accepted);
    return accepted;
}

size_t ReliableStream::Read(uint8_t* out, size_t size)
{
    const size_t count = std::min(size, m_readable.size());
    for (size_t index = 0; index < count; ++index)
        out[index] = m_readable[index];
    m_readable.erase(m_readable.begin(), m_readable.begin() + count);
    return count;
}

void ReliableStream::Close()
{
    m_closing = true;
}

void ReliableStream::Sample(int64_t rtt)
{
    const double sample = static_cast<double>(rtt);
    if (!m_haveSample)
    {
        m_srtt = sample;
        m_rttvar = sample / 2;
        m_haveSample = true;
    }
    else
    {
        const double delta = sample > m_srtt ? sample - m_srtt : m_srtt - sample;
        m_rttvar = 0.75 * m_rttvar + 0.25 * delta;
        m_srtt = 0.875 * m_srtt + 0.125 * sample;
    }
    m_rto = std::max(kMinRto, std::min(kMaxRto, static_cast<int64_t>(m_srtt + 4 * m_rttvar)));
}

void ReliableStream::Acknowledge(uint32_t cumulative, uint32_t bitmap, int64_t now)
{
    for (std::map<uint32_t, Pending>::iterator it = m_inflight.begin(); it != m_inflight.end();)
    {
        bool acked = Before(it->first, cumulative);
        if (!acked)
        {
            const uint32_t distance = it->first - cumulative;
            acked = distance >= 1 && distance <= 32 && ((bitmap >> (distance - 1)) & 1) != 0;
        }
        if (acked)
        {
            if (!it->second.retransmitted)
                Sample(now - it->second.sentAt);
            m_inflight.erase(it++);
        }
        else
            ++it;
    }
}

void ReliableStream::OnPacket(const uint8_t* data, size_t size, int64_t now)
{
    Reader reader(data, size);
    const unsigned int kind = reader.U8();
    if (kind == kAck)
    {
        const uint32_t cumulative = reader.U32();
        const uint32_t bitmap = reader.U32();
        if (reader.Ok())
            Acknowledge(cumulative, bitmap, now);
        return;
    }
    if (kind != kData)
        return;
    const unsigned int flags = reader.U8();
    const uint32_t seq = reader.U32();
    const uint32_t cumulative = reader.U32();
    if (!reader.Ok())
        return;
    Acknowledge(cumulative, 0, now);
    m_ackDue = true;
    if (Before(seq, m_expected))
        return;
    if (seq - m_expected >= kWindow * 2)
        return;
    Pending pending;
    pending.seq = seq;
    pending.fin = (flags & kFinFlag) != 0;
    pending.payload.assign(reader.Position(), reader.Position() + reader.Remaining());
    if (m_outOfOrder.find(seq) == m_outOfOrder.end() && m_readable.size() + pending.payload.size() <= kReceiveBuffer)
        m_outOfOrder[seq] = pending;
    for (std::map<uint32_t, Pending>::iterator it = m_outOfOrder.find(m_expected); it != m_outOfOrder.end();
         it = m_outOfOrder.find(m_expected))
    {
        m_readable.insert(m_readable.end(), it->second.payload.begin(), it->second.payload.end());
        if (it->second.fin)
            m_peerFinished = true;
        m_outOfOrder.erase(it);
        ++m_expected;
    }
}

std::vector<uint8_t> ReliableStream::DataPacket(const Pending& pending)
{
    Writer writer;
    writer.U8(kData);
    writer.U8(pending.fin ? kFinFlag : 0);
    writer.U32(pending.seq);
    writer.U32(m_expected);
    writer.Bytes(pending.payload.empty() ? NULL : &pending.payload[0], pending.payload.size());
    return writer.Data();
}

std::vector<uint8_t> ReliableStream::AckPacket() const
{
    uint32_t bitmap = 0;
    for (std::map<uint32_t, Pending>::const_iterator it = m_outOfOrder.begin(); it != m_outOfOrder.end(); ++it)
    {
        const uint32_t distance = it->first - m_expected;
        if (distance >= 1 && distance <= 32)
            bitmap |= 1u << (distance - 1);
    }
    Writer writer;
    writer.U8(kAck);
    writer.U32(m_expected);
    writer.U32(bitmap);
    return writer.Data();
}

void ReliableStream::Poll(int64_t now, std::vector<std::vector<uint8_t> >& out)
{
    if (m_failed)
        return;
    bool sentData = false;
    for (std::map<uint32_t, Pending>::iterator it = m_inflight.begin(); it != m_inflight.end(); ++it)
    {
        Pending& pending = it->second;
        const int64_t timeout = std::min(kMaxRto, m_rto << std::min(pending.retries, 6u));
        if (now - pending.sentAt < timeout)
            continue;
        if (++pending.retries > kMaxRetries)
        {
            m_failed = true;
            return;
        }
        pending.sentAt = now;
        pending.retransmitted = true;
        out.push_back(DataPacket(pending));
        sentData = true;
    }
    while (m_inflight.size() < kWindow && (!m_queued.empty() || (m_closing && !m_finSent)))
    {
        Pending pending;
        pending.seq = m_nextSeq++;
        const size_t count = std::min(kPayload, m_queued.size());
        pending.payload.assign(m_queued.begin(), m_queued.begin() + count);
        m_queued.erase(m_queued.begin(), m_queued.begin() + count);
        if (m_queued.empty() && m_closing)
        {
            pending.fin = true;
            m_finSent = true;
        }
        pending.sentAt = now;
        m_inflight[pending.seq] = pending;
        out.push_back(DataPacket(pending));
        sentData = true;
    }
    if (m_ackDue && !sentData)
        out.push_back(AckPacket());
    m_ackDue = false;
}
}
