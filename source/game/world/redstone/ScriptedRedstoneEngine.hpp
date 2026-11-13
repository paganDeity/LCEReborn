#ifndef GAME_WORLD_REDSTONE_SCRIPTEDREDSTONEENGINE_HPP
#define GAME_WORLD_REDSTONE_SCRIPTEDREDSTONEENGINE_HPP

#include <vector>

#include "game/world/block/BlockPos.hpp"
#include "game/world/redstone/RedstoneEngine.hpp"

class ScriptedRedstoneEngine : public RedstoneEngine
{
public:
    explicit ScriptedRedstoneEngine(Level& level) : RedstoneEngine(level), mRunning(false) {}

    virtual const BlockState* UpdateWire(const BlockPos& pos, const BlockState* state);

private:
    // updates the mod causes while it runs wait here, so the mod never re-enters itself
    bool mRunning;
    std::vector<BlockPos> mPending;
};

#endif
