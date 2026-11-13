#include "archive/ZipArchive.hpp"

#include <string.h>

#include "archive/ArchiveFiles.hpp"
#include "zlib.h"

namespace archive
{
namespace
{
const uint32_t kEndOfCentralDirectory = 0x06054b50;
const uint32_t kCentralDirectoryEntry = 0x02014b50;
const uint32_t kLocalFileHeader = 0x04034b50;

uint16_t Read16(const unsigned char* data)
{
    return static_cast<uint16_t>(data[0] | (data[1] << 8));
}

uint32_t Read32(const unsigned char* data)
{
    return static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8) |
           (static_cast<uint32_t>(data[2]) << 16) | (static_cast<uint32_t>(data[3]) << 24);
}
}

bool ZipArchive::Open(const std::string& path, std::string* error)
{
    m_data.clear();
    m_entries.clear();
    m_names.clear();

    if (!Files::Read(path, m_data))
    {
        *error = "the file could not be opened";
        return false;
    }
    if (m_data.size() < 22)
    {
        *error = "the file is not a zip archive";
        return false;
    }

    size_t end = m_data.size() - 22;
    size_t searchLimit = end > 0xFFFF ? end - 0xFFFF : 0;
    size_t eocd = std::string::npos;
    for (size_t offset = end + 1; offset-- > searchLimit;)
    {
        if (Read32(At(offset)) == kEndOfCentralDirectory)
        {
            eocd = offset;
            break;
        }
    }
    if (eocd == std::string::npos)
    {
        *error = "the file is not a zip archive";
        return false;
    }

    uint16_t entryCount = Read16(At(eocd + 10));
    uint32_t directorySize = Read32(At(eocd + 12));
    uint32_t directoryOffset = Read32(At(eocd + 16));
    if (entryCount == 0xFFFF || directoryOffset == 0xFFFFFFFFu)
    {
        *error = "ZIP64 archives are not supported";
        return false;
    }
    if (static_cast<size_t>(directoryOffset) + directorySize > m_data.size())
    {
        *error = "the zip central directory is damaged";
        return false;
    }

    size_t offset = directoryOffset;
    for (uint16_t i = 0; i < entryCount; ++i)
    {
        if (offset + 46 > m_data.size() || Read32(At(offset)) != kCentralDirectoryEntry)
        {
            *error = "the zip central directory is damaged";
            return false;
        }
        const unsigned char* header = At(offset);
        Entry entry;
        entry.flags = Read16(header + 8);
        entry.method = Read16(header + 10);
        entry.crc = Read32(header + 16);
        entry.compressedSize = Read32(header + 20);
        entry.size = Read32(header + 24);
        uint16_t nameLength = Read16(header + 28);
        uint16_t extraLength = Read16(header + 30);
        uint16_t commentLength = Read16(header + 32);
        entry.localOffset = Read32(header + 42);
        if (offset + 46 + nameLength > m_data.size())
        {
            *error = "the zip central directory is damaged";
            return false;
        }

        std::string name(reinterpret_cast<const char*>(header + 46), nameLength);
        for (size_t k = 0; k < name.size(); ++k)
        {
            if (name[k] == '\\')
                name[k] = '/';
        }
        if (!name.empty() && name[name.size() - 1] != '/')
        {
            if (m_entries.insert(std::make_pair(name, entry)).second)
                m_names.push_back(name);
        }
        offset += 46 + nameLength + extraLength + commentLength;
    }
    return true;
}

const unsigned char* ZipArchive::At(size_t offset) const
{
    return reinterpret_cast<const unsigned char*>(m_data.data()) + offset;
}

bool ZipArchive::Read(const std::string& name, std::string& out, std::string* error) const
{
    std::map<std::string, Entry>::const_iterator found = m_entries.find(name);
    if (found == m_entries.end())
    {
        *error = "missing from the archive";
        return false;
    }
    const Entry& entry = found->second;
    if ((entry.flags & 1) != 0)
    {
        *error = "encrypted";
        return false;
    }
    if (static_cast<size_t>(entry.localOffset) + 30 > m_data.size() ||
        Read32(At(entry.localOffset)) != kLocalFileHeader)
    {
        *error = "the local header is damaged";
        return false;
    }
    const unsigned char* local = At(entry.localOffset);
    size_t dataOffset = static_cast<size_t>(entry.localOffset) + 30 + Read16(local + 26) + Read16(local + 28);
    if (dataOffset + entry.compressedSize > m_data.size())
    {
        *error = "the entry data is truncated";
        return false;
    }

    out.resize(entry.size);
    if (entry.method == 0)
    {
        if (entry.compressedSize != entry.size)
        {
            *error = "the stored entry size is inconsistent";
            return false;
        }
        if (entry.size != 0)
            out.assign(m_data, dataOffset, entry.size);
    }
    else if (entry.method == 8)
    {
        z_stream stream;
        memset(&stream, 0, sizeof(stream));
        if (inflateInit2(&stream, -MAX_WBITS) != Z_OK)
        {
            *error = "the inflater could not start";
            return false;
        }
        stream.next_in = const_cast<Bytef*>(At(dataOffset));
        stream.avail_in = entry.compressedSize;
        stream.next_out = reinterpret_cast<Bytef*>(out.empty() ? NULL : &out[0]);
        stream.avail_out = entry.size;
        int result = inflate(&stream, Z_FINISH);
        bool complete = result == Z_STREAM_END && stream.total_out == entry.size;
        inflateEnd(&stream);
        if (!complete)
        {
            *error = "the compressed data is damaged";
            return false;
        }
    }
    else
    {
        *error = "the compression method is not supported";
        return false;
    }

    uLong crc = crc32(0L, Z_NULL, 0);
    if (!out.empty())
        crc = crc32(crc, reinterpret_cast<const Bytef*>(out.data()), static_cast<uInt>(out.size()));
    if (crc != entry.crc)
    {
        *error = "the checksum does not match";
        return false;
    }
    return true;
}
}
