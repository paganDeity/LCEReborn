#include "game/command/chat/ServerControlChatCommand.hpp"

#include "game/core/CMinecraftApp.hpp"
#include "game/server/ServerControl.hpp"

namespace
{
const wchar_t* const kNames[] = {L"save-all", L"stop", L"save-off", L"save-on"};
}

ServerControlChatCommand::ServerControlChatCommand(EAction action)
    : ChatCommand(kNames[action], L"", L"", action == eAction_Stop ? ePermission_Console : ePermission_Operator), mAction(action)
{
}

bool ServerControlChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() != 1)
        return false;
    if (mAction == eAction_SaveOff || mAction == eAction_SaveOn)
    {
        ServerControl::SetAutosave(mAction == eAction_SaveOn);
        Tell(source, mAction == eAction_SaveOn ? L"Automatic saving is now on" : L"Automatic saving is now off");
        return true;
    }
    if (mAction == eAction_SaveAll)
    {
        ServerControl::RequestSave();
        Tell(source, L"Saving the game");
        return true;
    }
    if (!CMinecraftApp::IsDedicatedServer())
    {
        Fail(source, L"A game hosted from the client stops when its host leaves it with Exit Game");
        return true;
    }
    ServerControl::RequestStop();
    Tell(source, L"Stopping the server");
    return true;
}
