#include "game/command/chat/ClearChatCommand.hpp"

#include "game/command/CommandArguments.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/StringNbtReader.hpp"
#include "game/data/Tag.hpp"
#include "game/item/Inventory.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/ui/AbstractContainerMenu.hpp"

using CommandArguments::ParseInt;

ClearChatCommand::ClearChatCommand()
    : ChatCommand(L"clear", L"[player] [item]{<data>} [maxCount]", L"targets item number", ePermission_Operator)
{
}

bool ClearChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() > 4)
        return false;
    Item* item = NULL;
    int maximum = -1;
    if (words.size() > 3 && !ParseInt(words[3], -1, 2147483647, &maximum))
    {
        Fail(source, L"The maximum count must be -1 or more.");
        return true;
    }
    PlayerVector targets;
    if (!ResolvePlayers(source, words, 1, targets))
        return true;
    std::wstring tagText;
    if (words.size() > 2)
    {
        std::wstring itemName = words[2];
        const size_t brace = itemName.find(L'{');
        if (brace != std::wstring::npos)
        {
            tagText = itemName.substr(brace);
            itemName.resize(brace);
        }
        item = FindItem(source, itemName);
        if (item == NULL)
            return true;
    }
    CompoundTag* tag = NULL;
    if (!tagText.empty())
    {
        std::wstring error;
        Tag* parsed = StringNbtReader::parse(tagText, &error);
        if (parsed == NULL || parsed->getId() != Tag::TAG_COMPOUND)
        {
            delete parsed;
            Fail(source, L"The item data is not valid: " + error);
            return true;
        }
        tag = static_cast<CompoundTag*>(parsed);
    }
    for (size_t index = 0; index < targets.size(); ++index)
    {
        const PlayerPtr& target = targets[index];
        const int removed = target->mInventory->clearInventory(item, -1, maximum, tag);
        target->mInventoryMenu->broadcastChanges();
        if (maximum == 0)
            Tell(source,
                 target->getName() + L" has " + Count(removed, L"item", L"items") + L" matching the criteria");
        else if (removed > 0)
            Tell(source,
                 L"Cleared the inventory of " + target->getName() + L", removing " + Count(removed, L"item", L"items"));
        else
            Fail(source, L"Could not clear the inventory of " + target->getName() + L", no items to remove");
    }
    delete tag;
    return true;
}
