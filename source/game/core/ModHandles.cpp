#include "game/core/ModHandles.hpp"

#include <utility>
#include <vector>

#include "game/entity/mob/Entity.hpp"

#if defined(_MSC_VER)
#define MOD_THREAD_LOCAL __declspec(thread)
#else
#define MOD_THREAD_LOCAL __thread
#endif

namespace
{
struct State
{
    State() : depth(0) {}
    std::vector<boost::shared_ptr<Entity> > entities;
    std::vector<ModHandles::GuiInfo> guis;
    std::vector<ModHandles::WorldInfo> worlds;
    std::vector<std::pair<void*, const void*> > objects;
    // the owner of an object that has one, at the same index as the object; empty for an object the game owns by itself
    std::vector<boost::shared_ptr<void> > owners;
    // objects made by a shared cast while a raw pointer to them is in use, kept until the scope ends
    std::vector<boost::shared_ptr<void> > cast;
    int depth;
};

MOD_THREAD_LOCAL State* sState = NULL;
MOD_THREAD_LOCAL const std::string* sCaller = NULL;

State& Mine()
{
    if (sState == NULL)
        sState = new State();
    return *sState;
}
}

#include <map>

#include "game/core/ModOwned.hpp"

namespace
{
const uint32_t kPersistentBit = 0x80000000u;
const size_t kMaxPersistentPerMod = 64;

struct Persistent
{
    boost::shared_ptr<void> owner;
    void* pointer;
    const void* key;
    std::string mod;
};

class PersistentObjects : public ModOwnedRegistryBase
{
public:
    PersistentObjects() : mNext(0) { InitializeCriticalSection(&mLock); }

    uint32_t Add(const boost::shared_ptr<void>& owner, void* pointer, const void* key, const std::string& mod)
    {
        EnterCriticalSection(&mLock);
        size_t owned = 0;
        for (std::map<uint32_t, Persistent>::const_iterator it = mItems.begin(); it != mItems.end(); ++it)
            owned += it->second.mod == mod ? 1 : 0;
        uint32_t id = 0;
        if (owned < kMaxPersistentPerMod && pointer != NULL)
        {
            id = kPersistentBit | ++mNext;
            Persistent entry = {owner, pointer, key, mod};
            mItems[id] = entry;
        }
        LeaveCriticalSection(&mLock);
        return id;
    }

    bool Find(uint32_t id, const std::string& mod, Persistent* out)
    {
        EnterCriticalSection(&mLock);
        std::map<uint32_t, Persistent>::const_iterator found = mItems.find(id);
        const bool good = found != mItems.end() && found->second.mod == mod;
        if (good)
            *out = found->second;
        LeaveCriticalSection(&mLock);
        return good;
    }

    void ReleaseObject(const void* pointer, const std::string& mod)
    {
        EnterCriticalSection(&mLock);
        for (std::map<uint32_t, Persistent>::iterator it = mItems.begin(); it != mItems.end(); ++it)
        {
            if (it->second.pointer == pointer && it->second.mod == mod)
            {
                mItems.erase(it);
                break;
            }
        }
        LeaveCriticalSection(&mLock);
    }

    void Release(uint32_t id, const std::string& mod)
    {
        EnterCriticalSection(&mLock);
        std::map<uint32_t, Persistent>::iterator found = mItems.find(id);
        if (found != mItems.end() && found->second.mod == mod)
            mItems.erase(found);
        LeaveCriticalSection(&mLock);
    }

protected:
    virtual void ReleaseOwner(const std::string& owner)
    {
        EnterCriticalSection(&mLock);
        for (std::map<uint32_t, Persistent>::iterator it = mItems.begin(); it != mItems.end();)
        {
            if (it->second.mod == owner)
                mItems.erase(it++);
            else
                ++it;
        }
        LeaveCriticalSection(&mLock);
    }

private:
    C4JCriticalSection mLock;
    std::map<uint32_t, Persistent> mItems;
    uint32_t mNext;
};

PersistentObjects& Persistents()
{
    static PersistentObjects objects;
    return objects;
}
}

namespace ModHandles
{
uint32_t AddPersistentObject(const boost::shared_ptr<void>& owner, void* pointer, const void* key)
{
    return Persistents().Add(owner, pointer, key, Caller::Mod());
}

void ReleasePersistent(uint32_t id)
{
    Persistents().Release(id, Caller::Mod());
}

Caller::Caller(const std::string& mod) : mPrevious(sCaller)
{
    sCaller = &mod;
}

Caller::~Caller()
{
    sCaller = mPrevious;
}

const std::string& Caller::Mod()
{
    static const std::string none;
    return sCaller != NULL ? *sCaller : none;
}

Scope::Scope()
{
    ++Mine().depth;
}

Scope::~Scope()
{
    State& state = Mine();
    if (--state.depth == 0)
    {
        state.entities.clear();
        state.guis.clear();
        state.worlds.clear();
        state.objects.clear();
        state.owners.clear();
        state.cast.clear();
    }
}

uint32_t AddObject(void* pointer, const void* key)
{
    // a null pointer is no object: the handle for it is never valid, so a call on it fails instead of running on nothing
    if (pointer == NULL)
        return 0;
    State& state = Mine();
    state.objects.push_back(std::make_pair(pointer, key));
    state.owners.push_back(boost::shared_ptr<void>());
    return static_cast<uint32_t>(state.objects.size());
}

uint32_t AddSharedObject(const boost::shared_ptr<void>& owner, void* pointer, const void* key)
{
    const uint32_t id = AddObject(pointer, key);
    if (id != 0)
        Mine().owners[id - 1] = owner;
    return id;
}

namespace
{
struct SharedCast
{
    const void* from;
    const void* to;
    ModHandles::SharedCaster cast;
};

std::vector<SharedCast>& SharedCasts()
{
    static std::vector<SharedCast> casts;
    return casts;
}
}

void AddSharedCast(const void* from, const void* to, SharedCaster cast)
{
    SharedCast entry = {from, to, cast};
    SharedCasts().push_back(entry);
}

void ReleasePersistentObject(const void* pointer)
{
    Persistents().ReleaseObject(pointer, Caller::Mod());
}

bool GetSharedObject(uint32_t id, const void* key, boost::shared_ptr<void>* owner)
{
    State& state = Mine();
    if ((id & kPersistentBit) != 0)
    {
        Persistent kept;
        if (!Persistents().Find(id, Caller::Mod(), &kept) || kept.key != key)
            return false;
        *owner = kept.owner;
        return true;
    }
    if (id < 1 || id > state.objects.size() || !state.owners[id - 1])
        return false;
    if (state.objects[id - 1].second == key)
    {
        *owner = state.owners[id - 1];
        return true;
    }
    const std::vector<SharedCast>& casts = SharedCasts();
    for (size_t index = 0; index < casts.size(); ++index)
    {
        if (casts[index].from == state.objects[id - 1].second && casts[index].to == key)
        {
            *owner = casts[index].cast(state.owners[id - 1]);
            return static_cast<bool>(*owner);
        }
    }
    return false;
}

namespace
{
struct Base
{
    const void* derived;
    const void* base;
    ModHandles::Upcaster upcast;
};

std::vector<Base>& Bases()
{
    static std::vector<Base> bases;
    return bases;
}
}

void AddBase(const void* derived, const void* base, Upcaster upcast)
{
    Base entry = {derived, base, upcast};
    Bases().push_back(entry);
}

bool GetObject(uint32_t id, const void* key, void** pointer)
{
    State& state = Mine();
    void* found = NULL;
    const void* actual = NULL;
    if ((id & kPersistentBit) != 0)
    {
        Persistent kept;
        if (!Persistents().Find(id, Caller::Mod(), &kept))
            return false;
        found = kept.pointer;
        actual = kept.key;
    }
    else
    {
        if (id < 1 || id > state.objects.size())
            return false;
        found = state.objects[id - 1].first;
        actual = state.objects[id - 1].second;
    }
    if (actual != key)
    {
        // the registered casts go one class at a time from the nearest base, so the first that reaches the wanted class wins
        bool converted = false;
        const std::vector<Base>& bases = Bases();
        for (size_t index = 0; index < bases.size() && !converted; ++index)
        {
            if (bases[index].derived == actual && bases[index].base == key)
            {
                found = bases[index].upcast(found);
                converted = true;
            }
        }
        // an object the game shares may still be the wanted class through a shared cast: the owner is cast and kept for the scope
        const std::vector<SharedCast>& casts = SharedCasts();
        for (size_t index = 0; index < casts.size() && !converted && (id & kPersistentBit) == 0; ++index)
        {
            if (casts[index].from == actual && casts[index].to == key && state.owners[id - 1])
            {
                const boost::shared_ptr<void> casted = casts[index].cast(state.owners[id - 1]);
                if (casted)
                {
                    state.cast.push_back(casted);
                    found = casted.get();
                    converted = true;
                }
            }
        }
        if (!converted)
            return false;
    }
    *pointer = found;
    return true;
}

uint32_t Add(const boost::shared_ptr<Entity>& entity)
{
    State& state = Mine();
    state.entities.push_back(entity);
    return static_cast<uint32_t>(state.entities.size());
}





uint32_t AddGui(Gui* gui, Font* font, int width, int height)
{
    State& state = Mine();
    GuiInfo info = {gui, font, width, height};
    state.guis.push_back(info);
    return static_cast<uint32_t>(state.guis.size());
}

uint32_t AddWorld(const WorldInfo& world)
{
    State& state = Mine();
    state.worlds.push_back(world);
    return static_cast<uint32_t>(state.worlds.size());
}

bool Get(uint32_t id, WorldInfo* world)
{
    State& state = Mine();
    if (id < 1 || id > state.worlds.size())
        return false;
    *world = state.worlds[id - 1];
    return true;
}


bool Get(uint32_t id, GuiInfo* info)
{
    State& state = Mine();
    if (id < 1 || id > state.guis.size())
        return false;
    *info = state.guis[id - 1];
    return true;
}

bool Get(uint32_t id, boost::shared_ptr<Entity>* entity)
{
    State& state = Mine();
    if (id < 1 || id > state.entities.size() || !state.entities[id - 1])
        return false;
    *entity = state.entities[id - 1];
    return true;
}

}
