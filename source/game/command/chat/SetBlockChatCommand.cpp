#include "game/command/chat/SetBlockChatCommand.hpp"

#include "game/command/CommandArguments.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/Tag.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/block/BlockState.hpp"

using CommandArguments::Lower;

SetBlockChatCommand::SetBlockChatCommand()
    : ChatCommand(L"setblock", L"<x y z> <block>{<data>} [data] [replace|destroy|keep]",
                  L"coord coord coord block number replace|destroy|keep", ePermission_Operator)
{
}

bool SetBlockChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 5 || words.size() > 7)
        return false;
    BlockPos pos;
    if (!ParseBlockPos(source, words, 1, &pos))
        return true;
    std::wstring blockName = words[4];
    CompoundTag* tag = NULL;
    const size_t brace = blockName.find(L'{');
    if (brace != std::wstring::npos)
    {
        if (!ReadCompound(source, blockName.substr(brace), L"The block data is not valid: ", &tag))
            return true;
        blockName.resize(brace);
    }
    Block* block = FindBlock(source, blockName);
    int data = 0;
    if (block == NULL || !ParseBlockData(source, words, 5, &data))
    {
        delete tag;
        return true;
    }
    const std::wstring mode = words.size() > 6 ? Lower(words[6]) : std::wstring(L"replace");
    if (mode != L"replace" && mode != L"destroy" && mode != L"keep")
    {
        delete tag;
        return false;
    }
    Level* level = source.level;
    if (!level->isInWorldBounds(pos) || !level->hasChunkAt(pos))
    {
        delete tag;
        Fail(source, L"Cannot place block outside of the world");
        return true;
    }
    if (mode == L"keep" && !level->isEmptyBlock(pos))
    {
        delete tag;
        Fail(source, L"Could not set the block");
        return true;
    }
    if (mode == L"destroy")
        level->destroyBlock(pos, true);
    if (!level->setBlock(pos, block->getBlockState(data), Block::UPDATE_ALL, false))
    {
        delete tag;
        Fail(source, L"Could not set the block");
        return true;
    }
    if (!ApplyBlockEntityTag(level, pos, tag))
    {
        delete tag;
        Fail(source, L"The block does not take data.");
        return true;
    }
    delete tag;
    Tell(source, L"Block placed");
    return true;
}
