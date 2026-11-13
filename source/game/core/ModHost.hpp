#ifndef GAME_CORE_MODHOST_HPP
#define GAME_CORE_MODHOST_HPP

#include <string>
#include <vector>

#include "mod/ModValue.hpp"

class ModHost
{
public:
    class Entry
    {
    public:
        std::string folder;
        std::string id;
        std::string name;
        std::string version;
        std::string state;
        std::string error;
        std::vector<std::string> commands;
        std::vector<std::string> depends;
    };

    // a command a mod answers, as the game lists it
    class CommandInfo
    {
    public:
        std::wstring name;
        std::wstring usage;
        int permission;
        std::wstring arguments;
    };

    virtual ~ModHost() {}

    static ModHost& Get();

    virtual bool Enabled() const = 0;
    virtual void Start(const std::string& directory) = 0;
    virtual void Reload() = 0;
    virtual std::vector<Entry> List() = 0;

    virtual bool RunHook(int hook, std::vector<mod::Value>& arguments) = 0;
    // the id and version of each mod that listens to a hook, one per line, in id order
    virtual std::string HookOwners(int hook) = 0;
    virtual void Update() = 0;
    virtual bool Overlay(const std::string& relative, std::string* path) = 0;
    virtual std::vector<CommandInfo> Commands() = 0;
    virtual bool Command(const std::wstring& name, const std::wstring& sender, const std::wstring& arguments,
                         std::wstring* reply) = 0;
};

#endif
