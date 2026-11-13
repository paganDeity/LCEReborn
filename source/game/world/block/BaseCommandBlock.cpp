#include "game/world/block/BaseCommandBlock.hpp"
#include "game/command/ChatCommands.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/BlockPos.hpp"
extern MinecraftServer* gMinecraftServer;
BaseCommandBlock::BaseCommandBlock()
{
    mSuccessCount = 0;
    mTrackOutput = true;
    mLastOutput = L"";
    mCommand = L"";
    mName = L"";
}
unsigned int BaseCommandBlock::getSuccessCount()
{
    return (unsigned int)mSuccessCount;
}
std::wstring BaseCommandBlock::getLastOutput()
{
    return mLastOutput;
}
CompoundTag* BaseCommandBlock::save(CompoundTag* tag)
{
    tag->putString(L"Command", mCommand);
    tag->putInt(L"SuccessCount", mSuccessCount);
    tag->putString(L"CustomName", mName);
    tag->putBoolean(L"TrackOutput", mTrackOutput);
    tag->putString(L"LastOutput", mLastOutput);
    tag->putBoolean(L"LPCondionalMode", false);
    tag->putBoolean(L"LPRedstoneMode", false);
    tag->putInt(L"LPCommandMode", 0);
    return tag;
}
void BaseCommandBlock::load(CompoundTag* tag)
{
    mCommand = tag->getString(L"Command");
    mSuccessCount = tag->getInt(L"SuccessCount");
    if (tag->contains(L"CustomName", Tag::TAG_STRING))
        setName(tag->getString(L"CustomName"), true);
    if (tag->contains(L"TrackOutput", Tag::TAG_BYTE))
        mTrackOutput = tag->getBoolean(L"TrackOutput");
    mLastOutput = tag->contains(L"LastOutput", Tag::TAG_STRING) ? tag->getString(L"LastOutput") : std::wstring();
}
bool BaseCommandBlock::hasPermission(EGameCommand)
{
    return false;
}
void BaseCommandBlock::setCommand(const std::wstring& command)
{
    mCommand = command;
    mSuccessCount = 0;
}
std::wstring BaseCommandBlock::getCommand()
{
    return mCommand;
}
bool BaseCommandBlock::performCommand(Level* level)
{
    if (level->mIsClientSide)
        return false;
    if (mCommand.empty())
    {
        mSuccessCount = 0;
        return false;
    }
    std::wstring output;
    const BlockPos pos = getCommandSenderBlockPosition();
    const bool ok = ChatCommands::RunFromBlock(level, pos.getX() + 0.5, pos.getY(), pos.getZ() + 0.5, mCommand, &output);
    mSuccessCount = ok ? 1 : 0;
    if (mTrackOutput)
        mLastOutput = output;
    return ok;
}
std::wstring BaseCommandBlock::getName()
{
    return mName;
}
std::wstring BaseCommandBlock::getDisplayName()
{
    return getName();
}
void BaseCommandBlock::setName(const std::wstring& name, bool)
{
    mName = name;
}
void BaseCommandBlock::sendMessage(ClientboundChatPacket::EChatPacketMessage, int*, unsigned int, std::wstring*,
                                   unsigned int)
{
}
bool BaseCommandBlock::isSendMessageEnabled()
{
    return false;
}
void BaseCommandBlock::setCommandStats(CommandStats::CommandStatType, int)
{
}
void BaseCommandBlock::setLastOutput(std::wstring output)
{
    mLastOutput = output;
}
bool BaseCommandBlock::usedBy(boost::shared_ptr<Player>)
{
    return false;
}
MinecraftServer* BaseCommandBlock::getServer()
{
    return gMinecraftServer;
}
void BaseCommandBlock::setSuccessCount(int count)
{
    mSuccessCount = count;
}
void BaseCommandBlock::setTrackOutput(bool trackOutput)
{
    mTrackOutput = trackOutput;
}
bool BaseCommandBlock::isTrackOutput()
{
    return mTrackOutput;
}
CommandStats* BaseCommandBlock::getCommandStats()
{
    return &mCommandStats;
}
