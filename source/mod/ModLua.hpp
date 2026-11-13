#ifndef MOD_MODLUA_HPP
#define MOD_MODLUA_HPP

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "mod/ModInstance.hpp"
#include "mod/ModManifest.hpp"

struct lua_State;

namespace mod
{
class LuaInstance : public Instance
{
public:
    LuaInstance(const Context& context, const Manifest& manifest);
    virtual ~LuaInstance();

    virtual bool Start(std::string* error);
    virtual bool Command(const std::string& name, const std::string& sender, const std::string& arguments,
                         std::string* reply);
    virtual bool Hook(int hook, std::vector<Value>& arguments, bool* cancel);
    virtual bool Custom(const std::string& name, std::vector<Value>& arguments, bool* cancel);
    virtual const std::vector<int>& Hooks() const { return m_hooks; }
    virtual int HookPriority(int hook) const;
    virtual const std::vector<std::string>& Listens() const { return m_listens; }
    virtual const std::vector<CommandSpec>& Commands() const { return m_commands; }
    virtual bool Failed() const { return m_failures >= m_context.limits.failureLimit; }
    virtual std::string LastError() const { return m_error; }

    const DataFiles& Data() const { return m_context.data; }
    Services* GetServices() const { return m_context.services; }
    EventBus* Bus() const { return m_context.bus; }
    size_t MemoryLimit() const { return m_memoryLimit; }
    int InstructionBudget() const { return m_instructionBudget; }
    const std::string& Id() const { return m_manifest.id; }
    const archive::Archive* Source() const { return m_requireSources.back(); }
    const archive::Archive* DependencySource(const std::string& id) const;
    void PushSource(const archive::Archive* source) { m_requireSources.push_back(source); }
    void PopSource() { m_requireSources.pop_back(); }
    void AddCommand(const std::string& name, const std::string& usage = std::string(), int permission = 0, const std::string& arguments = std::string())
    {
        CommandSpec spec = {name, usage, permission, arguments};
        m_commands.push_back(spec);
    }
    void AddHook(int hook, int priority);
    void AddListener(const std::string& name);
    void SetBudget(int budget) { m_budget = budget; }
    int Budget() const { return m_budget; }
    void NoteFailure(const std::string& message);

    size_t m_used;

private:
    bool Call(int arguments, int results, std::string* error);
    void Fail(const std::string& message);

    Context m_context;
    Manifest m_manifest;
    size_t m_memoryLimit;
    int m_instructionBudget;
    std::vector<const archive::Archive*> m_requireSources;
    std::vector<int> m_hookPriorities;
    std::vector<std::string> m_listens;
    lua_State* m_state;
    std::vector<CommandSpec> m_commands;
    std::vector<int> m_hooks;
    int m_failures;
    int m_budget;
    std::string m_error;
};
}

#endif
