#include "game/server/ServerControl.hpp"

#include "game/core/CConsoleMinecraftApp.hpp"
#include "platform/CProfile.hpp"

extern CConsoleMinecraftApp gApp;
extern C_4JProfile* gProfile;

namespace
{
volatile bool sStopRequested = false;
volatile bool sAutosaveOn = true;

// a save waits its turn: the request is dropped while the server is doing something else for the primary player
void RequestServerAction(eXuiServerAction action)
{
    const int pad = gProfile->GetPrimaryPad();
    if (gApp.GetXuiServerAction(pad) == eXuiServerAction_Idle)
        gApp.SetXuiServerAction(pad, action, 0);
}
}

void ServerControl::RequestSave()
{
    RequestServerAction(eXuiServerAction_SaveGame);
}

void ServerControl::RequestAutosave()
{
    RequestServerAction(eXuiServerAction_AutoSaveGame);
}

void ServerControl::RequestStop()
{
    sStopRequested = true;
}

bool ServerControl::StopRequested()
{
    return sStopRequested;
}

void ServerControl::SetAutosave(bool on)
{
    sAutosaveOn = on;
}

bool ServerControl::AutosaveOn()
{
    return sAutosaveOn;
}
