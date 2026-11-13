#include <stdio.h>
#include <string.h>

#include <vector>

#include "net/NetFrameCodec.hpp"
#include "net/NetSocket.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pNode.hpp"
#include "p2p_sim.hpp"

using namespace p2p;
using namespace p2psim;

namespace
{
int sFailures = 0;

void Check(bool condition, const char* what)
{
    if (!condition)
    {
        ++sFailures;
        printf("FAIL %s\n", what);
    }
}

std::vector<unsigned char> Script()
{
    std::vector<unsigned char> bytes;
    uint32_t seed = 12345;
    const size_t sizes[] = {0, 1, 5, 44, 300, 1000, 4096, 70000, 250000, 2, 900, 901, 899, 128000};
    for (size_t index = 0; index < sizeof(sizes) / sizeof(sizes[0]); ++index)
    {
        std::vector<unsigned char> payload(sizes[index]);
        for (size_t byte = 0; byte < payload.size(); ++byte)
        {
            seed = seed * 1664525u + 1013904223u;
            payload[byte] = static_cast<unsigned char>(seed >> 16);
        }
        std::vector<unsigned char> frame;
        lcenet::FrameCodec::Encode(static_cast<unsigned char>(index % 3 == 0 ? 0xfe : index), static_cast<unsigned char>(index & 3),
                                   payload.empty() ? 0 : &payload[0], payload.size(), frame);
        bytes.insert(bytes.end(), frame.begin(), frame.end());
    }
    return bytes;
}

bool OverTcp(const std::vector<unsigned char>& script, std::vector<unsigned char>& received)
{
    lcenet::Startup();
    lcenet::TcpSocket listener, client, server;
    std::string error;
    if (!listener.Listen(static_cast<unsigned short>(27131), error))
    {
        printf("%s\n", error.c_str());
        return false;
    }
    class Never : public lcenet::CancelFlag
    {
    public:
        virtual bool Cancelled() const { return false; }
    } never;
    lcenet::Address target;
    target.ip = 0x7f000001u;
    target.port = 27131;
    if (!client.Connect(target, 3000, never, error))
        return false;
    lcenet::Address peer;
    if (!listener.Accept(3000, server, peer))
        return false;
    size_t offset = 0;
    received.clear();
    unsigned char buffer[4096];
    while (received.size() < script.size())
    {
        if (offset < script.size())
        {
            const size_t chunk = std::min<size_t>(3000, script.size() - offset);
            if (!client.SendAll(&script[offset], chunk))
                return false;
            offset += chunk;
        }
        const int got = server.Receive(buffer, sizeof(buffer), 50);
        if (got < 0)
            return false;
        received.insert(received.end(), buffer, buffer + got);
    }
    return true;
}

bool OverPeerToPeer(const std::vector<unsigned char>& script, std::vector<unsigned char>& received)
{
    Identity serverId, alice, bob;
    Identity::Generate(serverId);
    Identity::Generate(alice);
    Identity::Generate(bob);
    SimNet net(serverId);
    Policy policy;
    policy.Allow(bob.Public());
    const Endpoint aliceAt(0x0a000001, 4001), bobAt(0x0a000002, 4002);
    Node a(alice, serverId.Public(), net.serverAt, &policy), b(bob, serverId.Public(), net.serverAt, NULL);
    net.Add(a, aliceAt);
    net.Add(b, bobAt);
    net.Step(1500);
    a.SetFriends(std::vector<PublicKey>(1, bob.Public()));
    b.SetFriends(std::vector<PublicKey>(1, alice.Public()));
    Lobby lobby;
    lobby.maxPlayers = 8;
    lobby.name = "w";
    lobby.mode = "g";
    lobby.version = 3;
    a.Publish(lobby);
    net.Step(1000);
    b.Dial(alice.Public());
    net.Step(200);
    boost::shared_ptr<PeerLink> dialer = b.DialedLink(alice.Public());
    boost::shared_ptr<PeerLink> host;
    for (int step = 0; step < 300 && !(dialer && host && dialer->State() == PeerLink::eState_Established
                                       && host->State() == PeerLink::eState_Established);
         ++step)
    {
        net.Step(100);
        if (!host)
            host = a.TakeAccepted();
    }
    if (!dialer || !host)
        return false;
    size_t offset = 0;
    received.clear();
    unsigned char buffer[4096];
    for (int step = 0; step < 20000 && received.size() < script.size(); ++step)
    {
        if (offset < script.size())
            offset += dialer->Write(&script[offset], std::min<size_t>(3000, script.size() - offset));
        net.Step(10);
        for (size_t got = host->Read(buffer, sizeof(buffer)); got > 0; got = host->Read(buffer, sizeof(buffer)))
            received.insert(received.end(), buffer, buffer + got);
    }
    return received.size() == script.size();
}
}

int main()
{
    const std::vector<unsigned char> script = Script();
    std::vector<unsigned char> tcp, p2p;
    Check(OverTcp(script, tcp), "the scripted frames cross a loopback TCP connection");
    Check(OverPeerToPeer(script, p2p), "the scripted frames cross a peer-to-peer link");
    Check(tcp == script, "the TCP byte stream is the scripted one");
    Check(p2p == script, "the peer-to-peer byte stream is the scripted one");
    Check(tcp == p2p, "both transports carry the identical byte stream");
    printf("%zu bytes in %zu frames: %s\n", script.size(), static_cast<size_t>(14), sFailures == 0 ? "ok" : "failed");
    return sFailures == 0 ? 0 : 1;
}
