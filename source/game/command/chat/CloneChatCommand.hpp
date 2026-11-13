#ifndef GAME_COMMAND_CHAT_CLONECHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_CLONECHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class CloneChatCommand : public ChatCommand
{
public:
    CloneChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);
};

#endif
