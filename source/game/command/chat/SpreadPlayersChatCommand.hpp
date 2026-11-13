#ifndef GAME_COMMAND_CHAT_SPREADPLAYERSCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_SPREADPLAYERSCHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class SpreadPlayersChatCommand : public ChatCommand
{
public:
    SpreadPlayersChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);
};

#endif
