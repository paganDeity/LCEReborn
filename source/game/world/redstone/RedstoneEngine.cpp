#include "game/world/redstone/RedstoneEngine.hpp"

#include "game/core/ModHooks.hpp"
#include "game/world/redstone/ScriptedRedstoneEngine.hpp"
#include "game/world/redstone/VanillaRedstoneEngine.hpp"

RedstoneEngine* RedstoneEngine::Create(Level& level)
{
    if (ModHooks::IsActive(kModHook_wire_update))
        return new ScriptedRedstoneEngine(level);
    return new VanillaRedstoneEngine(level);
}
