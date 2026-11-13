#include "game/core/ModItemNames.hpp"

#include "game/item/Item.hpp"
#include "game/item/ItemInstance.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/block/Block.hpp"

Item* ModItemNames::Find(const std::wstring& name)
{
    const ResourceLocation location(name);
    return Item::itemRegistry.containsKey(location) ? Item::itemRegistry.get(location) : NULL;
}

std::wstring ModItemNames::NameOf(const Item* item)
{
    return item != NULL ? Item::itemRegistry.getKey(const_cast<Item*>(item)).toString() : std::wstring();
}

bool ModItemNames::Resolve(const std::wstring& name, int count, int aux, ItemInstance** out)
{
    const ResourceLocation location(name);
    if (Item::itemRegistry.containsKey(ResourceLocation(location)))
    {
        Item* item = Item::itemRegistry.get(ResourceLocation(location));
        if (item != NULL)
        {
            *out = new ItemInstance(item, count, aux < 0 ? 0 : aux);
            return true;
        }
    }
    if (Block::blockRegistry->containsKey(ResourceLocation(location)))
    {
        Block* block = Block::blockRegistry->get(ResourceLocation(location));
        if (block != NULL)
        {
            *out = new ItemInstance(block, count, aux);
            return true;
        }
    }
    return false;
}
