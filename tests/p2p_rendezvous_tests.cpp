#include <stdio.h>
#include <string.h>

#include <map>
#include <vector>

#include "p2p/P2pFriendCode.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/RendezvousClient.hpp"
#include "p2p/RendezvousServer.hpp"
#include "p2p_check.hpp"

using namespace p2p;
using namespace p2p::rendezvous;

namespace
{
class Net
{
public:
    explicit Net(const Identity& serverIdentity) : server(serverIdentity, Server::Limits()), now(1000) {}

    void Attach(Client& client, const Endpoint& endpoint) { clients[endpoint] = &client; }

    void Send(const Endpoint& from, const std::vector<uint8_t>& datagram)
    {
        if (datagram.empty())
            return;
        std::vector<Server::Outgoing> out;
        server.Receive(from, datagram.data(), datagram.size(), now, out);
        for (size_t index = 0; index < out.size(); ++index)
        {
            std::map<Endpoint, Client*>::iterator target = clients.find(out[index].to);
            if (target == clients.end())
                continue;
            std::vector<uint8_t> reply;
            target->second->Receive(out[index].data.data(), out[index].data.size(), reply);
            Send(out[index].to, reply);
        }
    }

    bool Join(Client& client, const Endpoint& endpoint)
    {
        Attach(client, endpoint);
        Send(endpoint, client.Hello());
        return client.Connected();
    }

    Server server;
    std::map<Endpoint, Client*> clients;
    int64_t now;
};

size_t Drain(Client& client)
{
    Client::Event event;
    size_t count = 0;
    while (client.NextEvent(event))
        ++count;
    return count;
}

Lobby MakeLobby(unsigned int visibility, const char* name)
{
    Lobby lobby;
    lobby.visibility = visibility;
    lobby.maxPlayers = 8;
    lobby.players = 1;
    lobby.protocol = Lobby::eProtocol_Legacy;
    lobby.version = 3;
    lobby.port = 25565;
    lobby.name = name;
    lobby.mode = "survival";
    return lobby;
}

std::vector<PublicKey> Keys(const PublicKey& a)
{
    return std::vector<PublicKey>(1, a);
}
}

int RunRendezvousTests()
{
    const int before = CheckFailures();
    Identity serverId, alice, bob, charlie, dave;
    Identity::Generate(serverId);
    Identity::Generate(alice);
    Identity::Generate(bob);
    Identity::Generate(charlie);
    Identity::Generate(dave);
    const Endpoint aliceAt(0x0a000001, 1000), bobAt(0x0a000002, 2000), charlieAt(0x0a000003, 3000),
        daveAt(0x0a000004, 4000);

    Net net(serverId);
    Client a(alice, serverId.Public()), b(bob, serverId.Public()), c(charlie, serverId.Public()),
        d(dave, serverId.Public());
    Check(net.Join(a, aliceAt) && net.Join(b, bobAt) && net.Join(c, charlieAt) && net.Join(d, daveAt),
          "clients connect");

    {
        Identity other;
        Identity::Generate(other);
        Client wrong(alice, other.Public());
        Net lone(serverId);
        Check(!lone.Join(wrong, Endpoint(0x0a000009, 9)), "a client refuses a server with another key");
    }

    Client::Event e;
    net.Send(bobAt, b.Connect(alice.Public()));
    Check(b.NextEvent(e) && e.kind == eReply_Denied && e.request == eRequest_Connect, "an unregistered peer is denied");
    net.Send(aliceAt, a.Register(Keys(bob.Public())));
    Check(a.NextEvent(e) && e.kind == eReply_Ok, "register is accepted");

    net.Send(bobAt, b.Connect(alice.Public()));
    Check(b.NextEvent(e) && e.kind == eReply_Punch && e.peer == alice.Public() && e.address == aliceAt,
          "an allowed friend learns the address");
    Check(a.NextEvent(e) && e.kind == eReply_Punch && e.peer == bob.Public() && e.address == bobAt,
          "the target learns the requester's address at the same moment");

    net.Send(charlieAt, c.Connect(alice.Public()));
    Client::Event notAllowed, unknown;
    Check(c.NextEvent(notAllowed) && notAllowed.kind == eReply_Denied, "a peer that is not allowed is denied");
    net.Send(charlieAt, c.Connect(dave.Public()));
    Check(c.NextEvent(unknown) && unknown.kind == notAllowed.kind && unknown.request == notAllowed.request,
          "an unknown peer is denied the same way, so presence is not revealed");

    {
        const uint8_t hello[] = "handshake bytes";
        net.Send(charlieAt, c.Relay(alice.Public(), hello, sizeof(hello)));
        Check(c.NextEvent(e) && e.kind == eReply_Denied, "relay needs an authorised pair");
        net.Send(bobAt, b.Relay(alice.Public(), hello, sizeof(hello)));
        Check(a.NextEvent(e) && e.kind == eReply_Relayed && e.peer == bob.Public() && e.data.size() == sizeof(hello)
                  && memcmp(e.data.data(), hello, sizeof(hello)) == 0,
              "an authorised pair relays");
        net.Send(bobAt, b.Relay(alice.Public(), hello, 0));
        Check(b.NextEvent(e) && e.kind == eReply_Denied, "an empty relay is refused");
        std::vector<uint8_t> big(1000, 7);
        size_t delivered = 0;
        for (int index = 0; index < 200; ++index)
        {
            net.now += 5;
            net.Send(bobAt, b.Relay(alice.Public(), &big[0], big.size()));
        }
        while (a.NextEvent(e))
            if (e.kind == eReply_Relayed)
                ++delivered;
        Check(delivered > 5 && delivered < 100, "relay is limited per identity");
        Drain(b);
        net.now += 10000;
    }

    net.Send(aliceAt, a.Publish(MakeLobby(Lobby::eVisibility_Friends, "Alice's world")));
    Check(a.NextEvent(e) && e.kind == eReply_Ok, "a lobby is published");
    net.Send(bobAt, b.List(0));
    Check(b.NextEvent(e) && e.kind == eReply_Lobbies && e.lobbies.size() == 1 && e.lobbies[0].owner == alice.Public()
              && e.lobbies[0].name == "Alice's world" && e.lobbies[0].protocol == Lobby::eProtocol_Legacy
              && e.lobbies[0].version == 3,
          "a friend sees a friends lobby with its protocol and version");
    net.Send(charlieAt, c.List(0));
    Check(c.NextEvent(e) && e.kind == eReply_Lobbies && e.lobbies.empty(), "a stranger does not see it");
    net.Send(charlieAt, c.Connect(alice.Public()));
    Check(c.NextEvent(e) && e.kind == eReply_Denied, "a stranger gets no address for a friends lobby");

    {
        Lobby lobby = MakeLobby(Lobby::eVisibility_Friends, "x");
        Check(lobby.Compatible(Lobby::eProtocol_Legacy, 3), "the same protocol and version is compatible");
        Check(!lobby.Compatible(Lobby::eProtocol_Legacy, 4), "another version is not");
        Check(!lobby.Compatible(2, 3), "another protocol is not");
    }

    net.Send(bobAt, b.Register(Keys(charlie.Public())));
    Drain(b);
    net.Send(aliceAt, a.Publish(MakeLobby(Lobby::eVisibility_FriendsOfFriends, "Friends of friends")));
    Drain(a);
    net.Send(charlieAt, c.List(0));
    Check(c.NextEvent(e) && e.lobbies.size() == 1 && e.lobbies[0].name == "Friends of friends",
          "a friend of a friend sees the lobby");
    net.Send(charlieAt, c.Connect(alice.Public()));
    Check(c.NextEvent(e) && e.kind == eReply_Punch, "and may connect to it");
    Drain(a);
    net.Send(daveAt, d.List(0));
    Check(d.NextEvent(e) && e.lobbies.empty(), "a stranger still does not");

    net.Send(daveAt, d.Publish(MakeLobby(Lobby::eVisibility_Invite, "Invite only")));
    Drain(d);
    net.Send(charlieAt, c.List(0));
    Check(c.NextEvent(e) && e.lobbies.size() == 1, "an invite-only lobby is not listed to others");
    net.Send(charlieAt, c.Connect(dave.Public()));
    Check(c.NextEvent(e) && e.kind == eReply_Denied, "or given an address");
    net.Send(daveAt, d.Invite(charlie.Public()));
    Check(c.NextEvent(e) && e.kind == eReply_Invited && e.invitation.owner == dave.Public()
              && e.invitation.name == "Invite only",
          "an invitation is pushed to the invitee");
    Drain(d);
    net.Send(charlieAt, c.List(0));
    Check(c.NextEvent(e) && e.lobbies.size() == 2, "an invited peer sees the lobby");
    net.Send(charlieAt, c.Connect(dave.Public()));
    Check(c.NextEvent(e) && e.kind == eReply_Punch, "and may connect");
    Drain(d);

    {
        Lobby forged = MakeLobby(Lobby::eVisibility_Friends, "forged");
        forged.owner = alice.Public();
        net.Send(daveAt, d.PublishRecord(Lobby::Sign(alice, forged)));
        Check(d.NextEvent(e) && e.kind == eReply_Denied, "a lobby signed for another owner is refused");
        std::vector<uint8_t> record = Lobby::Sign(dave, forged);
        record[45] ^= 1;
        net.Send(daveAt, d.PublishRecord(record));
        Check(d.NextEvent(e) && e.kind == eReply_Denied, "a lobby with a bad signature is refused");
    }

    {
        net.Send(bobAt, b.Lookup(FriendCode::Of(alice.Public())));
        Check(b.NextEvent(e) && e.kind == eReply_Found && e.peer == alice.Public(), "a friend code finds its identity");
        net.Send(bobAt, b.Lookup("0000-0000-0000-0000"));
        Check(b.NextEvent(e) && e.kind == eReply_Denied, "an unknown code finds nothing");
        net.Send(bobAt, b.Reflect());
        Check(b.NextEvent(e) && e.kind == eReply_Reflected && e.address == bobAt, "the server reflects the address");
        size_t answered = 0;
        for (int index = 0; index < 50; ++index)
        {
            net.Send(bobAt, b.Lookup(FriendCode::Of(alice.Public())));
            answered += Drain(b);
        }
        Check(answered > 0 && answered < 20, "lookups are rate limited");
    }

    {
        net.now += 20000;
        net.server.Tick(net.now);
        net.Send(aliceAt, a.Publish(MakeLobby(Lobby::eVisibility_FriendsOfFriends, "Friends of friends")));
        Drain(a);
        net.now += 20000;
        net.server.Tick(net.now);
        net.Send(charlieAt, c.List(0));
        Check(c.NextEvent(e) && e.lobbies.size() == 1 && e.lobbies[0].name == "Friends of friends",
              "a heartbeat keeps a lobby alive and a silent one expires");
        net.now += 40000;
        net.server.Tick(net.now);
        net.Send(charlieAt, c.List(0));
        Check(c.NextEvent(e) && e.lobbies.empty(), "an unrefreshed lobby expires");
    }

    {
        std::vector<Lobby> lobbies;
        Lobby bad = MakeLobby(Lobby::eVisibility_Friends, "x");
        bad.players = 9;
        std::vector<uint8_t> record = Lobby::Sign(alice, bad);
        Lobby read;
        Check(!Lobby::Read(record.data(), record.size(), read, NULL), "a lobby with more players than room is refused");
        record = Lobby::Sign(alice, MakeLobby(Lobby::eVisibility_Friends, "x"));
        record[40] ^= 1;
        Check(!Lobby::Read(record.data(), record.size(), read, NULL), "a changed lobby fails its signature");
    }

    {
        Net flood(serverId);
        std::vector<Identity> ids(8);
        std::vector<Client*> made;
        size_t connected = 0;
        for (size_t index = 0; index < ids.size(); ++index)
        {
            Identity::Generate(ids[index]);
            made.push_back(new Client(ids[index], serverId.Public()));
            if (flood.Join(*made.back(), Endpoint(0x0b000001, 5000 + index)))
                ++connected;
        }
        Check(connected == 5, "one address can start only a few handshakes at a time");
        for (size_t index = 0; index < made.size(); ++index)
            delete made[index];
    }

    {
        const std::vector<uint8_t> request = c.List(0);
        net.Send(charlieAt, request);
        Check(Drain(c) == 1, "a request is answered");
        net.Send(charlieAt, request);
        Check(Drain(c) == 0, "a replayed request is ignored");
    }

    {
        Net lone(serverId);
        Identity player;
        Identity::Generate(player);
        const Endpoint firstAt(0x0c000001, 7001), secondAt(0x0c000002, 7002), thirdAt(0x0c000003, 7003);
        Client first(player, serverId.Public()), second(player, serverId.Public()), third(player, serverId.Public());
        Check(lone.Join(first, firstAt), "an identity connects");
        lone.Join(second, secondAt);
        Client::Event event;
        Check(second.NextEvent(event) && event.kind == eReply_InUse, "a second session for the same identity is refused");
        lone.Send(firstAt, first.Reflect());
        Check(first.NextEvent(event) && event.kind == eReply_Reflected, "the first session keeps working");
        lone.now += 30000;
        lone.Send(firstAt, first.Reflect());
        Drain(first);
        lone.now += 30000;
        lone.server.Tick(lone.now);
        lone.Join(third, thirdAt);
        Check(third.NextEvent(event) && event.kind == eReply_InUse, "a session that kept sending still holds the identity");
        lone.now += 50000;
        lone.server.Tick(lone.now);
        Client fourth(player, serverId.Public());
        const Endpoint fourthAt(0x0c000004, 7004);
        lone.Join(fourth, fourthAt);
        lone.Send(fourthAt, fourth.Reflect());
        Check(fourth.NextEvent(event) && event.kind == eReply_Reflected, "a session that went quiet no longer holds it");
    }

    {
        net.now += 400000;
        net.server.Tick(net.now);
        Check(net.server.Connections() == 0, "idle connections expire");
    }
    return CheckFailures() - before;
}
