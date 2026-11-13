#include "net/NetP2p.hpp"

namespace lcenet
{
namespace
{
class InertP2pService : public P2pService
{
public:
    virtual bool Enabled() const { return false; }
    virtual bool Start(const std::string&, const std::string&, const std::string&) { return false; }
    virtual bool Online() { return false; }
    virtual bool ValidFriendCode(const std::string&) { return false; }
    virtual std::string FriendCode() { return std::string(); }
    virtual void AddFriendByCode(const std::string&, const std::string&) {}
    virtual void SetNickname(const std::string&, const std::string&) {}
    virtual void RemoveFriend(const std::string&) {}
    virtual std::vector<P2pFriend> Friends() { return std::vector<P2pFriend>(); }
    virtual void Publish(const std::string&, unsigned int, unsigned int, unsigned int, unsigned int) {}
    virtual void Unpublish() {}
    virtual void Invite(const std::string&) {}
    virtual void RefreshLobbies() {}
    virtual std::vector<P2pLobbyInfo> Lobbies() { return std::vector<P2pLobbyInfo>(); }
    virtual boost::shared_ptr<Stream> TakeAccepted(std::string*) { return boost::shared_ptr<Stream>(); }
    virtual bool TakeLookupFailure() { return false; }
    virtual std::string Describe() { return "p2p is not available on this platform"; }
};
}

P2pService& P2pService::Get()
{
    static InertP2pService service;
    return service;
}
}
