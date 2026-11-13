#include <stdio.h>
#include <string.h>

#include <deque>
#include <fstream>
#include <set>
#include <sstream>

#include "game/core/CriticalSection.hpp"
#include "net/NetDialer.hpp"
#include "net/NetLog.hpp"
#include "game/core/PlayerIdentities.hpp"
#include "net/NetP2p.hpp"
#include "net/NetSocket.hpp"
#include "p2p/P2pClock.hpp"
#include "p2p/P2pFriendCode.hpp"
#include "p2p/P2pNode.hpp"
#include "platform/C4JEventImpl.hpp"
#include "platform/C4JThreadImpl.hpp"
#include "platform/pc/PcPlayerIdentities.hpp"

namespace lcenet
{
namespace
{
const int kThreadStack = 0x10000;
const int kPumpWaitMs = 2;
const int64_t kDialTimeoutMs = 25000;
const int64_t kRefreshMinMs = 3000;

std::string Hex(const p2p::PublicKey& key)
{
    static const char digits[] = "0123456789abcdef";
    std::string text;
    for (size_t index = 0; index < p2p::PublicKey::kSize; ++index)
    {
        text += digits[key.bytes[index] >> 4];
        text += digits[key.bytes[index] & 15];
    }
    return text;
}

bool ParseHex(const std::string& text, p2p::PublicKey& key)
{
    if (text.size() != p2p::PublicKey::kSize * 2)
        return false;
    for (size_t index = 0; index < p2p::PublicKey::kSize; ++index)
    {
        unsigned int byte = 0;
        for (size_t digit = 0; digit < 2; ++digit)
        {
            const char letter = text[index * 2 + digit];
            unsigned int value;
            if (letter >= '0' && letter <= '9')
                value = static_cast<unsigned int>(letter - '0');
            else if (letter >= 'a' && letter <= 'f')
                value = static_cast<unsigned int>(letter - 'a' + 10);
            else if (letter >= 'A' && letter <= 'F')
                value = static_cast<unsigned int>(letter - 'A' + 10);
            else
                return false;
            byte = (byte << 4) | value;
        }
        key.bytes[index] = static_cast<uint8_t>(byte);
    }
    return true;
}

class PcP2pService;

class PcP2pStream : public Stream
{
public:
    PcP2pStream(PcP2pService* service, boost::shared_ptr<p2p::PeerLink> link) : m_service(service), m_link(link) {}

    virtual bool SendAll(const void* data, size_t size);
    virtual int Receive(void* data, size_t size, int timeoutMs);
    virtual void Shutdown();
    virtual void Close();

private:
    PcP2pService* m_service;
    boost::shared_ptr<p2p::PeerLink> m_link;
};

class PcP2pService : public P2pService, private p2p::Node::Admission, private Dialer
{
public:
    PcP2pService()
        : m_thread(0), m_stop(false), m_started(false), m_wide(false), m_lastRefresh(0), m_lookupFailures(0), m_sleep(new C4JEventImpl(C4JEvent::Mode0))
    {
        InitializeCriticalSection(&m_lock);
    }

    virtual bool Enabled() const { return m_started; }
    virtual bool Start(const std::string& server, const std::string& serverKey, const std::string& directory);
    virtual bool Online();
    virtual bool ValidFriendCode(const std::string& code);
    virtual std::string FriendCode();
    virtual void AddFriendByCode(const std::string& code, const std::string& nickname);
    virtual bool TakeLookupFailure();
    virtual void SetNickname(const std::string& key, const std::string& nickname);
    virtual void RemoveFriend(const std::string& key);
    virtual std::vector<P2pFriend> Friends();
    virtual void Publish(const std::string& name, unsigned int players, unsigned int maxPlayers, unsigned int visibility,
                         unsigned int version);
    virtual void Unpublish();
    virtual void Invite(const std::string& key);
    virtual void RefreshLobbies();
    virtual std::vector<P2pLobbyInfo> Lobbies();
    virtual boost::shared_ptr<Stream> TakeAccepted(std::string* peer);
    virtual std::string Describe();

    void Lock() { EnterCriticalSection(&m_lock); }
    void Unlock() { LeaveCriticalSection(&m_lock); }
    void Sleep() { m_sleep->WaitForSignal(kPumpWaitMs); }

private:
    class FriendRecord
    {
    public:
        p2p::PublicKey key;
        std::string nickname;
    };

    virtual bool Admit(const p2p::PublicKey& peer);
    virtual boost::shared_ptr<Stream> Dial(const std::string& target, const CancelFlag& cancel, std::string& error);
    static int Entry(void* service);
    void Loop();
    void LoadFriends();
    void SaveFriends();
    void ApplyFriends();
    int FindFriend(const p2p::PublicKey& key);

    C4JCriticalSection m_lock;
    C4JThreadImpl* m_thread;
    bool m_stop;
    bool m_started;
    bool m_wide;
    int64_t m_lastRefresh;
    C4JEventImpl* m_sleep;
    UdpSocket m_socket;
    p2p::Identity m_identity;
    boost::shared_ptr<p2p::Node> m_node;
    Address m_serverAddress;
    std::string m_directory;
    std::vector<FriendRecord> m_friends;
    std::deque<std::string> m_nicknames;
    unsigned int m_lookupFailures;
    std::set<std::string> m_invited;
    std::deque<std::pair<boost::shared_ptr<p2p::PeerLink>, std::string> > m_accepted;
};

bool PcP2pStream::SendAll(const void* data, size_t size)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    while (size > 0)
    {
        m_service->Lock();
        if (m_link->State() == p2p::PeerLink::eState_Failed || m_link->State() == p2p::PeerLink::eState_Closed)
        {
            m_service->Unlock();
            return false;
        }
        const size_t written = m_link->State() == p2p::PeerLink::eState_Established ? m_link->Write(bytes, size) : 0;
        m_service->Unlock();
        bytes += written;
        size -= written;
        if (size > 0)
            m_service->Sleep();
    }
    return true;
}

int PcP2pStream::Receive(void* data, size_t size, int timeoutMs)
{
    const int64_t until = p2p::NowMs() + timeoutMs;
    for (;;)
    {
        m_service->Lock();
        const size_t got = m_link->Read(static_cast<uint8_t*>(data), size);
        const bool dead = m_link->State() == p2p::PeerLink::eState_Failed
                          || m_link->State() == p2p::PeerLink::eState_Closed || m_link->PeerClosed();
        m_service->Unlock();
        if (got > 0)
            return static_cast<int>(got);
        if (dead)
            return -1;
        if (p2p::NowMs() >= until)
            return 0;
        m_service->Sleep();
    }
}

void PcP2pStream::Shutdown()
{
    m_service->Lock();
    m_link->Close();
    m_service->Unlock();
}

void PcP2pStream::Close()
{
    Shutdown();
}

bool PcP2pService::Start(const std::string& server, const std::string& serverKey, const std::string& directory)
{
    if (m_started)
        return true;
    p2p::PublicKey key;
    std::string error;
    Address address;
    if (!ParseHex(serverKey, key) || !Resolve(server, address, error))
    {
        NET_LOG_WARN("Peer to peer is off: %s", error.empty() ? "the server key is not valid" : error.c_str());
        return false;
    }
    m_directory = directory;
    if (!PcActiveIdentityKey(m_identity))
    {
        NET_LOG_WARN("Peer to peer is off: there is no identity to use");
        return false;
    }
    std::string busy;
    if (!PlayerIdentities::Get().GoOnline(&busy))
    {
        NET_LOG_WARN("Peer to peer is off: %s", busy.c_str());
        return false;
    }
    if (!m_socket.Open(0, error))
    {
        NET_LOG_WARN("Peer to peer is off: %s", error.c_str());
        return false;
    }
    m_serverAddress = address;
    m_node.reset(new p2p::Node(m_identity, key, p2p::Endpoint(address.ip, address.port), this));
    LoadFriends();
    ApplyFriends();
    Dialer::Register("p2p", this);
    m_stop = false;
    m_started = true;
    m_thread = new C4JThreadImpl(&PcP2pService::Entry, this, "NetP2p", kThreadStack);
    m_thread->Run();
    NET_LOG_INFO("Peer to peer is on; friend code %s", FriendCode().c_str());
    return true;
}

int PcP2pService::Entry(void* service)
{
    static_cast<PcP2pService*>(service)->Loop();
    return 0;
}

void PcP2pService::Loop()
{
    unsigned char buffer[2048];
    for (;;)
    {
        std::vector<p2p::Node::Outgoing> out;
        Lock();
        if (m_stop)
        {
            Unlock();
            return;
        }
        m_node->Update(p2p::NowMs());
        p2p::PublicKey found;
        while (m_node->TakeFound(found))
        {
            if (FindFriend(found) < 0 && !(found == m_identity.Public()))
            {
                FriendRecord record;
                record.key = found;
                if (!m_nicknames.empty())
                    record.nickname = m_nicknames.front();
                m_friends.push_back(record);
                SaveFriends();
                ApplyFriends();
            }
            if (!m_nicknames.empty())
                m_nicknames.pop_front();
        }
        while (m_node->TakeLookupFailure())
        {
            if (!m_nicknames.empty())
                m_nicknames.pop_front();
            ++m_lookupFailures;
        }
        for (boost::shared_ptr<p2p::PeerLink> link = m_node->TakeAccepted(); link; link = m_node->TakeAccepted())
            m_accepted.push_back(std::make_pair(link, Hex(link->Peer())));
        m_node->TakeOutgoing(out);
        Unlock();
        for (size_t index = 0; index < out.size(); ++index)
        {
            Address to;
            to.ip = out[index].to.ip;
            to.port = out[index].to.port;
            m_socket.SendTo(to, &out[index].data[0], out[index].data.size());
        }
        Address from;
        const int received = m_socket.ReceiveFrom(buffer, sizeof(buffer), from, kPumpWaitMs);
        if (received > 0)
        {
            Lock();
            m_node->OnDatagram(p2p::Endpoint(from.ip, from.port), buffer, static_cast<size_t>(received), p2p::NowMs());
            Unlock();
        }
    }
}

bool PcP2pService::Admit(const p2p::PublicKey& peer)
{
    return m_wide || FindFriend(peer) >= 0 || m_invited.count(peer.AsString()) != 0;
}

int PcP2pService::FindFriend(const p2p::PublicKey& key)
{
    for (size_t index = 0; index < m_friends.size(); ++index)
        if (m_friends[index].key == key)
            return static_cast<int>(index);
    return -1;
}

void PcP2pService::LoadFriends()
{
    std::ifstream file((m_directory + "/p2p-friends.txt").c_str());
    std::string line;
    while (std::getline(file, line))
    {
        const size_t space = line.find(' ');
        FriendRecord record;
        if (!ParseHex(line.substr(0, space), record.key))
            continue;
        if (space != std::string::npos)
            record.nickname = line.substr(space + 1);
        m_friends.push_back(record);
    }
}

void PcP2pService::SaveFriends()
{
    std::ofstream file((m_directory + "/p2p-friends.txt").c_str(), std::ios::trunc);
    for (size_t index = 0; index < m_friends.size(); ++index)
        file << Hex(m_friends[index].key) << ' ' << m_friends[index].nickname << '\n';
}

void PcP2pService::ApplyFriends()
{
    std::vector<p2p::PublicKey> keys;
    for (size_t index = 0; index < m_friends.size(); ++index)
        keys.push_back(m_friends[index].key);
    m_node->SetFriends(keys);
}

bool PcP2pService::Online()
{
    if (!m_started)
        return false;
    Lock();
    const bool online = m_node->Online();
    Unlock();
    return online;
}

bool PcP2pService::ValidFriendCode(const std::string& code)
{
    return p2p::FriendCode::Normalize(code).size() == 16;
}

std::string PcP2pService::FriendCode()
{
    return m_started ? p2p::FriendCode::Of(m_identity.Public()) : std::string();
}

void PcP2pService::AddFriendByCode(const std::string& code, const std::string& nickname)
{
    if (!m_started)
        return;
    Lock();
    m_nicknames.push_back(nickname);
    m_node->LookupCode(code);
    Unlock();
}

bool PcP2pService::TakeLookupFailure()
{
    Lock();
    const bool failed = m_lookupFailures != 0;
    if (failed)
        --m_lookupFailures;
    Unlock();
    return failed;
}

void PcP2pService::SetNickname(const std::string& key, const std::string& nickname)
{
    p2p::PublicKey parsed;
    if (!m_started || !ParseHex(key, parsed))
        return;
    Lock();
    const int index = FindFriend(parsed);
    if (index >= 0)
    {
        m_friends[index].nickname = nickname;
        SaveFriends();
    }
    Unlock();
}

void PcP2pService::RemoveFriend(const std::string& key)
{
    p2p::PublicKey parsed;
    if (!m_started || !ParseHex(key, parsed))
        return;
    Lock();
    const int index = FindFriend(parsed);
    if (index >= 0)
    {
        m_friends.erase(m_friends.begin() + index);
        SaveFriends();
        ApplyFriends();
    }
    Unlock();
}

std::vector<P2pFriend> PcP2pService::Friends()
{
    std::vector<P2pFriend> result;
    Lock();
    for (size_t index = 0; index < m_friends.size(); ++index)
    {
        P2pFriend entry;
        entry.key = Hex(m_friends[index].key);
        entry.nickname = m_friends[index].nickname;
        entry.code = p2p::FriendCode::Of(m_friends[index].key);
        result.push_back(entry);
    }
    Unlock();
    return result;
}

void PcP2pService::Publish(const std::string& name, unsigned int players, unsigned int maxPlayers,
                           unsigned int visibility, unsigned int version)
{
    if (!m_started)
        return;
    p2p::Lobby lobby;
    lobby.visibility = visibility > p2p::Lobby::eVisibility_Invite ? p2p::Lobby::eVisibility_Friends : visibility;
    lobby.maxPlayers = maxPlayers > 255 ? 255 : maxPlayers;
    lobby.players = players > lobby.maxPlayers ? lobby.maxPlayers : players;
    lobby.protocol = p2p::Lobby::eProtocol_Legacy;
    lobby.version = version;
    lobby.name = name.substr(0, p2p::Lobby::kMaxName);
    lobby.mode = "game";
    Lock();
    m_wide = visibility == p2p::Lobby::eVisibility_FriendsOfFriends;
    m_node->Publish(lobby);
    Unlock();
}

void PcP2pService::Unpublish()
{
    if (!m_started)
        return;
    Lock();
    m_node->Unpublish();
    m_invited.clear();
    Unlock();
}

void PcP2pService::Invite(const std::string& key)
{
    p2p::PublicKey parsed;
    if (!m_started || !ParseHex(key, parsed))
        return;
    Lock();
    m_invited.insert(parsed.AsString());
    m_node->Invite(parsed);
    Unlock();
}

void PcP2pService::RefreshLobbies()
{
    if (!m_started)
        return;
    const int64_t now = p2p::NowMs();
    Lock();
    if (now - m_lastRefresh >= kRefreshMinMs)
    {
        m_lastRefresh = now;
        m_node->RefreshLobbies();
    }
    Unlock();
}

std::vector<P2pLobbyInfo> PcP2pService::Lobbies()
{
    std::vector<P2pLobbyInfo> result;
    if (!m_started)
        return result;
    Lock();
    const std::vector<p2p::Lobby>& lobbies = m_node->Lobbies();
    for (size_t index = 0; index < lobbies.size(); ++index)
    {
        P2pLobbyInfo info;
        info.name = lobbies[index].name;
        info.owner = Hex(lobbies[index].owner);
        info.players = lobbies[index].players;
        info.maxPlayers = lobbies[index].maxPlayers;
        info.protocol = lobbies[index].protocol;
        info.version = lobbies[index].version;
        info.visibility = lobbies[index].visibility;
        result.push_back(info);
    }
    Unlock();
    return result;
}

boost::shared_ptr<Stream> PcP2pService::TakeAccepted(std::string* peer)
{
    boost::shared_ptr<Stream> stream;
    if (!m_started)
        return stream;
    Lock();
    if (!m_accepted.empty())
    {
        stream.reset(new PcP2pStream(this, m_accepted.front().first));
        if (peer != 0)
            *peer = m_accepted.front().second;
        m_accepted.pop_front();
    }
    Unlock();
    return stream;
}

boost::shared_ptr<Stream> PcP2pService::Dial(const std::string& target, const CancelFlag& cancel, std::string& error)
{
    p2p::PublicKey host;
    if (target.compare(0, 4, "p2p:") != 0 || !ParseHex(target.substr(4), host) || !m_started)
    {
        error = "That isn't a peer-to-peer game.";
        return boost::shared_ptr<Stream>();
    }
    Lock();
    m_node->Dial(host);
    Unlock();
    const int64_t until = p2p::NowMs() + kDialTimeoutMs;
    while (p2p::NowMs() < until)
    {
        if (cancel.Cancelled())
        {
            error = "Cancelled.";
            return boost::shared_ptr<Stream>();
        }
        Lock();
        boost::shared_ptr<p2p::PeerLink> link = m_node->DialedLink(host);
        const bool refused = m_node->DialRefused(host);
        const bool up = link && link->State() == p2p::PeerLink::eState_Established;
        const bool failed = link && (link->State() == p2p::PeerLink::eState_Failed
                                     || link->State() == p2p::PeerLink::eState_Closed);
        Unlock();
        if (up)
            return boost::shared_ptr<Stream>(new PcP2pStream(this, link));
        if (refused)
        {
            error = "That game isn't available to you.";
            return boost::shared_ptr<Stream>();
        }
        if (failed)
        {
            error = "Couldn't reach that game.";
            return boost::shared_ptr<Stream>();
        }
        Sleep();
    }
    error = "The game didn't answer.";
    return boost::shared_ptr<Stream>();
}

std::string PcP2pService::Describe()
{
    if (!m_started)
        return "off";
    char text[256];
    Lock();
    snprintf(text, sizeof(text), "on online=%d friends=%u lobbies=%u accepted=%u code=%s", m_node->Online() ? 1 : 0,
             static_cast<unsigned int>(m_friends.size()), static_cast<unsigned int>(m_node->Lobbies().size()),
             static_cast<unsigned int>(m_accepted.size()), p2p::FriendCode::Of(m_identity.Public()).c_str());
    Unlock();
    return text;
}
}

P2pService& P2pService::Get()
{
    static PcP2pService service;
    return service;
}
}
