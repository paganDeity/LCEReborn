#include "game/network/PacketCodec.hpp"

#include "game/core/DataInputStream.hpp"
#include "game/core/DataOutputStream.hpp"
#include <string.h>

#include <string>

#include "game/core/ByteArrayInputStream.hpp"
#include "game/core/ByteArrayOutputStream.hpp"
#include "game/core/ModEvents_generated.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModHost.hpp"
#include "game/network/Packet.hpp"
#include "game/network/RebornCodec.hpp"
#include "game/network/ServerboundChatPacket.hpp"
#if LCE_PLATFORM_PC
#include "platform/pc/Log.hpp"
#endif


unsigned int PacketCodec::ChatLimit()
{
    return ServerboundChatPacket::kMaxLength;
}

namespace
{
// a packet larger than this is treated as a broken stream, not allocated
const int kMaxFrame = 16 * 1024 * 1024;

// every scripted frame starts with this marker and then a fingerprint of the mods that listen to packet_encode, so
// a peer without the packet mod, or with another one, is named at its first frame instead of its bytes being read
// as a length
const int kFrameMagic = 0x4C43454D;

int Fingerprint()
{
    const std::string owners = ModHost::Get().HookOwners(kModHook_packet_encode);
    unsigned int hash = 2166136261u;
    for (size_t index = 0; index < owners.size(); ++index)
        hash = (hash ^ static_cast<unsigned char>(owners[index])) * 16777619u;
    return static_cast<int>(hash);
}

#if LCE_PLATFORM_PC
const Logger sLog("Network");
#endif

void ReportForeignFrame(bool otherMod)
{
#if LCE_PLATFORM_PC
    sLog.WriteText(LogLevel::Warn, otherMod ? "the peer runs a different packet mod; both ends must run the same one"
                                            : "the peer's first bytes are not a packet mod frame; both ends must run the same packet mod");
#else
    (void)otherMod;
#endif
}

// The game's own packet bytes, put through a mod on the way out and back in. The frame is a marker, a length and the
// mod's bytes, so the mod can change their size and meaning freely.
class ScriptedPacketCodec : public PacketCodec
{
public:
    explicit ScriptedPacketCodec(bool local) : fingerprint(0), local(local) {}

    // read once, when the connection that owns this codec is made, so a frame does not take the mod host's lock
    void Refresh() { fingerprint = Fingerprint(); }

    LegacyPacketCodec legacy;
    int fingerprint;
    // the connection is the host's own player, in the same process: a mod decides per connection what to do with its bytes
    bool local;

    virtual boost::shared_ptr<Packet> Read(DataInputStream* input, bool isServer, unsigned int& packetsRead,
                                           bool& protocolError)
    {
        if (input->readInt() != kFrameMagic)
        {
            ReportForeignFrame(false);
            protocolError = true;
            return boost::shared_ptr<Packet>();
        }
        if (input->readInt() != fingerprint)
        {
            ReportForeignFrame(true);
            protocolError = true;
            return boost::shared_ptr<Packet>();
        }
        const int length = input->readInt();
        if (length < 0 || length > kMaxFrame)
        {
            protocolError = true;
            return boost::shared_ptr<Packet>();
        }
        arrayWithLength<unsigned char> frame(length, true);
        if (length > 0 && !input->readFully(frame))
        {
            delete[] frame.data;
            protocolError = true;
            return boost::shared_ptr<Packet>();
        }
        std::string data(reinterpret_cast<const char*>(frame.data), static_cast<size_t>(length));
        delete[] frame.data;
        ModEvents::PacketDecode(isServer, local, data);
        arrayWithLength<unsigned char> plain(static_cast<unsigned int>(data.size()), true);
        if (!data.empty())
            memcpy(plain.data, data.data(), data.size());
        ByteArrayInputStream bytes(plain);
        DataInputStream stream(&bytes);
        return legacy.Read(&stream, isServer, packetsRead, protocolError);
    }

    virtual void Write(boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer,
                       unsigned int& packetsWritten, bool wide)
    {
        ByteArrayOutputStream bytes;
        DataOutputStream stream(&bytes);
        legacy.Write(packet, &stream, isServer, packetsWritten, wide);
        std::string data(reinterpret_cast<const char*>(bytes.mBuffer.data), static_cast<size_t>(bytes.mCount));
        ModEvents::PacketEncode(isServer, local, packet->getPacketId(), data);
        output->writeInt(kFrameMagic);
        output->writeInt(fingerprint);
        output->writeInt(static_cast<int>(data.size()));
        for (size_t index = 0; index < data.size(); ++index)
            output->writeByte(static_cast<unsigned char>(data[index]));
    }
};
}

PacketCodec* CreateCodec(bool local, bool isServer)
{
    if (ModHooks::IsActive(kModHook_packet_encode))
    {
        ScriptedPacketCodec* scripted = new ScriptedPacketCodec(local);
        scripted->Refresh();
        return scripted;
    }
#if LCE_PLATFORM_PC
    if (!local && isServer && (ConnectionProtocols::ServerAccepts() & ConnectionProtocols::eProtocol_Reborn) != 0)
        return CreateRebornServerCodec((ConnectionProtocols::ServerAccepts() & ConnectionProtocols::eProtocol_Legacy) != 0);
    if (!local && !isServer && ConnectionProtocols::ClientSpeaks() == ConnectionProtocols::eProtocol_Reborn)
        return CreateRebornClientCodec();
#else
    (void)isServer;
#endif
    return new LegacyPacketCodec();
}

namespace
{
int sServerAccepts = ConnectionProtocols::eProtocol_Legacy;
int sClientSpeaks = ConnectionProtocols::eProtocol_Legacy;
}

void ConnectionProtocols::SetServerAccepts(int protocols)
{
    sServerAccepts = protocols;
}

void ConnectionProtocols::SetClientSpeaks(int protocol)
{
    sClientSpeaks = protocol;
}

int ConnectionProtocols::ServerAccepts()
{
    return sServerAccepts;
}

int ConnectionProtocols::ClientSpeaks()
{
    return sClientSpeaks;
}

bool ConnectionProtocols::Parse(const std::string& text, int* protocols)
{
    int found = 0;
    size_t start = 0;
    while (start <= text.size())
    {
        size_t end = text.find(',', start);
        if (end == std::string::npos)
            end = text.size();
        std::string word = text.substr(start, end - start);
        while (!word.empty() && word[0] == ' ')
            word.erase(0, 1);
        while (!word.empty() && word[word.size() - 1] == ' ')
            word.erase(word.size() - 1);
        if (word == "legacy")
            found |= eProtocol_Legacy;
        else if (word == "reborn")
            found |= eProtocol_Reborn;
        else
            return false;
        start = end + 1;
    }
    *protocols = found;
    return true;
}

boost::shared_ptr<Packet> LegacyPacketCodec::Read(DataInputStream* input, bool isServer, unsigned int& packetsRead,
                                                  bool& protocolError)
{
    return Packet::readPacket(input, isServer, packetsRead, protocolError);
}

void LegacyPacketCodec::Write(boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer,
                              unsigned int& packetsWritten, bool wide)
{
    Packet::writePacket(packet, output, isServer, packetsWritten, wide);
}
