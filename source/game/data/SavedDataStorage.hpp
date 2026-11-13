#ifndef GAME_DATA_SAVEDDATASTORAGE_HPP
#define GAME_DATA_SAVEDDATASTORAGE_HPP

#include <string>
#include <typeinfo>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "game/data/SavedData.hpp"
#include "game/world/generation/PlayerUID.hpp"

class LevelStorage;

class SavedDataStorage
{
public:
    SavedDataStorage(LevelStorage* levelStorage);
    virtual ~SavedDataStorage() {}

    virtual boost::shared_ptr<SavedData> get(const std::type_info& type, const std::wstring& id);
    virtual void set(const std::wstring& id, boost::shared_ptr<SavedData> data);
    virtual void save();
    virtual int getFreeAuxValueFor(const std::wstring& id);

    void save(boost::shared_ptr<SavedData> data);
    int getAuxValueForMap(PlayerUID uid, int dimension, int x, int z, int scale);

    // a saved data type the project adds makes itself from its file's id; the game's own types are made in get
    typedef boost::shared_ptr<SavedData> (*Maker)(const std::wstring& id);
    static void RegisterType(const std::type_info& type, Maker maker);
    static Maker MakerFor(const std::type_info& type);

protected:
    void loadAuxValues();

    LevelStorage* mLevelStorage;
    boost::unordered_map<std::wstring, boost::shared_ptr<SavedData> > mSavedData;
    std::vector<boost::shared_ptr<SavedData> > mAllSavedData;
    boost::unordered_map<std::wstring, short> mAuxValues;
};

#endif
