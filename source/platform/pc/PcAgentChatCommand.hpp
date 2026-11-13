#ifndef PLATFORM_PC_PCAGENTCHATCOMMAND_HPP
#define PLATFORM_PC_PCAGENTCHATCOMMAND_HPP

#if LCE_DEBUG_BUILD
#include "game/command/ChatCommand.hpp"

class PcAgentChatCommand : public ChatCommand
{
public:
    PcAgentChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);

private:
    static void Show(int id);
    static void List(const CommandSource& source);
    static bool Answer(const CommandSource& source, const Words& words);
};
#endif

#endif
