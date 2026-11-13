#include "game/command/chat/BanChatCommand.hpp"

#include "game/command/ChatCommands.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/network/CGameNetworkManager.hpp"
#include "game/network/INetworkPlayer.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/world/generation/PlayerList.hpp"
#include "game/network/ServerGamePacketListenerImpl.hpp"
#include "game/server/ServerAccess.hpp"
#include "game/server/ServerPlayer.hpp"

namespace
{
struct Form
{
    const wchar_t* name;
    const wchar_t* usage;
    const wchar_t* arguments;
};

const Form kForms[] = {{L"ban", L"<player|uuid> [reason]", L"player text"},
                       {L"pardon", L"<player|uuid>", L"player"},
                       {L"banlist", L"[players|ips]", L"players|ips"},
                       {L"ban-ip", L"<address|player> [reason]", L"player text"},
                       {L"pardon-ip", L"<address>", L"text"}};
}

BanChatCommand::BanChatCommand(EAction action)
    : ChatCommand(kForms[action].name, kForms[action].usage, kForms[action].arguments, ePermission_Operator),
      mAction(action)
{
}

bool BanChatCommand::run(const CommandSource& source, const Words& words)
{
    if (mAction == eAction_Ban)
        return Ban(source, words);
    if (mAction == eAction_Pardon)
        return Pardon(source, words);
    if (mAction == eAction_BanAddress)
        return BanAddress(source, words);
    if (mAction == eAction_PardonAddress)
        return PardonAddress(source, words);
    return List(source, words);
}

bool BanChatCommand::Ban(const CommandSource& source, const Words& words)
{
    if (words.size() < 2)
        return false;
    BanEntry entry;
    PlayerPtr online;
    IdentifyPlayer(words[1], &entry.id, &entry.name, &online);
    entry.reason = CommandArguments::Join(words, 2);
    const std::wstring shown = entry.name.empty() ? entry.id : entry.name;
    INetworkPlayer* network = online && online->mConnection ? online->mConnection->getNetworkPlayer() : NULL;
    if (network != NULL && (network->IsHost() || network->IsLocal()))
    {
        Fail(source, L"You can't ban the host or a player playing on the host's console.");
        return true;
    }
    AccessList<BanEntry>& bans = ServerAccess::Bans();
    if (bans.Listed(entry.id, entry.name))
    {
        Fail(source, shown + L" is already banned");
        return true;
    }
    bans.Add(entry);
    if (online && online->mConnection)
        online->mConnection->kick();
    Tell(source, L"Banned " + shown + (entry.reason.empty() ? std::wstring() : L": " + entry.reason));
    if (entry.id.empty())
        Tell(source, L"No player called " + entry.name + L" is online, so the first player with that name to join is the one banned, and is kept by "
                         L"their UUID from then on. Anyone can join with any name, so give the player's UUID instead if you know it.");
    return true;
}

bool BanChatCommand::Pardon(const CommandSource& source, const Words& words)
{
    if (words.size() != 2)
        return false;
    std::wstring id;
    std::wstring name;
    PlayerPtr online;
    IdentifyPlayer(words[1], &id, &name, &online);
    AccessList<BanEntry>& bans = ServerAccess::Bans();
    BanEntry known;
    if (id.empty() && bans.FindByName(name, &known))
        id = known.id;
    const std::wstring shown = name.empty() ? id : name;
    if (!bans.Remove(id, name))
    {
        Fail(source, shown + L" is not banned");
        return true;
    }
    Tell(source, L"Pardoned " + shown);
    return true;
}

bool BanChatCommand::List(const CommandSource& source, const Words& words)
{
    if (words.size() > 2)
        return false;
    const std::wstring which = words.size() == 2 ? CommandArguments::Lower(words[1]) : std::wstring(L"players");
    if (which != L"players" && which != L"ips")
        return false;
    const bool addresses = which == L"ips";
    const std::vector<BanEntry>& entries = addresses ? ServerAccess::AddressBans().Entries() : ServerAccess::Bans().Entries();
    if (entries.empty())
        Tell(source, addresses ? L"No address is banned" : L"Nobody is banned");
    else
        Tell(source, L"There " + std::wstring(entries.size() == 1 ? L"is " : L"are ")
                         + (addresses ? Count(static_cast<int>(entries.size()), L"banned address", L"banned addresses")
                                      : Count(static_cast<int>(entries.size()), L"banned player", L"banned players"))
                         + L":");
    for (size_t index = 0; index < entries.size(); ++index)
    {
        const BanEntry& entry = entries[index];
        Tell(source, (entry.name.empty() ? entry.id : entry.name) + (entry.reason.empty() ? std::wstring() : L": " + entry.reason));
    }
    return true;
}

bool BanChatCommand::BanAddress(const CommandSource& source, const Words& words)
{
    if (words.size() < 2)
        return false;
    BanEntry entry;
    PlayerPtr online;
    if (!ServerAccess::ParseAddress(words[1], &entry.id))
    {
        online = gMinecraftServer->getPlayers()->getPlayerByNameIgnoringCase(words[1]);
        INetworkPlayer* network = online && online->mConnection ? online->mConnection->getNetworkPlayer() : NULL;
        const std::wstring address = network != NULL ? CGameNetworkManager::sInstance.GetPlayerAddress(network) : std::wstring();
        if (address.empty())
        {
            Fail(source, words[1] + L" is not an address (four numbers with dots, such as 203.0.113.7) or a player connected from one.");
            return true;
        }
        entry.id = address;
        entry.name = online->getName();
    }
    entry.reason = CommandArguments::Join(words, 2);
    if (!ServerAccess::AddressBans().Add(entry))
    {
        Fail(source, entry.id + L" is already banned");
        return true;
    }
    if (online && online->mConnection)
        online->mConnection->kick();
    Tell(source, L"Banned the address " + entry.id + (entry.name.empty() ? std::wstring() : L" of " + entry.name)
                     + (entry.reason.empty() ? std::wstring() : L": " + entry.reason)
                     + (online ? std::wstring() : std::wstring(L". Players connected from it stay until they leave or are kicked.")));
    return true;
}

bool BanChatCommand::PardonAddress(const CommandSource& source, const Words& words)
{
    if (words.size() != 2)
        return false;
    std::wstring address;
    if (!ServerAccess::ParseAddress(words[1], &address) || !ServerAccess::AddressBans().Remove(address, std::wstring()))
    {
        Fail(source, words[1] + L" is not a banned address");
        return true;
    }
    Tell(source, L"Pardoned the address " + address);
    return true;
}
