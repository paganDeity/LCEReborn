#include "platform/pc/PcModHost.hpp"

#include <mutex>
#include <set>

#include "game/command/ChatCommands.hpp"
#include "game/command/ModCommand.hpp"
#include "game/command/CommandLimits.hpp"
#include "game/core/ModApiFunctions_generated.hpp"
#include "game/core/ModHandles.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModOwned.hpp"
#include "game/core/ModHost.hpp"
#include "game/core/System.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/world/generation/PlayerList.hpp"
#include "archive/ArchiveFiles.hpp"
#include "mod/ModDataFiles.hpp"
#include "mod/ModManager.hpp"
#include "platform/pc/PcPaths.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcLaunchOptions.hpp"
#include "platform/pc/PcUtf8.hpp"

namespace
{
const Logger kModLog("Mods");

class PcModHost : public ModHost, public mod::Services
{
public:
    PcModHost() : m_manager(this), m_enabled(false) {}

    virtual bool Enabled() const { return m_enabled; }
    virtual void Start(const std::string& directory);
    virtual void Reload();
    virtual std::vector<Entry> List();
    virtual bool RunHook(int hook, std::vector<mod::Value>& arguments);
    virtual std::string HookOwners(int hook);
    virtual void Update();
    virtual bool Overlay(const std::string& relative, std::string* path);
    virtual std::vector<CommandInfo> Commands();
    virtual bool Command(const std::wstring& name, const std::wstring& sender, const std::wstring& arguments,
                         std::wstring* reply);

    virtual void Log(const std::string& mod, const std::string& text);
    virtual void Say(const std::string& text);
    virtual bool Run(const std::string& line, std::string* reply);
    virtual std::vector<std::string> Players();
    virtual int64_t TimeMs();
    virtual bool ReadGameFile(const std::string& relative, std::string* data);
    virtual bool Call(const std::string& mod, int function, const std::vector<mod::Value>& arguments, mod::Value* result);
    virtual void Unloaded(const std::string& mod);

private:
    void RefreshHooks();

    std::recursive_mutex m_mutex;
    std::mutex m_overlayMutex;
    std::vector<std::string> m_overlayRoots;
    std::set<std::string> m_overlaid;
    mod::Manager m_manager;
    bool m_enabled;
};

PcModHost sHost;

void PcModHost::Start(const std::string& directory)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    // mod code runs here with no hook around it, so this scope keeps its handles valid when a hook fires inside one of its calls
    ModHandles::Scope handles;
    m_manager.SetDirectory(directory);
    std::string limitsText;
    if (archive::Files::Read(archive::Files::Join(directory, "limits.txt"), limitsText))
    {
        mod::Limits limits;
        std::string problem;
        if (limits.Apply(limitsText, &problem))
            m_manager.SetLimits(limits);
        else
            LOG_INFO(kModLog, "limits.txt ignored: %s", problem.c_str());
    }
    m_manager.LoadAll();
    m_enabled = true;
    RefreshHooks();
}

void PcModHost::Reload()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    // mod code runs here with no hook around it, so this scope keeps its handles valid when a hook fires inside one of its calls
    ModHandles::Scope handles;
    if (m_enabled)
    {
        m_manager.Reload();
        RefreshHooks();
    }
}

std::vector<ModHost::Entry> PcModHost::List()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::vector<Entry> entries;
    const std::vector<mod::Manager::Status> list = m_manager.List();
    for (size_t index = 0; index < list.size(); ++index)
    {
        Entry entry;
        entry.folder = list[index].folder;
        entry.id = list[index].id;
        entry.name = list[index].name;
        entry.version = list[index].version;
        entry.state = list[index].state;
        entry.error = list[index].error;
        entry.commands = list[index].commands;
        entry.depends = list[index].depends;
        entries.push_back(entry);
    }
    return entries;
}

void PcModHost::RefreshHooks()
{
    {
        std::lock_guard<std::mutex> lock(m_overlayMutex);
        m_overlayRoots = m_manager.DataDirectories();
    }
    for (int hook = 0; hook < kModHook_Count; ++hook)
        ModHooks::SetActive(hook, m_manager.HooksActive(hook));
}

bool PcModHost::RunHook(int hook, std::vector<mod::Value>& arguments)
{
    if (!m_enabled)
        return true;
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return m_manager.RunHook(hook, arguments);
}

std::string PcModHost::HookOwners(int hook)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::string joined;
    const std::vector<std::string> owners = m_manager.HookOwners(hook);
    for (size_t index = 0; index < owners.size(); ++index)
        joined += owners[index] + "\n";
    return joined;
}

bool PcModHost::Overlay(const std::string& relative, std::string* path)
{
    if (!mod::DataFiles::Safe(relative))
        return false;
    std::lock_guard<std::mutex> lock(m_overlayMutex);
    for (size_t index = m_overlayRoots.size(); index-- > 0;)
    {
        const std::string candidate = archive::Files::Join(archive::Files::Join(m_overlayRoots[index], "assets"), relative);
        int64_t seconds, nanoseconds;
        uint64_t size;
        if (archive::Files::Stamp(candidate, &seconds, &nanoseconds, &size))
        {
            if (m_overlaid.insert(candidate).second)
                LOG_INFO(kModLog, "overlay %s", candidate.c_str());
            *path = candidate;
            return true;
        }
    }
    return false;
}

bool PcModHost::ReadGameFile(const std::string& relative, std::string* data)
{
    return archive::Files::Read(PcGameContentFile(relative), *data);
}

void PcModHost::Unloaded(const std::string& mod)
{
    ModOwnedRegistryBase::ReleaseOwnerEverywhere(mod);
}

bool PcModHost::Call(const std::string& mod, int function, const std::vector<mod::Value>& arguments, mod::Value* result)
{
    ModHandles::Caller caller(mod);
    return ModApiDispatch(function, arguments.empty() ? NULL : &arguments[0], arguments.size(), result);
}

void PcModHost::Update()
{
    if (!m_enabled)
        return;
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    // mod code runs here with no hook around it, so this scope keeps its handles valid when a hook fires inside one of its calls
    ModHandles::Scope handles;
    m_manager.Poll(System::processTimeInMilliSecs());
    RefreshHooks();
    ModCommand::Sync();
}

std::vector<ModHost::CommandInfo> PcModHost::Commands()
{
    std::vector<CommandInfo> commands;
    if (!m_enabled)
        return commands;
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    const std::vector<mod::CommandSpec> specs = m_manager.CommandSpecs();
    for (size_t index = 0; index < specs.size(); ++index)
    {
        CommandInfo info = {PcUtf8ToWide(specs[index].name), PcUtf8ToWide(specs[index].usage), specs[index].permission,
                            PcUtf8ToWide(specs[index].arguments)};
        commands.push_back(info);
    }
    return commands;
}

bool PcModHost::Command(const std::wstring& name, const std::wstring& sender, const std::wstring& arguments,
                        std::wstring* reply)
{
    if (!m_enabled)
        return false;
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    // mod code runs here with no hook around it, so this scope keeps its handles valid when a hook fires inside one of its calls
    ModHandles::Scope handles;
    std::string text;
    if (!m_manager.Command(PcWideToUtf8(name), PcWideToUtf8(sender), PcWideToUtf8(arguments), &text))
        return false;
    *reply = PcUtf8ToWide(text);
    return true;
}

void PcModHost::Log(const std::string& mod, const std::string& text)
{
    LOG_INFO(kModLog, "%s: %s", mod.c_str(), text.c_str());
}

void PcModHost::Say(const std::string& text)
{
    ChatCommands::Announce(PcUtf8ToWide(text));
}

bool PcModHost::Run(const std::string& line, std::string* reply)
{
    std::wstring output;
    const bool ok = ChatCommands::RunCaptured(PcUtf8ToWide(line), &output);
    *reply = PcWideToUtf8(output);
    return ok;
}

std::vector<std::string> PcModHost::Players()
{
    std::vector<std::string> names;
    if (gMinecraftServer == NULL || gMinecraftServer->getPlayers() == NULL)
        return names;
    std::vector<boost::shared_ptr<ServerPlayer> >* players = gMinecraftServer->getPlayers()->getPlayers();
    for (size_t index = 0; index < players->size(); ++index)
        names.push_back(PcWideToUtf8((*players)[index]->getName()));
    return names;
}

int64_t PcModHost::TimeMs()
{
    return static_cast<int64_t>(System::processTimeInMilliSecs());
}
}

ModHost& ModHost::Get()
{
    return sHost;
}

void PcStartMods(const char* program)
{
    const PcLaunchOptions& options = PcGetLaunchOptions();
    if (!options.commandLimits.empty())
    {
        std::string problem;
        if (!CommandLimits::Apply(options.commandLimits, &problem))
            LOG_INFO(kModLog, "--command-limits ignored: %s", problem.c_str());
    }
    const std::string directory = options.modsDir.empty() ? std::string("mods") : options.modsDir;
    sHost.Start(directory);
    LOG_INFO(kModLog, "%s: mods folder %s", program, directory.c_str());
}
