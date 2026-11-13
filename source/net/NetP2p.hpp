#ifndef NET_NETP2P_HPP
#define NET_NETP2P_HPP

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "net/NetStream.hpp"

namespace lcenet
{
class P2pLobbyInfo
{
public:
    P2pLobbyInfo() : players(0), maxPlayers(0), protocol(0), version(0), visibility(0) {}

    std::string name;
    std::string owner;
    unsigned int players;
    unsigned int maxPlayers;
    unsigned int protocol;
    unsigned int version;
    unsigned int visibility;
};

class P2pFriend
{
public:
    std::string key;
    std::string nickname;
    std::string code;
};

class P2pService
{
public:
    virtual ~P2pService() {}

    static P2pService& Get();

    virtual bool Enabled() const = 0;
    virtual bool Start(const std::string& server, const std::string& serverKey, const std::string& directory) = 0;
    virtual bool Online() = 0;
    virtual bool ValidFriendCode(const std::string& code) = 0;
    virtual std::string FriendCode() = 0;
    virtual void AddFriendByCode(const std::string& code, const std::string& nickname) = 0;
    virtual bool TakeLookupFailure() = 0;
    virtual void SetNickname(const std::string& key, const std::string& nickname) = 0;
    virtual void RemoveFriend(const std::string& key) = 0;
    virtual std::vector<P2pFriend> Friends() = 0;
    virtual void Publish(const std::string& name, unsigned int players, unsigned int maxPlayers, unsigned int visibility,
                         unsigned int version) = 0;
    virtual void Unpublish() = 0;
    virtual void Invite(const std::string& key) = 0;
    virtual void RefreshLobbies() = 0;
    virtual std::vector<P2pLobbyInfo> Lobbies() = 0;
    virtual boost::shared_ptr<Stream> TakeAccepted(std::string* peer) = 0;
    virtual std::string Describe() = 0;
};
}

#endif
