#include "game/command/chat/LootChatCommand.hpp"

#include <algorithm>
#include <boost/unordered_map.hpp>
#include <vector>

#include "game/command/CommandArguments.hpp"
#include "game/command/chat/ReplaceItemChatCommand.hpp"
#include "game/core/BuiltInLootTables.hpp"
#include "game/core/LootTable.hpp"
#include "game/core/LootTableContext.hpp"
#include "game/entity/mob/ItemEntity.hpp"
#include "game/entity/mob/Mob.hpp"
#include "game/item/Container.hpp"
#include "game/item/Inventory.hpp"
#include "game/item/ItemInstance.hpp"
#include "game/item/not_null_ptr.hpp"
#include "game/server/ServerLevel.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/ui/AbstractContainerMenu.hpp"
#include "game/util/Random.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/block/BlockEntity.hpp"
#include "game/world/block/BlockPos.hpp"

using CommandArguments::Lower;

LootChatCommand::LootChatCommand()
    : ChatCommand(L"loot", L"give <player> <source> | insert <x y z> <source> | spawn <x y z> <source> | replace entity <player> <slot> [count] <source> | replace block <x y z> <slot> [count] <source>, where a source is loot <table> or kill <target>",
                  L"give targets loot loottable;insert coord coord coord loot loottable;spawn coord coord coord loot loottable;replace entity|block text slot number loot loottable",
                  ePermission_Operator)
{
}

const std::vector<std::wstring>& LootChatCommand::TableNames()
{
    static std::vector<std::wstring> names;
    if (names.empty())
    {
        boost::unordered_map<ResourceLocation*, LootTable*>* tables = BuiltInLootTables::getLootTables();
        for (boost::unordered_map<ResourceLocation*, LootTable*>::iterator it = tables->begin(); it != tables->end(); ++it)
        {
            if (it->first != NULL)
                names.push_back(it->first->getNamespace() + L":" + it->first->getPath());
        }
        std::sort(names.begin(), names.end());
    }
    return names;
}

// the built-in tables are keyed by their location, so a name is found by comparing it with each one, with or without minecraft:
LootTable* LootChatCommand::FindTable(const std::wstring& name)
{
    const std::wstring wanted = Lower(name);
    const std::wstring prefixed = wanted.find(L':') == std::wstring::npos ? L"minecraft:" + wanted : wanted;
    boost::unordered_map<ResourceLocation*, LootTable*>* tables = BuiltInLootTables::getLootTables();
    for (boost::unordered_map<ResourceLocation*, LootTable*>::iterator it = tables->begin(); it != tables->end(); ++it)
    {
        if (it->first != NULL && Lower(it->first->getNamespace() + L":" + it->first->getPath()) == prefixed)
            return it->second;
    }
    return NULL;
}

bool LootChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 2)
        return false;
    const std::wstring mode = Lower(words[1]);
    const bool give = mode == L"give";
    const bool replace = mode == L"replace";
    if (!give && !replace && mode != L"insert" && mode != L"spawn")
        return false;
    // give <player> loot <table> has one word before "loot", insert and spawn have a position of three, and replace has entity <target> or
    // block <x y z> then a slot and an optional count of slots to fill
    const std::wstring replaceKind = replace && words.size() > 2 ? Lower(words[2]) : std::wstring();
    if (replace && replaceKind != L"entity" && replaceKind != L"block")
        return false;
    const size_t sourceAt = replace ? words.size() - 2 : (give ? 3 : 5);
    const size_t slotAt = replaceKind == L"entity" ? 4 : 6;
    // the loot comes from a table, "loot <table>", or from what a mob drops, "kill <target>"
    const bool fromMob = words.size() > sourceAt && Lower(words[sourceAt]) == L"kill";
    if (words.size() < 4 || words.size() < sourceAt + 2 || (Lower(words[sourceAt]) != L"loot" && !fromMob)
        || (replace && (sourceAt <= slotAt || sourceAt > slotAt + 2)) || (!replace && words.size() != sourceAt + 2))
        return false;
    int slots = 0;
    if (replace && sourceAt == slotAt + 2 && !CommandArguments::ParseInt(words[slotAt + 1], 1, 64, &slots))
    {
        Fail(source, L"The count of slots must be a whole number from 1 to 64.");
        return true;
    }
    LootTable* table = NULL;
    boost::shared_ptr<Mob> killed;
    if (fromMob)
    {
        EntityList mobs;
        if (!ResolveEntities(source, words[sourceAt + 1], mobs))
            return true;
        for (size_t index = 0; index < mobs.size() && !killed; ++index)
            killed = boost::dynamic_pointer_cast<Mob>(mobs[index]);
        ResourceLocation* location = killed ? killed->getDefaultLootTable() : NULL;
        table = location != NULL ? BuiltInLootTables::getLootTable(location) : NULL;
        if (table == NULL || table == LootTable::EMPTY_LOOT_TABLE)
        {
            Fail(source, killed ? L"That mob has no loot table of its own" : L"No mob was found");
            return true;
        }
    }
    else
    {
        table = FindTable(words[sourceAt + 1]);
        if (table == NULL || table == LootTable::EMPTY_LOOT_TABLE)
        {
            Fail(source, L"Unknown loot table " + words[sourceAt + 1]);
            return true;
        }
    }

    PlayerVector players;
    BlockPos pos;
    boost::shared_ptr<Container> container;
    if (give || replaceKind == L"entity")
    {
        EntityList targets;
        if (give ? !ResolvePlayers(source, words, 2, players) : !ResolveEntities(source, words[3], targets))
            return true;
        for (size_t index = 0; index < targets.size(); ++index)
        {
            const PlayerPtr player = boost::dynamic_pointer_cast<ServerPlayer>(targets[index]);
            if (player)
                players.push_back(player);
        }
    }
    else
    {
        if (!ParseBlockPos(source, words, replace ? 3 : 2, &pos))
            return true;
        if (mode == L"insert" || replaceKind == L"block")
        {
            container = boost::dynamic_pointer_cast<Container>(source.level->getBlockEntity(pos));
            if (!container)
            {
                Fail(source, L"That block has no container to put the loot in.");
                return true;
            }
        }
    }

    ServerLevel* level = static_cast<ServerLevel*>(source.level);
    LootTableContext::Builder builder(level);
    if (killed)
        builder.withLootedEntity(killed);
    LootTableContext context = builder.build();
    Random random;
    std::vector<not_null_ptr<ItemInstance> >* items = table->getRandomItems(&random, context);
    int made = 0;
    int kept = 0;
    for (size_t index = 0; items != NULL && index < items->size(); ++index)
    {
        not_null_ptr<ItemInstance> stack = (*items)[index];
        if (stack->isEmpty())
            continue;
        if (replace && slots > 0 && made >= slots)
            break;
        ++made;
        if (replace)
        {
            const std::wstring slotName = Lower(words[slotAt]);
            bool placed = false;
            if (container)
                placed = ReplaceItemChatCommand::SetContainerSlot(container, slotName, made - 1, stack->copy());
            for (size_t target = 0; target < players.size(); ++target)
                placed = ReplaceItemChatCommand::SetPlayerSlot(players[target], slotName, made - 1, stack->copy()) || placed;
            if (placed)
                ++kept;
        }
        else if (give)
        {
            for (size_t target = 0; target < players.size(); ++target)
            {
                not_null_ptr<ItemInstance> copy = stack->copy();
                if (!players[target]->mInventory->add(copy) || copy->getCount() > 0)
                    players[target]->drop(copy, false);
                players[target]->mInventoryMenu->broadcastChanges();
            }
        }
        else if (container)
        {
            for (unsigned int slot = 0; slot < container->getContainerSize(); ++slot)
            {
                if (container->getItem(slot)->isEmpty())
                {
                    container->setItem(slot, stack->copy());
                    ++kept;
                    break;
                }
            }
        }
        else
        {
            level->addEntity(boost::shared_ptr<ItemEntity>(new ItemEntity(level, pos.getX() + 0.5, pos.getY() + 0.5, pos.getZ() + 0.5, stack->copy())));
        }
    }
    delete items;
    if (container)
        container->setChanged();
    if (made == 0)
        Tell(source, L"The loot table gave nothing");
    else if (replace)
        Tell(source, kept == 0 ? std::wstring(L"No slot of that name was found") : L"Replaced " + Count(kept, L"slot", L"slots") + L" with loot");
    else if (give)
        Tell(source, L"Gave " + Count(made, L"item", L"items") + L" from the loot table");
    else if (container)
        Tell(source, L"Inserted " + Count(kept, L"item", L"items") + L" from the loot table" + (kept < made ? L" (the container was full)" : L""));
    else
        Tell(source, L"Dropped " + Count(made, L"item", L"items") + L" from the loot table at " + Position(pos));
    return true;
}
