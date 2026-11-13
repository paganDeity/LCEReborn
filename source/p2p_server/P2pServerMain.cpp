#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

#include "net/NetSocket.hpp"
#include "p2p/P2pClock.hpp"
#include "p2p/P2pFriendCode.hpp"
#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pIdentityStore.hpp"
#include "p2p/RendezvousServer.hpp"

namespace
{
const unsigned short kDefaultPort = 25580;
}

int main(int argc, char** argv)
{
    unsigned short port = kDefaultPort;
    std::string keyPath = "rendezvous.key";
    for (int index = 1; index < argc; ++index)
    {
        if (strcmp(argv[index], "--port") == 0 && index + 1 < argc)
            port = static_cast<unsigned short>(atoi(argv[++index]));
        else if (strcmp(argv[index], "--key") == 0 && index + 1 < argc)
            keyPath = argv[++index];
        else
        {
            fprintf(stderr, "usage: LCERebornP2pServer [--port N] [--key FILE]\n");
            return 2;
        }
    }

    p2p::Identity identity;
    if (!p2p::IdentityStore::LoadOrCreate(keyPath, identity))
    {
        fprintf(stderr, "The key file %s could not be read or created.\n", keyPath.c_str());
        return 1;
    }
    lcenet::Startup();
    lcenet::UdpSocket socket;
    std::string error;
    if (!socket.Open(port, error))
    {
        fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    printf("Rendezvous server on port %u\nKey file %s (keep it private)\nFriend code %s\nPublic key ", port,
           keyPath.c_str(), p2p::FriendCode::Of(identity.Public()).c_str());
    for (size_t index = 0; index < p2p::PublicKey::kSize; ++index)
        printf("%02x", identity.Public().bytes[index]);
    printf("\n");
    fflush(stdout);

    p2p::rendezvous::Server server(identity, p2p::rendezvous::Server::Limits());
    std::vector<unsigned char> buffer(2048);
    int64_t lastTick = p2p::NowMs();
    for (;;)
    {
        lcenet::Address from;
        const int received = socket.ReceiveFrom(&buffer[0], buffer.size(), from, 100);
        const int64_t now = p2p::NowMs();
        if (received > 0)
        {
            std::vector<p2p::rendezvous::Server::Outgoing> out;
            server.Receive(p2p::Endpoint(from.ip, from.port), &buffer[0], static_cast<size_t>(received),
                           now, out);
            for (size_t index = 0; index < out.size(); ++index)
            {
                lcenet::Address to;
                to.ip = out[index].to.ip;
                to.port = out[index].to.port;
                socket.SendTo(to, &out[index].data[0], out[index].data.size());
            }
        }
        if (now - lastTick >= 1000)
        {
            server.Tick(now);
            lastTick = now;
        }
    }
}
