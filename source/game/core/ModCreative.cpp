#include "game/core/ModCreative.hpp"

#include <vector>

#include "game/core/ModItemNames.hpp"
#include "game/item/ItemInstance.hpp"
#include "game/ui/UIScene_CreativeMenu.hpp"
#include "platform/pc/Log.hpp"

static const Logger sLog("ModCreative");

namespace
{
const size_t kMaxEntries = 4096;
const int kTabCount = 8;

struct Entry
{
    std::wstring item;
    int tab;
};

bool sOpen = false;
std::vector<Entry> sEntries;

UIScene_CreativeMenu::ECreative_Inventory_Groups GroupOf(int tab)
{
    static const UIScene_CreativeMenu::ECreative_Inventory_Groups groups[kTabCount] = {
        UIScene_CreativeMenu::ECreative_Inventory_Groups_BuildingBlocks,
        UIScene_CreativeMenu::ECreative_Inventory_Groups_Decoration,
        UIScene_CreativeMenu::ECreative_Inventory_Groups_Redstone,
        UIScene_CreativeMenu::ECreative_Inventory_Groups_Materials,
        UIScene_CreativeMenu::ECreative_Inventory_Groups_Food,
        UIScene_CreativeMenu::ECreative_Inventory_Groups_ToolsAndCombat,
        UIScene_CreativeMenu::ECreative_Inventory_Groups_BrewingIngredients,
        UIScene_CreativeMenu::ECreative_Inventory_Groups_Miscellaneous};
    return groups[tab];
}
}

void ModCreative::Open()
{
    sOpen = true;
}

void ModCreative::Close()
{
    sOpen = false;
}

bool ModCreative::Declare(const std::wstring& item, int tab)
{
    if (!sOpen || tab < 0 || tab >= kTabCount || sEntries.size() >= kMaxEntries)
        return false;
    Entry entry = {item, tab};
    sEntries.push_back(entry);
    return true;
}

void ModCreative::Finish()
{
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        ItemInstance* stack = NULL;
        if (!ModItemNames::Resolve(sEntries[index].item, 1, 0, &stack))
        {
            LOG_WARN(sLog, "A creative menu entry for a mod was skipped: an item name was not found");
            continue;
        }
        UIScene_CreativeMenu::addToGroup(GroupOf(sEntries[index].tab), *stack);
        delete stack;
    }
}
