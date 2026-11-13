#ifndef GAME_COMMAND_CHAT_LOCATECHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_LOCATECHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class LocateChatCommand : public ChatCommand
{
public:
    LocateChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);
};

#endif
