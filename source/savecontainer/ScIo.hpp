#ifndef SAVECONTAINER_SCIO_HPP
#define SAVECONTAINER_SCIO_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace savecontainer
{
class Out
{
public:
    explicit Out(std::vector<uint8_t>& bytes) : m_bytes(&bytes) {}
    explicit Out(std::vector<uint8_t>* bytes) : m_bytes(bytes) {}
    void U8(uint8_t value)
    {
        if (m_bytes != NULL)
            m_bytes->push_back(value);
    }
    void U16(uint16_t value)
    {
        U8(static_cast<uint8_t>(value));
        U8(static_cast<uint8_t>(value >> 8));
    }
    void U32(uint32_t value)
    {
        U16(static_cast<uint16_t>(value));
        U16(static_cast<uint16_t>(value >> 16));
    }
    void U64(uint64_t value)
    {
        U32(static_cast<uint32_t>(value));
        U32(static_cast<uint32_t>(value >> 32));
    }
    void Bytes(const uint8_t* data, size_t size)
    {
        if (size != 0 && m_bytes != NULL)
            m_bytes->insert(m_bytes->end(), data, data + size);
    }

private:
    std::vector<uint8_t>* m_bytes;
};

class In
{
public:
    In(const uint8_t* bytes, size_t size) : m_bytes(bytes), m_size(size), m_at(0), m_ok(true) {}
    bool Ok() const { return m_ok; }
    size_t Left() const { return m_size - m_at; }
    size_t At() const { return m_at; }
    uint8_t U8()
    {
        if (m_at + 1 > m_size)
        {
            m_ok = false;
            return 0;
        }
        return m_bytes[m_at++];
    }
    uint16_t U16()
    {
        const uint16_t low = U8();
        return static_cast<uint16_t>(low | (U8() << 8));
    }
    uint32_t U32()
    {
        const uint32_t low = U16();
        return low | (static_cast<uint32_t>(U16()) << 16);
    }
    uint64_t U64()
    {
        const uint64_t low = U32();
        return low | (static_cast<uint64_t>(U32()) << 32);
    }
    const uint8_t* Take(size_t size)
    {
        if (size > Left())
        {
            m_ok = false;
            return NULL;
        }
        const uint8_t* at = m_bytes + m_at;
        m_at += size;
        return at;
    }

private:
    const uint8_t* m_bytes;
    size_t m_size;
    size_t m_at;
    bool m_ok;
};
}

#endif
