#ifndef GAME_DATA_WORLDSAVEDDATA_HPP
#define GAME_DATA_WORLDSAVEDDATA_HPP

#include <string>

#include <boost/shared_ptr.hpp>

#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerLevel.hpp"

// the overworld's record of a saved data type the project adds, read from the save the first time and made empty when
// the save has none, or none when no world is running
template <class T> boost::shared_ptr<T> WorldSavedData(const std::wstring& id)
{
    ServerLevel* level = gMinecraftServer != NULL ? gMinecraftServer->getLevel(0) : NULL;
    if (level == NULL || level->getSavedDataStorage() == NULL)
        return boost::shared_ptr<T>();
    boost::shared_ptr<T> data = boost::dynamic_pointer_cast<T>(level->getSavedData(typeid(T), id));
    if (!data)
    {
        data = boost::shared_ptr<T>(new T(id));
        level->setSavedData(id, data);
    }
    return data;
}

#endif
