#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

#include <zlib.h>

#include "savecontainer/ScBlob.hpp"
#include "savecontainer/ScCodec.hpp"
#include "savecontainer/ScContainer.hpp"
#include "savecontainer/ScExport.hpp"
#include "savecontainer/ScRegion.hpp"
#include "savecontainer/ScWriter.hpp"

using namespace savecontainer;

namespace
{
int failures = 0;

void Check(bool ok, const char* what)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok)
        ++failures;
}

void Put32(std::vector<uint8_t>& bytes, size_t at, uint32_t value)
{
    for (int byte = 0; byte < 4; ++byte)
        bytes[at + byte] = static_cast<uint8_t>(value >> (8 * byte));
}

std::vector<uint16_t> Name(const char* text)
{
    std::vector<uint16_t> name;
    for (; *text != 0; ++text)
        name.push_back(static_cast<uint16_t>(*text));
    return name;
}

std::vector<uint8_t> Region(const std::vector<int>& slots, const std::vector<size_t>& sizes)
{
    size_t sectors = 2;
    for (size_t index = 0; index < slots.size(); ++index)
        sectors += (sizes[index] + 8) / 4096 + 1;
    std::vector<uint8_t> file(sectors * 4096, 0);
    size_t next = 2;
    for (size_t index = 0; index < slots.size(); ++index)
    {
        const size_t count = (sizes[index] + 8) / 4096 + 1;
        Put32(file, static_cast<size_t>(slots[index]) * 4, static_cast<uint32_t>((next << 8) | count));
        Put32(file, 4096 + static_cast<size_t>(slots[index]) * 4, 1000 + static_cast<uint32_t>(index));
        Put32(file, next * 4096, static_cast<uint32_t>(sizes[index]) | 0x80000000u);
        Put32(file, next * 4096 + 4, static_cast<uint32_t>(sizes[index] * 4));
        for (size_t byte = 0; byte < sizes[index]; ++byte)
            file[next * 4096 + 8 + byte] = static_cast<uint8_t>((byte * 31 + index * 7) & 0xFF);
        next += count;
    }
    return file;
}

Blob Sample()
{
    Blob blob;
    blob.originalVersion = 9;
    blob.saveVersion = 11;
    BlobFile level;
    level.name = Name("level.dat");
    level.timestamp = 123456789012LL;
    for (int byte = 0; byte < 700; ++byte)
        level.data.push_back(static_cast<uint8_t>(byte * 5));
    blob.files.push_back(level);
    BlobFile empty;
    empty.name = Name("r.0.0.mcr");
    blob.files.push_back(empty);
    BlobFile region;
    region.name = Name("DIM1/r.0.-1.mcr");
    std::vector<int> slots;
    std::vector<size_t> sizes;
    slots.push_back(0);
    sizes.push_back(100);
    slots.push_back(33);
    sizes.push_back(4088);
    slots.push_back(500);
    sizes.push_back(0);
    slots.push_back(1023);
    sizes.push_back(9000);
    region.data = Region(slots, sizes);
    blob.files.push_back(region);
    return blob;
}

bool SameFiles(const Blob& left, const Blob& right)
{
    if (left.files.size() != right.files.size() || left.originalVersion != right.originalVersion
        || left.saveVersion != right.saveVersion)
        return false;
    for (size_t index = 0; index < left.files.size(); ++index)
        if (left.files[index].name != right.files[index].name || left.files[index].data != right.files[index].data
            || left.files[index].timestamp != right.files[index].timestamp)
            return false;
    return true;
}

bool ReadFile(const char* path, std::vector<uint8_t>& bytes)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
        return false;
    uint8_t buffer[65536];
    size_t got;
    bytes.clear();
    while ((got = fread(buffer, 1, sizeof(buffer), file)) > 0)
        bytes.insert(bytes.end(), buffer, buffer + got);
    fclose(file);
    return true;
}
std::vector<uint8_t> Built(const Blob& blob)
{
    std::vector<uint8_t> bytes;
    blob.Build(bytes);
    return bytes;
}

std::vector<uint8_t> Logical(const std::vector<uint8_t>& bytes)
{
    std::vector<uint8_t> out;
    Container::Encode(&bytes[0], bytes.size(), out);
    return out;
}

bool SameState(const std::vector<uint8_t>& file, const Blob& blob, size_t* valid)
{
    std::vector<uint8_t> decoded;
    if (!Container::Decode(&file[0], file.size(), decoded, valid))
        return false;
    return Logical(decoded) == Logical(Built(blob));
}

void TouchChunk(Blob& blob, size_t file, int slot)
{
    std::vector<uint8_t>& data = blob.files[file].data;
    const uint32_t entry = data[slot * 4] | (data[slot * 4 + 1] << 8) | (data[slot * 4 + 2] << 16) | (data[slot * 4 + 3] << 24);
    data[(entry >> 8) * 4096 + 8 + 3] ^= 0x5A;
}

void DropChunk(Blob& blob, size_t file, int slot)
{
    for (int byte = 0; byte < 4; ++byte)
        blob.files[file].data[slot * 4 + byte] = 0;
}
}

int main()
{
    const Blob original = Sample();
    std::vector<uint8_t> blobBytes, native, decoded;
    original.Build(blobBytes);
    Blob parsed;
    Check(Blob::Parse(&blobBytes[0], blobBytes.size(), parsed) && SameFiles(original, parsed),
          "a blob parses back to the files it was built from");

    Container::Encode(&blobBytes[0], blobBytes.size(), native);
    Check(Container::IsContainer(&native[0], native.size()), "an encoded save starts with the container magic");
    Check(Container::Decode(&native[0], native.size(), decoded, NULL), "an encoded save decodes");
    Blob again;
    Check(Blob::Parse(&decoded[0], decoded.size(), again) && SameFiles(original, again),
          "the decoded blob holds the same files, region sectors included");
    Check(decoded == blobBytes, "a packed blob comes back byte for byte");
    std::vector<uint8_t> second;
    Container::Encode(&decoded[0], decoded.size(), second);
    Check(second == native, "encoding the decoded save gives the same bytes");

    Check(native.size() < blobBytes.size(), "the container is smaller than the sector-padded blob");

    bool allRefused = true;
    for (size_t at = 0; at < native.size(); at += native.size() / 97 + 1)
    {
        std::vector<uint8_t> broken = native;
        broken[at] ^= 0x10;
        std::vector<uint8_t> out;
        if (Container::Decode(&broken[0], broken.size(), out, NULL))
            allRefused = false;
    }
    Check(allRefused, "a changed byte anywhere is refused");
    std::vector<uint8_t> cut(native.begin(), native.end() - 3), out;
    Check(!Container::Decode(&cut[0], cut.size(), out, NULL), "a truncated save is refused");
    Check(!Container::Decode(&blobBytes[0], blobBytes.size(), out, NULL), "a blob is not a container");

    BlobFile odd;
    odd.name = Name("r.5.5.mcr");
    odd.data.assign(5000, 7);
    Blob withOdd = original;
    withOdd.files.push_back(odd);
    std::vector<uint8_t> oddBytes, oddNative, oddDecoded;
    withOdd.Build(oddBytes);
    Container::Encode(&oddBytes[0], oddBytes.size(), oddNative);
    Blob oddAgain;
    Check(Container::Decode(&oddNative[0], oddNative.size(), oddDecoded, NULL) && Blob::Parse(&oddDecoded[0], oddDecoded.size(), oddAgain)
              && SameFiles(withOdd, oddAgain),
          "a region file that does not parse is kept as it is");


    {
        Blob state = Sample();
        Writer writer;
        std::vector<uint8_t> file, piece;
        bool replace = false;
        Check(writer.Prepare(&Built(state)[0], Built(state).size(), file, &replace, 0) && replace && file.size() > 12,
              "the first save is a whole file");
        Check(SameState(file, state, NULL), "the whole file holds the state");
        const size_t whole = file.size();

        TouchChunk(state, 2, 33);
        const Blob afterOne = state;
        const std::vector<uint8_t> one = Built(state);
        Check(writer.Prepare(&one[0], one.size(), piece, &replace, file.size()) && !replace && !piece.empty()
                  && piece.size() < whole / 2 + 200,
              "a changed chunk is saved alone");
        file.insert(file.end(), piece.begin(), piece.end());
        Check(SameState(file, state, NULL), "the file with the appended change holds the new state");

        Check(writer.Prepare(&one[0], one.size(), piece, &replace, file.size()) && !replace && piece.empty(),
              "an unchanged world appends nothing");
        Blob stamped = state;
        {
            std::vector<uint8_t>& region = stamped.files[2].data;
            const uint32_t entry = region[33 * 4] | (region[33 * 4 + 1] << 8) | (region[33 * 4 + 2] << 16);
            (void)entry;
            region[4096 + 33 * 4] ^= 0x7F;
        }
        const std::vector<uint8_t> stampedBytes = Built(stamped);
        Check(writer.Prepare(&stampedBytes[0], stampedBytes.size(), piece, &replace, file.size()) && !replace && piece.empty(),
              "a chunk that only changed its timestamp is not written again");
        writer.Prepare(&one[0], one.size(), piece, &replace, file.size());

        state.files[0].data[10] ^= 1;
        DropChunk(state, 2, 0);
        BlobFile added;
        added.name = Name("data/new.dat");
        added.data.assign(300, 9);
        state.files.push_back(added);
        const std::vector<uint8_t> two = Built(state);
        const size_t beforeTwo = file.size();
        Check(writer.Prepare(&two[0], two.size(), piece, &replace, file.size()) && !replace, "a removed chunk and a new file are saved");
        file.insert(file.end(), piece.begin(), piece.end());
        size_t valid = 0;
        Check(SameState(file, state, &valid) && valid == file.size(), "removal and addition replay");

        std::vector<uint8_t> torn(file.begin(), file.end() - 5);
        Check(SameState(torn, afterOne, &valid) && valid == beforeTwo, "a torn last segment is ignored and the earlier state is kept");

        std::vector<uint8_t> hurt = file;
        hurt[whole + 20] ^= 0x40;
        std::vector<uint8_t> hurtOut;
        Check(!Container::Decode(&hurt[0], hurt.size(), hurtOut, NULL), "damage in an earlier segment is refused");

        Check(writer.Prepare(&two[0], two.size(), piece, &replace, file.size() + 1) && replace,
              "a file of the wrong size is replaced whole");

        std::vector<uint8_t> decoded;
        size_t length = 0;
        Container::Decode(&file[0], file.size(), decoded, &length);
        Writer adopted;
        adopted.Adopt(&decoded[0], decoded.size(), length);
        TouchChunk(state, 2, 33);
        const std::vector<uint8_t> three = Built(state);
        Check(adopted.Prepare(&three[0], three.size(), piece, &replace, length) && !replace && !piece.empty(),
              "a loaded save carries on with appends");

        Writer growing;
        std::vector<uint8_t> big;
        Blob heavy = Sample();
        growing.Prepare(&Built(heavy)[0], Built(heavy).size(), big, &replace, 0);
        bool compacted = false;
        for (int round = 0; round < 600 && !compacted; ++round)
        {
            TouchChunk(heavy, 2, 1023);
            const std::vector<uint8_t> bytes = Built(heavy);
            growing.Prepare(&bytes[0], bytes.size(), piece, &replace, big.size());
            if (replace)
            {
                compacted = true;
                Check(piece.size() < big.size(), "the file is rewritten smaller once most of it is superseded");
            }
            else
                big.insert(big.end(), piece.begin(), piece.end());
        }
        Check(compacted, "a file full of superseded records is compacted");
    }


    {
        std::vector<uint8_t> raw;
        for (int index = 0; index < 20000; ++index)
            raw.push_back(static_cast<uint8_t>(index < 9000 ? 0 : (index % 7 == 0 ? 0xFF : index * 13)));
        std::vector<uint8_t> packed(raw.size() + 2048);
        size_t packedSize = 0;
        Check(Codec::ZstdCompress(&raw[0], raw.size(), &packed[0], packed.size(), &packedSize) && packedSize < raw.size() / 4,
              "zstd packs a chunk");
        std::vector<uint8_t> back(raw.size());
        Check(Codec::ZstdDecompress(&packed[0], packedSize, &back[0], back.size()) && back == raw, "zstd unpacks it");
        Check(!Codec::ZstdDecompress(&packed[0], packedSize, &back[0], back.size() - 1), "zstd refuses a wrong length");
        packed[packedSize / 2] ^= 0x33;
        Check(!Codec::ZstdDecompress(&packed[0], packedSize, &back[0], back.size()) || back != raw,
              "zstd does not return the original from damaged input");

        std::vector<uint8_t> legacy, restored;
        bool rle = false;
        Check(Codec::LegacyCompress(&raw[0], raw.size(), legacy, &rle) && rle
                  && Codec::LegacyDecompress(&legacy[0], legacy.size(), rle, raw.size(), restored) && restored == raw,
              "the old compression (run lengths, then zlib) round trips");

        std::vector<uint8_t> second(5000);
        for (size_t index = 0; index < second.size(); ++index)
            second[index] = static_cast<uint8_t>((index * 2654435761u) >> 13);
        std::vector<uint8_t> secondZstd(second.size() + 2048);
        size_t secondSize = 0;
        Codec::ZstdCompress(&second[0], second.size(), &secondZstd[0], secondZstd.size(), &secondSize);
        std::vector<uint8_t> firstZstd(raw.size() + 2048);
        size_t firstSize = 0;
        Codec::ZstdCompress(&raw[0], raw.size(), &firstZstd[0], firstZstd.size(), &firstSize);
        std::vector<ChunkRef> chunks(2);
        chunks[0].slot = 3;
        chunks[0].timestamp = 77;
        chunks[0].header = static_cast<uint32_t>(firstSize) | Codec::kZstdFlag | Codec::kCompressedFlag;
        chunks[0].rawLength = static_cast<uint32_t>(raw.size());
        chunks[0].payload = &firstZstd[0];
        chunks[0].length = firstSize;
        chunks[1].slot = 900;
        chunks[1].timestamp = 78;
        chunks[1].header = static_cast<uint32_t>(legacy.size()) | Codec::kRleFlag | Codec::kCompressedFlag;
        chunks[1].rawLength = static_cast<uint32_t>(raw.size());
        chunks[1].payload = &legacy[0];
        chunks[1].length = legacy.size();
        Blob mixed;
        BlobFile region;
        region.name = Name("r.1.1.mcr");
        Region::Build(chunks, region.data);
        mixed.files.push_back(region);
        const std::vector<uint8_t> mixedBytes = Built(mixed);
        std::vector<uint8_t> exported;
        Check(Export::ToLegacyChunks(&mixedBytes[0], mixedBytes.size(), exported), "a save with zstd chunks exports");
        Blob exportedBlob;
        std::vector<ChunkRef> after;
        bool exportOk = Blob::Parse(&exported[0], exported.size(), exportedBlob)
                        && Region::Scan(&exportedBlob.files[0].data[0], exportedBlob.files[0].data.size(), after)
                        && after.size() == 2;
        Check(exportOk && (after[0].header & Codec::kZstdFlag) == 0 && (after[0].header & Codec::kRleFlag) != 0
                  && after[0].slot == 3 && after[0].timestamp == 77 && after[0].rawLength == raw.size(),
              "the exported chunk is in the old compression");
        std::vector<uint8_t> unpacked;
        Check(exportOk && Codec::LegacyDecompress(after[0].payload, after[0].length, true, raw.size(), unpacked) && unpacked == raw,
              "and holds the same bytes");
        Check(exportOk && after[1].length == legacy.size() && memcmp(after[1].payload, &legacy[0], legacy.size()) == 0,
              "a chunk already in the old compression is left alone");
        std::vector<uint8_t> unused;
        Container::Encode(&mixedBytes[0], mixedBytes.size(), unused);
        std::vector<uint8_t> roundTrip;
        Check(!unused.empty() && Container::Decode(&unused[0], unused.size(), roundTrip, NULL) && roundTrip == mixedBytes,
              "zstd chunks survive the container byte for byte");
    }

    const char* world = getenv("LCE_SAVE_TEST_WORLD");
    if (world != NULL)
    {
        std::vector<uint8_t> legacy, realBlob, realNative, realDecoded, realNativeAgain;
        bool good = ReadFile(world, legacy) && legacy.size() > 8;
        uLongf size = good ? static_cast<uLongf>(legacy[4] | (legacy[5] << 8) | (legacy[6] << 16) | (legacy[7] << 24)) : 0;
        if (good)
        {
            realBlob.assign(size, 0);
            good = uncompress(&realBlob[0], &size, &legacy[8], static_cast<uLong>(legacy.size() - 8)) == Z_OK;
        }
        Check(good, "the real world unwraps");
        if (good)
        {
            Container::Encode(&realBlob[0], realBlob.size(), realNative);
            Check(!realNative.empty() && Container::Decode(&realNative[0], realNative.size(), realDecoded, NULL),
                  "the real world encodes and decodes");
            Container::Encode(&realDecoded[0], realDecoded.size(), realNativeAgain);
            Check(realNativeAgain == realNative, "the real world keeps every chunk record through a round trip");
            printf("real world: blob %lu bytes, legacy file %lu bytes, container %lu bytes\n",
                   static_cast<unsigned long>(realBlob.size()), static_cast<unsigned long>(legacy.size()),
                   static_cast<unsigned long>(realNative.size()));
        }
    }

    printf("%s\n", failures == 0 ? "all passed" : "FAILED");
    return failures == 0 ? 0 : 1;
}
