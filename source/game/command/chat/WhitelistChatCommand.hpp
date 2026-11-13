#ifndef GAME_COMMAND_CHAT_WHITELISTCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_WHITELISTCHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class WhitelistChatCommand : public ChatCommand
{
public:
    WhitelistChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);
};

#endif
