#ifndef GAME_WORLD_CHUNK_SCRIPTEDCHUNKGENERATOR_HPP
#define GAME_WORLD_CHUNK_SCRIPTEDCHUNKGENERATOR_HPP

#include "game/world/chunk/ChunkGenerator.hpp"

class ScriptedChunkGenerator : public ChunkGenerator
{
public:
    explicit ScriptedChunkGenerator(Level* level);

    virtual LevelChunk* createChunk(int chunkX, int chunkZ);
    virtual void postProcess(int chunkX, int chunkZ);
    virtual bool postProcessLoadedChunk(LevelChunk* chunk, int chunkX, int chunkZ);
    virtual std::vector<Biome::MobSpawnerData*>* getMobsAt(MobCategory* category, const BlockPos& pos);
    virtual BlockPos* findNearestMapFeature(Level* level, const std::wstring& feature, const BlockPos& pos,
                                            bool takeBlock);
    virtual void recreateLogicStructuresForChunk(LevelChunk* chunk, int chunkX, int chunkZ);
    virtual bool isPosInFeature(Level* level, const std::wstring& feature, const BlockPos& pos);

private:
    Level* mLevel;
};

#endif
