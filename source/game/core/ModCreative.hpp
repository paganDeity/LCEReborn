#ifndef GAME_CORE_MODCREATIVE_HPP
#define GAME_CORE_MODCREATIVE_HPP

#include <string>

class ModCreative
{
public:
    static void Open();
    static void Close();
    // Show an item or block in the creative menu, only from Item_staticCtor_after. tab is 0 building, 1 decoration, 2 redstone, 3 materials, 4 food, 5 tools
    // and combat, 6 brewing, 7 miscellaneous. The menu has these eight tabs and no more.
    static bool Declare(const std::wstring& item, int tab);
    static void Finish();
};

#endif
