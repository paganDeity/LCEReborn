#include "game/command/chat/ReplaceItemChatCommand.hpp"

#include <boost/pointer_cast.hpp>

#include "game/command/CommandArguments.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/item/Container.hpp"
#include "game/item/Inventory.hpp"
#include "game/item/Item.hpp"
#include "game/item/ItemInstance.hpp"
#include "game/item/not_null_ptr.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/ui/AbstractContainerMenu.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/BlockEntity.hpp"
#include "game/world/block/BlockPos.hpp"

using CommandArguments::Lower;
using CommandArguments::ParseInt;

namespace
{
// where each part of a player's inventory starts in the numbering Inventory::setItem takes: the 36 slots, then the armour from the feet up, then the off hand
const int kHotbarStart = 0;
const int kInventoryStart = 9;
const int kArmorStart = 36;
const int kOffhand = 40;

bool NumberAfter(const std::wstring& name, const std::wstring& prefix, int limit, int* number)
{
    if (name.compare(0, prefix.size(), prefix) != 0)
        return false;
    return ParseInt(name.substr(prefix.size()), 0, limit - 1, number);
}

struct PlayerSlot
{
    const wchar_t* name;
    int slot;
};

// the named slots of a player; the numbered ones (hotbar and inventory) are built from their ranges, and the main hand is the selected hotbar slot, which is -1 here
const PlayerSlot kNamedSlots[] = {{L"slot.armor.feet", kArmorStart},      {L"slot.armor.legs", kArmorStart + 1},
                                  {L"slot.armor.chest", kArmorStart + 2}, {L"slot.armor.head", kArmorStart + 3},
                                  {L"slot.weapon.mainhand", -1},          {L"slot.weapon.offhand", kOffhand}};
const int kHotbarCount = 9;
const int kInventoryCount = 27;
const int kContainerCount = 27;
}

const std::vector<std::wstring>& ReplaceItemChatCommand::PlayerSlotNames()
{
    static std::vector<std::wstring> names;
    if (names.empty())
    {
        for (int index = 0; index < kHotbarCount; ++index)
            names.push_back(L"slot.hotbar." + CommandArguments::Number(index));
        for (int index = 0; index < kInventoryCount; ++index)
            names.push_back(L"slot.inventory." + CommandArguments::Number(index));
        for (size_t index = 0; index < sizeof(kNamedSlots) / sizeof(kNamedSlots[0]); ++index)
            names.push_back(kNamedSlots[index].name);
    }
    return names;
}

const std::vector<std::wstring>& ReplaceItemChatCommand::ContainerSlotNames()
{
    static std::vector<std::wstring> names;
    if (names.empty())
    {
        for (int index = 0; index < kContainerCount; ++index)
            names.push_back(L"slot.container." + CommandArguments::Number(index));
    }
    return names;
}

ReplaceItemChatCommand::ReplaceItemChatCommand()
    : ChatCommand(L"replaceitem", L"entity <player> <slot> <item>{<data>} [count] | block <x y z> <slot> <item>{<data>} [count]",
                  L"entity targets slot item number;block coord coord coord containerslot item number", ePermission_Operator)
{
}

bool ReplaceItemChatCommand::ParsePlayerSlot(const std::wstring& name, int* slot)
{
    int number = 0;
    if (NumberAfter(name, L"slot.hotbar.", kHotbarCount, &number))
        *slot = kHotbarStart + number;
    else if (NumberAfter(name, L"slot.inventory.", kInventoryCount, &number))
        *slot = kInventoryStart + number;
    else
    {
        for (size_t index = 0; index < sizeof(kNamedSlots) / sizeof(kNamedSlots[0]); ++index)
        {
            if (name == kNamedSlots[index].name)
            {
                *slot = kNamedSlots[index].slot;
                return true;
            }
        }
        return false;
    }
    return true;
}

bool ReplaceItemChatCommand::ParseContainerSlot(const std::wstring& name, int* slot)
{
    return NumberAfter(name, L"slot.container.", 1 << 16, slot);
}

bool ReplaceItemChatCommand::SetPlayerSlot(const boost::shared_ptr<ServerPlayer>& player, const std::wstring& slotName, int offset,
                                           not_null_ptr<ItemInstance> stack)
{
    int slot = 0;
    if (!ParsePlayerSlot(slotName, &slot))
        return false;
    if (slot < 0)
        slot = player->mInventory->m_selected;
    slot += offset;
    if (slot < 0 || slot >= kOffhand + 1)
        return false;
    player->mInventory->setItem(static_cast<unsigned int>(slot), stack);
    player->mInventoryMenu->broadcastChanges();
    return true;
}

bool ReplaceItemChatCommand::SetContainerSlot(const boost::shared_ptr<Container>& container, const std::wstring& slotName, int offset,
                                              not_null_ptr<ItemInstance> stack)
{
    int slot = 0;
    if (!ParseContainerSlot(slotName, &slot))
        return false;
    slot += offset;
    if (slot < 0 || static_cast<unsigned int>(slot) >= container->getContainerSize())
        return false;
    container->setItem(static_cast<unsigned int>(slot), stack);
    container->setChanged();
    return true;
}

bool ReplaceItemChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 2)
        return false;
    const std::wstring mode = Lower(words[1]);
    const bool entity = mode == L"entity";
    if (!entity && mode != L"block")
        return false;
    // entity <player> <slot> <item> [count] has one word before the slot, block <x y z> <slot> <item> [count] has three
    const size_t slotAt = entity ? 3 : 5;
    if (words.size() < slotAt + 2 || words.size() > slotAt + 3)
        return false;
    const std::wstring slotName = Lower(words[slotAt]);
    int count = 1;
    if (words.size() > slotAt + 2 && !ParseInt(words[slotAt + 2], 1, 64, &count))
    {
        Fail(source, L"The count must be a whole number from 1 to 64.");
        return true;
    }
    std::wstring itemName = words[slotAt + 1];
    CompoundTag* tag = NULL;
    const size_t brace = itemName.find(L'{');
    if (brace != std::wstring::npos)
    {
        if (!ReadCompound(source, itemName.substr(brace), L"The item data is not valid: ", &tag))
            return true;
        itemName.resize(brace);
    }
    Item* item = FindItem(source, itemName);
    if (item == NULL)
    {
        delete tag;
        return true;
    }
    not_null_ptr<ItemInstance> stack(new ItemInstance(item, count, 0));
    if (tag != NULL)
        stack->setTag(tag);
    const std::wstring itemLabel = stack->getHoverName();

    if (entity)
    {
        EntityList targets;
        if (!ResolveEntities(source, words[2], targets))
            return true;
        int replaced = 0;
        std::wstring last;
        for (size_t index = 0; index < targets.size(); ++index)
        {
            const PlayerPtr player = boost::dynamic_pointer_cast<ServerPlayer>(targets[index]);
            if (!player || !SetPlayerSlot(player, slotName, 0, stack->copy()))
                continue;
            last = player->getName();
            ++replaced;
        }
        if (replaced == 0)
        {
            Fail(source, L"No player with that slot was found (slots: slot.hotbar.N, slot.inventory.N, slot.armor.feet|legs|chest|head, slot.weapon.mainhand|offhand).");
            return true;
        }
        Tell(source, L"Replaced a slot on " + (replaced == 1 ? last : Count(replaced, L"player", L"players")) + L" with [" + itemLabel + L"]");
        return true;
    }

    BlockPos pos;
    if (!ParseBlockPos(source, words, 2, &pos))
        return true;
    const boost::shared_ptr<Container> container = boost::dynamic_pointer_cast<Container>(source.level->getBlockEntity(pos));
    if (!container || !SetContainerSlot(container, slotName, 0, stack))
    {
        Fail(source, L"That block has no such slot (slot.container.N, below the size of its container).");
        return true;
    }
    Tell(source, L"Replaced a slot at " + Position(pos) + L" with [" + itemLabel + L"]");
    return true;
}
