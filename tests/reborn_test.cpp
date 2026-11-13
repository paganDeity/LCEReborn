#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <vector>

#include "reborn/RbBatch.hpp"
#include "reborn/RbFrame.hpp"
#include "reborn/RbHello.hpp"
#include "reborn/RbVarint.hpp"

using namespace reborn;

namespace
{
int failures = 0;

void Check(bool ok, const char* what)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok)
        ++failures;
}

// hands out at most three bytes a read, as a socket may
class ByteSourceOf : public ByteSource
{
public:
    explicit ByteSourceOf(const std::vector<uint8_t>& bytes) : reads(0), m_bytes(bytes), m_at(0) {}

    virtual bool Read(uint8_t* data, size_t size)
    {
        while (size > 0)
        {
            if (m_at == m_bytes.size())
                return false;
            const size_t take = size < 3 ? size : 3;
            const size_t left = m_bytes.size() - m_at;
            const size_t count = take < left ? take : left;
            memcpy(data, &m_bytes[m_at], count);
            m_at += count;
            data += count;
            size -= count;
            ++reads;
        }
        return true;
    }

    size_t reads;

private:
    std::vector<uint8_t> m_bytes;
    size_t m_at;
};

bool OneByteIds(uint64_t id, void*)
{
    return id <= 0xFF;
}
}

int main()
{
    const uint64_t values[] = {0, 1, 127, 128, 255, 300, 16383, 16384, 0xFFFFFFFFull, 0x100000000ull, 0xFFFFFFFFFFFFFFFFull};
    bool varints = true;
    for (size_t index = 0; index < sizeof(values) / sizeof(values[0]); ++index)
    {
        std::vector<uint8_t> bytes;
        Varint::Write(bytes, values[index]);
        size_t at = 0;
        uint64_t back = 0;
        varints = varints && Varint::Read(&bytes[0], bytes.size(), &at, &back) && back == values[index] && at == bytes.size()
                  && Varint::Size(values[index]) == bytes.size();
    }
    Check(varints, "varints round trip up to 64 bits and report their size");
    const uint8_t overlong[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01};
    size_t at = 0;
    uint64_t value = 0;
    Check(!Varint::Read(overlong, sizeof(overlong), &at, &value), "an overlong varint is refused");
    const uint8_t cut[] = {0x80};
    at = 0;
    Check(!Varint::Read(cut, 1, &at, &value), "a cut varint is not read");

    std::vector<uint8_t> stream;
    const uint8_t first[] = {1, 2, 3};
    std::vector<uint8_t> big(5000, 7);
    Frame::Write(stream, 5, first, 3);
    Frame::Write(stream, 70000, &big[0], big.size());
    Frame::Write(stream, 9, NULL, 0);
    uint64_t id = 0;
    std::vector<uint8_t> payload;
    size_t skipped = 0;
    ByteSourceOf whole(stream);
    bool intact = ReadFrame(whole, 1 << 20, NULL, NULL, &id, &payload, &skipped) && id == 5 && payload.size() == 3 && payload[2] == 3;
    intact = intact && ReadFrame(whole, 1 << 20, NULL, NULL, &id, &payload, &skipped) && id == 70000 && payload == big;
    intact = intact && ReadFrame(whole, 1 << 20, NULL, NULL, &id, &payload, &skipped) && id == 9 && payload.empty();
    Check(intact && whole.reads >= stream.size() / 3, "frames come out whole when the bytes arrive a few at a time");
    Check(!ReadFrame(whole, 1 << 20, NULL, NULL, &id, &payload, &skipped), "the end of the stream ends the reading");

    std::vector<uint8_t> mixed;
    const uint8_t unknown[] = {9, 9, 9, 9};
    Frame::Write(mixed, 4242, unknown, 4);
    Frame::Write(mixed, 0x101, unknown, 2);
    Frame::Write(mixed, 5, first, 3);
    ByteSourceOf skipping(mixed);
    Check(ReadFrame(skipping, 1 << 20, OneByteIds, NULL, &id, &payload, &skipped) && id == 5 && payload.size() == 3 && skipped == 2,
          "frames with numbers the reader does not take are skipped by length, and the stream stays in step");

    std::vector<uint8_t> oversize;
    Frame::Write(oversize, 1, &big[0], big.size());
    ByteSourceOf strict(oversize);
    Check(!ReadFrame(strict, 100, NULL, NULL, &id, &payload, &skipped), "a frame over the limit is refused");

    std::vector<uint8_t> empty(1, 0);
    ByteSourceOf zero(empty);
    Check(!ReadFrame(zero, 100, NULL, NULL, &id, &payload, &skipped), "a zero length frame is refused");

    std::vector<uint8_t> cutShort;
    Frame::Write(cutShort, 5, first, 3);
    cutShort.pop_back();
    ByteSourceOf shortSource(cutShort);
    Check(!ReadFrame(shortSource, 100, NULL, NULL, &id, &payload, &skipped), "a frame cut short is not read");

    std::vector<uint8_t> head;
    Preamble::Write(head);
    Check(Preamble::Sniff(&head[0], 2) == Preamble::eSniff_NeedMore && Preamble::Sniff(&head[0], 4) == Preamble::eSniff_Reborn,
          "the preamble is recognised once all four bytes are in");
    const uint8_t legacyFirst[] = {2, 0, 0, 0};
    const uint8_t legacySecond[] = {232, 0, 0, 0};
    const uint8_t almost[] = {'L', 'C', 'E', 0};
    Check(Preamble::Sniff(legacyFirst, 4) == Preamble::eSniff_Legacy && Preamble::Sniff(legacySecond, 1) == Preamble::eSniff_Legacy
              && Preamble::Sniff(almost, 4) == Preamble::eSniff_Legacy,
          "bytes that cannot be the preamble are Legacy at once");

    Hello client, server;
    client.minVersion = 1;
    client.maxVersion = 3;
    client.features = eFeature_Batching | eFeature_Zstd | eFeature_LongChat;
    client.registryHash.assign(32, 0xAB);
    client.mods.push_back("lib@1.2");
    server.minVersion = 2;
    server.maxVersion = 5;
    server.features = eFeature_Zstd | eFeature_LongChat | eFeature_Deltas;
    server.registryHash.assign(32, 0xAB);
    server.mods.push_back("lib@1.2");
    std::vector<uint8_t> encoded;
    client.Encode(encoded);
    Hello decoded;
    Check(Hello::Decode(&encoded[0], encoded.size(), decoded) && decoded.maxVersion == 3 && decoded.features == client.features
              && decoded.registryHash == client.registryHash && decoded.mods == client.mods,
          "a hello round trips");
    encoded.push_back(0);
    Check(!Hello::Decode(&encoded[0], encoded.size(), decoded), "a hello with trailing bytes is refused");
    Agreement agreement;
    std::string refusal;
    Check(Negotiate(client, server, agreement, &refusal) == eRefusal_None && agreement.version == 3
              && agreement.features == (eFeature_Zstd | eFeature_LongChat),
          "the newest shared version and the shared features are agreed");
    server.minVersion = 4;
    Check(Negotiate(client, server, agreement, &refusal) == eRefusal_Version, "no shared version is a refusal");
    server.minVersion = 2;
    server.mods.push_back("other@1");
    Check(Negotiate(client, server, agreement, &refusal) == eRefusal_Mod && refusal.find("other@1") != std::string::npos,
          "a mod the server requires and the client lacks is named in the refusal");
    server.mods.pop_back();
    server.registryHash.assign(32, 0xCD);
    Check(Negotiate(client, server, agreement, &refusal) == eRefusal_Registry, "different registries are refused");

    std::vector<uint8_t> frames;
    for (int index = 0; index < 40; ++index)
        Frame::Write(frames, 20 + index % 3, &big[0], 30);
    Frame::Write(frames, 4242, unknown, 4);
    std::vector<uint8_t> batch;
    Batch::Pack(frames, 256, batch);
    std::vector<uint8_t> unpacked;
    Check(batch.size() < frames.size() / 4 && Batch::Unpack(&batch[0], batch.size(), 1 << 20, unpacked) && unpacked == frames,
          "a repetitive batch is compressed and unpacks to the same frames");
    MemorySource inner(&unpacked[0], unpacked.size());
    size_t innerFrames = 0;
    skipped = 0;
    while (ReadFrame(inner, 1 << 20, OneByteIds, NULL, &id, &payload, &skipped))
        ++innerFrames;
    Check(innerFrames == 40 && skipped == 1 && inner.AtEnd(), "the frames inside a batch read like a stream and skip the same way");
    std::vector<uint8_t> few;
    Frame::Write(few, 5, first, 3);
    Batch::Pack(few, 256, batch);
    Check(batch.size() == few.size() + 2 && batch[0] == 0 && Batch::Unpack(&batch[0], batch.size(), 100, unpacked) && unpacked == few,
          "a small batch is sent as it is");
    Check(!Batch::Unpack(&batch[0], batch.size(), 2, unpacked), "a batch longer than the limit is refused");
    batch[0] = 2;
    Check(!Batch::Unpack(&batch[0], batch.size(), 100, unpacked), "a batch with a flag this build does not know is refused");
    Batch::Pack(frames, 256, batch);
    batch[batch.size() / 2] ^= 0x5A;
    batch[batch.size() / 2 + 1] ^= 0xA5;
    Check(!Batch::Unpack(&batch[0], batch.size(), 1 << 20, unpacked) || unpacked != frames,
          "damaged compressed data is refused or does not pass for the original");
    batch.clear();
    Varint::Write(batch, 1);
    Varint::Write(batch, 1000000);
    batch.push_back(0x28);
    Check(!Batch::Unpack(&batch[0], batch.size(), 1 << 20, unpacked), "a compressed batch that claims more than it holds is refused");

    printf("%s\n", failures == 0 ? "all passed" : "FAILED");
    return failures == 0 ? 0 : 1;
}
