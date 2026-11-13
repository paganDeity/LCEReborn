#ifndef GAME_COMMAND_CHAT_BOSSBARCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_BOSSBARCHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class BossBarChatCommand : public ChatCommand
{
public:
    BossBarChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);
};

#endif
