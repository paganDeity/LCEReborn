#ifndef MOD_MODWASM_HPP
#define MOD_MODWASM_HPP

#include <stdint.h>

#include <string>
#include <utility>
#include <vector>

#include "mod/ModInstance.hpp"
#include "mod/ModManifest.hpp"

struct M3Environment;
struct M3Runtime;
struct M3Function;

namespace mod
{
class WasmInstance : public Instance
{
public:
    WasmInstance(const Context& context, const Manifest& manifest);
    virtual ~WasmInstance();

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
    const archive::Archive* Source() const { return m_context.source; }
    Services* GetServices() const { return m_context.services; }
    EventBus* Bus() const { return m_context.bus; }
    void AddListener(const std::string& name);
    const std::string& Id() const { return m_manifest.id; }
    void AddCommand(const std::string& name, const std::string& usage = std::string(), int permission = 0, const std::string& arguments = std::string())
    {
        CommandSpec spec = {name, usage, permission, arguments};
        m_commands.push_back(spec);
    }
    void AppendReply(const std::string& text) { m_reply += text; }
    void AddHook(int hook, int priority);
    void SetArgument(int index, const Value& value) { m_changes.push_back(std::make_pair(index, value)); }

private:
    int64_t Budget() const;
    bool Deliver(const std::string& blob, int32_t* result);

    Context m_context;
    Manifest m_manifest;
    uint32_t m_memoryLimit;
    std::vector<std::string> m_listens;
    M3Function* m_customEntry;
    M3Environment* m_environment;
    M3Runtime* m_runtime;
    M3Function* m_command;
    M3Function* m_allocate;
    M3Function* m_hookEntry;
    std::vector<std::pair<int, Value> > m_changes;
    std::vector<int> m_hooks;
    std::vector<int> m_hookPriorities;
    std::string m_binary;
    std::vector<CommandSpec> m_commands;
    std::string m_reply;
    std::string m_error;
    int m_failures;
};
}

#endif
