#include "game/world/redstone/VanillaRedstoneEngine.hpp"

#include "game/world/block/Blocks.hpp"

const BlockState* VanillaRedstoneEngine::UpdateWire(const BlockPos& pos, const BlockState* state)
{
    return Blocks::redstone_wire->updatePowerStrength(&mLevel, pos, state);
}
