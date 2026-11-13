#ifndef GAME_CORE_MODRECIPES_HPP
#define GAME_CORE_MODRECIPES_HPP

#include <string>

class ModRecipes
{
public:
    static void Open();
    static void Close();
    // Add a crafting table recipe, only from Item_staticCtor_after. rows is up to 3 rows of up to 3 symbols joined by / (a space is an
    // empty slot), for instance "##/ S"; key says what each symbol is, as symbol=name joined by commas, for instance
    // "#=minecraft:planks,S=mymod:ruby". result is a name too, count how many it makes. It joins the crafting menu's recipes like any
    // other, in the tab group names (structures, tools, food, armour, mechanisms, vehicles or misc) and in the row of the item named
    // like: the menu sorts a tab into rows by kind of item, and a mod item has no kind of its own, so name a game item of the kind you
    // want it beside (for a lamp, minecraft:redstone_lamp; an empty like uses the result's own kind, which is a row of its own and is
    // dropped when the tab has no row free). False when the call is too late or the text is malformed; a name no item or block has is
    // reported in the log and the recipe skipped.
    static bool DeclareShaped(const std::wstring& result, int count, const std::wstring& rows, const std::wstring& key,
                       const std::wstring& group, const std::wstring& like);
    // Add a crafting recipe where the order does not matter, only from Item_staticCtor_after. ingredients is up to 9 names joined by
    // commas; repeat a name to need more than one. group and like are as for shaped.
    static bool DeclareShapeless(const std::wstring& result, int count, const std::wstring& ingredients,
                          const std::wstring& group, const std::wstring& like);
    // Add a furnace recipe, only from Item_staticCtor_after, giving experience when the result is taken.
    static bool DeclareSmelting(const std::wstring& input, const std::wstring& result, float experience);
    static void Finish();
};

#endif
