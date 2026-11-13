#include "savecontainer/ScExport.hpp"

#include "savecontainer/ScBlob.hpp"
#include "savecontainer/ScCodec.hpp"
#include "savecontainer/ScRegion.hpp"

namespace savecontainer
{
bool Export::ToLegacyChunks(const uint8_t* bytes, size_t size, std::vector<uint8_t>& out)
{
    BlobView view;
    if (!BlobView::Parse(bytes, size, view))
        return false;
    Blob blob;
    blob.originalVersion = view.originalVersion;
    blob.saveVersion = view.saveVersion;
    for (size_t index = 0; index < view.files.size(); ++index)
    {
        const BlobViewFile& source = view.files[index];
        BlobFile file;
        file.name = source.name;
        file.timestamp = source.timestamp;
        std::vector<ChunkRef> chunks;
        bool convert = false;
        if (IsRegionName(source.name) && Region::Scan(source.data, source.size, chunks))
            for (size_t chunk = 0; chunk < chunks.size() && !convert; ++chunk)
                convert = (chunks[chunk].header & Codec::kZstdFlag) != 0;
        if (!convert)
        {
            if (source.size != 0)
                file.data.assign(source.data, source.data + source.size);
            blob.files.push_back(file);
            continue;
        }
        std::vector<std::vector<uint8_t> > payloads(chunks.size());
        for (size_t chunk = 0; chunk < chunks.size(); ++chunk)
        {
            ChunkRef& entry = chunks[chunk];
            if ((entry.header & Codec::kZstdFlag) == 0)
                continue;
            std::vector<uint8_t> raw(entry.rawLength);
            if (!Codec::ZstdDecompress(entry.payload, entry.length, raw.empty() ? NULL : &raw[0], raw.size()))
                return false;
            bool rle = false;
            if (!Codec::LegacyCompress(raw.empty() ? NULL : &raw[0], raw.size(), payloads[chunk], &rle))
                return false;
            entry.header = static_cast<uint32_t>(payloads[chunk].size()) | (entry.header & Codec::kCompressedFlag)
                           | (rle ? Codec::kRleFlag : 0);
            entry.payload = &payloads[chunk][0];
            entry.length = payloads[chunk].size();
        }
        Region::Build(chunks, file.data);
        blob.files.push_back(file);
    }
    blob.Build(out);
    return true;
}
}
