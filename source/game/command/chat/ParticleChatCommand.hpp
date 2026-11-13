#ifndef GAME_COMMAND_CHAT_PARTICLECHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_PARTICLECHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class ParticleChatCommand : public ChatCommand
{
public:
    ParticleChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);
};

#endif
