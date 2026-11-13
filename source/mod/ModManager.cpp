#include "mod/ModManager.hpp"

#include <algorithm>

#include "archive/ArchiveFiles.hpp"
#include "mod/ModLua.hpp"
#include "mod/ModVersion.hpp"
#include "mod/ModWasm.hpp"

namespace mod
{
namespace
{
const char kDataSuffix[] = "_modData";

bool EndsWith(const std::string& text, const std::string& suffix)
{
    return text.size() >= suffix.size() && text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}
}

class Manager::Record
{
public:
    Record()
        : instance(NULL), zipped(false), source(NULL), rank(0), settled(false), stampSeconds(0), stampNanoseconds(0), stampSize(0), entrySeconds(0),
          entryNanoseconds(0), entrySize(0)
    {
    }
    ~Record()
    {
        delete instance;
        delete source;
    }

    std::string folder;
    std::string path;
    std::string dataDirectory;
    bool zipped;
    archive::Archive* source;
    Manifest manifest;
    Instance* instance;
    std::string error;
    unsigned int rank;
    bool settled;
    int64_t stampSeconds;
    int64_t stampNanoseconds;
    uint64_t stampSize;
    int64_t entrySeconds;
    int64_t entryNanoseconds;
    uint64_t entrySize;
};

Manager::Manager(Services* services) : m_services(services), m_lastPoll(0), m_rank(0)
{
}

Manager::~Manager()
{
    Clear();
}

void Manager::SetDirectory(const std::string& directory)
{
    m_directory = directory;
}

void Manager::Clear()
{
    for (size_t index = 0; index < m_records.size(); ++index)
        Release(m_records[index]);
    m_records.clear();
}

int Manager::Find(const std::string& folder) const
{
    for (size_t index = 0; index < m_records.size(); ++index)
        if (m_records[index]->folder == folder)
            return static_cast<int>(index);
    return -1;
}

Manager::Record* Manager::FindId(const std::string& id) const
{
    for (size_t index = 0; index < m_records.size(); ++index)
        if (m_records[index]->instance != NULL && m_records[index]->manifest.id == id)
            return m_records[index];
    return NULL;
}

bool Manager::Candidates(std::vector<std::string>* names) const
{
    std::vector<std::string> directories;
    std::vector<std::string> files;
    if (!archive::Files::List(m_directory, &directories, &files))
        return false;
    for (size_t index = 0; index < directories.size(); ++index)
        if (!EndsWith(directories[index], kDataSuffix))
            names->push_back(directories[index]);
    for (size_t index = 0; index < files.size(); ++index)
        if (EndsWith(files[index], ".zip"))
            names->push_back(files[index]);
    std::sort(names->begin(), names->end());
    return true;
}

bool Manager::Stamp(const Record& record, int64_t* seconds, int64_t* nanoseconds, uint64_t* size, bool entry) const
{
    if (record.zipped)
    {
        if (entry)
            return false;
        return archive::Files::Stamp(record.path, seconds, nanoseconds, size);
    }
    return archive::Files::Stamp(archive::Files::Join(record.path, entry ? record.manifest.entry : std::string("mod.txt")),
                                 seconds, nanoseconds, size);
}

Manager::Record* Manager::Prepare(const std::string& folder)
{
    Record* record = new Record();
    record->folder = folder;
    record->path = archive::Files::Join(m_directory, folder);
    record->zipped = !archive::Files::IsDirectory(record->path);
    record->dataDirectory = record->zipped
                                ? archive::Files::Join(m_directory, folder.substr(0, folder.size() - 4) + kDataSuffix)
                                : archive::Files::Join(record->path, "modData");
    Stamp(*record, &record->stampSeconds, &record->stampNanoseconds, &record->stampSize, false);
    std::string problem;
    record->source = archive::Archive::Open(record->path, &problem);
    std::string text;
    if (record->source == NULL || !record->source->Read("mod.txt", text, &problem))
    {
        record->error = record->source == NULL ? problem : "there is no mod.txt";
        record->settled = true;
        return record;
    }
    if (!Manifest::Parse(text, record->manifest, &problem))
    {
        record->error = problem;
        record->settled = true;
        return record;
    }
    Stamp(*record, &record->entrySeconds, &record->entryNanoseconds, &record->entrySize, true);
    return record;
}

bool Manager::Start(Record* record)
{
    Context context;
    context.services = m_services;
    context.bus = this;
    context.limits = m_limits;
    context.source = record->source;
    context.data = DataFiles(record->dataDirectory, m_limits.maxFileBytes);
    for (size_t index = 0; index < record->manifest.depends.size(); ++index)
    {
        const Dependency& wanted = record->manifest.depends[index];
        Record* found = FindId(wanted.id);
        if (found == NULL)
        {
            record->error = "needs the mod " + wanted.id + (wanted.constraint.empty() ? "" : " " + wanted.constraint);
            return false;
        }
        if (!Version::Satisfies(found->manifest.version, wanted.constraint))
        {
            record->error = "needs " + wanted.id + " " + wanted.constraint + ", found " + found->manifest.version;
            return false;
        }
        context.dependencies.push_back(std::make_pair(wanted.id, static_cast<const archive::Archive*>(found->source)));
    }
    if (FindId(record->manifest.id) != NULL)
    {
        record->error = "another mod already has the id " + record->manifest.id;
        return false;
    }
    const std::string& entry = record->manifest.entry;
    Instance* instance;
    if (entry.size() > 5 && entry.substr(entry.size() - 5) == ".wasm")
        instance = new WasmInstance(context, record->manifest);
    else
        instance = new LuaInstance(context, record->manifest);
    std::string problem;
    if (!instance->Start(&problem))
    {
        record->error = problem;
        m_services->Log(record->manifest.id, "could not start: " + problem);
        delete instance;
        return false;
    }
    record->instance = instance;
    record->rank = ++m_rank;
    m_services->Log(record->manifest.id, "loaded " + record->manifest.name + " " + record->manifest.version);
    return true;
}

void Manager::StartPending()
{
    bool progress = true;
    while (progress)
    {
        progress = false;
        for (size_t index = 0; index < m_records.size(); ++index)
        {
            Record* record = m_records[index];
            if (record->settled || record->instance != NULL)
                continue;
            bool ready = true;
            for (size_t dependency = 0; dependency < record->manifest.depends.size() && ready; ++dependency)
            {
                const std::string& id = record->manifest.depends[dependency].id;
                if (FindId(id) != NULL)
                    continue;
                for (size_t other = 0; other < m_records.size(); ++other)
                    if (!m_records[other]->settled && m_records[other]->instance == NULL && m_records[other] != record
                        && m_records[other]->manifest.id == id)
                        ready = false;
            }
            if (!ready)
                continue;
            record->settled = true;
            progress = true;
            Start(record);
        }
    }
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        Record* record = m_records[index];
        if (!record->settled && record->instance == NULL)
        {
            record->settled = true;
            record->error = "the mods it needs wait on each other";
        }
    }
}

void Manager::Scan()
{
    std::vector<std::string> folders;
    if (!Candidates(&folders))
        return;
    for (size_t index = 0; index < folders.size() && m_records.size() < m_limits.maxMods; ++index)
        if (Find(folders[index]) < 0)
            m_records.push_back(Prepare(folders[index]));
}

void Manager::LoadAll()
{
    Scan();
    StartPending();
}

void Manager::Reload()
{
    Clear();
    m_rank = 0;
    LoadAll();
}

void Manager::CollectDependents(Record* record, std::vector<Record*>& out) const
{
    if (std::find(out.begin(), out.end(), record) != out.end())
        return;
    out.push_back(record);
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        Record* other = m_records[index];
        for (size_t dependency = 0; dependency < other->manifest.depends.size(); ++dependency)
            if (other->manifest.depends[dependency].id == record->manifest.id)
                CollectDependents(other, out);
    }
}

void Manager::Release(Record* record)
{
    const std::string id = record->manifest.id;
    const bool started = record->instance != NULL;
    delete record;
    if (started)
        m_services->Unloaded(id);
}

void Manager::Unload(Record* record)
{
    m_records.erase(std::find(m_records.begin(), m_records.end(), record));
    Release(record);
}

bool Manager::Poll(int64_t nowMs)
{
    if (nowMs - m_lastPoll < m_limits.pollMilliseconds)
        return false;
    m_lastPoll = nowMs;
    std::vector<std::string> folders;
    Candidates(&folders);
    std::vector<Record*> stale;
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        Record* record = m_records[index];
        bool gone = std::find(folders.begin(), folders.end(), record->folder) == folders.end();
        bool changed = gone;
        if (!gone)
        {
            int64_t seconds = 0, nanoseconds = 0, entrySeconds = 0, entryNanoseconds = 0;
            uint64_t size = 0, entrySize = 0;
            const bool hasManifest = Stamp(*record, &seconds, &nanoseconds, &size, false);
            changed = hasManifest != (record->stampSize != 0 || record->stampSeconds != 0) || seconds != record->stampSeconds
                      || nanoseconds != record->stampNanoseconds || size != record->stampSize;
            if (!changed && !record->manifest.entry.empty())
            {
                const bool hasEntry = Stamp(*record, &entrySeconds, &entryNanoseconds, &entrySize, true);
                changed = hasEntry != (record->entrySize != 0 || record->entrySeconds != 0) || entrySeconds != record->entrySeconds
                          || entryNanoseconds != record->entryNanoseconds || entrySize != record->entrySize;
            }
        }
        if (changed)
            CollectDependents(record, stale);
    }
    bool any = !stale.empty();
    std::vector<std::string> reload;
    for (size_t index = 0; index < stale.size(); ++index)
    {
        m_services->Log(stale[index]->manifest.id.empty() ? stale[index]->folder : stale[index]->manifest.id, "changed, reloading");
        if (std::find(folders.begin(), folders.end(), stale[index]->folder) != folders.end())
            reload.push_back(stale[index]->folder);
    }
    for (size_t index = 0; index < stale.size(); ++index)
        Unload(stale[index]);
    const size_t before = m_records.size();
    Scan();
    if (m_records.size() != before)
        any = true;
    if (any)
        StartPending();
    return any;
}

std::vector<std::string> Manager::DataDirectories() const
{
    std::vector<std::pair<unsigned int, std::string> > ranked;
    for (size_t index = 0; index < m_records.size(); ++index)
        if (m_records[index]->instance != NULL)
            ranked.push_back(std::make_pair(m_records[index]->rank, m_records[index]->dataDirectory));
    std::sort(ranked.begin(), ranked.end());
    std::vector<std::string> directories;
    for (size_t index = 0; index < ranked.size(); ++index)
        directories.push_back(ranked[index].second);
    return directories;
}

std::vector<Instance*> Manager::Ordered() const
{
    std::vector<std::pair<unsigned int, Instance*> > ranked;
    for (size_t index = 0; index < m_records.size(); ++index)
        if (m_records[index]->instance != NULL)
            ranked.push_back(std::make_pair(m_records[index]->rank, m_records[index]->instance));
    std::sort(ranked.begin(), ranked.end());
    std::vector<Instance*> instances;
    for (size_t index = 0; index < ranked.size(); ++index)
        instances.push_back(ranked[index].second);
    return instances;
}

bool Manager::HooksActive(int hook) const
{
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        if (m_records[index]->instance == NULL || m_records[index]->instance->Failed())
            continue;
        const std::vector<int>& hooks = m_records[index]->instance->Hooks();
        for (size_t at = 0; at < hooks.size(); ++at)
            if (hooks[at] == hook)
                return true;
    }
    return false;
}

std::vector<std::string> Manager::HookOwners(int hook) const
{
    std::vector<std::string> owners;
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        if (m_records[index]->instance == NULL || m_records[index]->instance->Failed())
            continue;
        const std::vector<int>& hooks = m_records[index]->instance->Hooks();
        for (size_t at = 0; at < hooks.size(); ++at)
        {
            if (hooks[at] == hook)
            {
                owners.push_back(m_records[index]->manifest.id + " " + m_records[index]->manifest.version);
                break;
            }
        }
    }
    std::sort(owners.begin(), owners.end());
    return owners;
}

bool Manager::RunHook(int hook, std::vector<Value>& arguments)
{
    std::vector<std::pair<std::pair<int, unsigned int>, Instance*> > order;
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        Record* record = m_records[index];
        if (record->instance == NULL)
            continue;
        const std::vector<int>& hooks = record->instance->Hooks();
        for (size_t at = 0; at < hooks.size(); ++at)
            if (hooks[at] == hook)
                order.push_back(std::make_pair(std::make_pair(record->instance->HookPriority(hook), record->rank), record->instance));
    }
    std::sort(order.begin(), order.end());
    bool cancel = false;
    for (size_t index = 0; index < order.size() && !cancel; ++index)
        order[index].second->Hook(hook, arguments, &cancel);
    return !cancel;
}

bool Manager::Fire(const std::string& name, std::vector<Value>& arguments, const Instance* from)
{
    bool cancel = false;
    const std::vector<Instance*> instances = Ordered();
    for (size_t index = 0; index < instances.size() && !cancel; ++index)
    {
        Instance* instance = instances[index];
        if (instance == from)
            continue;
        const std::vector<std::string>& listens = instance->Listens();
        if (std::find(listens.begin(), listens.end(), name) != listens.end())
            instance->Custom(name, arguments, &cancel);
    }
    return !cancel;
}

bool Manager::Command(const std::string& name, const std::string& sender, const std::string& arguments, std::string* reply)
{
    const std::vector<Instance*> instances = Ordered();
    for (size_t index = 0; index < instances.size(); ++index)
        if (instances[index]->Command(name, sender, arguments, reply))
            return true;
    return false;
}

std::vector<CommandSpec> Manager::CommandSpecs() const
{
    std::vector<CommandSpec> specs;
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        if (m_records[index]->instance == NULL)
            continue;
        const std::vector<CommandSpec>& own = m_records[index]->instance->Commands();
        specs.insert(specs.end(), own.begin(), own.end());
    }
    return specs;
}

std::vector<Manager::Status> Manager::List() const
{
    std::vector<Status> list;
    for (size_t index = 0; index < m_records.size(); ++index)
    {
        const Record* record = m_records[index];
        Status status;
        status.folder = record->folder;
        status.id = record->manifest.id;
        status.name = record->manifest.name;
        status.version = record->manifest.version;
        if (record->instance == NULL)
        {
            status.state = "error";
            status.error = record->error;
        }
        else if (record->instance->Failed())
        {
            status.state = "failed";
            status.error = record->instance->LastError();
        }
        else
            status.state = "running";
        if (record->instance != NULL)
            for (size_t command = 0; command < record->instance->Commands().size(); ++command)
                status.commands.push_back(record->instance->Commands()[command].name);
        for (size_t dependency = 0; dependency < record->manifest.depends.size(); ++dependency)
            status.depends.push_back(record->manifest.depends[dependency].id);
        list.push_back(status);
    }
    return list;
}
}
