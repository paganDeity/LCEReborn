#include "game/network/RebornCodec.hpp"

#if LCE_PLATFORM_PC
#include <stdint.h>

#include <deque>
#include <string>
#include <utility>
#include <vector>

#include "game/core/ByteArrayInputStream.hpp"
#include "game/core/ByteArrayOutputStream.hpp"
#include "game/core/CriticalSection.hpp"
#include "game/core/DataInputStream.hpp"
#include "game/core/DataOutputStream.hpp"
#include "game/core/InputStream.hpp"
#include "game/core/ModHost.hpp"
#include "game/network/DisconnectPacket.hpp"
#include "game/network/Packet.hpp"
#include "game/network/PacketCodec.hpp"
#include "game/network/PacketRead.hpp"
#include "game/network/ServerboundChatPacket.hpp"
#include "platform/pc/Log.hpp"
#include "reborn/RbBatch.hpp"
#include "reborn/RbFrame.hpp"
#include "reborn/RbHello.hpp"
#include "reborn/RbVarint.hpp"

namespace
{
const Logger sLog("Network");

// control frames sit above the Legacy packet numbers, which are one byte
const uint64_t kFrameHello = 0x100;
const uint64_t kFrameRefusal = 0x101;
const uint64_t kFrameBatch = 0x102;
const size_t kMaxFrame = 16 * 1024 * 1024;
const size_t kPreambleSize = sizeof(reborn::Preamble::kMagic);
// below this a batch is sent as it is, since zstd's own header would eat most of what it saves
const size_t kCompressFrom = 512;
const uint64_t kLocalFeatures = reborn::eFeature_Batching | reborn::eFeature_Zstd | reborn::eFeature_LongChat;

reborn::Hello LocalHello()
{
    reborn::Hello hello;
    hello.features = kLocalFeatures;
    const std::vector<ModHost::Entry> mods = ModHost::Get().List();
    for (size_t index = 0; index < mods.size(); ++index)
    {
        if (mods[index].state == "running")
            hello.mods.push_back(mods[index].id + "@" + mods[index].version);
    }
    return hello;
}

void WriteBytes(DataOutputStream* output, const std::vector<uint8_t>& bytes)
{
    for (size_t index = 0; index < bytes.size(); ++index)
        output->writeByte(bytes[index]);
}

void AppendFrame(std::vector<uint8_t>& out, uint64_t id, const std::vector<uint8_t>& payload)
{
    reborn::Frame::Write(out, id, payload.empty() ? NULL : &payload[0], payload.size());
}

class StreamSource : public reborn::ByteSource
{
public:
    explicit StreamSource(DataInputStream* input) : m_input(input) {}

    virtual bool Read(uint8_t* data, size_t size)
    {
        for (size_t index = 0; index < size; ++index)
        {
            const int value = m_input->read();
            if (value < 0)
                return false;
            data[index] = static_cast<uint8_t>(value);
        }
        return true;
    }

private:
    DataInputStream* m_input;
};

// a control frame of the opening, or any frame when the connection is not past it
bool ReadOpeningFrame(DataInputStream* input, uint64_t* id, std::vector<uint8_t>* payload)
{
    StreamSource source(input);
    return reborn::ReadFrame(source, kMaxFrame, NULL, NULL, id, payload, NULL);
}

bool KnownPacket(uint64_t id, void* isServer)
{
    return id <= 0xFF && PacketRead::Known(static_cast<int>(id), *static_cast<bool*>(isServer));
}

bool KnownPacketOrBatch(uint64_t id, void* isServer)
{
    return id == kFrameBatch || KnownPacket(id, isServer);
}

void LogSkipped(size_t skipped)
{
    if (skipped != 0)
        LOG_DEBUG(sLog, "skipped %u Reborn frames with numbers this game does not read", static_cast<unsigned int>(skipped));
}

// the packet frames of a connection past its opening: each packet keeps its Legacy encoding inside a frame, and once
// both hellos agree, the frames written between two flushes go out as one batch
class PacketFrames
{
public:
    PacketFrames() : m_batching(false), m_compress(false), m_longChat(false) { InitializeCriticalSection(&m_lock); }
    ~PacketFrames() { DeleteCriticalSection(&m_lock); }

    // the read thread learns what was agreed; the write thread uses it
    void Agree(uint64_t features)
    {
        EnterCriticalSection(&m_lock);
        m_batching = (features & reborn::eFeature_Batching) != 0;
        m_compress = (features & reborn::eFeature_Zstd) != 0;
        m_longChat = (features & reborn::eFeature_LongChat) != 0;
        LeaveCriticalSection(&m_lock);
    }

    unsigned int ChatLimit()
    {
        EnterCriticalSection(&m_lock);
        const bool longChat = m_longChat;
        LeaveCriticalSection(&m_lock);
        return longChat ? ServerboundChatPacket::kLongLength : ServerboundChatPacket::kMaxLength;
    }

    void Write(boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer, unsigned int& packetsWritten,
               bool wide)
    {
        ByteArrayOutputStream bytes;
        DataOutputStream stream(&bytes);
        m_legacy.Write(packet, &stream, isServer, packetsWritten, wide);
        std::vector<uint8_t> frame;
        reborn::Frame::Write(frame, bytes.mBuffer.data[0], bytes.mBuffer.data + 1, bytes.mCount - 1);
        bool batching = false;
        EnterCriticalSection(&m_lock);
        batching = m_batching;
        LeaveCriticalSection(&m_lock);
        if (batching)
            m_pending.insert(m_pending.end(), frame.begin(), frame.end());
        else
            WriteBytes(output, frame);
    }

    void Flush(DataOutputStream* output)
    {
        if (m_pending.empty())
            return;
        bool compress = false;
        EnterCriticalSection(&m_lock);
        compress = m_compress;
        LeaveCriticalSection(&m_lock);
        std::vector<uint8_t> payload;
        reborn::Batch::Pack(m_pending, compress ? kCompressFrom : m_pending.size() + 1, payload);
        m_pending.clear();
        std::vector<uint8_t> frame;
        AppendFrame(frame, kFrameBatch, payload);
        WriteBytes(output, frame);
    }

    // the next packet the game knows; a frame with a number it does not know is skipped by its length
    boost::shared_ptr<Packet> Read(DataInputStream* input, bool isServer, unsigned int& packetsRead, bool& protocolError)
    {
        while (m_inbound.empty())
        {
            uint64_t id = 0;
            std::vector<uint8_t> payload;
            size_t skipped = 0;
            StreamSource source(input);
            const bool read = reborn::ReadFrame(source, kMaxFrame, KnownPacketOrBatch, &isServer, &id, &payload, &skipped);
            LogSkipped(skipped);
            if (!read || (id == kFrameBatch && !Unbatch(payload, isServer)))
            {
                protocolError = true;
                return boost::shared_ptr<Packet>();
            }
            if (id != kFrameBatch)
                m_inbound.push_back(std::make_pair(id, payload));
        }
        const std::pair<uint64_t, std::vector<uint8_t> > next = m_inbound.front();
        m_inbound.pop_front();
        arrayWithLength<unsigned char> plain(static_cast<unsigned int>(next.second.size() + 1), true);
        plain.data[0] = static_cast<unsigned char>(next.first);
        for (size_t index = 0; index < next.second.size(); ++index)
            plain.data[index + 1] = next.second[index];
        ByteArrayInputStream bytes(plain);
        DataInputStream stream(&bytes);
        PacketRead::SetChatLimit(ChatLimit());
        return m_legacy.Read(&stream, isServer, packetsRead, protocolError);
    }

    LegacyPacketCodec& Legacy() { return m_legacy; }

private:
    // a batch holds packet frames only; one inside another is skipped like any number the game does not read
    bool Unbatch(const std::vector<uint8_t>& payload, bool isServer)
    {
        std::vector<uint8_t> frames;
        if (payload.empty() || !reborn::Batch::Unpack(&payload[0], payload.size(), kMaxFrame, frames))
        {
            LOG_WARN(sLog, "a Reborn batch could not be unpacked");
            return false;
        }
        reborn::MemorySource source(frames.empty() ? NULL : &frames[0], frames.size());
        size_t skipped = 0;
        while (!source.AtEnd())
        {
            uint64_t id = 0;
            std::vector<uint8_t> frame;
            if (!reborn::ReadFrame(source, kMaxFrame, KnownPacket, &isServer, &id, &frame, &skipped))
            {
                LogSkipped(skipped);
                if (source.AtEnd())
                    return true;
                LOG_WARN(sLog, "a Reborn batch ends inside a frame");
                return false;
            }
            m_inbound.push_back(std::make_pair(id, frame));
        }
        LogSkipped(skipped);
        return true;
    }

    LegacyPacketCodec m_legacy;
    C4JCriticalSection m_lock;
    bool m_batching;
    bool m_compress;
    bool m_longChat;
    // the write thread's
    std::vector<uint8_t> m_pending;
    // the read thread's
    std::deque<std::pair<uint64_t, std::vector<uint8_t> > > m_inbound;
};

DisconnectPacket::eDisconnectReason ReasonFor(reborn::ERefusal refusal)
{
    switch (refusal)
    {
    case reborn::eRefusal_Version:
        return DisconnectPacket::eDisconnectReason_ClientOutOfDate;
    case reborn::eRefusal_Registry:
    case reborn::eRefusal_Mod:
        return DisconnectPacket::eDisconnectReason_MissingMods;
    default:
        return DisconnectPacket::eDisconnectReason_LoginFailed;
    }
}

// the four bytes read to tell the protocols apart, given back to the Legacy reader ahead of the rest of the stream
class PrefixedInputStream : public InputStream
{
public:
    PrefixedInputStream(const uint8_t* prefix, size_t size, InputStream* rest) : m_prefix(prefix, prefix + size), m_at(0), m_rest(rest) {}

    bool Drained() const { return m_at == m_prefix.size(); }

    virtual int read()
    {
        if (!Drained())
            return m_prefix[m_at++];
        return m_rest->read();
    }

    virtual int read(arrayWithLength<unsigned char> data) { return read(data, 0, data.length); }

    virtual int read(arrayWithLength<unsigned char> data, unsigned int offset, unsigned int length)
    {
        unsigned int copied = 0;
        while (copied < length && !Drained())
            data.data[offset + copied++] = m_prefix[m_at++];
        if (copied == length)
            return static_cast<int>(copied);
        const int more = m_rest->read(data, offset + copied, length - copied);
        if (more < 0)
            return copied == 0 ? more : static_cast<int>(copied);
        return static_cast<int>(copied) + more;
    }

    virtual void close() {}

    virtual long long skip(long long count)
    {
        long long skipped = 0;
        while (skipped < count && !Drained())
        {
            ++m_at;
            ++skipped;
        }
        return skipped + (skipped < count ? m_rest->skip(count - skipped) : 0);
    }

private:
    std::vector<uint8_t> m_prefix;
    size_t m_at;
    InputStream* m_rest;
};

class RebornClientCodec : public PacketCodec
{
public:
    RebornClientCodec() : m_helloSent(false), m_answered(false), m_refused(false) {}

    virtual boost::shared_ptr<Packet> Read(DataInputStream* input, bool isServer, unsigned int& packetsRead,
                                           bool& protocolError)
    {
        if (m_refused)
            return boost::shared_ptr<Packet>();
        if (!m_answered)
        {
            m_answered = true;
            uint64_t id = 0;
            std::vector<uint8_t> payload;
            if (!ReadOpeningFrame(input, &id, &payload))
            {
                LOG_WARN(sLog, "the server did not answer the Reborn hello; it may speak only Legacy");
                protocolError = true;
                return boost::shared_ptr<Packet>();
            }
            if (id == kFrameRefusal)
                return Refused(payload);
            reborn::Hello server;
            reborn::Agreement agreement;
            std::string refusal;
            if (id != kFrameHello || payload.empty() || !reborn::Hello::Decode(&payload[0], payload.size(), server)
                || reborn::Negotiate(LocalHello(), server, agreement, &refusal) != reborn::eRefusal_None)
            {
                LOG_WARN(sLog, "the server's Reborn answer is not one this game can use%s%s", refusal.empty() ? "" : ": ",
                         refusal.c_str());
                protocolError = true;
                return boost::shared_ptr<Packet>();
            }
            m_frames.Agree(agreement.features);
            LOG_INFO(sLog, "Reborn protocol version %u with the server, features %llu", agreement.version,
                     static_cast<unsigned long long>(agreement.features));
        }
        return m_frames.Read(input, isServer, packetsRead, protocolError);
    }

    virtual void Write(boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer,
                       unsigned int& packetsWritten, bool wide)
    {
        if (!m_helloSent)
        {
            m_helloSent = true;
            std::vector<uint8_t> hello;
            LocalHello().Encode(hello);
            std::vector<uint8_t> opening;
            reborn::Preamble::Write(opening);
            AppendFrame(opening, kFrameHello, hello);
            WriteBytes(output, opening);
        }
        m_frames.Write(packet, output, isServer, packetsWritten, wide);
    }

    virtual void Flush(DataOutputStream* output) { m_frames.Flush(output); }
    virtual unsigned int ChatLimit() { return m_frames.ChatLimit(); }

private:
    // the server's refusal is a reason the game can show and the server's own words for the log; reads stop without an
    // error after it, because a read error closes the connection before the reason is handled
    boost::shared_ptr<Packet> Refused(const std::vector<uint8_t>& payload)
    {
        m_refused = true;
        size_t at = 0;
        uint64_t reason = DisconnectPacket::eDisconnectReason_LoginFailed;
        if (payload.empty() || !reborn::Varint::Read(&payload[0], payload.size(), &at, &reason)
            || reason > DisconnectPacket::eDisconnectReason_MissingMods)
            reason = DisconnectPacket::eDisconnectReason_LoginFailed;
        const std::string text(payload.begin() + at, payload.end());
        LOG_WARN(sLog, "the server refused this game: %s", text.c_str());
        return boost::shared_ptr<Packet>(new DisconnectPacket(static_cast<DisconnectPacket::eDisconnectReason>(reason)));
    }

    PacketFrames m_frames;
    bool m_helloSent;
    bool m_answered;
    bool m_refused;
};

// The server cannot know the protocol before the peer's first bytes, and the game may write to the connection before
// they arrive (a keep alive goes out on the first tick), so writes are held until the read thread has decided.
class RebornServerCodec : public PacketCodec
{
public:
    explicit RebornServerCodec(bool acceptLegacy)
        : m_acceptLegacy(acceptLegacy), m_state(eState_Undecided), m_answered(false), m_prefix(NULL), m_prefixed(NULL)
    {
        InitializeCriticalSection(&m_lock);
    }

    virtual ~RebornServerCodec()
    {
        delete m_prefixed;
        delete m_prefix;
        DeleteCriticalSection(&m_lock);
    }

    virtual boost::shared_ptr<Packet> Read(DataInputStream* input, bool isServer, unsigned int& packetsRead,
                                           bool& protocolError)
    {
        const EState state = State();
        if (state == eState_Refused)
            return boost::shared_ptr<Packet>();
        if (state == eState_RefusalSent)
        {
            protocolError = true;
            return boost::shared_ptr<Packet>();
        }
        if (state == eState_Undecided && !Decide(input))
        {
            protocolError = true;
            return boost::shared_ptr<Packet>();
        }
        switch (State())
        {
        case eState_Legacy:
            if (!m_prefix->Drained())
                return m_frames.Legacy().Read(m_prefixed, isServer, packetsRead, protocolError);
            return m_frames.Legacy().Read(input, isServer, packetsRead, protocolError);
        case eState_Reborn:
            return m_frames.Read(input, isServer, packetsRead, protocolError);
        default:
            return boost::shared_ptr<Packet>();
        }
    }

    virtual void Write(boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer,
                       unsigned int& packetsWritten, bool wide)
    {
        const EState state = State();
        if (state == eState_Undecided)
        {
            m_held.push_back(std::make_pair(packet, wide));
            return;
        }
        if (state == eState_RefusalSent)
            return;
        if (!m_answered && state != eState_Legacy)
        {
            m_answered = true;
            WriteBytes(output, m_answer);
            if (state == eState_Refused)
            {
                m_held.clear();
                SetState(eState_RefusalSent);
                return;
            }
        }
        while (!m_held.empty())
        {
            WriteOne(state, m_held.front().first, output, isServer, packetsWritten, m_held.front().second);
            m_held.pop_front();
        }
        WriteOne(state, packet, output, isServer, packetsWritten, wide);
    }

    virtual void Flush(DataOutputStream* output) { m_frames.Flush(output); }
    virtual unsigned int ChatLimit() { return m_frames.ChatLimit(); }

private:
    enum EState
    {
        eState_Undecided,
        eState_Legacy,
        eState_Reborn,
        eState_Refused,
        eState_RefusalSent
    };

    EState State()
    {
        EnterCriticalSection(&m_lock);
        const EState state = m_state;
        LeaveCriticalSection(&m_lock);
        return state;
    }

    void SetState(EState state)
    {
        EnterCriticalSection(&m_lock);
        m_state = state;
        LeaveCriticalSection(&m_lock);
    }

    // reads the preamble or the first bytes of a Legacy packet; the answer for a Reborn peer is prepared here and
    // sent by the write thread
    bool Decide(DataInputStream* input)
    {
        uint8_t head[kPreambleSize];
        for (size_t index = 0; index < kPreambleSize; ++index)
        {
            const int value = input->read();
            if (value < 0)
                return false;
            head[index] = static_cast<uint8_t>(value);
        }
        if (reborn::Preamble::Sniff(head, kPreambleSize) != reborn::Preamble::eSniff_Reborn)
        {
            if (!m_acceptLegacy)
            {
                LOG_WARN(sLog, "a Legacy client connected, and this server takes only Reborn connections (protocols=reborn)");
                RefuseLegacy();
                return true;
            }
            m_prefix = new PrefixedInputStream(head, kPreambleSize, input);
            m_prefixed = new DataInputStream(m_prefix);
            SetState(eState_Legacy);
            return true;
        }
        uint64_t id = 0;
        std::vector<uint8_t> payload;
        reborn::Hello client;
        if (!ReadOpeningFrame(input, &id, &payload) || id != kFrameHello || payload.empty()
            || !reborn::Hello::Decode(&payload[0], payload.size(), client))
        {
            LOG_WARN(sLog, "a Reborn client sent a hello this server cannot read");
            return false;
        }
        const reborn::Hello server = LocalHello();
        reborn::Agreement agreement;
        std::string refusal;
        const reborn::ERefusal refused = reborn::Negotiate(client, server, agreement, &refusal);
        if (refused != reborn::eRefusal_None)
        {
            LOG_INFO(sLog, "refused a Reborn client: %s", refusal.c_str());
            std::vector<uint8_t> words;
            reborn::Varint::Write(words, ReasonFor(refused));
            words.insert(words.end(), refusal.begin(), refusal.end());
            AppendFrame(m_answer, kFrameRefusal, words);
            SetState(eState_Refused);
            return true;
        }
        std::vector<uint8_t> hello;
        server.Encode(hello);
        AppendFrame(m_answer, kFrameHello, hello);
        m_frames.Agree(agreement.features);
        LOG_INFO(sLog, "a Reborn client, protocol version %u, features %llu", agreement.version,
                 static_cast<unsigned long long>(agreement.features));
        SetState(eState_Reborn);
        return true;
    }

    // a Legacy peer reads the game's own disconnect packet, so it is told why, as the game tells a client of another version
    void RefuseLegacy()
    {
        ByteArrayOutputStream bytes;
        DataOutputStream stream(&bytes);
        unsigned int written = 0;
        m_frames.Legacy().Write(boost::shared_ptr<Packet>(new DisconnectPacket(DisconnectPacket::eDisconnectReason_ClientOutOfDate)),
                       &stream, true, written, false);
        m_answer.assign(bytes.mBuffer.data, bytes.mBuffer.data + bytes.mCount);
        SetState(eState_Refused);
    }

    void WriteOne(EState state, boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer,
                  unsigned int& packetsWritten, bool wide)
    {
        if (state == eState_Reborn)
            m_frames.Write(packet, output, isServer, packetsWritten, wide);
        else
            m_frames.Legacy().Write(packet, output, isServer, packetsWritten, wide);
    }

    PacketFrames m_frames;
    const bool m_acceptLegacy;
    C4JCriticalSection m_lock;
    EState m_state;
    // the rest is the write thread's, apart from m_answer and the prefix, which the read thread fills before it
    // leaves the undecided state
    bool m_answered;
    std::vector<uint8_t> m_answer;
    std::deque<std::pair<boost::shared_ptr<Packet>, bool> > m_held;
    PrefixedInputStream* m_prefix;
    DataInputStream* m_prefixed;
};
}

PacketCodec* CreateRebornClientCodec()
{
    return new RebornClientCodec();
}

PacketCodec* CreateRebornServerCodec(bool acceptLegacy)
{
    return new RebornServerCodec(acceptLegacy);
}
#endif
