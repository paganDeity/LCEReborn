#ifndef MOD_MODINSTANCE_HPP
#define MOD_MODINSTANCE_HPP

#include <string>
#include <utility>
#include <vector>

#include "archive/Archive.hpp"
#include "mod/ModDataFiles.hpp"
#include "mod/ModLimits.hpp"
#include "mod/ModServices.hpp"

namespace mod
{
class Instance;

class EventBus
{
public:
    virtual ~EventBus() {}

    virtual bool Fire(const std::string& name, std::vector<Value>& arguments, const Instance* from) = 0;
};

class Context
{
public:
    Context() : services(NULL), bus(NULL), source(NULL) {}

    Services* services;
    EventBus* bus;
    Limits limits;
    const archive::Archive* source;
    DataFiles data;
    std::vector<std::pair<std::string, const archive::Archive*> > dependencies;
};

// a command a mod answers: what the game lists in /help and checks the sender's permission against (0 anyone, 2 operators, 4 console)
struct CommandSpec
{
    std::string name;
    std::string usage;
    int permission;
    // the kinds of the arguments in turn, in the game's own words (targets, coord, number, item, text ...), which the command box completes by
    std::string arguments;
};

class Instance
{
public:
    virtual ~Instance() {}

    virtual bool Start(std::string* error) = 0;
    virtual bool Command(const std::string& name, const std::string& sender, const std::string& arguments,
                         std::string* reply) = 0;
    virtual bool Hook(int hook, std::vector<Value>& arguments, bool* cancel) = 0;
    virtual bool Custom(const std::string& name, std::vector<Value>& arguments, bool* cancel) = 0;
    virtual const std::vector<int>& Hooks() const = 0;
    virtual int HookPriority(int hook) const = 0;
    virtual const std::vector<std::string>& Listens() const = 0;
    virtual const std::vector<CommandSpec>& Commands() const = 0;
    virtual bool Failed() const = 0;
    virtual std::string LastError() const = 0;
};
}

#endif
