#include <stdio.h>
#include <string.h>

#include <deque>
#include <map>
#include <set>
#include <vector>

#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pNode.hpp"
#include "p2p/RendezvousServer.hpp"
#include "p2p_check.hpp"
#include "p2p_sim.hpp"

using namespace p2p;
using namespace p2psim;

namespace
{
bool Online(void* context)
{
    Node** nodes = static_cast<Node**>(context);
    return nodes[0]->Online() && nodes[1]->Online() && nodes[2]->Online();
}

std::vector<PublicKey> Keys(const PublicKey& a)
{
    return std::vector<PublicKey>(1, a);
}

Lobby MakeLobby(unsigned int visibility)
{
    Lobby lobby;
    lobby.visibility = visibility;
    lobby.maxPlayers = 8;
    lobby.players = 1;
    lobby.protocol = Lobby::eProtocol_Legacy;
    lobby.version = 3;
    lobby.port = 25565;
    lobby.name = "world";
    lobby.mode = "survival";
    return lobby;
}

struct Pair
{
    boost::shared_ptr<PeerLink> dialer;
    boost::shared_ptr<PeerLink> host;
    Node* guest;
    Node* owner;
    PublicKey owned;
};

bool Linked(void* context)
{
    Pair* pair = static_cast<Pair*>(context);
    if (!pair->host)
        pair->host = pair->owner->TakeAccepted();
    return pair->dialer && pair->host && pair->dialer->State() == PeerLink::eState_Established
           && pair->host->State() == PeerLink::eState_Established;
}

bool Exchange(SimNet& net, PeerLink& a, PeerLink& b, size_t size, uint32_t seed)
{
    std::vector<uint8_t> toB(size), toA(size);
    for (size_t index = 0; index < size; ++index)
    {
        seed = seed * 1664525u + 1013904223u;
        toB[index] = static_cast<uint8_t>(seed >> 16);
        toA[index] = static_cast<uint8_t>(~(seed >> 16));
    }
    size_t sentA = 0, sentB = 0;
    std::vector<uint8_t> gotA, gotB;
    for (int step = 0; step < 4000 && (gotA.size() < size || gotB.size() < size); ++step)
    {
        if (sentA < size)
            sentA += a.Write(&toB[sentA], size - sentA);
        if (sentB < size)
            sentB += b.Write(&toA[sentB], size - sentB);
        net.Step(10);
        uint8_t buffer[2048];
        for (size_t got = b.Read(buffer, sizeof(buffer)); got > 0; got = b.Read(buffer, sizeof(buffer)))
            gotB.insert(gotB.end(), buffer, buffer + got);
        for (size_t got = a.Read(buffer, sizeof(buffer)); got > 0; got = a.Read(buffer, sizeof(buffer)))
            gotA.insert(gotA.end(), buffer, buffer + got);
    }
    return gotB == toB && gotA == toA;
}
}

int RunNodeTests()
{
    const int before = CheckFailures();
    for (int mode = 0; mode < 2; ++mode)
    {
        Identity serverId, alice, bob, charlie;
        Identity::Generate(serverId);
        Identity::Generate(alice);
        Identity::Generate(bob);
        Identity::Generate(charlie);
        SimNet net(serverId);
        net.blockDirect = mode == 1;
        Policy policy;
        policy.Allow(bob.Public());
        const Endpoint aliceAt(0x0a000001, 4001), bobAt(0x0a000002, 4002), charlieAt(0x0a000003, 4003);
        Node a(alice, serverId.Public(), net.serverAt, &policy), b(bob, serverId.Public(), net.serverAt, NULL),
            c(charlie, serverId.Public(), net.serverAt, NULL);
        net.Add(a, aliceAt);
        net.Add(b, bobAt);
        net.Add(c, charlieAt);
        Node* all[3] = {&a, &b, &c};
        Check(net.Run(Online, all, 5000), "all nodes reach the rendezvous server");
        Check(a.Reflected() == aliceAt || a.Reflected() == Endpoint(), "the reflected address is the node's");
        a.SetFriends(Keys(bob.Public()));
        b.SetFriends(Keys(alice.Public()));
        a.Publish(MakeLobby(Lobby::eVisibility_Friends));
        net.Step(500);
        b.RefreshLobbies();
        net.Step(500);
        Check(b.Lobbies().size() == 1 && b.Lobbies()[0].owner == alice.Public(), "the friend lists the host's lobby");
        Check(a.Reflected() == aliceAt, "the reflected address is the node's address");
        b.LookupCode("AAAA-AAAA-AAAA-AAAA");
        net.Step(500);
        PublicKey stranger;
        Check(!b.TakeFound(stranger) && b.TakeLookupFailure() && !b.TakeLookupFailure(),
              "a friend code nobody has is reported as not found, once");

        Pair pair;
        pair.guest = &b;
        pair.owner = &a;
        b.Dial(alice.Public());
        net.Step(100);
        pair.dialer = b.DialedLink(alice.Public());
        Check(net.Run(Linked, &pair, 15000), "the guest and host establish a link");
        if (pair.dialer && pair.host)
        {
            Check(pair.dialer->Peer() == alice.Public() && pair.host->Peer() == bob.Public(), "each knows who it talks to");
            if (mode == 0)
                Check(!pair.dialer->UsedRelay() && !pair.host->UsedRelay() && !pair.dialer->OnRelay(),
                      "a direct path is used when it works");
            else
                Check(pair.dialer->UsedRelay() && pair.dialer->OnRelay(), "the relay is used when the punch fails");
            Check(Exchange(net, *pair.dialer, *pair.host, mode == 0 ? 60000 : 4000, 7 + mode),
                  mode == 0 ? "60 KB each way arrives intact over a direct path" : "4 KB each way arrives intact over the relay");
        }
    }

    {
        Identity serverId, alice, bob, charlie, mallory;
        Identity::Generate(serverId);
        Identity::Generate(alice);
        Identity::Generate(bob);
        Identity::Generate(charlie);
        Identity::Generate(mallory);
        SimNet net(serverId);
        Policy policy;
        policy.Allow(bob.Public());
        const Endpoint aliceAt(0x0a000001, 4001), bobAt(0x0a000002, 4002), charlieAt(0x0a000003, 4003);
        Node a(alice, serverId.Public(), net.serverAt, &policy), b(bob, serverId.Public(), net.serverAt, NULL),
            c(charlie, serverId.Public(), net.serverAt, NULL);
        net.Add(a, aliceAt);
        net.Add(b, bobAt);
        net.Add(c, charlieAt);
        Node* all[3] = {&a, &b, &c};
        net.Run(Online, all, 5000);
        a.SetFriends(Keys(bob.Public()));
        b.SetFriends(Keys(alice.Public()));
        c.SetFriends(std::vector<PublicKey>());
        std::vector<PublicKey> both;
        both.push_back(bob.Public());
        a.SetFriends(both);
        b.SetFriends(Keys(charlie.Public()));
        a.Publish(MakeLobby(Lobby::eVisibility_FriendsOfFriends));
        net.Step(1000);
        c.Dial(alice.Public());
        net.Step(100);
        boost::shared_ptr<PeerLink> link = c.DialedLink(alice.Public());
        Check(link.get() != NULL, "the server lets a friend of a friend dial");
        net.Step(8000);
        Check(link && (link->State() == PeerLink::eState_Closed || link->State() == PeerLink::eState_Failed),
              "the host closes a peer its own policy does not admit");

        while (a.TakeAccepted())
        {
        }
        class Capture : public PeerLink::Output
        {
        public:
            virtual void Direct(const Endpoint&, const std::vector<uint8_t>& datagram) { sent.push_back(datagram); }
            virtual void Relay(const PublicKey&, const std::vector<uint8_t>&) {}
            std::vector<std::vector<uint8_t> > sent;
        } capture;
        PeerLink intruder(mallory, true, 99, alice.Public(), aliceAt, &capture, net.now);
        intruder.Update(net.now + 500);
        for (size_t index = 0; index < capture.sent.size(); ++index)
            a.OnDatagram(Endpoint(0x0c000001, 6000), capture.sent[index].data(), capture.sent[index].size(), net.now);
        Check(!a.TakeAccepted(), "a handshake from a peer nobody punched creates no link");
    }
    return CheckFailures() - before;
}
