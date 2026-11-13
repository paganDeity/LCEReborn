#include "game/command/chat/CloneChatCommand.hpp"

#include <vector>

#include "game/command/CommandArguments.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockEntity.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/block/Blocks.hpp"

using CommandArguments::Lower;

namespace
{
const int kCloneLimit = 32768;

struct Copied
{
    BlockPos offset;
    const BlockState* state;
    CompoundTag* blockEntity;
};
}

CloneChatCommand::CloneChatCommand()
    : ChatCommand(L"clone", L"<x1 y1 z1> <x2 y2 z2> <x y z> [replace|masked|filtered <block>] [normal|force|move]",
                  L"coord coord coord coord coord coord coord coord coord replace|masked|filtered block normal|force|move",
                  ePermission_Operator)
{
}

bool CloneChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 10 || words.size() > 13)
        return false;
    BlockPos first;
    BlockPos second;
    BlockPos target;
    if (!ParseBlockPos(source, words, 1, &first) || !ParseBlockPos(source, words, 4, &second)
        || !ParseBlockPos(source, words, 7, &target))
        return true;
    size_t next = 10;
    const std::wstring mask = words.size() > next ? Lower(words[next++]) : std::wstring(L"replace");
    if (mask != L"replace" && mask != L"masked" && mask != L"filtered")
        return false;
    Block* filter = NULL;
    if (mask == L"filtered")
    {
        if (words.size() <= next)
            return false;
        filter = FindBlock(source, words[next++]);
        if (filter == NULL)
            return true;
    }
    const std::wstring mode = words.size() > next ? Lower(words[next++]) : std::wstring(L"normal");
    if (words.size() > next || (mode != L"normal" && mode != L"force" && mode != L"move"))
        return false;

    BlockPos low;
    BlockPos high;
    if (!CheckArea(source, first, second, kCloneLimit, L"Cannot access blocks outside of the world", &low, &high))
        return true;
    const BlockPos size(high.x - low.x, high.y - low.y, high.z - low.z);
    const BlockPos far(target.x + size.x, target.y + size.y, target.z + size.z);
    BlockPos targetLow;
    BlockPos targetHigh;
    if (!CheckArea(source, target, far, 0, L"Cannot place blocks outside of the world", &targetLow, &targetHigh))
        return true;
    const bool overlaps = targetLow.x <= high.x && targetHigh.x >= low.x && targetLow.y <= high.y && targetHigh.y >= low.y
                          && targetLow.z <= high.z && targetHigh.z >= low.z;
    if (overlaps && mode != L"force")
    {
        Fail(source, L"The source and destination areas cannot overlap");
        return true;
    }

    // everything is read before anything is written, so an overlapping clone copies the area as it was
    Level* level = source.level;
    std::vector<Copied> copied;
    for (int y = low.y; y <= high.y; ++y)
    {
        for (int z = low.z; z <= high.z; ++z)
        {
            for (int x = low.x; x <= high.x; ++x)
            {
                const BlockPos pos(x, y, z);
                const BlockState* state = level->getBlockState(pos);
                if (mask == L"masked" && state->getBlock() == Blocks::air)
                    continue;
                if (filter != NULL && state->getBlock() != filter)
                    continue;
                Copied entry = {BlockPos(x - low.x, y - low.y, z - low.z), state, NULL};
                boost::shared_ptr<BlockEntity> blockEntity = level->getBlockEntity(pos);
                if (blockEntity)
                    entry.blockEntity = blockEntity->save(new CompoundTag());
                copied.push_back(entry);
            }
        }
    }
    if (mode == L"move")
    {
        const BlockState* air = Blocks::air->defaultBlockState();
        for (size_t index = 0; index < copied.size(); ++index)
        {
            const BlockPos& offset = copied[index].offset;
            level->setBlock(BlockPos(low.x + offset.x, low.y + offset.y, low.z + offset.z), air, Block::UPDATE_CLIENTS, false);
        }
    }
    int changed = 0;
    for (size_t index = 0; index < copied.size(); ++index)
    {
        const Copied& entry = copied[index];
        const BlockPos pos(target.x + entry.offset.x, target.y + entry.offset.y, target.z + entry.offset.z);
        if (level->setBlock(pos, entry.state, Block::UPDATE_ALL, false))
            ++changed;
        if (entry.blockEntity != NULL)
        {
            ApplyBlockEntityTag(level, pos, entry.blockEntity);
            delete entry.blockEntity;
        }
    }
    if (changed == 0)
    {
        Fail(source, L"No blocks cloned");
        return true;
    }
    Tell(source, Count(changed, L"block", L"blocks") + L" cloned");
    return true;
}
