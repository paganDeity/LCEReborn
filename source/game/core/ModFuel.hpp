#ifndef GAME_CORE_MODFUEL_HPP
#define GAME_CORE_MODFUEL_HPP

#include <string>

class Item;

class ModFuel
{
public:
    static void Open();
    static void Close();
    // Make an item or block burn in a furnace, only from Item_staticCtor_after: ticks is how long one lasts (200 smelts one item; coal is 1600, a plank 300).
    // It also overrides the game's own time for that item when it already burns.
    static bool Declare(const std::wstring& item, int ticks);
    static int Duration(Item* item);
};

#endif
