#include "game/core/ModItemKinds.hpp"

#include "game/core/ModKindTable.hpp"

#include "game/item/AxeItem.hpp"
#include "game/item/FoodItem.hpp"
#include "game/item/HoeItem.hpp"
#include "game/item/Item.hpp"
#include "game/item/ModItem_generated.hpp"
#include "game/item/PickaxeItem.hpp"
#include "game/item/ShovelItem.hpp"
#include "game/item/SwordItem.hpp"

namespace
{
typedef Item* (*Factory)(const ModItemKinds::Numbers& numbers);

// a tier of tool of the mod's own: level, uses, speed, damage bonus
const Item::Tier* TierOf(const ModItemKinds::Numbers& numbers)
{
    return new Item::Tier(static_cast<int>(numbers.a), static_cast<int>(numbers.b), numbers.c, numbers.d, 10, 5);
}

Item* MakeFood(const ModItemKinds::Numbers& numbers)
{
    return new ModItem<FoodItem>(static_cast<int>(numbers.a), numbers.b, numbers.c != 0.0f);
}

template <class Tool> Item* MakeTool(const ModItemKinds::Numbers& numbers)
{
    return new ModItem<Tool>(TierOf(numbers));
}

typedef ModKind<Factory> Kind;

const Kind kKinds[] = {{L"food", MakeFood},
                       {L"sword", MakeTool<SwordItem>},
                       {L"pickaxe", MakeTool<PickaxeItem>},
                       {L"axe", MakeTool<AxeItem>},
                       {L"shovel", MakeTool<ShovelItem>},
                       {L"hoe", MakeTool<HoeItem>}};
}

bool ModItemKinds::Known(const std::wstring& kind)
{
    return ModKindFind(kKinds, kind) != NULL;
}

Item* ModItemKinds::Make(const std::wstring& kind, const Numbers& numbers)
{
    const Kind* found = ModKindFind(kKinds, kind);
    return found != NULL ? found->make(numbers) : NULL;
}
