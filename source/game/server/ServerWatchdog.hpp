#ifndef GAME_SERVER_SERVERWATCHDOG_HPP
#define GAME_SERVER_SERVERWATCHDOG_HPP

// Whether the server's loop is still going round. The loop says so each time round (Beat, paused or not), and whoever runs the program asks
// how long ago that was: a server that has stopped going round for a minute is stuck, not slow, and a save taken from it could be half
// written. A program with no window ends itself then; the game, whose player can see it and close it, says so in the log.
namespace ServerWatchdog
{
void Beat(long long nowMs);
// stop watching: the server is not running (it is starting, or it has stopped)
void Rest();
// whole seconds since the loop last went round; 0 while not watching
int StalledSeconds(long long nowMs);

// how long the loop may stand still before the program acts; 0 is never
int Limit();
void SetLimit(int seconds);
bool Stalled(long long nowMs);
}

#endif
