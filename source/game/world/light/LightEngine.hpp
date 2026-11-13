#ifndef GAME_WORLD_LIGHT_LIGHTENGINE_HPP
#define GAME_WORLD_LIGHT_LIGHTENGINE_HPP

#include "game/world/LevelSource.hpp"

class BlockPos;
class Level;

class LightEngine
{
public:
    virtual ~LightEngine() {}

    virtual bool Check(LightLayer::variety layer, const BlockPos& pos, bool onlyLoadedChunks, bool skipCentre) = 0;
    bool Check(const BlockPos& pos, bool onlyLoadedChunks, bool skipCentre);

    static LightEngine* Create(Level& level);
    static void EnableThreadCache();
    static void DestroyThreadCache();

protected:
    explicit LightEngine(Level& level) : mLevel(level) {}

    Level& mLevel;
};

#endif
