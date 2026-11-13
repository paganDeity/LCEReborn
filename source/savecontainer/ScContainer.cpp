#include "savecontainer/ScContainer.hpp"

#include <map>

#include "savecontainer/ScBlob.hpp"
#include "savecontainer/ScCrc32.hpp"
#include "savecontainer/ScFormat.hpp"
#include "savecontainer/ScIo.hpp"
#include "savecontainer/ScRegion.hpp"
#include "savecontainer/ScWriter.hpp"

namespace savecontainer
{
namespace
{
class StoredChunk
{
public:
    StoredChunk() : timestamp(0), header(0), rawLength(0) {}
    int32_t timestamp;
    uint32_t header;
    uint32_t rawLength;
    std::vector<uint8_t> payload;
};

class StoredFile
{
public:
    StoredFile() : kind(format::kKindRaw), timestamp(0) {}
    std::vector<uint16_t> name;
    uint8_t kind;
    int64_t timestamp;
    std::vector<uint8_t> raw;
    std::map<uint16_t, StoredChunk> chunks;
};

uint32_t Read32(const uint8_t* at)
{
    return static_cast<uint32_t>(at[0]) | (static_cast<uint32_t>(at[1]) << 8) | (static_cast<uint32_t>(at[2]) << 16)
           | (static_cast<uint32_t>(at[3]) << 24);
}

bool Apply(const uint8_t* payload, size_t size, std::vector<StoredFile>& files, int16_t& original, int16_t& current)
{
    In in(payload, size);
    const uint8_t flags = in.U8();
    if ((flags & format::kFlagReset) != 0)
        files.clear();
    original = static_cast<int16_t>(in.U16());
    current = static_cast<int16_t>(in.U16());
    const uint32_t count = in.U32();
    if (!in.Ok() || count > 100000)
        return false;
    std::vector<StoredFile> next;
    for (uint32_t index = 0; index < count; ++index)
    {
        StoredFile file;
        file.kind = in.U8();
        const uint16_t units = in.U16();
        if (units > Blob::kNameUnits || (file.kind != format::kKindRaw && file.kind != format::kKindRegion))
            return false;
        for (uint16_t unit = 0; unit < units; ++unit)
            file.name.push_back(in.U16());
        file.timestamp = static_cast<int64_t>(in.U64());
        if (!in.Ok())
            return false;
        for (size_t old = 0; old < files.size(); ++old)
        {
            if (files[old].name == file.name)
            {
                if (files[old].kind == file.kind)
                {
                    file.raw.swap(files[old].raw);
                    file.chunks.swap(files[old].chunks);
                }
                break;
            }
        }
        next.push_back(file);
    }
    const uint32_t items = in.U32();
    if (!in.Ok())
        return false;
    for (uint32_t item = 0; item < items; ++item)
    {
        const uint8_t type = in.U8();
        const uint32_t index = in.U32();
        if (!in.Ok() || index >= next.size())
            return false;
        StoredFile& file = next[index];
        if (type == format::kItemRaw)
        {
            const uint32_t length = in.U32();
            const uint8_t* data = in.Take(length);
            const uint32_t crc = in.U32();
            if (!in.Ok() || file.kind != format::kKindRaw || crc != Crc32(data, length, 0))
                return false;
            file.raw.assign(data, data + length);
        }
        else if (type == format::kItemChunks)
        {
            const uint32_t chunkCount = in.U32();
            if (!in.Ok() || file.kind != format::kKindRegion || chunkCount > Region::kSlots)
                return false;
            for (uint32_t chunk = 0; chunk < chunkCount; ++chunk)
            {
                const uint16_t slot = in.U16();
                StoredChunk stored;
                stored.timestamp = static_cast<int32_t>(in.U32());
                stored.header = in.U32();
                stored.rawLength = in.U32();
                const uint32_t crc = in.U32();
                const size_t length = stored.header & Region::kLengthMask;
                const uint8_t* data = in.Take(length);
                if (!in.Ok() || slot >= Region::kSlots || crc != Crc32(data, length, 0))
                    return false;
                if (length != 0)
                    stored.payload.assign(data, data + length);
                file.chunks[slot] = stored;
            }
        }
        else if (type == format::kItemRemoved)
        {
            const uint32_t removed = in.U32();
            if (!in.Ok() || file.kind != format::kKindRegion || removed > Region::kSlots)
                return false;
            for (uint32_t slot = 0; slot < removed; ++slot)
                file.chunks.erase(in.U16());
            if (!in.Ok())
                return false;
        }
        else
            return false;
    }
    if (in.Left() != 0)
        return false;
    files.swap(next);
    return true;
}
}

bool Container::IsContainer(const uint8_t* bytes, size_t size)
{
    return size >= 4 && Read32(bytes) == format::kMagic;
}

void Container::Encode(const uint8_t* blob, size_t blobSize, std::vector<uint8_t>& native)
{
    Writer writer;
    bool replace = false;
    if (!writer.Prepare(blob, blobSize, native, &replace, 0))
        native.clear();
}

bool Container::Decode(const uint8_t* native, size_t nativeSize, std::vector<uint8_t>& bytes, size_t* validLength)
{
    bytes.clear();
    if (!IsContainer(native, nativeSize) || nativeSize < format::kFileHeaderBytes)
        return false;
    In header(native, format::kFileHeaderBytes);
    header.U32();
    if (header.U16() != format::kVersion)
        return false;
    std::vector<StoredFile> files;
    int16_t original = 0, current = 0;
    size_t at = format::kFileHeaderBytes;
    size_t segments = 0;
    while (at < nativeSize)
    {
        if (nativeSize - at < 8 || Read32(native + at) != format::kSegmentMagic)
            return false;
        const uint32_t length = Read32(native + at + 4);
        const uint64_t end = static_cast<uint64_t>(at) + 8 + length + 4;
        if (end > nativeSize)
        {
            if (segments == 0)
                return false;
            break;
        }
        const uint8_t* payload = native + at + 8;
        const bool good = Read32(payload + length) == Crc32(payload, length, 0)
                          && Apply(payload, length, files, original, current);
        if (!good)
        {
            if (segments == 0 || end != nativeSize)
                return false;
            break;
        }
        ++segments;
        at = static_cast<size_t>(end);
    }
    if (segments == 0)
        return false;
    if (validLength != NULL)
        *validLength = at;
    Blob blob;
    blob.originalVersion = original;
    blob.saveVersion = current;
    for (size_t index = 0; index < files.size(); ++index)
    {
        BlobFile file;
        file.name = files[index].name;
        file.timestamp = files[index].timestamp;
        if (files[index].kind == format::kKindRaw)
            file.data = files[index].raw;
        else
        {
            std::vector<ChunkRef> chunks;
            for (std::map<uint16_t, StoredChunk>::const_iterator it = files[index].chunks.begin();
                 it != files[index].chunks.end(); ++it)
            {
                ChunkRef chunk;
                chunk.slot = it->first;
                chunk.timestamp = it->second.timestamp;
                chunk.header = it->second.header;
                chunk.rawLength = it->second.rawLength;
                chunk.payload = it->second.payload.empty() ? NULL : &it->second.payload[0];
                chunk.length = it->second.payload.size();
                chunks.push_back(chunk);
            }
            if (!chunks.empty())
                Region::Build(chunks, file.data);
        }
        blob.files.push_back(file);
    }
    blob.Build(bytes);
    return true;
}
}
