#include "game/command/chat/FillChatCommand.hpp"

#include <algorithm>

#include "game/command/CommandArguments.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/Tag.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/block/Blocks.hpp"

using CommandArguments::Lower;
using CommandArguments::Number;
using CommandArguments::ParseInt;

namespace
{
const int kFillLimit = 32768;

Block* StillLiquid(Block* block)
{
    if (block == Blocks::flowing_water)
        return Blocks::water;
    if (block == Blocks::flowing_lava)
        return Blocks::lava;
    return block;
}
}

FillChatCommand::FillChatCommand()
    : ChatCommand(L"fill", L"<x1 y1 z1> <x2 y2 z2> <block>{<data>} [data] [mode] [replaceBlock] [replaceData]",
                  L"coord coord coord coord coord coord block number replace|destroy|keep|hollow|outline block number",
                  ePermission_Operator)
{
}

bool FillChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 8 || words.size() > 12)
        return false;
    BlockPos first;
    BlockPos second;
    if (!ParseBlockPos(source, words, 1, &first) || !ParseBlockPos(source, words, 4, &second))
        return true;
    std::wstring blockName = words[7];
    std::wstring tagText;
    const size_t brace = blockName.find(L'{');
    if (brace != std::wstring::npos)
    {
        tagText = blockName.substr(brace);
        blockName.resize(brace);
    }
    Block* block = FindBlock(source, blockName);
    int data = 0;
    if (block == NULL || !ParseBlockData(source, words, 8, &data))
        return true;
    const std::wstring mode = words.size() > 9 ? Lower(words[9]) : std::wstring(L"replace");
    if (mode != L"replace" && mode != L"destroy" && mode != L"keep" && mode != L"hollow" && mode != L"outline")
        return false;
    Block* filter = NULL;
    int filterData = -1;
    if (words.size() > 10)
    {
        if (mode != L"replace")
            return false;
        filter = FindBlock(source, words[10]);
        if (filter == NULL)
            return true;
        filter = StillLiquid(filter);
        if (words.size() > 11 && !ParseInt(words[11], -1, 15, &filterData))
        {
            Fail(source, L"The data value must be a whole number from -1 to 15.");
            return true;
        }
    }

    BlockPos low;
    BlockPos high;
    if (!CheckArea(source, first, second, kFillLimit, L"Cannot place blocks outside of the world", &low, &high))
        return true;
    Level* level = source.level;
    const int x0 = low.x, y0 = low.y, z0 = low.z;
    const int x1 = high.x, y1 = high.y, z1 = high.z;
    const BlockState* state = block->getBlockState(data);
    const BlockState* air = Blocks::air->defaultBlockState();
    CompoundTag* tag = NULL;
    if (!tagText.empty())
    {
        if (!ReadCompound(source, tagText, L"The block data is not valid: ", &tag))
            return true;
    }
    int changed = 0;
    for (int y = y0; y <= y1; ++y)
    {
        for (int z = z0; z <= z1; ++z)
        {
            for (int x = x0; x <= x1; ++x)
            {
                const BlockPos pos(x, y, z);
                const bool edge = x == x0 || x == x1 || y == y0 || y == y1 || z == z0 || z == z1;
                const BlockState* placed = state;
                if (mode == L"hollow" && !edge)
                    placed = air;
                else if (mode == L"outline" && !edge)
                    continue;
                else if (mode == L"keep" && !level->isEmptyBlock(pos))
                    continue;
                else if (filter != NULL)
                {
                    const BlockState* current = level->getBlockState(pos);
                    Block* currentBlock = current->getBlock();
                    if (StillLiquid(currentBlock) != filter
                        || (filterData >= 0 && currentBlock->convertBlockStateToLegacyData(current) != filterData))
                        continue;
                }
                if (mode == L"destroy")
                    level->destroyBlock(pos, true);
                if (level->setBlock(pos, placed, Block::UPDATE_ALL, false))
                {
                    ++changed;
                    if (tag != NULL)
                        ApplyBlockEntityTag(level, pos, tag);
                }
            }
        }
    }
    delete tag;
    if (changed == 0)
    {
        Fail(source, L"No blocks filled");
        return true;
    }
    Tell(source, Count(changed, L"block", L"blocks") + L" filled");
    return true;
}
