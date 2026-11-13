#include "game/core/ModHost.hpp"

namespace
{
class InertModHost : public ModHost
{
public:
    virtual bool Enabled() const { return false; }
    virtual void Start(const std::string&) {}
    virtual void Reload() {}
    virtual std::vector<Entry> List() { return std::vector<Entry>(); }
    virtual bool RunHook(int, std::vector<mod::Value>&) { return true; }
    virtual std::string HookOwners(int) { return std::string(); }
    virtual void Update() {}
    virtual bool Overlay(const std::string&, std::string*) { return false; }
    virtual std::vector<CommandInfo> Commands() { return std::vector<CommandInfo>(); }
    virtual bool Command(const std::wstring&, const std::wstring&, const std::wstring&, std::wstring*) { return false; }
};
}

ModHost& ModHost::Get()
{
    static InertModHost host;
    return host;
}
