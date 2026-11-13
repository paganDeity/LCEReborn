#include "game/world/light/ScriptedLightEngine.hpp"

#include "game/core/ModEvents_generated.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/BlockPos.hpp"

bool ScriptedLightEngine::Check(LightLayer::variety layer, const BlockPos& pos, bool onlyLoadedChunks, bool)
{
    if (!mLevel.hasChunksAt(pos, onlyLoadedChunks ? 0 : 17))
        return false;
    ModEvents::CheckLight(&mLevel, pos.x, pos.y, pos.z, layer);
    return true;
}
