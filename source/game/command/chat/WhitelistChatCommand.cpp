#include "game/command/chat/WhitelistChatCommand.hpp"

#include "game/command/CommandArguments.hpp"
#include "game/server/ServerAccess.hpp"
#include "game/server/ServerPlayer.hpp"

using CommandArguments::Lower;

WhitelistChatCommand::WhitelistChatCommand()
    : ChatCommand(L"whitelist", L"add <player|uuid> | remove <player|uuid> | list | on | off",
                  L"add player;remove player;list;on;off", ePermission_Operator)
{
}

bool WhitelistChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 2)
        return false;
    const std::wstring action = Lower(words[1]);
    AccessList<WhitelistEntry>& list = ServerAccess::Whitelist();
    if (action == L"on" || action == L"off")
    {
        if (words.size() != 2)
            return false;
        const bool on = action == L"on";
        if (ServerAccess::WhitelistOn() == on)
        {
            Fail(source, on ? L"The whitelist is already on" : L"The whitelist is already off");
            return true;
        }
        ServerAccess::ChangeWhitelistOn(on);
        Tell(source, on ? L"The whitelist is on: only listed players may join" : L"The whitelist is off: anyone may join");
        return true;
    }
    if (action == L"list")
    {
        if (words.size() != 2)
            return false;
        const std::vector<WhitelistEntry>& entries = list.Entries();
        std::wstring names;
        for (size_t index = 0; index < entries.size(); ++index)
            names += (index == 0 ? L"" : L", ") + (entries[index].name.empty() ? entries[index].id : entries[index].name);
        Tell(source, std::wstring(L"The whitelist is ") + (ServerAccess::WhitelistOn() ? L"on" : L"off") + L" and has "
                         + Count(static_cast<int>(entries.size()), L"player", L"players") + (entries.empty() ? L"" : L": " + names));
        return true;
    }
    if ((action != L"add" && action != L"remove") || words.size() != 3)
        return false;
    std::wstring id;
    std::wstring name;
    PlayerPtr online;
    IdentifyPlayer(words[2], &id, &name, &online);
    const std::wstring shown = name.empty() ? id : name;
    if (action == L"add")
    {
        if (list.Listed(id, name))
        {
            Fail(source, shown + L" is already on the whitelist");
            return true;
        }
        list.Add(id, name);
        Tell(source, L"Added " + shown + L" to the whitelist");
        if (id.empty())
            Tell(source, L"No player called " + name + L" is online, so the first player with that name to join is the one listed, and is kept by "
                             L"their UUID from then on. Anyone can join with any name, so give the player's UUID instead if you know it.");
        return true;
    }
    WhitelistEntry known;
    if (id.empty() && list.FindByName(name, &known))
        id = known.id;
    if (!list.Remove(id, name))
    {
        Fail(source, shown + L" is not on the whitelist");
        return true;
    }
    Tell(source, L"Removed " + shown + L" from the whitelist");
    return true;
}
