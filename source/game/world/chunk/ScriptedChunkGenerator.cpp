#include "game/world/chunk/ScriptedChunkGenerator.hpp"

#include <string.h>

#include "game/core/ModEvents_generated.hpp"
#include "game/world/Level.hpp"
#include "game/world/LevelData.hpp"
#include "game/world/biome/BiomeSource.hpp"
#include "game/world/chunk/ChunkPrimer.hpp"
#include "game/world/chunk/LevelChunk.hpp"

ScriptedChunkGenerator::ScriptedChunkGenerator(Level* level) : mLevel(level)
{
    mLevelSize = level->getLevelData()->getXZSize();
}

LevelChunk* ScriptedChunkGenerator::createChunk(int chunkX, int chunkZ)
{
    const unsigned int count = 0x8000;
    unsigned short* elementIds = new unsigned short[count];
    memset(elementIds, 0, count * sizeof(unsigned short));
    arrayWithLength<unsigned short> elements(elementIds, count);
    ChunkPrimer primer(false, elements);

    ModEvents::GenerateTerrain(mLevel, &primer, chunkX, chunkZ);

    LevelChunk* chunk = new LevelChunk(mLevel, &primer, chunkX, chunkZ);
    delete[] elementIds;

    arrayWithLength<Biome*> biomes;
    mLevel->getBiomeSource()->getBiomeBlock(biomes, chunkX << 4, chunkZ << 4, 16, 16, true);
    arrayWithLength<unsigned char> biomeIds = chunk->getBiomes();
    for (unsigned int index = 0; index < biomeIds.length; ++index)
        biomeIds.data[index] = static_cast<unsigned char>(biomes.data[index]->id);
    delete[] biomes.data;

    chunk->recalcHeightmap();
    return chunk;
}

void ScriptedChunkGenerator::postProcess(int, int)
{
}

bool ScriptedChunkGenerator::postProcessLoadedChunk(LevelChunk*, int, int)
{
    return false;
}

std::vector<Biome::MobSpawnerData*>* ScriptedChunkGenerator::getMobsAt(MobCategory* category, const BlockPos& pos)
{
    return mLevel->getBiome(pos)->getMobs(category);
}

BlockPos* ScriptedChunkGenerator::findNearestMapFeature(Level*, const std::wstring&, const BlockPos&, bool)
{
    return NULL;
}

void ScriptedChunkGenerator::recreateLogicStructuresForChunk(LevelChunk*, int, int)
{
}

bool ScriptedChunkGenerator::isPosInFeature(Level*, const std::wstring&, const BlockPos&)
{
    return false;
}
