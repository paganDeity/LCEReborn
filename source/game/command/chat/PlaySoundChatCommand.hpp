#ifndef GAME_COMMAND_CHAT_PLAYSOUNDCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_PLAYSOUNDCHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class PlaySoundChatCommand : public ChatCommand
{
public:
    PlaySoundChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);
};

#endif
