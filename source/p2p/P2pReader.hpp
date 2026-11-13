#ifndef P2P_P2PREADER_HPP
#define P2P_P2PREADER_HPP

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <string>
#include <vector>

#include "p2p/P2pPublicKey.hpp"

namespace p2p
{
class Reader
{
public:
    Reader(const uint8_t* data, size_t size) : m_data(data), m_size(size), m_offset(0), m_ok(true) {}

    unsigned int U8() { return Need(1) ? m_data[m_offset++] : 0; }
    unsigned int U16()
    {
        const unsigned int high = U8();
        return (high << 8) | U8();
    }
    uint32_t U32()
    {
        const uint32_t high = U16();
        return (high << 16) | U16();
    }
    bool Bytes(void* out, size_t size)
    {
        if (!Need(size))
            return false;
        memcpy(out, m_data + m_offset, size);
        m_offset += size;
        return true;
    }
    PublicKey Key()
    {
        PublicKey key;
        Bytes(key.bytes, PublicKey::kSize);
        return key;
    }
    std::string Text(size_t maxLength)
    {
        const size_t length = U8();
        if (length > maxLength || !Need(length))
        {
            m_ok = false;
            return std::string();
        }
        std::string text(reinterpret_cast<const char*>(m_data + m_offset), length);
        m_offset += length;
        return text;
    }
    bool Ok() const { return m_ok; }
    size_t Remaining() const { return m_ok ? m_size - m_offset : 0; }
    const uint8_t* Position() const { return m_data + m_offset; }

private:
    bool Need(size_t size)
    {
        if (!m_ok || m_size - m_offset < size)
        {
            m_ok = false;
            return false;
        }
        return true;
    }

    const uint8_t* m_data;
    size_t m_size;
    size_t m_offset;
    bool m_ok;
};
}

#endif
