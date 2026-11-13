#ifndef GAME_CORE_MODITEMNAMES_HPP
#define GAME_CORE_MODITEMNAMES_HPP

#include <string>

class Item;
class ItemInstance;

namespace ModItemNames
{
// the item registered under a name, or NULL
Item* Find(const std::wstring& name);
// the name an item is registered under, such as minecraft:diamond, or an empty string for none
std::wstring NameOf(const Item* item);
bool Resolve(const std::wstring& name, int count, int aux, ItemInstance** out);
}

#endif
