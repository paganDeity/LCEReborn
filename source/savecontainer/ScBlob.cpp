#include "savecontainer/ScBlob.hpp"

#include <string.h>

namespace savecontainer
{
namespace
{
uint32_t ReadU32(const uint8_t* at)
{
    return static_cast<uint32_t>(at[0]) | (static_cast<uint32_t>(at[1]) << 8) | (static_cast<uint32_t>(at[2]) << 16)
           | (static_cast<uint32_t>(at[3]) << 24);
}

uint16_t ReadU16(const uint8_t* at)
{
    return static_cast<uint16_t>(at[0] | (at[1] << 8));
}

void WriteU32(uint8_t* at, uint32_t value)
{
    at[0] = static_cast<uint8_t>(value);
    at[1] = static_cast<uint8_t>(value >> 8);
    at[2] = static_cast<uint8_t>(value >> 16);
    at[3] = static_cast<uint8_t>(value >> 24);
}

void WriteU16(uint8_t* at, uint16_t value)
{
    at[0] = static_cast<uint8_t>(value);
    at[1] = static_cast<uint8_t>(value >> 8);
}
}

bool BlobView::Parse(const uint8_t* bytes, size_t size, BlobView& view)
{
    view = BlobView();
    if (size < Blob::kHeaderBytes)
        return false;
    const uint32_t indexOffset = ReadU32(bytes);
    const uint32_t count = ReadU32(bytes + 4);
    view.originalVersion = static_cast<int16_t>(ReadU16(bytes + 8));
    view.saveVersion = static_cast<int16_t>(ReadU16(bytes + 10));
    if (indexOffset < Blob::kHeaderBytes || indexOffset > size || count > 100000
        || static_cast<size_t>(count) * Blob::kEntryBytes > size - indexOffset)
        return false;
    for (uint32_t index = 0; index < count; ++index)
    {
        const uint8_t* entry = bytes + indexOffset + static_cast<size_t>(index) * Blob::kEntryBytes;
        BlobViewFile file;
        size_t units = 0;
        while (units < Blob::kNameUnits && ReadU16(entry + units * 2) != 0)
            ++units;
        for (size_t unit = 0; unit < units; ++unit)
            file.name.push_back(ReadU16(entry + unit * 2));
        const uint32_t fileSize = ReadU32(entry + 128);
        const uint32_t start = ReadU32(entry + 132);
        uint64_t stamp = 0;
        for (int byte = 7; byte >= 0; --byte)
            stamp = (stamp << 8) | entry[136 + byte];
        file.timestamp = static_cast<int64_t>(stamp);
        if (fileSize != 0)
        {
            if (start < Blob::kHeaderBytes || start > size || fileSize > size - start)
                return false;
            file.data = bytes + start;
            file.size = fileSize;
        }
        view.files.push_back(file);
    }
    return true;
}

bool Blob::Parse(const uint8_t* bytes, size_t size, Blob& blob)
{
    blob = Blob();
    if (size < kHeaderBytes)
        return false;
    const uint32_t indexOffset = ReadU32(bytes);
    const uint32_t count = ReadU32(bytes + 4);
    blob.originalVersion = static_cast<int16_t>(ReadU16(bytes + 8));
    blob.saveVersion = static_cast<int16_t>(ReadU16(bytes + 10));
    if (indexOffset < kHeaderBytes || indexOffset > size || count > 100000
        || static_cast<size_t>(count) * kEntryBytes > size - indexOffset)
        return false;
    for (uint32_t index = 0; index < count; ++index)
    {
        const uint8_t* entry = bytes + indexOffset + static_cast<size_t>(index) * kEntryBytes;
        BlobFile file;
        size_t units = 0;
        while (units < kNameUnits && ReadU16(entry + units * 2) != 0)
            ++units;
        for (size_t unit = 0; unit < units; ++unit)
            file.name.push_back(ReadU16(entry + unit * 2));
        const uint32_t fileSize = ReadU32(entry + 128);
        const uint32_t start = ReadU32(entry + 132);
        uint64_t stamp = 0;
        for (int byte = 7; byte >= 0; --byte)
            stamp = (stamp << 8) | entry[136 + byte];
        file.timestamp = static_cast<int64_t>(stamp);
        if (fileSize != 0)
        {
            if (start < kHeaderBytes || start > size || fileSize > size - start)
                return false;
            file.data.assign(bytes + start, bytes + start + fileSize);
        }
        blob.files.push_back(file);
    }
    return true;
}

void Blob::Build(std::vector<uint8_t>& bytes) const
{
    size_t total = kHeaderBytes;
    for (size_t index = 0; index < files.size(); ++index)
        total += files[index].data.size();
    const size_t indexOffset = total;
    total += files.size() * kEntryBytes;
    bytes.assign(total, 0);
    WriteU32(&bytes[0], static_cast<uint32_t>(indexOffset));
    WriteU32(&bytes[4], static_cast<uint32_t>(files.size()));
    WriteU16(&bytes[8], static_cast<uint16_t>(originalVersion));
    WriteU16(&bytes[10], static_cast<uint16_t>(saveVersion));
    size_t at = kHeaderBytes;
    for (size_t index = 0; index < files.size(); ++index)
    {
        const BlobFile& file = files[index];
        uint8_t* entry = &bytes[indexOffset + index * kEntryBytes];
        for (size_t unit = 0; unit < file.name.size() && unit < kNameUnits; ++unit)
            WriteU16(entry + unit * 2, file.name[unit]);
        WriteU32(entry + 128, static_cast<uint32_t>(file.data.size()));
        WriteU32(entry + 132, static_cast<uint32_t>(at));
        uint64_t stamp = static_cast<uint64_t>(file.timestamp);
        for (int byte = 0; byte < 8; ++byte)
        {
            entry[136 + byte] = static_cast<uint8_t>(stamp);
            stamp >>= 8;
        }
        if (!file.data.empty())
            memcpy(&bytes[at], &file.data[0], file.data.size());
        at += file.data.size();
    }
}
}
