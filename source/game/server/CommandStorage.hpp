#ifndef GAME_SERVER_COMMANDSTORAGE_HPP
#define GAME_SERVER_COMMANDSTORAGE_HPP

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include "game/data/SavedData.hpp"

class CompoundTag;

// the data /data keeps under a namespaced id of its own, apart from any block or entity, saved with the world
class CommandStorage : public SavedData
{
public:
    static const std::wstring FILE_ID;

    explicit CommandStorage(const std::wstring& id);
    virtual ~CommandStorage();

    virtual void load(CompoundTag* tag);
    virtual CompoundTag* save(CompoundTag* tag);

    // a copy the caller owns; an id never written reads as an empty compound
    CompoundTag* read(const std::wstring& id) const;
    // keeps a copy
    void write(const std::wstring& id, CompoundTag* tag);

    static boost::shared_ptr<CommandStorage> get();

private:
    std::map<std::wstring, CompoundTag*> mEntries;
};

#endif
