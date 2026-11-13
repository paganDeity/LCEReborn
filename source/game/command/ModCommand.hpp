#ifndef GAME_COMMAND_MODCOMMAND_HPP
#define GAME_COMMAND_MODCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

// a command a mod answers, as the game's own: it is listed by /help, suggested and completed in the command box, and run only by a sender of its permission
class ModCommand : public ChatCommand
{
public:
    ModCommand(const std::wstring& name, const std::wstring& usage, int permission, const std::wstring& arguments);

    virtual bool run(const CommandSource& source, const Words& words);

    // make the registered mod commands the ones the mods now declare: new ones are added, ones whose mod is gone are removed, and a name the game or another
    // command already has is left alone
    static void Sync();
};

#endif
