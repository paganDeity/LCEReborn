#include "game/command/ModCommand.hpp"

#include "game/command/ChatCommands.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/core/ModHost.hpp"

ModCommand::ModCommand(const std::wstring& name, const std::wstring& usage, int permission, const std::wstring& arguments)
    : ChatCommand(name, usage, arguments.empty() ? std::wstring(L"text") : arguments, permission)
{
}

bool ModCommand::run(const CommandSource& source, const Words& words)
{
    std::wstring reply;
    const std::wstring sender = SourceName(source);
    if (!ModHost::Get().Command(getName(), sender, CommandArguments::Join(words, 1), &reply))
        return true;
    if (reply.empty())
        return true;
    if (source.output)
        ChatCommands::Reply(source.output, reply);
    else
        ChatCommands::Echo(reply);
    return true;
}

void ModCommand::Sync()
{
    const std::vector<ModHost::CommandInfo> declared = ModHost::Get().Commands();
    std::vector<ChatCommand*> registered;
    const std::vector<ChatCommand*>& all = ChatCommands::Commands();
    for (size_t index = 0; index < all.size(); ++index)
    {
        if (dynamic_cast<ModCommand*>(all[index]) != NULL)
            registered.push_back(all[index]);
    }
    for (size_t index = 0; index < registered.size(); ++index)
    {
        bool still = false;
        for (size_t other = 0; other < declared.size() && !still; ++other)
            still = declared[other].name == registered[index]->getName()
                    && declared[other].permission == registered[index]->getPermission();
        if (!still)
            ChatCommands::Unregister(registered[index]);
    }
    for (size_t index = 0; index < declared.size(); ++index)
    {
        if (ChatCommands::Find(declared[index].name) == NULL)
            ChatCommands::Register(new ModCommand(declared[index].name, declared[index].usage, declared[index].permission, declared[index].arguments));
    }
}
