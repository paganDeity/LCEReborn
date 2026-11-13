#ifndef GAME_WORLD_REDSTONE_REDSTONEENGINE_HPP
#define GAME_WORLD_REDSTONE_REDSTONEENGINE_HPP

class BlockPos;
class BlockState;
class Level;

// decides the power of a redstone wire after something around it changed
class RedstoneEngine
{
public:
    virtual ~RedstoneEngine() {}

    // the wire at pos was placed, removed or had a neighbour change
    virtual const BlockState* UpdateWire(const BlockPos& pos, const BlockState* state) = 0;

    static RedstoneEngine* Create(Level& level);

protected:
    explicit RedstoneEngine(Level& level) : mLevel(level) {}

    Level& mLevel;
};

#endif
