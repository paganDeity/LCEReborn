#ifndef GAME_COMMAND_COMMANDLIMITS_HPP
#define GAME_COMMAND_COMMANDLIMITS_HPP

#include <string>

namespace CommandLimits
{
int ExecuteForks();
int CompareBlocks();
int CommandChain();
bool Apply(const std::string& text, std::string* error);
}

#endif
