#ifndef GAME_WORLD_CHUNK_SCRIPTEDCHUNKSTORAGE_HPP
#define GAME_WORLD_CHUNK_SCRIPTEDCHUNKSTORAGE_HPP

#include <string>

#include "game/world/chunk/ChunkStorage.hpp"

class DataFixerUpper;

// chunks kept wherever a mod keeps them: the game gives it each chunk in its own serialised form and asks for it back
class ScriptedChunkStorage : public ChunkStorage
{
public:
    ScriptedChunkStorage(const std::wstring& world, int dimension, DataFixerUpper* fixer);

    virtual LevelChunk* load(Level* level, int x, int z, LevelChunk* chunk);
    virtual void save(Level* level, LevelChunk* chunk);
    virtual void saveEntities(Level* level, LevelChunk* chunk);
    virtual void tick();
    virtual void flush();

private:
    std::wstring mWorld;
    int mDimension;
    DataFixerUpper* mFixer;
};

#endif
