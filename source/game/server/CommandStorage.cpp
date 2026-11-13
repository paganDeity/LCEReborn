#include "game/server/CommandStorage.hpp"

#include "game/data/CompoundTag.hpp"
#include "game/data/SavedDataStorage.hpp"
#include "game/data/WorldSavedData.hpp"

const std::wstring CommandStorage::FILE_ID = L"command_storage";

namespace
{
boost::shared_ptr<SavedData> MakeCommandStorage(const std::wstring& id)
{
    return boost::shared_ptr<SavedData>(new CommandStorage(id));
}

struct Registration
{
    Registration() { SavedDataStorage::RegisterType(typeid(CommandStorage), MakeCommandStorage); }
} sRegistration;
}

CommandStorage::CommandStorage(const std::wstring& id) : SavedData(id)
{
}

CommandStorage::~CommandStorage()
{
    for (std::map<std::wstring, CompoundTag*>::iterator it = mEntries.begin(); it != mEntries.end(); ++it)
        delete it->second;
}

void CommandStorage::load(CompoundTag* tag)
{
    for (std::map<std::wstring, CompoundTag*>::iterator it = mEntries.begin(); it != mEntries.end(); ++it)
        delete it->second;
    mEntries.clear();
    CompoundTag* contents = tag->getCompound(L"contents");
    std::vector<std::wstring>* keys = contents->getAllKeys();
    for (size_t index = 0; index < keys->size(); ++index)
        mEntries[(*keys)[index]] = static_cast<CompoundTag*>(contents->getCompound((*keys)[index])->copy());
    delete keys;
}

CompoundTag* CommandStorage::save(CompoundTag* tag)
{
    CompoundTag* contents = new CompoundTag();
    for (std::map<std::wstring, CompoundTag*>::const_iterator it = mEntries.begin(); it != mEntries.end(); ++it)
        contents->put(it->first, it->second->copy());
    tag->put(L"contents", contents);
    return tag;
}

CompoundTag* CommandStorage::read(const std::wstring& id) const
{
    std::map<std::wstring, CompoundTag*>::const_iterator it = mEntries.find(id);
    return it != mEntries.end() ? static_cast<CompoundTag*>(it->second->copy()) : new CompoundTag();
}

void CommandStorage::write(const std::wstring& id, CompoundTag* tag)
{
    std::map<std::wstring, CompoundTag*>::iterator it = mEntries.find(id);
    if (it != mEntries.end())
        delete it->second;
    mEntries[id] = static_cast<CompoundTag*>(tag->copy());
    setDirty();
}

boost::shared_ptr<CommandStorage> CommandStorage::get()
{
    return WorldSavedData<CommandStorage>(FILE_ID);
}
