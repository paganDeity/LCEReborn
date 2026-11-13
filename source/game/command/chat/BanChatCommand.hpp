#ifndef GAME_COMMAND_CHAT_BANCHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_BANCHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class BanChatCommand : public ChatCommand
{
public:
    enum EAction
    {
        eAction_Ban,
        eAction_Pardon,
        eAction_List,
        eAction_BanAddress,
        eAction_PardonAddress
    };

    explicit BanChatCommand(EAction action);

    virtual bool run(const CommandSource& source, const Words& words);

private:
    bool Ban(const CommandSource& source, const Words& words);
    bool Pardon(const CommandSource& source, const Words& words);
    bool List(const CommandSource& source, const Words& words);
    bool BanAddress(const CommandSource& source, const Words& words);
    bool PardonAddress(const CommandSource& source, const Words& words);

    EAction mAction;
};

#endif
