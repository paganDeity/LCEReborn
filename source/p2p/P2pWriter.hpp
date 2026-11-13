#ifndef P2P_P2PWRITER_HPP
#define P2P_P2PWRITER_HPP

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <string>
#include <vector>

#include "p2p/P2pPublicKey.hpp"

namespace p2p
{
class Writer
{
public:
    void U8(unsigned int value) { m_data.push_back(static_cast<uint8_t>(value)); }
    void U16(unsigned int value)
    {
        U8(value >> 8);
        U8(value);
    }
    void U32(uint32_t value)
    {
        U16(value >> 16);
        U16(value & 0xffff);
    }
    void Bytes(const void* data, size_t size)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        m_data.insert(m_data.end(), bytes, bytes + size);
    }
    void Key(const PublicKey& key) { Bytes(key.bytes, PublicKey::kSize); }
    void Text(const std::string& text) { U8(static_cast<unsigned int>(text.size())); Bytes(text.data(), text.size()); }
    const std::vector<uint8_t>& Data() const { return m_data; }
    std::vector<uint8_t>& Data() { return m_data; }

private:
    std::vector<uint8_t> m_data;
};
}

#endif
