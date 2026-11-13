#ifndef GAME_COMMAND_CHAT_EXECUTECHATCOMMAND_HPP
#define GAME_COMMAND_CHAT_EXECUTECHATCOMMAND_HPP

#include "game/command/ChatCommand.hpp"

class ExecuteChatCommand : public ChatCommand
{
public:
    ExecuteChatCommand();

    virtual bool run(const CommandSource& source, const Words& words);

private:
    static bool Condition(const CommandSource& source, const CommandSource& at, const Words& words, size_t index,
                          size_t* length, bool* passed);
    static bool Fork(const CommandSource& source, const std::vector<CommandSource>& from, const std::wstring& word,
                     bool moveTo, bool rotate, bool becomeEntity, std::vector<CommandSource>& to);
};

#endif
