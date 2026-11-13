#ifndef MOD_MODMANAGER_HPP
#define MOD_MODMANAGER_HPP

#include <stdint.h>

#include <string>
#include <vector>

#include "mod/ModInstance.hpp"
#include "mod/ModLimits.hpp"
#include "mod/ModManifest.hpp"
#include "mod/ModServices.hpp"

namespace mod
{
class Manager : public EventBus
{
public:
    class Status
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

    explicit Manager(Services* services);
    virtual ~Manager();

    void SetDirectory(const std::string& directory);
    void SetLimits(const Limits& limits) { m_limits = limits; }
    const Limits& GetLimits() const { return m_limits; }
    void LoadAll();
    void Reload();
    bool Poll(int64_t nowMs);

    bool HooksActive(int hook) const;
    // the id and version of every running mod that listens to a hook, in id order
    std::vector<std::string> HookOwners(int hook) const;
    bool RunHook(int hook, std::vector<Value>& arguments);
    virtual bool Fire(const std::string& name, std::vector<Value>& arguments, const Instance* from);
    bool Command(const std::string& name, const std::string& sender, const std::string& arguments, std::string* reply);
    std::vector<CommandSpec> CommandSpecs() const;
    std::vector<Status> List() const;
    std::vector<std::string> DataDirectories() const;

private:
    class Record;

    void Scan();
    bool Candidates(std::vector<std::string>* names) const;
    bool Stamp(const Record& record, int64_t* seconds, int64_t* nanoseconds, uint64_t* size, bool entry) const;
    Record* Prepare(const std::string& folder);
    void StartPending();
    bool Start(Record* record);
    Record* FindId(const std::string& id) const;
    int Find(const std::string& folder) const;
    void Unload(Record* record);
    void Release(Record* record);
    void CollectDependents(Record* record, std::vector<Record*>& out) const;
    void Clear();
    std::vector<Instance*> Ordered() const;

    Services* m_services;
    Limits m_limits;
    std::string m_directory;
    std::vector<Record*> m_records;
    int64_t m_lastPoll;
    unsigned int m_rank;
};
}

#endif
