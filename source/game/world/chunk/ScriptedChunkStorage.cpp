#include "game/world/chunk/ScriptedChunkStorage.hpp"

#include <string.h>

#include <string>

#include "game/core/ByteArrayInputStream.hpp"
#include "game/core/ByteArrayOutputStream.hpp"
#include "game/core/DataInputStream.hpp"
#include "game/core/DataOutputStream.hpp"
#include "game/core/ModEvents_generated.hpp"
#include "game/world/Level.hpp"
#include "game/world/chunk/LevelChunk.hpp"
#include "game/world/chunk/OldChunkStorage.hpp"

ScriptedChunkStorage::ScriptedChunkStorage(const std::wstring& world, int dimension, DataFixerUpper* fixer)
    : mWorld(world), mDimension(dimension), mFixer(fixer)
{
}

LevelChunk* ScriptedChunkStorage::load(Level* level, int x, int z, LevelChunk* chunk)
{
    std::string data;
    bool found = false;
    ModEvents::ChunkFetch(mWorld, mDimension, x, z, data, found);
    if (!found || data.empty())
        return NULL;
    arrayWithLength<unsigned char> bytes(static_cast<unsigned int>(data.size()), true);
    memcpy(bytes.data, data.data(), data.size());
    ByteArrayInputStream input(bytes);
    DataInputStream stream(&input);
    return OldChunkStorage::load(level, &stream, mFixer, chunk);
}

void ScriptedChunkStorage::save(Level* level, LevelChunk* chunk)
{
    level->checkSession();
    ByteArrayOutputStream output;
    DataOutputStream stream(&output);
    OldChunkStorage::save(chunk, level, &stream);
    const std::string data(reinterpret_cast<const char*>(output.mBuffer.data), static_cast<size_t>(output.mCount));
    ModEvents::ChunkStore(mWorld, mDimension, chunk->x, chunk->z, data);
}

void ScriptedChunkStorage::saveEntities(Level*, LevelChunk*)
{
}

void ScriptedChunkStorage::tick()
{
}

void ScriptedChunkStorage::flush()
{
}
