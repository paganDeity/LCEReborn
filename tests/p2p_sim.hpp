#ifndef TESTS_P2P_SIM_HPP
#define TESTS_P2P_SIM_HPP

#include <deque>
#include <map>
#include <set>
#include <vector>

#include "p2p/P2pIdentity.hpp"
#include "p2p/P2pNode.hpp"
#include "p2p/RendezvousServer.hpp"

namespace p2psim
{
using namespace p2p;

class Policy : public Node::Admission
{
public:
    virtual bool Admit(const PublicKey& peer) { return allowed.count(peer.AsString()) != 0; }
    void Allow(const PublicKey& peer) { allowed.insert(peer.AsString()); }

    std::set<std::string> allowed;
};

class SimNet
{
public:
    class Packet
    {
    public:
        int64_t at;
        Endpoint from;
        Endpoint to;
        std::vector<uint8_t> data;
    };

    SimNet(const Identity& serverIdentity)
        : serverKey(serverIdentity.Public()),
          server(serverIdentity, rendezvous::Server::Limits()),
          serverAt(0x0a0000fe, 25580),
          now(1000),
          blockDirect(false),
          directSent(0),
          relayedAtServer(0)
    {
    }

    void Add(Node& node, const Endpoint& at) { nodes[at] = &node; }

    void Step(int64_t ms)
    {
        for (int64_t end = now + ms; now < end; now += 10)
        {
            for (std::map<Endpoint, Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
            {
                it->second->Update(now);
                std::vector<Node::Outgoing> out;
                it->second->TakeOutgoing(out);
                for (size_t index = 0; index < out.size(); ++index)
                    Push(it->first, out[index].to, out[index].data);
            }
            server.Tick(now);
            while (!queue.empty() && queue.front().at <= now)
            {
                Packet packet = queue.front();
                queue.pop_front();
                Deliver(packet);
            }
        }
    }

    bool Run(bool (*done)(void*), void* context, int64_t limitMs)
    {
        for (int64_t waited = 0; waited < limitMs; waited += 100)
        {
            if (done(context))
                return true;
            Step(100);
        }
        return done(context);
    }

    PublicKey serverKey;
    rendezvous::Server server;
    Endpoint serverAt;
    std::map<Endpoint, Node*> nodes;
    std::deque<Packet> queue;
    int64_t now;
    bool blockDirect;
    size_t directSent;
    size_t relayedAtServer;

private:
    void Push(const Endpoint& from, const Endpoint& to, const std::vector<uint8_t>& data)
    {
        if (to != serverAt && from != serverAt)
        {
            ++directSent;
            if (blockDirect)
                return;
        }
        Packet packet;
        packet.at = now + 20;
        packet.from = from;
        packet.to = to;
        packet.data = data;
        queue.push_back(packet);
    }

    void Deliver(const Packet& packet)
    {
        if (packet.to == serverAt)
        {
            std::vector<rendezvous::Server::Outgoing> out;
            server.Receive(packet.from, packet.data.data(), packet.data.size(), now, out);
            for (size_t index = 0; index < out.size(); ++index)
                Push(serverAt, out[index].to, out[index].data);
            return;
        }
        std::map<Endpoint, Node*>::iterator node = nodes.find(packet.to);
        if (node != nodes.end())
            node->second->OnDatagram(packet.from, packet.data.data(), packet.data.size(), now);
    }
};
}

#endif
