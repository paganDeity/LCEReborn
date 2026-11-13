#ifndef GAME_SERVER_SERVERCONTROL_HPP
#define GAME_SERVER_SERVERCONTROL_HPP

// what an operator or the console asks of the running server, the same for the dedicated server and a game hosted from
// the client: a save now, and for the dedicated server a stop, which its main loop carries out by saving and exiting
namespace ServerControl
{
void RequestSave();
// the save a server makes on its own every few minutes
void RequestAutosave();
void RequestStop();
bool StopRequested();
// /save-off and /save-on: an autosave from either server's timer is skipped while it is off; a save asked for still runs
void SetAutosave(bool on);
bool AutosaveOn();
}

#endif
