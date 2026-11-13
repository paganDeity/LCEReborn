#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

#include "p2p/P2pHandshake.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pLobby.hpp"
#include "p2p/P2pPeerLink.hpp"
#include "p2p/P2pReliableStream.hpp"
#include "p2p/RendezvousMessages.hpp"
#include "p2p/RendezvousProtocol.hpp"
#include "p2p_check.hpp"

using namespace p2p;
using namespace p2p::rendezvous;

namespace
{
bool HostIsLittle()
{
    const uint16_t probe = 1;
    uint8_t bytes[2];
    memcpy(bytes, &probe, 2);
    return bytes[0] == 1;
}

uint32_t Swap32(uint32_t v)
{
    return (v >> 24) | ((v >> 8) & 0xff00u) | ((v << 8) & 0xff0000u) | (v << 24);
}

class NativeEncoder
{
public:
    explicit NativeEncoder(bool swappedHost) : m_swapped(swappedHost) {}

    void Put(unsigned int width, uint32_t value)
    {
        const uint32_t held = m_swapped ? Swap32(value) : value;
        uint8_t memory[4];
        memcpy(memory, &held, sizeof(memory));
        const bool little = HostIsLittle();
        for (int byte = static_cast<int>(width) - 1; byte >= 0; --byte)
        {
            const int logical = m_swapped ? 3 - byte : byte;
            m_data.push_back(memory[little ? logical : 3 - logical]);
        }
    }

    void Raw(const uint8_t* data, size_t size) { m_data.insert(m_data.end(), data, data + size); }
    void Key(const PublicKey& key) { Raw(key.bytes, PublicKey::kSize); }
    const std::vector<uint8_t>& Data() const { return m_data; }

private:
    bool m_swapped;
    std::vector<uint8_t> m_data;
};

PublicKey KeyOf(uint8_t seed)
{
    PublicKey key;
    for (size_t index = 0; index < PublicKey::kSize; ++index)
        key.bytes[index] = static_cast<uint8_t>(seed + index * 7);
    return key;
}

template <typename Build>
void Compare(const char* name, const std::vector<uint8_t>& actual, Build build)
{
    NativeEncoder plain(false), swapped(true);
    build(plain);
    build(swapped);
    std::string label = std::string(name) + " matches the reference encoder";
    Check(actual == plain.Data(), label.c_str());
    label = std::string(name) + " matches the byte-swapped encoder";
    Check(actual == swapped.Data(), label.c_str());
}

class Capture : public PeerLink::Output
{
public:
    virtual void Direct(const Endpoint&, const std::vector<uint8_t>& datagram) { sent.push_back(datagram); }
    virtual void Relay(const PublicKey&, const std::vector<uint8_t>& datagram) { sent.push_back(datagram); }
    std::vector<std::vector<uint8_t> > sent;
};
}

int RunWireTests()
{
    const int before = CheckFailures();
    const PublicKey k1 = KeyOf(1), k2 = KeyOf(90);
    const uint8_t data[5] = {1, 2, 3, 0xfe, 0xff};
    const Endpoint at(0x0a0b0c0d, 0xabcd);
    std::vector<PublicKey> keys;
    keys.push_back(k1);
    keys.push_back(k2);

    struct Reg
    {
        static void Build(NativeEncoder& e, const std::vector<PublicKey>& k)
        {
            e.Put(1, eRequest_Register);
            e.Put(2, static_cast<uint32_t>(k.size()));
            for (size_t i = 0; i < k.size(); ++i)
                e.Key(k[i]);
        }
    };
    {
        NativeEncoder a(false), b(true);
        Reg::Build(a, keys);
        Reg::Build(b, keys);
        Check(Messages::Register(keys) == a.Data() && a.Data() == b.Data(), "Register matches both encoders");
    }

    const std::vector<uint8_t> record(7, 0x5a);
#define BUILD(...) [&](NativeEncoder& e) { __VA_ARGS__ }
    Compare("Publish", Messages::Publish(record), BUILD(e.Put(1, eRequest_Publish); e.Raw(&record[0], record.size());));
    Compare("Unpublish", Messages::Unpublish(), BUILD(e.Put(1, eRequest_Unpublish);));
    Compare("List", Messages::List(0x0102), BUILD(e.Put(1, eRequest_List); e.Put(2, 0x0102);));
    Compare("Connect", Messages::Connect(k1), BUILD(e.Put(1, eRequest_Connect); e.Key(k1);));
    Compare("Relay", Messages::Relay(k2, data, sizeof(data)),
            BUILD(e.Put(1, eRequest_Relay); e.Key(k2); e.Put(2, sizeof(data)); e.Raw(data, sizeof(data));));
    Compare("Lookup", Messages::Lookup("ABCD-EFGH"),
            BUILD(e.Put(1, eRequest_Lookup); e.Put(1, 9); e.Raw(reinterpret_cast<const uint8_t*>("ABCD-EFGH"), 9);));
    Compare("Reflect", Messages::Reflect(), BUILD(e.Put(1, eRequest_Reflect);));
    Compare("Invite", Messages::Invite(k1), BUILD(e.Put(1, eRequest_Invite); e.Key(k1);));
    Compare("Answer", Messages::Answer(eReply_Denied, eRequest_Connect), BUILD(e.Put(1, eReply_Denied); e.Put(1, eRequest_Connect);));
    {
        std::vector<std::vector<uint8_t> > records(2, record);
        Compare("Lobbies", Messages::Lobbies(records, true),
                BUILD(e.Put(1, eReply_Lobbies); e.Put(1, 2); e.Put(1, 1); for (int i = 0; i < 2; ++i)
                      {
                          e.Put(2, static_cast<uint32_t>(record.size()));
                          e.Raw(&record[0], record.size());
                      }));
    }
    Compare("Punch", Messages::Punch(k1, at), BUILD(e.Put(1, eReply_Punch); e.Key(k1); e.Put(4, at.ip); e.Put(2, at.port);));
    Compare("Found", Messages::Found(k2), BUILD(e.Put(1, eReply_Found); e.Key(k2);));
    Compare("Reflected", Messages::Reflected(at), BUILD(e.Put(1, eReply_Reflected); e.Put(4, at.ip); e.Put(2, at.port);));
    Compare("Invited", Messages::Invited(record), BUILD(e.Put(1, eReply_Invited); e.Raw(&record[0], record.size());));
    Compare("Relayed", Messages::Relayed(k1, data, sizeof(data)),
            BUILD(e.Put(1, eReply_Relayed); e.Key(k1); e.Raw(data, sizeof(data));));

    {
        Identity owner;
        Identity::Generate(owner);
        Lobby lobby;
        lobby.visibility = Lobby::eVisibility_FriendsOfFriends;
        lobby.maxPlayers = 12;
        lobby.players = 3;
        lobby.protocol = Lobby::eProtocol_Legacy;
        lobby.version = 0x0203;
        lobby.port = 0x6345;
        lobby.name = "Name";
        lobby.mode = "Mode";
        const std::vector<uint8_t> signedRecord = Lobby::Sign(owner, lobby);
        const std::vector<uint8_t> body(signedRecord.begin(), signedRecord.end() - Identity::kSignatureSize);
        Compare("Lobby record", body,
                BUILD(e.Key(owner.Public()); e.Put(1, Lobby::eVisibility_FriendsOfFriends); e.Put(1, 12); e.Put(1, 3);
                      e.Put(1, Lobby::eProtocol_Legacy); e.Put(2, 0x0203); e.Put(2, 0x6345); e.Put(1, 4);
                      e.Raw(reinterpret_cast<const uint8_t*>("Name"), 4); e.Put(1, 4);
                      e.Raw(reinterpret_cast<const uint8_t*>("Mode"), 4);));
        Lobby read;
        size_t used = 0;
        Check(Lobby::Read(&signedRecord[0], signedRecord.size(), read, &used) && used == signedRecord.size()
                  && read.owner == owner.Public() && read.visibility == lobby.visibility
                  && read.maxPlayers == 12 && read.players == 3 && read.protocol == lobby.protocol
                  && read.version == 0x0203 && read.port == 0x6345 && read.name == "Name" && read.mode == "Mode",
              "a lobby record decodes to the fields that were encoded");
    }

    {
        ReliableStream stream;
        const uint8_t payload[3] = {9, 8, 7};
        stream.Write(payload, sizeof(payload));
        std::vector<std::vector<uint8_t> > out;
        stream.Poll(0, out);
        Check(out.size() == 1, "a write produces one data packet");
        if (out.size() == 1)
            Compare("Stream data packet", out[0],
                    BUILD(e.Put(1, 1); e.Put(1, 0); e.Put(4, 0); e.Put(4, 0); e.Raw(payload, sizeof(payload));));
        ReliableStream receiver;
        receiver.OnPacket(&out[0][0], out[0].size(), 0);
        std::vector<std::vector<uint8_t> > acks;
        receiver.Poll(0, acks);
        Check(acks.size() == 1, "a received packet is acknowledged");
        if (acks.size() == 1)
            Compare("Stream ack packet", acks[0], BUILD(e.Put(1, 2); e.Put(4, 1); e.Put(4, 0);));
    }

    {
        Identity a, b;
        Identity::Generate(a);
        Identity::Generate(b);
        Capture capture;
        PeerLink link(a, true, 0x0102030405060708ull, b.Public(), Endpoint(1, 2), &capture, 0);
        link.Update(1000);
        Check(!capture.sent.empty(), "a dialing link sends its first handshake");
        if (!capture.sent.empty())
        {
            const std::vector<uint8_t>& first = capture.sent[0];
            Check(first.size() == PeerLink::kHeaderSize + kFirstMessageSize, "the first datagram is padded to its size");
            Compare("Peer datagram header", std::vector<uint8_t>(first.begin(), first.begin() + PeerLink::kHeaderSize),
                    BUILD(e.Put(1, PeerLink::eDatagram_Handshake); e.Put(4, 0x01020304); e.Put(4, 0x05060708);));
            Check(first[PeerLink::kHeaderSize] == 1, "the handshake carries its version byte");
        }
    }

    {
        Identity a, b;
        Identity::Generate(a);
        Identity::Generate(b);
        Handshake initiator(a, true), responder(b, false);
        std::vector<uint8_t> second, third, none;
        const std::vector<uint8_t> first = initiator.Start();
        responder.Receive(first.data(), first.size(), second);
        initiator.Receive(second.data(), second.size(), third);
        responder.Receive(third.data(), third.size(), none);
        Check(first.size() == 192 && second.size() == 145 && third.size() == 112,
              "the three handshake messages have their fixed sizes");
        std::vector<uint8_t> sealed1, sealed2;
        initiator.GetSession().Seal(data, sizeof(data), sealed1);
        initiator.GetSession().Seal(data, sizeof(data), sealed2);
        const uint8_t counter0[8] = {0, 0, 0, 0, 0, 0, 0, 0}, counter1[8] = {1, 0, 0, 0, 0, 0, 0, 0};
        Check(sealed1.size() == kSessionOverhead + sizeof(data) && memcmp(&sealed1[0], counter0, 8) == 0
                  && memcmp(&sealed2[0], counter1, 8) == 0,
              "a sealed datagram starts with its counter, least significant byte first");
    }
#undef BUILD
    return CheckFailures() - before;
}
