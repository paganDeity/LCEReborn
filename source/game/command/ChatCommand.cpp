#include "game/command/ChatCommand.hpp"

#include <algorithm>
#include <math.h>
#include <stdio.h>

#include <boost/pointer_cast.hpp>

#include "game/command/ChatCommands.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/command/CommandDispatcher.hpp"
#include "game/command/ConsoleCommandSender.hpp"
#include "game/command/TargetSelector.hpp"
#include "game/core/GameType.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/NbtCapabilities.hpp"
#include "game/data/StringNbtReader.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/entity/mob/LivingEntity.hpp"
#include "game/entity/mob/MobEffect.hpp"
#include "game/item/Enchantment.hpp"
#include "game/item/Item.hpp"
#include "game/network/ClientboundChatPacket.hpp"
#include "game/network/GameCommandPacket.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerLevel.hpp"
#include "game/world/LevelData.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockEntity.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/generation/PlayerList.hpp"

using CommandArguments::Lower;
using CommandArguments::Number;
using CommandArguments::ParseInt;
using CommandArguments::Pretty;
using CommandArguments::StripNamespace;

ChatCommand::ChatCommand(const std::wstring& name, const std::wstring& usage, const std::wstring& arguments,
                         int permission)
    : mName(name),
      mUsage(usage),
      mArguments(arguments),
      mPermission(permission),
      mAliases()
{
}

ChatCommand::~ChatCommand()
{
}

const std::wstring& ChatCommand::getName() const
{
    return mName;
}

const std::wstring& ChatCommand::getUsage() const
{
    return mUsage;
}

const std::wstring& ChatCommand::getArguments() const
{
    return mArguments;
}

int ChatCommand::getPermission() const
{
    return mPermission;
}

const std::vector<std::wstring>& ChatCommand::getAliases() const
{
    return mAliases;
}

bool ChatCommand::canUse(int permission) const
{
    return permission >= mPermission;
}

std::wstring ChatCommand::usageLine() const
{
    std::wstring line = L"§e/" + mName;
    if (!mUsage.empty())
        line += L" §7" + mUsage;
    return line;
}

std::wstring ChatCommand::usageError() const
{
    return L"Usage: /" + mName + (mUsage.empty() ? std::wstring() : L" " + mUsage);
}

void ChatCommand::addAlias(const std::wstring& alias)
{
    mAliases.push_back(alias);
}

void ChatCommand::Tell(const CommandSource& source, const std::wstring& text)
{
    if (source.output)
        ChatCommands::Reply(source.output, text);
    else
        ChatCommands::Echo(text);
}

bool ChatCommand::ReadData(const CommandSource& source, const std::wstring& text, const std::wstring& invalid,
                           Tag** tag)
{
    std::wstring error;
    *tag = StringNbtReader::parse(text, &error);
    if (*tag == NULL)
    {
        Fail(source, invalid + error);
        return false;
    }
    gMinecraftServer->getLevel(0)->getLevelData()->addNbtCapabilities(NbtCapabilities::Required(*tag));
    return true;
}

bool ChatCommand::ReadCompound(const CommandSource& source, const std::wstring& text, const std::wstring& invalid,
                               CompoundTag** tag)
{
    *tag = NULL;
    Tag* parsed = NULL;
    if (!ReadData(source, text, invalid, &parsed))
        return false;
    if (parsed->getId() != Tag::TAG_COMPOUND)
    {
        delete parsed;
        Fail(source, invalid + L"a compound is needed");
        return false;
    }
    *tag = static_cast<CompoundTag*>(parsed);
    return true;
}

void ChatCommand::Fail(const CommandSource& source, const std::wstring& text)
{
    if (source.output)
        ChatCommands::Error(source.output, text);
    else if (source.console)
        ChatCommands::Echo(L"§c" + text);
}

std::wstring ChatCommand::LongNumber(long long value)
{
    wchar_t buffer[32];
    swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%lld", value);
    return buffer;
}

std::wstring ChatCommand::Position(double x, double y, double z)
{
    wchar_t buffer[96];
    swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%.1f, %.1f, %.1f", x, y, z);
    return buffer;
}

std::wstring ChatCommand::Position(const BlockPos& pos)
{
    return Number(pos.x) + L", " + Number(pos.y) + L", " + Number(pos.z);
}

std::wstring ChatCommand::Count(int count, const wchar_t* one, const wchar_t* many)
{
    return Number(count) + L" " + (count == 1 ? one : many);
}

std::wstring ChatCommand::SourceName(const CommandSource& source)
{
    if (!source.entity)
        return L"Server";
    return TargetSelector::DisplayName(source.entity);
}

BlockPos ChatCommand::SourceBlockPos(const CommandSource& source)
{
    return BlockPos(static_cast<int>(floor(source.x)), static_cast<int>(floor(source.y)),
                    static_cast<int>(floor(source.z)));
}

bool ChatCommand::InOverworld(const CommandSource& source)
{
    return source.level == gMinecraftServer->getLevel(0);
}

void ChatCommand::Perform(const CommandSource& source, boost::shared_ptr<GameCommandPacket> packet)
{
    boost::shared_ptr<CommandSender> sender = source.player;
    if (!sender)
        sender = ConsoleCommandSender::Get();
    gMinecraftServer->getCommandDispatcher()->performCommand(sender, packet->command, packet->data);
}

void ChatCommand::Broadcast(const std::wstring& text)
{
    ChatCommands::Announce(text);
}

bool ChatCommand::ResolvePlayers(const CommandSource& source, const Words& words, size_t index, PlayerVector& players)
{
    players.clear();
    if (index >= words.size())
    {
        PlayerPtr self = boost::dynamic_pointer_cast<ServerPlayer>(source.entity);
        if (!self)
        {
            Fail(source, L"No player was found.");
            return false;
        }
        players.push_back(self);
        return true;
    }
    std::wstring error;
    if (!TargetSelector::Players(source, words[index], players, &error))
    {
        Fail(source, error);
        return false;
    }
    return true;
}

void ChatCommand::IdentifyPlayer(const std::wstring& word, std::wstring* id, std::wstring* name, PlayerPtr* online)
{
    id->clear();
    name->clear();
    PlayerList* players = gMinecraftServer->getPlayers();
    *online = players->getPlayerByNameIgnoringCase(word);
    if (*online)
    {
        *id = (*online)->getXuid().toString();
        *name = (*online)->getName();
        return;
    }
    if (!PlayerList::ParsePlayerId(word, id))
    {
        *name = word;
        return;
    }
    std::vector<PlayerPtr>* everyone = players->getPlayers();
    for (size_t index = 0; index < everyone->size() && !*online; ++index)
    {
        if ((*everyone)[index]->getXuid().toString() == *id)
            *online = (*everyone)[index];
    }
    if (*online)
        *name = (*online)->getName();
}

bool ChatCommand::ResolveEntities(const CommandSource& source, const std::wstring& word, EntityList& entities)
{
    std::wstring error;
    if (!TargetSelector::Entities(source, word, entities, &error))
    {
        Fail(source, error);
        return false;
    }
    return true;
}

bool ChatCommand::ResolveLiving(const CommandSource& source, const Words& words, size_t index, LivingList& living)
{
    living.clear();
    EntityList entities;
    if (index >= words.size() && source.entity)
        entities.push_back(source.entity);
    else if (index < words.size() && !ResolveEntities(source, words[index], entities))
        return false;
    for (size_t entity = 0; entity < entities.size(); ++entity)
    {
        LivingPtr target = boost::dynamic_pointer_cast<LivingEntity>(entities[entity]);
        if (target)
            living.push_back(target);
    }
    if (living.empty())
    {
        Fail(source, L"No entity was found.");
        return false;
    }
    return true;
}

bool ChatCommand::ParseGameType(const std::wstring& word, const GameType** type)
{
    const std::wstring value = Lower(word);
    if (value == L"survival" || value == L"s" || value == L"0")
        *type = GameType::SURVIVAL;
    else if (value == L"creative" || value == L"c" || value == L"1")
        *type = GameType::CREATIVE;
    else if (value == L"adventure" || value == L"a" || value == L"2")
        *type = GameType::ADVENTURE;
    else
        return false;
    return true;
}

Item* ChatCommand::FindItem(const CommandSource& source, const std::wstring& word)
{
    const std::wstring name = StripNamespace(word);
    Item* item = NULL;
    int id = 0;
    if (Item::itemRegistry.containsKey(ResourceLocation(name)))
        item = Item::itemRegistry.get(ResourceLocation(name));
    else if (ParseInt(name, 1, 32767, &id))
        item = Item::byId(id);
    if (item == NULL || name == L"air")
    {
        Fail(source, L"There is no such item with name " + word + L".");
        return NULL;
    }
    return item;
}

Block* ChatCommand::FindBlock(const CommandSource& source, const std::wstring& word)
{
    const std::wstring name = StripNamespace(word);
    Block* block = NULL;
    int id = 0;
    if (Block::blockRegistry->containsKey(ResourceLocation(name)))
        block = Block::blockRegistry->get(ResourceLocation(name));
    else if (ParseInt(name, 0, 4095, &id))
        block = Block::byId(id);
    if (block == NULL)
    {
        Fail(source, L"There is no such block with name " + word + L".");
        return NULL;
    }
    return block;
}

MobEffect* ChatCommand::FindEffect(const CommandSource& source, const std::wstring& word)
{
    const std::wstring name = StripNamespace(word);
    MobEffect* effect = NULL;
    int id = 0;
    if (MobEffect::mobEffectRegistry->containsKey(ResourceLocation(name)))
        effect = MobEffect::byName(name);
    else if (ParseInt(name, 1, 255, &id))
        effect = MobEffect::byId(id);
    if (effect == NULL)
        Fail(source, L"There is no such mob effect with name " + word + L".");
    return effect;
}

std::wstring ChatCommand::EffectName(MobEffect* effect)
{
    return Pretty(MobEffect::mobEffectRegistry->getKey(effect).getPath());
}

Enchantment* ChatCommand::FindEnchantment(const CommandSource& source, const std::wstring& word)
{
    const std::wstring name = StripNamespace(word);
    Enchantment* enchantment = NULL;
    int id = 0;
    if (Enchantment::enchantmentRegistry->containsKey(ResourceLocation(name)))
        enchantment = Enchantment::byName(name);
    else if (ParseInt(name, 0, 255, &id))
        enchantment = Enchantment::byId(id);
    if (enchantment == NULL)
        Fail(source, L"There is no such enchantment with name " + word + L".");
    return enchantment;
}

std::wstring ChatCommand::EnchantmentName(Enchantment* enchantment)
{
    return Pretty(Enchantment::enchantmentRegistry->getKey(enchantment).getPath());
}

bool ChatCommand::ParsePosition(const CommandSource& source, const Words& words, size_t index, bool centre,
                                double result[3])
{
    const double origin[3] = {source.x, source.y, source.z};
    std::wstring error;
    if (!CommandArguments::ParsePosition(words, index, origin, source.yRot, source.xRot, centre, result, &error))
    {
        Fail(source, error);
        return false;
    }
    return true;
}

bool ChatCommand::ParseBlockPos(const CommandSource& source, const Words& words, size_t index, BlockPos* pos)
{
    double result[3];
    if (!ParsePosition(source, words, index, false, result))
        return false;
    *pos = BlockPos(static_cast<int>(floor(result[0])), static_cast<int>(floor(result[1])),
                    static_cast<int>(floor(result[2])));
    return true;
}

bool ChatCommand::ParseBlockData(const CommandSource& source, const Words& words, size_t index, int* data)
{
    *data = 0;
    if (index < words.size() && !ParseInt(words[index], 0, 15, data))
    {
        Fail(source, L"The data value must be a whole number from 0 to 15.");
        return false;
    }
    return true;
}

bool ChatCommand::ApplyBlockEntityTag(Level* level, const BlockPos& pos, CompoundTag* tag)
{
    if (tag == NULL)
        return true;
    boost::shared_ptr<BlockEntity> blockEntity = level->getBlockEntity(pos);
    if (!blockEntity)
        return false;
    CompoundTag* current = blockEntity->save(new CompoundTag());
    current->merge(tag);
    current->putInt(L"x", pos.x);
    current->putInt(L"y", pos.y);
    current->putInt(L"z", pos.z);
    blockEntity->load(current);
    delete current;
    return true;
}

bool ChatCommand::CheckArea(const CommandSource& source, const BlockPos& first, const BlockPos& second, int limit,
                            const std::wstring& outside, BlockPos* low, BlockPos* high)
{
    *low = BlockPos(std::min(first.x, second.x), std::min(first.y, second.y), std::min(first.z, second.z));
    *high = BlockPos(std::max(first.x, second.x), std::max(first.y, second.y), std::max(first.z, second.z));
    const long long volume = (long long)(high->x - low->x + 1) * (high->y - low->y + 1) * (high->z - low->z + 1);
    if (limit > 0 && volume > limit)
    {
        Fail(source, L"Too many blocks in the specified area (" + LongNumber(volume) + L" > " + Number(limit) + L")");
        return false;
    }
    Level* level = source.level;
    if (!level->isInWorldBounds(low->x, low->y, low->z) || !level->isInWorldBounds(high->x, high->y, high->z))
    {
        Fail(source, outside);
        return false;
    }
    for (int chunkX = low->x & ~15; chunkX <= high->x; chunkX += 16)
    {
        for (int chunkZ = low->z & ~15; chunkZ <= high->z; chunkZ += 16)
        {
            if (!level->hasChunkAt(BlockPos(chunkX, low->y, chunkZ)))
            {
                Fail(source, outside);
                return false;
            }
        }
    }
    return true;
}

bool ChatCommand::ParseFlag(const std::wstring& word, bool* value)
{
    const std::wstring lower = Lower(word);
    if (lower == L"true")
        *value = true;
    else if (lower == L"false")
        *value = false;
    else
        return false;
    return true;
}
