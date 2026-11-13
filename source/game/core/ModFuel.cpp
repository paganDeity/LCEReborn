#include "game/core/ModFuel.hpp"

#include <map>
#include <vector>

#include "game/core/ModItemNames.hpp"
#include "game/item/Item.hpp"
#include "game/item/ItemInstance.hpp"

namespace
{
const size_t kMaxEntries = 1024;

struct Entry
{
    std::wstring item;
    int ticks;
};

bool sOpen = false;
bool sResolved = false;
std::vector<Entry> sEntries;
std::map<Item*, int> sDurations;

void Resolve()
{
    sResolved = true;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        ItemInstance* stack = NULL;
        if (!ModItemNames::Resolve(sEntries[index].item, 1, 0, &stack))
            continue;
        sDurations[stack->getItem()] = sEntries[index].ticks;
        delete stack;
    }
}
}

void ModFuel::Open()
{
    sOpen = true;
}

void ModFuel::Close()
{
    sOpen = false;
}

bool ModFuel::Declare(const std::wstring& item, int ticks)
{
    if (!sOpen || ticks < 0 || ticks > 1000000 || sEntries.size() >= kMaxEntries)
        return false;
    Entry entry = {item, ticks};
    sEntries.push_back(entry);
    return true;
}

int ModFuel::Duration(Item* item)
{
    if (sEntries.empty())
        return -1;
    if (!sResolved)
        Resolve();
    std::map<Item*, int>::const_iterator found = sDurations.find(item);
    return found == sDurations.end() ? -1 : found->second;
}
