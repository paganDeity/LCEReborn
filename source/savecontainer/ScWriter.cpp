#include "savecontainer/ScWriter.hpp"

#include <string.h>

#include "savecontainer/ScBlob.hpp"
#include "savecontainer/ScCrc32.hpp"
#include "savecontainer/ScFormat.hpp"
#include "savecontainer/ScHash.hpp"
#include "savecontainer/ScIo.hpp"
#include "savecontainer/ScRegion.hpp"

namespace savecontainer
{
namespace
{
const uint64_t kSlack = 1024 * 1024;

uint64_t NameKey(const std::vector<uint16_t>& name)
{
    std::vector<uint8_t> bytes;
    Out out(bytes);
    for (size_t index = 0; index < name.size(); ++index)
        out.U16(name[index]);
    return Hash64(bytes.empty() ? NULL : &bytes[0], bytes.size(), 0x6E616D65ULL);
}

uint64_t ChunkHash(const ChunkRef& chunk)
{
    const uint64_t seed = (static_cast<uint64_t>(chunk.header) << 32) ^ chunk.rawLength;
    return Hash64(chunk.payload, chunk.length, seed);
}

void WriteChunk(Out& out, const ChunkRef& chunk)
{
    out.U16(chunk.slot);
    out.U32(static_cast<uint32_t>(chunk.timestamp));
    out.U32(chunk.header);
    out.U32(chunk.rawLength);
    out.U32(Crc32(chunk.payload, chunk.length, 0));
    out.Bytes(chunk.payload, chunk.length);
}
}

Writer::Writer() : m_valid(false), m_fileSize(0), m_live(0)
{
}

void Writer::Reset()
{
    m_valid = false;
    m_fileSize = 0;
    m_live = 0;
    m_table.clear();
    m_raw.clear();
    m_chunks.clear();
}

bool Writer::Build(const uint8_t* blob, size_t size, std::vector<uint8_t>* out, bool full, bool* changed)
{
    BlobView view;
    if (!BlobView::Parse(blob, size, view))
        return false;

    std::vector<uint8_t> table;
    std::vector<uint8_t> items;
    Out tableOut(table);
    Out itemOut(out != NULL ? &items : NULL);
    std::map<uint64_t, uint64_t> newRaw;
    std::map<uint64_t, ChunkHashes> newChunks;
    uint32_t itemCount = 0;
    uint64_t live = 0;

    std::vector<std::vector<ChunkRef> > regions(view.files.size());
    std::vector<bool> isRegion(view.files.size(), false);
    for (size_t index = 0; index < view.files.size(); ++index)
        isRegion[index] = IsRegionName(view.files[index].name)
                          && Region::Scan(view.files[index].data, view.files[index].size, regions[index]);

    tableOut.U16(static_cast<uint16_t>(view.originalVersion));
    tableOut.U16(static_cast<uint16_t>(view.saveVersion));
    tableOut.U32(static_cast<uint32_t>(view.files.size()));
    for (size_t index = 0; index < view.files.size(); ++index)
    {
        const BlobViewFile& file = view.files[index];
        tableOut.U8(isRegion[index] ? format::kKindRegion : format::kKindRaw);
        tableOut.U16(static_cast<uint16_t>(file.name.size()));
        for (size_t unit = 0; unit < file.name.size(); ++unit)
            tableOut.U16(file.name[unit]);
        tableOut.U64(static_cast<uint64_t>(file.timestamp));
    }

    for (size_t index = 0; index < view.files.size(); ++index)
    {
        const BlobViewFile& file = view.files[index];
        const uint64_t key = NameKey(file.name);
        if (!isRegion[index])
        {
            const uint64_t hash = Hash64(file.data, file.size, 1);
            newRaw[key] = hash;
            live += file.size + 9;
            std::map<uint64_t, uint64_t>::const_iterator known = m_raw.find(key);
            if (full || known == m_raw.end() || known->second != hash)
            {
                itemOut.U8(format::kItemRaw);
                itemOut.U32(static_cast<uint32_t>(index));
                itemOut.U32(static_cast<uint32_t>(file.size));
                itemOut.Bytes(file.data, file.size);
                if (out != NULL)
                    itemOut.U32(Crc32(file.data, file.size, 0));
                ++itemCount;
            }
            continue;
        }
        ChunkHashes& hashes = newChunks[key];
        std::map<uint64_t, ChunkHashes>::const_iterator known = m_chunks.find(key);
        const ChunkHashes* before = (full || known == m_chunks.end()) ? NULL : &known->second;
        std::vector<const ChunkRef*> written;
        const std::vector<ChunkRef>& chunks = regions[index];
        for (size_t chunk = 0; chunk < chunks.size(); ++chunk)
        {
            const uint64_t hash = ChunkHash(chunks[chunk]);
            hashes[chunks[chunk].slot] = hash;
            live += chunks[chunk].length + format::kChunkOverhead;
            if (before == NULL)
            {
                written.push_back(&chunks[chunk]);
                continue;
            }
            ChunkHashes::const_iterator old = before->find(chunks[chunk].slot);
            if (old == before->end() || old->second != hash)
                written.push_back(&chunks[chunk]);
        }
        if (!written.empty())
        {
            itemOut.U8(format::kItemChunks);
            itemOut.U32(static_cast<uint32_t>(index));
            itemOut.U32(static_cast<uint32_t>(written.size()));
            if (out != NULL)
                for (size_t chunk = 0; chunk < written.size(); ++chunk)
                    WriteChunk(itemOut, *written[chunk]);
            ++itemCount;
        }
        if (before != NULL)
        {
            std::vector<uint16_t> removed;
            for (ChunkHashes::const_iterator old = before->begin(); old != before->end(); ++old)
                if (hashes.find(old->first) == hashes.end())
                    removed.push_back(old->first);
            if (!removed.empty())
            {
                itemOut.U8(format::kItemRemoved);
                itemOut.U32(static_cast<uint32_t>(index));
                itemOut.U32(static_cast<uint32_t>(removed.size()));
                for (size_t slot = 0; slot < removed.size(); ++slot)
                    itemOut.U16(removed[slot]);
                ++itemCount;
            }
        }
    }

    const bool different = full || itemCount != 0 || table != m_table;
    if (changed != NULL)
        *changed = different;
    if (out != NULL && different)
    {
        std::vector<uint8_t> payload;
        Out payloadOut(payload);
        payloadOut.U8(full ? format::kFlagReset : 0);
        payloadOut.Bytes(&table[0], table.size());
        payloadOut.U32(itemCount);
        payloadOut.Bytes(items.empty() ? NULL : &items[0], items.size());
        out->clear();
        Out segment(*out);
        if (full)
        {
            segment.U32(format::kMagic);
            segment.U16(format::kVersion);
            segment.U16(0);
        }
        segment.U32(format::kSegmentMagic);
        segment.U32(static_cast<uint32_t>(payload.size()));
        segment.Bytes(&payload[0], payload.size());
        segment.U32(Crc32(&payload[0], payload.size(), 0));
    }
    m_table.swap(table);
    m_raw.swap(newRaw);
    m_chunks.swap(newChunks);
    m_live = live;
    return true;
}

bool Writer::Prepare(const uint8_t* blob, size_t size, std::vector<uint8_t>& out, bool* replace, uint64_t onDiskSize)
{
    out.clear();
    const bool full = !m_valid || onDiskSize != m_fileSize || m_fileSize > 2 * m_live + kSlack;
    *replace = full;
    bool changed = false;
    if (!Build(blob, size, &out, full, &changed))
    {
        Reset();
        return false;
    }
    if (full)
        m_fileSize = out.size();
    else
        m_fileSize += out.size();
    m_valid = true;
    return true;
}

void Writer::Adopt(const uint8_t* blob, size_t size, uint64_t fileLength)
{
    Reset();
    bool changed = false;
    if (Build(blob, size, NULL, false, &changed))
    {
        m_valid = true;
        m_fileSize = fileLength;
    }
    else
        Reset();
}
}
