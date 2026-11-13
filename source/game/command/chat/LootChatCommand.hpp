#ifndef GAME_COMMAND_CHAT_LOOTCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_LOOTCHATCOMMAND_HPP

#include <vector>

#include "game/command/ChatCommand.hpp"

class LootTable;

class LootChatCommand : public ChatCommand
{
public:
    LootChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);

    // the names of the built-in tables as typed, with their namespace, in order
    static const std::vector<std::wstring>& TableNames();

private:
    static LootTable* FindTable(const std::wstring& name);
};

#endif
