#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

#include "net/NetSocket.hpp"
#include "p2p/P2pFriendCode.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/RendezvousClient.hpp"

using namespace p2p;
using namespace p2p::rendezvous;

namespace
{
bool Pump(lcenet::UdpSocket& socket, const lcenet::Address& server, Client& client, int milliseconds)
{
    unsigned char buffer[2048];
    lcenet::Address from;
    const int received = socket.ReceiveFrom(buffer, sizeof(buffer), from, milliseconds);
    if (received <= 0)
        return false;
    std::vector<uint8_t> reply;
    client.Receive(buffer, static_cast<size_t>(received), reply);
    if (!reply.empty())
        socket.SendTo(server, reply.data(), reply.size());
    return true;
}

bool WaitFor(lcenet::UdpSocket& socket, const lcenet::Address& server, Client& client, unsigned int kind, Client::Event& event)
{
    for (int attempt = 0; attempt < 30; ++attempt)
    {
        while (client.NextEvent(event))
            if (event.kind == kind)
                return true;
        Pump(socket, server, client, 100);
    }
    return false;
}
}

int main(int argc, char** argv)
{
    if (argc != 3)
        return 2;
    PublicKey serverKey;
    for (size_t index = 0; index < PublicKey::kSize; ++index)
    {
        unsigned int byte = 0;
        sscanf(argv[2] + index * 2, "%2x", &byte);
        serverKey.bytes[index] = static_cast<uint8_t>(byte);
    }
    lcenet::Startup();
    lcenet::Address server;
    std::string error;
    if (!lcenet::Resolve(argv[1], server, error))
    {
        printf("%s\n", error.c_str());
        return 1;
    }
    Identity alice, bob;
    Identity::Generate(alice);
    Identity::Generate(bob);
    Client a(alice, serverKey), b(bob, serverKey);
    lcenet::UdpSocket sa, sb;
    if (!sa.Open(0, error) || !sb.Open(0, error))
        return 1;
    const std::vector<uint8_t> helloA = a.Hello();
    const std::vector<uint8_t> helloB = b.Hello();
    sa.SendTo(server, helloA.data(), helloA.size());
    sb.SendTo(server, helloB.data(), helloB.size());
    for (int attempt = 0; attempt < 30 && !(a.Connected() && b.Connected()); ++attempt)
    {
        Pump(sa, server, a, 50);
        Pump(sb, server, b, 50);
    }
    if (!a.Connected() || !b.Connected())
    {
        printf("FAIL handshake over udp\n");
        return 1;
    }
    Client::Event event;
    const std::vector<uint8_t> registerA = a.Register(std::vector<PublicKey>(1, bob.Public()));
    sa.SendTo(server, registerA.data(), registerA.size());
    if (!WaitFor(sa, server, a, eReply_Ok, event))
    {
        printf("FAIL register\n");
        return 1;
    }
    const std::vector<uint8_t> connect = b.Connect(alice.Public());
    sb.SendTo(server, connect.data(), connect.size());
    Client::Event toB, toA;
    const bool gotB = WaitFor(sb, server, b, eReply_Punch, toB);
    const bool gotA = WaitFor(sa, server, a, eReply_Punch, toA);
    if (!gotA || !gotB)
    {
        printf("FAIL punch\n");
        return 1;
    }
    printf("ok punch: bob sees alice at %s, alice sees bob at %s\n", [&] {
        lcenet::Address x;
        x.ip = toB.address.ip;
        x.port = toB.address.port;
        static std::string text;
        text = x.Text();
        return text.c_str();
    }(), [&] {
        lcenet::Address x;
        x.ip = toA.address.ip;
        x.port = toA.address.port;
        static std::string text2;
        text2 = x.Text();
        return text2.c_str();
    }());
    return 0;
}
