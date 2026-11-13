#include "game/world/block/IdMapper.hpp"

#include "game/world/block/Block.hpp"
#include "game/world/block/BlockState.hpp"

#include <string.h>

IdMapper<const BlockState*>::IdMapper()
{
    memset(mBlockStates, 0, sizeof(mBlockStates));
}

int IdMapper<const BlockState*>::getId(const BlockState* state)
{
    return state->mElementId;
}

const BlockState* IdMapper<const BlockState*>::byId(int id)
{
    const BlockState* state = mBlockStates[id];
    if (state == NULL)
    {
        int data = id & 0xF;
        state = Block::byId(id >> 4)->getBlockState(data);
    }

    return state;
}

void IdMapper<const BlockState*>::removeMapping(BlockState* state)
{
    const int id = state->mElementId;
    if (id >= 0 && id < BLOCK_STATE_COUNT && mBlockStates[id] == state)
        mBlockStates[id] = NULL;
}

void IdMapper<const BlockState*>::addMapping(BlockState* state, int id)
{
    state->mElementId = id;
    mBlockStates[id] = state;
}
