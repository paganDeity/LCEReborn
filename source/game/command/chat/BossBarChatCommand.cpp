#include "game/command/chat/BossBarChatCommand.hpp"

#include "game/command/CommandArguments.hpp"
#include "game/server/CustomBossEvents.hpp"
#include "game/server/ServerPlayer.hpp"

using CommandArguments::Join;
using CommandArguments::Lower;
using CommandArguments::Number;
using CommandArguments::ParseInt;

namespace
{
const wchar_t* const kColors[] = {L"pink", L"blue", L"red", L"green", L"yellow", L"purple", L"white"};
const wchar_t* const kStyles[] = {L"progress", L"notched_6", L"notched_10", L"notched_12", L"notched_20"};

template <size_t N> int IndexOf(const wchar_t* const (&names)[N], const std::wstring& word)
{
    for (size_t index = 0; index < N; ++index)
    {
        if (word == names[index])
            return static_cast<int>(index);
    }
    return -1;
}

// an id without a namespace is the game's own, as in Java
std::wstring BarId(const std::wstring& word)
{
    return word.find(L':') == std::wstring::npos ? L"minecraft:" + word : word;
}

// the rest of the line as plain text, without the quotes around it
std::wstring BarName(const std::vector<std::wstring>& words, size_t first)
{
    std::wstring name = Join(words, first);
    if (name.size() >= 2 && name[0] == L'"' && name[name.size() - 1] == L'"')
        name = name.substr(1, name.size() - 2);
    return name;
}

std::wstring Shown(const CustomBossEvents::Bar* bar)
{
    return L"[" + bar->name + L"]";
}
}

BossBarChatCommand::BossBarChatCommand()
    : ChatCommand(L"bossbar", L"<add|remove|list|set|get> ...", L"add|remove|list|set|get text name|color|style|value|max|visible|players",
                  ePermission_Operator)
{
}

bool BossBarChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 2)
        return false;
    boost::shared_ptr<CustomBossEvents> bars = CustomBossEvents::get();
    if (!bars)
    {
        Fail(source, L"Boss bars need a world the server runs");
        return true;
    }
    const std::wstring action = Lower(words[1]);
    if (action == L"list")
    {
        if (words.size() != 2)
            return false;
        if (bars->bars().empty())
        {
            Tell(source, L"There are no custom bossbars active");
            return true;
        }
        std::wstring names;
        for (size_t index = 0; index < bars->bars().size(); ++index)
            names += std::wstring(index == 0 ? L"" : L", ") + Shown(bars->bars()[index]);
        Tell(source, L"There are " + Number(static_cast<int>(bars->bars().size())) + L" custom bossbars active: " + names);
        return true;
    }
    if (words.size() < 3)
        return false;
    const std::wstring id = BarId(words[2]);
    if (action == L"add")
    {
        if (words.size() < 4)
            return false;
        CustomBossEvents::Bar* bar = bars->add(id, BarName(words, 3));
        if (bar == NULL)
        {
            Fail(source, L"A bossbar already exists with the ID '" + id + L"'");
            return true;
        }
        Tell(source, L"Created custom bossbar " + Shown(bar));
        return true;
    }
    CustomBossEvents::Bar* bar = bars->find(id);
    if (bar == NULL)
    {
        Fail(source, L"No bossbar exists with the ID '" + id + L"'");
        return true;
    }
    if (action == L"remove")
    {
        if (words.size() != 3)
            return false;
        const std::wstring shown = Shown(bar);
        bars->remove(id);
        Tell(source, L"Removed custom bossbar " + shown);
        return true;
    }
    if (words.size() < 4)
        return false;
    const std::wstring field = Lower(words[3]);
    if (action == L"get")
    {
        if (words.size() != 4)
            return false;
        if (field == L"value")
            Tell(source, L"Custom bossbar " + Shown(bar) + L" has a value of " + Number(bar->value));
        else if (field == L"max")
            Tell(source, L"Custom bossbar " + Shown(bar) + L" has a maximum of " + Number(bar->max));
        else if (field == L"visible")
            Tell(source, L"Custom bossbar " + Shown(bar) + (bar->visible ? L" is currently shown" : L" is currently hidden"));
        else if (field == L"players")
            Tell(source, L"Custom bossbar " + Shown(bar) + L" has " + Count(static_cast<int>(bar->players.size()), L"player", L"players")
                             + L" listed");
        else
            return false;
        return true;
    }
    if (action != L"set")
        return false;
    if (field == L"players")
    {
        PlayerVector players;
        if (words.size() > 4 && !ResolvePlayers(source, words, 4, players))
            return true;
        bars->setPlayers(bar, players);
        Tell(source, L"Custom bossbar " + Shown(bar) + L" now has " + Count(static_cast<int>(players.size()), L"player", L"players"));
        return true;
    }
    if (words.size() < 5)
        return false;
    if (field == L"name")
    {
        bar->name = BarName(words, 4);
        bars->changed(bar);
        Tell(source, L"Custom bossbar " + Shown(bar) + L" has been renamed");
        return true;
    }
    if (words.size() != 5)
        return false;
    const std::wstring value = Lower(words[4]);
    if (field == L"color" && IndexOf(kColors, value) >= 0)
    {
        bar->color = static_cast<BossEvent::BossBarColor>(IndexOf(kColors, value));
        Tell(source, L"Custom bossbar " + Shown(bar) + L" has changed color");
    }
    else if (field == L"style" && IndexOf(kStyles, value) >= 0)
    {
        bar->overlay = static_cast<BossEvent::BossBarOverlay>(IndexOf(kStyles, value));
        Tell(source, L"Custom bossbar " + Shown(bar) + L" has changed style");
    }
    else if (field == L"value")
    {
        int number = 0;
        if (!ParseInt(value, 0, 2147483647, &number))
        {
            Fail(source, L"The value must be a whole number of 0 or more, not " + words[4]);
            return true;
        }
        bar->value = number;
        Tell(source, L"Custom bossbar " + Shown(bar) + L" has changed value to " + Number(number));
    }
    else if (field == L"max")
    {
        int number = 0;
        if (!ParseInt(value, 1, 2147483647, &number))
        {
            Fail(source, L"The maximum must be a whole number of 1 or more, not " + words[4]);
            return true;
        }
        bar->max = number;
        Tell(source, L"Custom bossbar " + Shown(bar) + L" has changed maximum to " + Number(number));
    }
    else if (field == L"visible" && (value == L"true" || value == L"false"))
    {
        bar->visible = value == L"true";
        Tell(source, L"Custom bossbar " + Shown(bar) + (bar->visible ? L" is now visible" : L" is now hidden"));
    }
    else
    {
        return false;
    }
    bars->changed(bar);
    return true;
}
