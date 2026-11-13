#ifndef GAME_WORLD_LIGHT_VANILLALIGHTENGINE_HPP
#define GAME_WORLD_LIGHT_VANILLALIGHTENGINE_HPP

#include "game/core/CriticalSection.hpp"
#include "game/world/light/LightEngine.hpp"

class Block;

class VanillaLightEngine : public LightEngine
{
public:
    explicit VanillaLightEngine(Level& level);
    virtual ~VanillaLightEngine();

    virtual bool Check(LightLayer::variety layer, const BlockPos& pos, bool onlyLoadedChunks, bool skipCentre);

    static void EnableThreadCache();
    static void DestroyThreadCache();

private:
    int getBrightnessCached(unsigned int* cache, LightLayer::variety layer, int x, int y, int z);
    int getEmissionCached(unsigned int* cache, Block* block, const BlockPos& pos);
    int getExpectedLight(unsigned int* cache, const BlockPos& pos, LightLayer::variety layer, bool ignoreEmission);
    int getBlockingCached(unsigned int* cache, LightLayer::variety layer, Block* block, Block* extraBlock,
                          const BlockPos& pos);
    void initCachePartial(unsigned int* cache, int x, int y, int z);
    void initCacheComplete(unsigned int* cache, int x, int y, int z);
    void setBrightnessCached(unsigned int* cache, unsigned long long* dirty, LightLayer::variety layer, int x, int y,
                             int z, int brightness);
    void flushCache(unsigned int* cache, unsigned long long dirty, LightLayer::variety layer, int centreX, int centreZ);

    unsigned int* mUpdateQueue;
    C4JCriticalSection mMutex;
};

#endif
