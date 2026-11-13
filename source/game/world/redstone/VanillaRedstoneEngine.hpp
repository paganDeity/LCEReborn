#ifndef GAME_WORLD_REDSTONE_VANILLAREDSTONEENGINE_HPP
#define GAME_WORLD_REDSTONE_VANILLAREDSTONEENGINE_HPP

#include "game/world/redstone/RedstoneEngine.hpp"

class VanillaRedstoneEngine : public RedstoneEngine
{
public:
    explicit VanillaRedstoneEngine(Level& level) : RedstoneEngine(level) {}

    virtual const BlockState* UpdateWire(const BlockPos& pos, const BlockState* state);
};

#endif
