#ifndef GAME_COMMAND_CHAT_SERVERCONTROLCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_SERVERCONTROLCHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class ServerControlChatCommand : public ChatCommand
{
public:
    enum EAction
    {
        eAction_SaveAll,
        eAction_Stop,
        eAction_SaveOff,
        eAction_SaveOn
    };

    explicit ServerControlChatCommand(EAction action);

    virtual bool run(const CommandSource& source, const Words& words);

private:
    EAction mAction;
};

#endif
