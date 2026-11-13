#include "game/world/light/LightEngine.hpp"

#include "game/core/ModHooks.hpp"
#include "game/core/PIXNamedEvents.hpp"
#include "game/world/Dimension.hpp"
#include "game/world/Level.hpp"
#include "game/world/light/ScriptedLightEngine.hpp"
#include "game/world/light/VanillaLightEngine.hpp"

LightEngine* LightEngine::Create(Level& level)
{
    if (ModHooks::IsActive(kModHook_check_light))
        return new ScriptedLightEngine(level);
    return new VanillaLightEngine(level);
}

void LightEngine::EnableThreadCache()
{
    VanillaLightEngine::EnableThreadCache();
}

void LightEngine::DestroyThreadCache()
{
    VanillaLightEngine::DestroyThreadCache();
}

bool LightEngine::Check(const BlockPos& pos, bool onlyLoadedChunks, bool skipCentre)
{
    bool changed = false;

    if (mLevel.mDimension->isHasSkyLight())
    {
        PIXBeginNamedEvent(0.0f, "Check sky light");
        changed = Check(LightLayer::SKY, pos, onlyLoadedChunks, false);
        PIXEndNamedEvent();
    }

    PIXBeginNamedEvent(0.0f, "Check block light");
    bool blockChanged = Check(LightLayer::BLOCK, pos, onlyLoadedChunks, skipCentre);
    PIXEndNamedEvent();

    return changed || blockChanged;
}
