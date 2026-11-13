#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <deque>
#include <vector>

#include "p2p/P2pReliableStream.hpp"
#include "p2p_check.hpp"

using namespace p2p;

namespace
{
class Rng
{
public:
    explicit Rng(uint32_t seed) : m_state(seed) {}
    uint32_t Next()
    {
        m_state = m_state * 1664525u + 1013904223u;
        return m_state >> 8;
    }
    bool Chance(unsigned int percent) { return Next() % 100 < percent; }

private:
    uint32_t m_state;
};

class Wire
{
public:
    Wire(unsigned int loss, unsigned int duplicate, unsigned int reorder, uint32_t seed)
        : m_loss(loss), m_duplicate(duplicate), m_reorder(reorder), m_rng(seed)
    {
    }

    void Send(const std::vector<uint8_t>& packet)
    {
        if (m_rng.Chance(m_loss))
            return;
        Enqueue(packet);
        if (m_rng.Chance(m_duplicate))
            Enqueue(packet);
    }

    bool Take(std::vector<uint8_t>& packet)
    {
        if (m_queue.empty())
            return false;
        packet = m_queue.front();
        m_queue.pop_front();
        return true;
    }

private:
    void Enqueue(const std::vector<uint8_t>& packet)
    {
        if (m_rng.Chance(m_reorder) && !m_queue.empty())
            m_queue.insert(m_queue.begin() + (m_rng.Next() % m_queue.size()), packet);
        else
            m_queue.push_back(packet);
    }

    unsigned int m_loss;
    unsigned int m_duplicate;
    unsigned int m_reorder;
    Rng m_rng;
    std::deque<std::vector<uint8_t> > m_queue;
};

bool Transfer(unsigned int loss, unsigned int duplicate, unsigned int reorder, size_t size, uint32_t seed)
{
    ReliableStream a, b;
    Wire toB(loss, duplicate, reorder, seed), toA(loss, duplicate, reorder, seed + 1);
    std::vector<uint8_t> sent(size), received;
    Rng content(seed + 2);
    for (size_t index = 0; index < size; ++index)
        sent[index] = static_cast<uint8_t>(content.Next());
    size_t offset = 0;
    std::vector<std::vector<uint8_t> > out;
    for (int64_t now = 0; now < 600000 && received.size() < size; now += 5)
    {
        if (offset < size)
            offset += a.Write(&sent[offset], size - offset);
        out.clear();
        a.Poll(now, out);
        for (size_t index = 0; index < out.size(); ++index)
            toB.Send(out[index]);
        out.clear();
        b.Poll(now, out);
        for (size_t index = 0; index < out.size(); ++index)
            toA.Send(out[index]);
        std::vector<uint8_t> packet;
        while (toB.Take(packet))
            b.OnPacket(&packet[0], packet.size(), now);
        while (toA.Take(packet))
            a.OnPacket(&packet[0], packet.size(), now);
        uint8_t buffer[4096];
        for (size_t got = b.Read(buffer, sizeof(buffer)); got > 0; got = b.Read(buffer, sizeof(buffer)))
            received.insert(received.end(), buffer, buffer + got);
        if (a.Failed() || b.Failed())
            return false;
    }
    return received == sent;
}
}

int RunStreamTests()
{
    const int before = CheckFailures();
    Check(Transfer(0, 0, 0, 100000, 1), "a clean link delivers every byte in order");
    Check(Transfer(0, 0, 0, 3, 2), "a tiny write is delivered");
    Check(Transfer(15, 0, 0, 200000, 3), "15% loss is recovered");
    Check(Transfer(0, 20, 20, 200000, 4), "duplicates and reordering are absorbed");
    Check(Transfer(25, 10, 30, 300000, 5), "loss, duplicates and reordering together");

    {
        ReliableStream a, b;
        std::vector<std::vector<uint8_t> > out;
        const uint8_t text[] = "close me";
        a.Write(text, sizeof(text));
        a.Close();
        b.Close();
        for (int64_t now = 0; now < 2000 && !(a.Finished() && b.Finished()); now += 5)
        {
            out.clear();
            a.Poll(now, out);
            for (size_t i = 0; i < out.size(); ++i)
                b.OnPacket(&out[i][0], out[i].size(), now);
            out.clear();
            b.Poll(now, out);
            for (size_t i = 0; i < out.size(); ++i)
                a.OnPacket(&out[i][0], out[i].size(), now);
        }
        uint8_t buffer[32];
        const size_t got = b.Read(buffer, sizeof(buffer));
        Check(got == sizeof(text) && memcmp(buffer, text, got) == 0 && b.PeerClosed(),
              "a close is delivered after the data");
        Check(a.Finished() && b.Finished(), "both ends finish after a close");
    }
    {
        ReliableStream a;
        const uint8_t byte = 1;
        a.Write(&byte, 1);
        std::vector<std::vector<uint8_t> > out;
        for (int64_t now = 0; now < 400000 && !a.Failed(); now += 50)
        {
            out.clear();
            a.Poll(now, out);
        }
        Check(a.Failed(), "a silent peer is reported as failed");
    }
    return CheckFailures() - before;
}
