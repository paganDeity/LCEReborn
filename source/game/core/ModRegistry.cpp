#include "game/core/ModRegistry.hpp"
#include "game/core/ModText.hpp"

#include <wchar.h>

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "game/core/ModCreative.hpp"
#include "game/core/ModBlocks.hpp"
#include "game/core/ModEntities.hpp"
#include "game/core/ModFuel.hpp"
#include "game/core/ModIds.hpp"
#include "game/core/ModItemKinds.hpp"
#include "game/core/ModStrings.hpp"
#include "game/core/ModSurfaces.hpp"
#include "game/world/DimensionType.hpp"
#include "game/core/ModAtlasIcons.hpp"
#include "game/core/ModRecipes.hpp"
#include "game/core/ModSounds.hpp"
#include "game/item/Item.hpp"
#include "game/item/ModItem_generated.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/core/NbtIo.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/ListTag.hpp"

namespace
{
const int kFirstItemId = 4096;
const int kLastItemId = 32000;
const size_t kMaxItemsPerMod = 256;

struct Entry
{
    std::wstring space;
    std::wstring name;
    std::wstring key;
    std::wstring title;
    std::wstring icon;
    int stackSize;
    // the game class an item is made from, empty for a plain mod item
    std::wstring kind;
    ModItemKinds::Numbers numbers;
    Item* item;
    int id;
    bool registered;
};

bool sOpen = false;
std::vector<Entry> sEntries;
ModIds sIds(L"LceModItems", kFirstItemId, kLastItemId);

bool ByKey(const Entry& a, const Entry& b)
{
    return a.key < b.key;
}

void Assign()
{
    std::vector<int> ids;
    if (!sIds.Reassign(sEntries, &ids))
        return;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].id >= 0)
        {
            Item::itemRegistry.unbindId(sEntries[index].item);
            sEntries[index].id = -1;
        }
    }
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        Entry& entry = sEntries[index];
        if (ids[index] < 0)
            continue;
        if (!entry.registered)
        {
            Item::itemRegistry.registerMapping(ids[index], ResourceLocation(entry.space, entry.name), entry.item);
            entry.registered = true;
        }
        else
            Item::itemRegistry.bindId(entry.item, ids[index]);
        entry.id = ids[index];
    }
}
}

bool ModRegistry::DeclareDimension(const std::string& owner, const std::wstring& name, int id)
{
    if (!sOpen || !ModText::IsName(name))
        return false;
    return DimensionType::Register(id, std::wstring(owner.begin(), owner.end()) + L"_" + name) != NULL;
}

bool ModRegistry::DeclareItemOf(const std::string& owner, const std::wstring& name, const std::wstring& kind,
                   const std::wstring& title, const std::wstring& icon, float a, float b, float c, float d)
{
    // a kind is a class of the game, or an item already registered whose class and stack size the new one inherits
    std::wstring made = kind;
    int stackSize = 1;
    if (!ModItemKinds::Known(kind))
    {
        const std::wstring key = kind.find(L':') != std::wstring::npos ? kind : std::wstring(owner.begin(), owner.end()) + L":" + kind;
        const Entry* parent = NULL;
        for (size_t index = 0; index < sEntries.size(); ++index)
        {
            if (sEntries[index].key == key && !sEntries[index].kind.empty())
                parent = &sEntries[index];
        }
        if (parent == NULL)
            return false;
        made = parent->kind;
        stackSize = parent->stackSize;
    }
    if (!DeclareItem(owner, name, title, icon, stackSize))
        return false;
    Entry& entry = sEntries.back();
    entry.kind = made;
    entry.numbers.a = a;
    entry.numbers.b = b;
    entry.numbers.c = c;
    entry.numbers.d = d;
    return true;
}

bool ModRegistry::DeclareItem(const std::string& owner, const std::wstring& name, const std::wstring& title,
                 const std::wstring& icon, int stackSize)
{
    if (!sOpen || !ModText::IsName(name) || title.empty() || stackSize < 1 || stackSize > 64)
        return false;
    const std::wstring space(owner.begin(), owner.end());
    size_t owned = 0;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].space == space)
            ++owned;
        if (sEntries[index].space == space && sEntries[index].name == name)
            return false;
    }
    if (owned >= kMaxItemsPerMod)
        return false;
    Entry entry;
    entry.space = space;
    entry.name = name;
    entry.key = space + L":" + name;
    entry.title = title;
    entry.icon = icon;
    entry.stackSize = stackSize;
    entry.numbers.a = entry.numbers.b = entry.numbers.c = entry.numbers.d = 0.0f;
    entry.item = NULL;
    entry.id = -1;
    entry.registered = false;
    sEntries.push_back(entry);
    return true;
}

bool ModRegistry::DeclareItemIcon(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image)
{
    std::vector<int> argb;
    image->SnapshotArgb(&argb);
    const int width = image->Width();
    const int height = image->Height();
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    if (!sOpen)
        return false;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].key == key)
        {
            if (!ModAtlasIcons::Declare(ModAtlasIcons::eAtlas_Items, key, width, height, argb))
                return false;
            sEntries[index].icon = key;
            return true;
        }
    }
    // not an item of the mod's own: the item a block of the game's own class places (a door) may be what is meant
    return ModBlocks::DeclareItemIcon(owner, name, image);
}

void ModRegistry::Open()
{
    sOpen = true;
    ModRecipes::Open();
    ModCreative::Open();
    ModSounds::Open();
    ModBlocks::Open();
    ModFuel::Open();
    ModEntities::Open();
}

void ModRegistry::Close()
{
    ModEntities::Close();
    ModFuel::Close();
    ModBlocks::Close();
    ModSounds::Close();
    ModCreative::Close();
    ModRecipes::Close();
    sOpen = false;
    ModBlocks::Create();
    std::sort(sEntries.begin(), sEntries.end(), ByKey);
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        Entry& entry = sEntries[index];
        // a plain mod item is the game's Item with a name, a picture and a stack size; a kind is another of the game's item classes
        entry.item = entry.kind.empty() ? new ModItem<Item>() : ModItemKinds::Make(entry.kind, entry.numbers);
        entry.item->setIconName(entry.icon);
        entry.item->setDescriptionId(ModStrings::Add(entry.title));
        if (entry.kind.empty())
            entry.item->setMaxStackSize(entry.stackSize);
    }
    Assign();
}

void ModRegistry::Reset()
{
    sIds.Clear();
    ModBlocks::Reset();
    Assign();
}

void ModRegistry::Load(CompoundTag* tag)
{
    sIds.Load(tag);
    ModBlocks::Load(tag);
    Assign();
}

void ModRegistry::Save(CompoundTag* tag)
{
    sIds.Save(tag);
    ModBlocks::Save(tag);
}

namespace
{
// a list of names in a tag: how the server tells a client which mobs and dimensions it has
void PutNames(CompoundTag* tag, const std::wstring& key, const std::vector<std::wstring>& names)
{
    tag->putInt(key + L"Count", static_cast<int>(names.size()));
    for (size_t index = 0; index < names.size(); ++index)
    {
        wchar_t number[16];
        swprintf(number, 16, L"%d", static_cast<int>(index));
        tag->putString(key + number, names[index]);
    }
}

std::vector<std::wstring> GetNames(CompoundTag* tag, const std::wstring& key)
{
    std::vector<std::wstring> names;
    const int count = tag->contains(key + L"Count") ? tag->getInt(key + L"Count") : 0;
    for (int index = 0; index < count && index < 64; ++index)
    {
        wchar_t number[16];
        swprintf(number, 16, L"%d", index);
        names.push_back(tag->getString(key + number));
    }
    return names;
}

std::vector<std::wstring> ModDimensionNames()
{
    std::vector<std::wstring> names;
    for (int index = 3; index < DimensionType::count(); ++index)
        names.push_back(DimensionType::at(index)->getName());
    return names;
}
}

std::vector<unsigned char> ModRegistry::WriteSync()
{
    std::vector<unsigned char> bytes;
    if (sEntries.empty() && !ModBlocks::Any() && ModEntities::Keys().empty() && ModDimensionNames().empty())
        return bytes;
    CompoundTag* tag = new CompoundTag();
    std::vector<std::wstring> keys;
    for (size_t index = 0; index < sEntries.size(); ++index)
        keys.push_back(sEntries[index].key);
    sIds.SaveOnly(tag, keys);
    ModBlocks::SaveDeclared(tag);
    PutNames(tag, L"LceModMobs", ModEntities::Keys());
    PutNames(tag, L"LceModDimensions", ModDimensionNames());
    arrayWithLength<unsigned char> packed = NbtIo::compress(tag);
    bytes.assign(packed.data, packed.data + packed.length);
    return bytes;
}

bool ModRegistry::ApplySync(const std::vector<unsigned char>& data, std::vector<std::wstring>* missing)
{
    if (data.empty())
        return false;
    std::vector<unsigned char> copy(data);
    CompoundTag* tag = NbtIo::decompress(arrayWithLength<unsigned char>(&copy[0], static_cast<unsigned int>(copy.size())));
    if (tag == NULL)
        return false;
    const std::vector<std::wstring> items = ModIds::NamesIn(tag, L"LceModItems");
    for (size_t index = 0; index < items.size(); ++index)
    {
        bool found = false;
        for (size_t entry = 0; entry < sEntries.size(); ++entry)
            found = found || sEntries[entry].key == items[index];
        if (!found)
            missing->push_back(items[index]);
    }
    ModBlocks::Missing(tag, missing);
    const std::vector<std::wstring> mobs = GetNames(tag, L"LceModMobs");
    for (size_t index = 0; index < mobs.size(); ++index)
    {
        if (!ModEntities::Has(mobs[index]))
            missing->push_back(mobs[index]);
    }
    const std::vector<std::wstring> dimensions = GetNames(tag, L"LceModDimensions");
    for (size_t index = 0; index < dimensions.size(); ++index)
    {
        if (DimensionType::getByName(dimensions[index])->getName() != dimensions[index])
            missing->push_back(dimensions[index]);
    }
    if (!missing->empty())
        return true;
    sIds.Merge(tag);
    ModBlocks::Merge(tag);
    Assign();
    return true;
}

