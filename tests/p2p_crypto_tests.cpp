#include <stdio.h>

#include "p2p_check.hpp"
#include <string.h>

#include <string>
#include <vector>

#include "p2p/P2pHandshake.hpp"
#include "p2p/P2pFriendCode.hpp"
#include "p2p/P2pIdentity.hpp"

namespace
{
struct Pair
{
    Pair(const p2p::Identity& a, const p2p::Identity& b) : initiator(a, true), responder(b, false) {}

    p2p::Handshake initiator;
    p2p::Handshake responder;
    std::vector<uint8_t> first, second, third;
};

bool Run(Pair& pair)
{
    std::vector<uint8_t> none;
    pair.first = pair.initiator.Start();
    if (pair.responder.Receive(pair.first.data(), pair.first.size(), pair.second) == p2p::Handshake::eStatus_Failed)
        return false;
    if (pair.initiator.Receive(pair.second.data(), pair.second.size(), pair.third) == p2p::Handshake::eStatus_Failed)
        return false;
    return pair.responder.Receive(pair.third.data(), pair.third.size(), none) == p2p::Handshake::eStatus_Done;
}
}

int RunCryptoTests()
{
    const int before = CheckFailures();
    p2p::Identity alice, bob, mallory;
    Check(p2p::Identity::Generate(alice) && p2p::Identity::Generate(bob) && p2p::Identity::Generate(mallory),
          "generate identities");
    Check(alice.Public() != bob.Public(), "identities differ");

    p2p::Identity again;
    p2p::Identity::FromSeed(alice.Seed(), again);
    Check(again.Public() == alice.Public(), "a seed gives the same identity");

    const std::string code = p2p::FriendCode::Of(alice.Public());
    Check(code.size() == 19 && code[4] == '-' && code[9] == '-' && code[14] == '-', "friend code layout");
    Check(p2p::FriendCode::Matches(code, alice.Public()), "friend code matches its key");
    std::string lower = code;
    for (size_t i = 0; i < lower.size(); ++i)
        lower[i] = static_cast<char>(tolower(lower[i]));
    Check(p2p::FriendCode::Matches(lower, alice.Public()), "friend code ignores case");
    Check(!p2p::FriendCode::Matches(code, bob.Public()), "friend code rejects another key");

    uint8_t message[5] = {1, 2, 3, 4, 5};
    uint8_t signature[p2p::Identity::kSignatureSize];
    alice.Sign(message, sizeof(message), signature);
    Check(p2p::Identity::Verify(alice.Public(), message, sizeof(message), signature), "signature verifies");
    message[0] ^= 1;
    Check(!p2p::Identity::Verify(alice.Public(), message, sizeof(message), signature), "altered message fails");
    Check(!p2p::Identity::Verify(bob.Public(), message, sizeof(message), signature), "another key fails");

    {
        Pair pair(alice, bob);
        pair.initiator.Expect(bob.Public());
        pair.responder.Expect(alice.Public());
        Check(Run(pair), "handshake completes");
        Check(pair.initiator.Complete() && pair.responder.Complete(), "both sides complete");
        Check(pair.initiator.Peer() == bob.Public() && pair.responder.Peer() == alice.Public(), "peers are known");
        Check(pair.first.size() >= pair.second.size() && pair.first.size() >= pair.third.size(),
              "a request is never smaller than its reply");

        std::vector<uint8_t> wire, plain;
        const uint8_t text[] = "hello bob";
        Check(pair.initiator.GetSession().Seal(text, sizeof(text), wire), "seal");
        Check(pair.responder.GetSession().Open(wire.data(), wire.size(), plain) && plain.size() == sizeof(text)
                  && memcmp(plain.data(), text, sizeof(text)) == 0,
              "open");
        Check(!pair.responder.GetSession().Open(wire.data(), wire.size(), plain), "a replay is refused");
        std::vector<uint8_t> second;
        pair.initiator.GetSession().Seal(text, sizeof(text), second);
        second[second.size() - 1] ^= 1;
        Check(!pair.responder.GetSession().Open(second.data(), second.size(), plain), "a changed byte is refused");
        std::vector<uint8_t> back;
        Check(pair.responder.GetSession().Seal(text, sizeof(text), back)
                  && pair.initiator.GetSession().Open(back.data(), back.size(), plain),
              "the other direction works");
        Check(!pair.initiator.GetSession().Open(wire.data(), wire.size(), plain),
              "a message does not open in the direction it was sent");
        std::vector<uint8_t> empty;
        Check(pair.initiator.GetSession().Seal(NULL, 0, empty)
                  && pair.responder.GetSession().Open(empty.data(), empty.size(), plain) && plain.empty(),
              "an empty message works");
    }
    {
        Pair pair(alice, bob);
        pair.initiator.Expect(mallory.Public());
        Check(!Run(pair), "an initiator refuses an unexpected responder");
    }
    {
        Pair pair(alice, bob);
        pair.responder.Expect(mallory.Public());
        Check(!Run(pair), "a responder refuses an unexpected initiator");
    }
    {
        Pair pair(alice, bob);
        std::vector<uint8_t> none;
        pair.first = pair.initiator.Start();
        pair.responder.Receive(pair.first.data(), pair.first.size(), pair.second);
        pair.second[40] ^= 1;
        Check(pair.initiator.Receive(pair.second.data(), pair.second.size(), pair.third)
                  == p2p::Handshake::eStatus_Failed,
              "a changed reply fails");
    }
    {
        Pair pair(alice, bob);
        std::vector<uint8_t> none;
        pair.first = pair.initiator.Start();
        pair.first.resize(100);
        Check(pair.responder.Receive(pair.first.data(), pair.first.size(), pair.second)
                  == p2p::Handshake::eStatus_Failed,
              "a short first message is refused");
    }
    {
        Pair pair(alice, bob);
        std::vector<uint8_t> none;
        pair.first = pair.initiator.Start();
        pair.responder.Receive(pair.first.data(), pair.first.size(), pair.second);
        pair.initiator.Receive(pair.second.data(), pair.second.size(), pair.third);
        pair.third[5] ^= 1;
        Check(pair.responder.Receive(pair.third.data(), pair.third.size(), none) == p2p::Handshake::eStatus_Failed,
              "a changed final message fails");
    }

    return CheckFailures() - before;
}
