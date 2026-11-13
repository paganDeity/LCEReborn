#ifndef GAME_WORLD_LIGHT_SCRIPTEDLIGHTENGINE_HPP
#define GAME_WORLD_LIGHT_SCRIPTEDLIGHTENGINE_HPP

#include "game/world/light/LightEngine.hpp"

class ScriptedLightEngine : public LightEngine
{
public:
    explicit ScriptedLightEngine(Level& level) : LightEngine(level) {}

    virtual bool Check(LightLayer::variety layer, const BlockPos& pos, bool onlyLoadedChunks, bool skipCentre);
};

#endif
