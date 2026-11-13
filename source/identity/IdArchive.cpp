#include "identity/IdArchive.hpp"

#include "p2p/P2pReader.hpp"
#include "p2p/P2pWriter.hpp"

namespace identity
{
namespace
{
const uint32_t kLocalSignature = 0x04034b50u;
const uint32_t kCentralSignature = 0x02014b50u;
const uint32_t kEndSignature = 0x06054b50u;

uint32_t Crc32(const std::vector<uint8_t>& data)
{
    uint32_t crc = 0xffffffffu;
    for (size_t index = 0; index < data.size(); ++index)
    {
        crc ^= data[index];
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}

class LittleWriter
{
public:
    void U16(unsigned int value)
    {
        m_data.push_back(static_cast<uint8_t>(value));
        m_data.push_back(static_cast<uint8_t>(value >> 8));
    }
    void U32(uint32_t value)
    {
        U16(value & 0xffff);
        U16(value >> 16);
    }
    void Bytes(const uint8_t* data, size_t size) { m_data.insert(m_data.end(), data, data + size); }
    std::vector<uint8_t> m_data;
};

class LittleReader
{
public:
    LittleReader(const std::vector<uint8_t>& data, size_t offset) : m_data(data), m_offset(offset), m_ok(true) {}

    unsigned int U16()
    {
        if (!Need(2))
            return 0;
        const unsigned int value = m_data[m_offset] | (m_data[m_offset + 1] << 8);
        m_offset += 2;
        return value;
    }
    uint32_t U32()
    {
        const uint32_t low = U16();
        return low | (static_cast<uint32_t>(U16()) << 16);
    }
    bool Ok() const { return m_ok; }
    size_t Offset() const { return m_offset; }
    void Skip(size_t size)
    {
        if (Need(size))
            m_offset += size;
    }

private:
    bool Need(size_t size)
    {
        if (!m_ok || m_data.size() - m_offset < size)
            m_ok = false;
        return m_ok;
    }

    const std::vector<uint8_t>& m_data;
    size_t m_offset;
    bool m_ok;
};
}

bool Archive::ValidName(const std::string& name)
{
    if (name.empty() || name.size() > kMaxName)
        return false;
    for (size_t index = 0; index < name.size(); ++index)
    {
        const char letter = name[index];
        const bool plain = (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z')
                           || (letter >= '0' && letter <= '9') || letter == '.' || letter == '-' || letter == '_';
        if (!plain)
            return false;
    }
    return name[0] != '.';
}

void Archive::Set(const std::string& name, const std::vector<uint8_t>& data)
{
    for (size_t index = 0; index < m_names.size(); ++index)
    {
        if (m_names[index] == name)
        {
            m_data[index] = data;
            return;
        }
    }
    m_names.push_back(name);
    m_data.push_back(data);
}

bool Archive::Get(const std::string& name, std::vector<uint8_t>& data) const
{
    for (size_t index = 0; index < m_names.size(); ++index)
    {
        if (m_names[index] == name)
        {
            data = m_data[index];
            return true;
        }
    }
    return false;
}

std::vector<uint8_t> Archive::Write() const
{
    LittleWriter out;
    std::vector<uint32_t> offsets;
    for (size_t index = 0; index < m_names.size(); ++index)
    {
        offsets.push_back(static_cast<uint32_t>(out.m_data.size()));
        out.U32(kLocalSignature);
        out.U16(10);
        out.U16(0);
        out.U16(0);
        out.U16(0);
        out.U16(0x21);
        out.U32(Crc32(m_data[index]));
        out.U32(static_cast<uint32_t>(m_data[index].size()));
        out.U32(static_cast<uint32_t>(m_data[index].size()));
        out.U16(static_cast<unsigned int>(m_names[index].size()));
        out.U16(0);
        out.Bytes(reinterpret_cast<const uint8_t*>(m_names[index].data()), m_names[index].size());
        if (!m_data[index].empty())
            out.Bytes(&m_data[index][0], m_data[index].size());
    }
    const uint32_t directory = static_cast<uint32_t>(out.m_data.size());
    for (size_t index = 0; index < m_names.size(); ++index)
    {
        out.U32(kCentralSignature);
        out.U16(20);
        out.U16(10);
        out.U16(0);
        out.U16(0);
        out.U16(0);
        out.U16(0x21);
        out.U32(Crc32(m_data[index]));
        out.U32(static_cast<uint32_t>(m_data[index].size()));
        out.U32(static_cast<uint32_t>(m_data[index].size()));
        out.U16(static_cast<unsigned int>(m_names[index].size()));
        out.U16(0);
        out.U16(0);
        out.U16(0);
        out.U16(0);
        out.U32(0);
        out.U32(offsets[index]);
        out.Bytes(reinterpret_cast<const uint8_t*>(m_names[index].data()), m_names[index].size());
    }
    const uint32_t directorySize = static_cast<uint32_t>(out.m_data.size()) - directory;
    out.U32(kEndSignature);
    out.U16(0);
    out.U16(0);
    out.U16(static_cast<unsigned int>(m_names.size()));
    out.U16(static_cast<unsigned int>(m_names.size()));
    out.U32(directorySize);
    out.U32(directory);
    out.U16(0);
    return out.m_data;
}

bool Archive::Read(const std::vector<uint8_t>& bytes, Archive& archive)
{
    if (bytes.size() < 22 || bytes.size() > kMaxTotal)
        return false;
    LittleReader end(bytes, bytes.size() - 22);
    if (end.U32() != kEndSignature)
        return false;
    end.U16();
    end.U16();
    const unsigned int thisDisk = end.U16();
    const unsigned int count = end.U16();
    const uint32_t directorySize = end.U32();
    const uint32_t directoryOffset = end.U32();
    if (!end.Ok() || thisDisk != count || count > kMaxEntries || directoryOffset > bytes.size()
        || directorySize > bytes.size() - directoryOffset)
        return false;
    Archive result;
    size_t total = 0;
    LittleReader central(bytes, directoryOffset);
    for (unsigned int index = 0; index < count; ++index)
    {
        if (central.U32() != kCentralSignature)
            return false;
        central.U16();
        central.U16();
        const unsigned int flags = central.U16();
        const unsigned int method = central.U16();
        central.U16();
        central.U16();
        const uint32_t crc = central.U32();
        const uint32_t compressed = central.U32();
        const uint32_t size = central.U32();
        const unsigned int nameLength = central.U16();
        const unsigned int extraLength = central.U16();
        const unsigned int commentLength = central.U16();
        central.U16();
        central.U16();
        central.U32();
        const uint32_t localOffset = central.U32();
        const size_t nameAt = central.Offset();
        central.Skip(nameLength + extraLength + commentLength);
        if (!central.Ok() || (flags & 1) != 0 || method != 0 || compressed != size || nameLength > kMaxName)
            return false;
        const std::string name(reinterpret_cast<const char*>(&bytes[nameAt]), nameLength);
        if (!ValidName(name))
            return false;
        LittleReader local(bytes, localOffset);
        if (local.U32() != kLocalSignature)
            return false;
        local.Skip(22);
        const unsigned int localName = local.U16();
        const unsigned int localExtra = local.U16();
        local.Skip(localName + localExtra);
        if (!local.Ok() || bytes.size() - local.Offset() < size)
            return false;
        total += size;
        if (total > kMaxTotal)
            return false;
        std::vector<uint8_t> data(bytes.begin() + local.Offset(), bytes.begin() + local.Offset() + size);
        if (Crc32(data) != crc)
            return false;
        result.Set(name, data);
    }
    archive = result;
    return true;
}
}
