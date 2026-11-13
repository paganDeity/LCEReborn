#include "game/core/ModBlocks.hpp"
#include "game/core/ModText.hpp"
#include "game/client/render/ModModel.hpp"
#include "game/core/ModSurfaces.hpp"

#include <wchar.h>

#include <algorithm>
#include <vector>

#include "game/client/audio/SoundType.hpp"
#include "game/core/ModAtlasIcons.hpp"
#include "game/core/ModBlockKinds.hpp"
#include "game/core/ModIds.hpp"
#include "game/core/ModStrings.hpp"
#include "game/item/Item.hpp"
#include "game/item/ModItem_generated.hpp"
#include "game/world/block/BlockItem.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/ScriptedBlock.hpp"
#include "game/world/block/ScriptedEntityBlock.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/block/IdMapper.hpp"
#include "game/world/block/Material.hpp"
#include "platform/pc/Log.hpp"

static const Logger sLog("ModBlocks");

namespace
{
const int kFirstBlockId = 319;
const int kLastBlockId = 1023;
const size_t kMaxBlocksPerMod = 256;
const int kItemIdOffset = 768;

struct Entry
{
    std::wstring space;
    std::wstring name;
    std::wstring key;
    std::wstring title;
    std::wstring texture;
    std::wstring material;
    float hardness;
    float resistance;
    int light;
    std::vector<ScriptedBlock::PropertySpec> properties;
    std::vector<std::wstring> faceNames;
    std::vector<ScriptedBlock::FaceSpec> faces;
    std::vector<ScriptedBlock::ShapeSpec> shapes;
    bool entity;
    int slots;
    bool rotates;
    bool interactive;
    bool randomTick;
    std::wstring power;
    bool hasDrop;
    std::wstring dropItem;
    int dropMin;
    int dropMax;
    // the game class a mod object is made from, empty for a scripted block
    std::wstring kind;
    // the icon name a kind block gives its class, which adds the slot to it; the parent's when the block has none of its own
    std::wstring iconKey;
    std::wstring itemIcon;
    std::wstring model;
    boost::shared_ptr<ModSurface> modelPicture;
    std::vector<std::wstring> slotsGiven;
    Block* block;
    Item* item;
    int id;
    bool registered;
};

bool sOpen = false;
std::vector<Entry> sEntries;
ModIds sIds(L"LceModBlocks", kFirstBlockId, kLastBlockId);

struct MaterialKind
{
    const wchar_t* name;
    Material* material;
    const SoundType* sound;
};

bool FindMaterial(const std::wstring& name, MaterialKind* out)
{
    const MaterialKind kinds[] = {{L"stone", Material::STONE, SoundType::STONE},
                                  {L"wood", Material::WOOD, SoundType::WOOD},
                                  {L"dirt", Material::DIRT, SoundType::GRAVEL},
                                  {L"grass", Material::GRASS, SoundType::GRASS},
                                  {L"sand", Material::SAND, SoundType::SAND},
                                  {L"glass", Material::GLASS, SoundType::GLASS},
                                  {L"metal", Material::METAL, SoundType::METAL},
                                  {L"wool", Material::WOOL, SoundType::CLOTH},
                                  {L"ice", Material::ICE, SoundType::GLASS},
                                  {L"clay", Material::CLAY, SoundType::GRAVEL},
                                  {L"snow", Material::SNOW, SoundType::SNOW}};
    for (size_t index = 0; index < sizeof(kinds) / sizeof(kinds[0]); ++index)
    {
        if (name == kinds[index].name)
        {
            *out = kinds[index];
            return true;
        }
    }
    return false;
}

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
        Entry& entry = sEntries[index];
        if (entry.id < 0)
            continue;
        Block::blockRegistry->unbindId(entry.block);
        Item::itemRegistry.unbindId(entry.item);
        const BlockStateDefinition* definition = entry.block->getBlockStateDefinition();
        (void)definition;
        for (int data = 0; data < 16; ++data)
        {
            const BlockState* state = entry.block->getBlockState(data);
            if (state != NULL)
                Block::blockStateRegistry->removeMapping(const_cast<BlockState*>(state));
        }
        entry.id = -1;
    }
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        Entry& entry = sEntries[index];
        if (ids[index] < 0)
        {
            LOG_WARN(sLog, "No block number is left for a mod block");
            continue;
        }
        if (!entry.registered)
        {
            Block::registerBlock(ids[index], ResourceLocation(entry.space, entry.name), entry.block);
            Item::registerBlock(entry.block, entry.item);
            entry.registered = true;
        }
        else
        {
            Block::blockRegistry->bindId(entry.block, ids[index]);
            Item::itemRegistry.bindId(entry.item, ids[index] + kItemIdOffset);
        }
        Block::finishRegistration(entry.block);
        entry.id = ids[index];
    }
}
}

void ModBlocks::Open()
{
    sOpen = true;
}

void ModBlocks::Close()
{
    sOpen = false;
}

bool ModBlocks::Declare(const std::string& owner, const std::wstring& name, const std::wstring& title,
             const std::wstring& texture, const std::wstring& material, float hardness, float resistance, int light)
{
    MaterialKind kind;
    if (!sOpen || !ModText::IsName(name) || title.empty() || !FindMaterial(material, &kind) || hardness < -1.0f || hardness > 1000.0f
        || resistance < 0.0f || resistance > 100000.0f || light < 0 || light > 15)
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
    if (owned >= kMaxBlocksPerMod)
        return false;
    Entry entry;
    entry.space = space;
    entry.name = name;
    entry.key = space + L":" + name;
    entry.title = title;
    entry.texture = texture;
    entry.material = material;
    entry.hardness = hardness;
    entry.resistance = resistance;
    entry.light = light;
    entry.entity = false;
    entry.slots = 0;
    entry.rotates = false;
    entry.interactive = false;
    entry.randomTick = false;
    entry.hasDrop = false;
    entry.dropMin = 1;
    entry.dropMax = 1;
    entry.block = NULL;
    entry.item = NULL;
    entry.id = -1;
    entry.registered = false;
    sEntries.push_back(entry);
    return true;
}

namespace
{
// a block a new one derives from: mod:name, or the bare name of one of the caller's own, declared earlier
bool FindParent(const std::string& owner, const std::wstring& name, Entry* parent)
{
    const std::wstring key = name.find(L':') != std::wstring::npos ? name : std::wstring(owner.begin(), owner.end()) + L":" + name;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].key == key)
        {
            *parent = sEntries[index];
            return true;
        }
    }
    return false;
}

// everything a definition carries except its identity and the values the caller states
void Inherit(Entry& child, const Entry& parent)
{
    child.kind = parent.kind;
    child.iconKey = parent.iconKey;
    child.itemIcon = parent.itemIcon;
    child.model = parent.model;
    child.modelPicture = parent.modelPicture;
    child.texture = parent.texture;
    child.properties = parent.properties;
    child.faceNames = parent.faceNames;
    child.faces = parent.faces;
    child.shapes = parent.shapes;
    child.entity = parent.entity;
    child.slots = parent.slots;
    child.rotates = parent.rotates;
    child.interactive = parent.interactive;
    child.randomTick = parent.randomTick;
    child.power = parent.power;
    child.hasDrop = parent.hasDrop;
    child.dropItem = parent.dropItem;
    child.dropMin = parent.dropMin;
    child.dropMax = parent.dropMax;
}
}

bool ModBlocks::DeclareKind(const std::string& owner, const std::wstring& name, const std::wstring& kind,
                 const std::wstring& title, const std::wstring& material, float hardness, float resistance, int light)
{
    Entry parent;
    const bool derived = !ModBlockKinds::Known(kind) && FindParent(owner, kind, &parent);
    if (!derived && !ModBlockKinds::Known(kind))
        return false;
    if (!Declare(owner, name, title, L"", material, hardness, resistance, light))
        return false;
    if (derived)
        Inherit(sEntries.back(), parent);
    else
        sEntries.back().kind = kind;
    return true;
}

void ModBlocks::Missing(CompoundTag* tag, std::vector<std::wstring>* names)
{
    const std::vector<std::wstring> remote = ModIds::NamesIn(tag, L"LceModBlocks");
    for (size_t index = 0; index < remote.size(); ++index)
    {
        if (Find(remote[index]) == NULL)
            names->push_back(remote[index]);
    }
}

Block* ModBlocks::Find(const std::wstring& key)
{
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].key == key)
            return sEntries[index].block;
    }
    return NULL;
}

bool ModBlocks::Any()
{
    return !sEntries.empty();
}

void ModBlocks::BuildPalette(const unsigned short* elements, const unsigned short* extraElements, size_t count,
                  std::vector<PaletteEntry>* palette)
{
    palette->clear();
    std::vector<bool> seen(kLastBlockId + 1, false);
    for (int pass = 0; pass < 2; ++pass)
    {
        const unsigned short* values = pass == 0 ? elements : extraElements;
        for (size_t index = 0; index < count; ++index)
        {
            const int id = (values[index] & 0x7fff) >> 4;
            if (id >= kFirstBlockId && id <= kLastBlockId && !seen[id])
                seen[id] = true;
        }
    }
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        const int id = sEntries[index].id;
        if (id >= kFirstBlockId && id <= kLastBlockId && seen[id])
        {
            PaletteEntry entry = {id, sEntries[index].key};
            palette->push_back(entry);
        }
    }
}

void ModBlocks::BuildRemap(const std::vector<PaletteEntry>& palette, std::vector<unsigned short>* table)
{
    table->resize(kLastBlockId + 1);
    // without a palette the chunk was saved by a build whose mod numbers are unknown, so only the registration rule
    // below applies to it
    for (size_t id = 0; id < table->size(); ++id)
        (*table)[id] = static_cast<unsigned short>(id < static_cast<size_t>(kFirstBlockId) || palette.empty() ? id : 0);
    for (size_t index = 0; index < palette.size(); ++index)
    {
        if (palette[index].id < kFirstBlockId || palette[index].id > kLastBlockId)
            continue;
        unsigned short current = 0;
        for (size_t entry = 0; entry < sEntries.size(); ++entry)
        {
            if (sEntries[entry].key == palette[index].name && sEntries[entry].id >= 0)
                current = static_cast<unsigned short>(sEntries[entry].id);
        }
        if (current == 0)
            LOG_WARN(sLog, "A chunk holds a block of a mod that is not loaded; it becomes air");
        (*table)[palette[index].id] = current;
    }
    // a number no block is registered under, from an older save or another build, is air rather than a null block
    const std::vector<bool>& registered = RegisteredIds();
    for (size_t id = 0; id < table->size(); ++id)
    {
        if (!registered[(*table)[id]])
            (*table)[id] = 0;
    }
}

const std::vector<bool>& ModBlocks::RegisteredIds()
{
    static std::vector<bool> registered;
    if (registered.empty())
    {
        registered.resize(kLastBlockId + 1);
        for (size_t id = 0; id < registered.size(); ++id)
            registered[id] = Block::existsWithId(static_cast<int>(id));
    }
    return registered;
}

namespace
{
Entry* FindEntry(const std::string& owner, const std::wstring& name)
{
    // the scripted block settings only mean something for a scripted block
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].key == key)
            return sEntries[index].kind.empty() ? &sEntries[index] : NULL;
    }
    return NULL;
}

Entry* FindKindEntry(const std::string& owner, const std::wstring& name)
{
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].key == key)
            return sEntries[index].kind.empty() ? NULL : &sEntries[index];
    }
    return NULL;
}

// a picture for one slot of the game class a block is made from; the class finds it by the block's icon name
bool DeclareSlot(Entry* entry, const std::wstring& slot, int width, int height, const std::vector<int>& argb)
{
    const std::vector<std::wstring> slots = ModBlockKinds::Slots(entry->kind);
    const bool known = slots.empty() ? slot == L"all" : std::find(slots.begin(), slots.end(), slot) != slots.end();
    if (!known)
        return false;
    if (std::find(entry->slotsGiven.begin(), entry->slotsGiven.end(), slot) != entry->slotsGiven.end())
        return false;
    const std::wstring key = entry->key + (slots.empty() ? L"" : L"_" + slot);
    if (!ModAtlasIcons::Declare(ModAtlasIcons::eAtlas_Terrain, key, width, height, argb))
        return false;
    entry->slotsGiven.push_back(slot);
    if (entry->slotsGiven.size() == (slots.empty() ? 1u : slots.size()))
        entry->iconKey = entry->key;
    return true;
}

bool SideOf(const std::wstring& name, int* side)
{
    static const wchar_t* const names[8] = {L"down", L"up", L"north", L"south", L"west", L"east", L"sides", L"all"};
    for (int index = 0; index < 8; ++index)
    {
        if (name == names[index])
        {
            *side = index;
            return true;
        }
    }
    return false;
}

bool ParseWhen(const Entry& entry, const std::wstring& when, int* property, int* value)
{
    *property = -1;
    *value = 0;
    if (when.empty())
        return true;
    const size_t equals = when.find(L'=');
    if (equals == std::wstring::npos || equals + 1 >= when.size())
        return false;
    const std::wstring name = when.substr(0, equals);
    for (size_t index = 0; index < entry.properties.size(); ++index)
    {
        if (entry.properties[index].name != name)
            continue;
        int parsed = 0;
        for (size_t digit = equals + 1; digit < when.size(); ++digit)
        {
            if (when[digit] < L'0' || when[digit] > L'9' || parsed > 100)
                return false;
            parsed = parsed * 10 + (when[digit] - L'0');
        }
        if (parsed >= entry.properties[index].count)
            return false;
        *property = static_cast<int>(index);
        *value = parsed;
        return true;
    }
    return false;
}
}

bool ModBlocks::DeclareProperty(const std::string& owner, const std::wstring& name, const std::wstring& property, int count)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL || !ModText::IsName(property) || count < 2 || count > 16)
        return false;
    int product = count;
    for (size_t index = 0; index < entry->properties.size(); ++index)
    {
        if (entry->properties[index].name == property)
            return false;
        product *= entry->properties[index].count;
    }
    if (product > 16)
        return false;
    ScriptedBlock::PropertySpec spec = {property, count};
    entry->properties.push_back(spec);
    return true;
}

bool ModBlocks::DeclareDrop(const std::string& owner, const std::wstring& name, const std::wstring& item, int minimum,
                 int maximum)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL || minimum < 0 || maximum < minimum || maximum > 64)
        return false;
    entry->hasDrop = true;
    entry->dropItem = item;
    entry->dropMin = minimum;
    entry->dropMax = maximum;
    return true;
}

bool ModBlocks::DeclareRandomTicks(const std::string& owner, const std::wstring& name)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL)
        return false;
    entry->randomTick = true;
    return true;
}

bool ModBlocks::DeclarePower(const std::string& owner, const std::wstring& name, const std::wstring& property)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL)
        return false;
    for (size_t index = 0; index < entry->properties.size(); ++index)
    {
        if (entry->properties[index].name == property)
        {
            entry->power = property;
            return true;
        }
    }
    return false;
}

bool ModBlocks::DeclareInteractive(const std::string& owner, const std::wstring& name)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL)
        return false;
    entry->interactive = true;
    return true;
}

bool ModBlocks::DeclareRotates(const std::string& owner, const std::wstring& name)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL)
        return false;
    for (size_t index = 0; index < entry->properties.size(); ++index)
    {
        if (entry->properties[index].name == L"facing" && entry->properties[index].count >= 4)
        {
            entry->rotates = true;
            return true;
        }
    }
    return false;
}

bool ModBlocks::DeclareEntity(const std::string& owner, const std::wstring& name, int slots)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL || slots != 0 && slots != 27 && slots != 54)
        return false;
    entry->entity = true;
    entry->slots = slots;
    return true;
}

bool ModBlocks::DeclareShape(const std::string& owner, const std::wstring& name, const std::wstring& boxes,
                  const std::wstring& when)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    ScriptedBlock::ShapeSpec spec;
    if (entry == NULL || entry->shapes.size() >= 64 || !ParseWhen(*entry, when, &spec.property, &spec.value))
        return false;
    size_t start = 0;
    while (start <= boxes.size())
    {
        size_t end = boxes.find(L';', start);
        if (end == std::wstring::npos)
            end = boxes.size();
        const std::wstring text = boxes.substr(start, end - start);
        int values[6];
        int count = 0;
        size_t position = 0;
        while (position <= text.size() && count < 6)
        {
            size_t comma = text.find(L',', position);
            if (comma == std::wstring::npos)
                comma = text.size();
            if (comma == position || comma - position > 2)
                return false;
            int parsed = 0;
            for (size_t digit = position; digit < comma; ++digit)
            {
                if (text[digit] < L'0' || text[digit] > L'9')
                    return false;
                parsed = parsed * 10 + (text[digit] - L'0');
            }
            values[count++] = parsed;
            position = comma + 1;
            if (comma == text.size())
                break;
        }
        if (count != 6 || spec.boxes.size() >= 16)
            return false;
        for (int index = 0; index < 6; ++index)
        {
            if (values[index] > 16)
                return false;
        }
        if (values[0] >= values[3] || values[1] >= values[4] || values[2] >= values[5])
            return false;
        const ScriptedBlock::Box box = {values[0] / 16.0f, values[1] / 16.0f, values[2] / 16.0f,
                                        values[3] / 16.0f, values[4] / 16.0f, values[5] / 16.0f};
        spec.boxes.push_back(box);
        start = end + 1;
    }
    entry->shapes.push_back(spec);
    return true;
}

bool ModBlocks::DeclareFace(const std::string& owner, const std::wstring& name, const std::wstring& side,
                 const std::wstring& texture, const std::wstring& when)
{
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    ScriptedBlock::FaceSpec spec;
    if (entry == NULL || !SideOf(side, &spec.side) || texture.empty() || entry->faces.size() >= 256
        || !ParseWhen(*entry, when, &spec.property, &spec.value))
        return false;
    spec.key = texture;
    entry->faces.push_back(spec);
    return true;
}

bool ModBlocks::DeclareModel(const std::string& owner, const std::wstring& name, const std::wstring& model, const boost::shared_ptr<ModSurface>& picture)
{
    int width = 0;
    int height = 0;
    std::vector<ModModel::PartSpec> parts;
    std::wstring problem;
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    if (!sOpen || !picture || !ModModel::Parse(model, &width, &height, &parts, &problem))
        return false;
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].key == key && sEntries[index].entity)
        {
            sEntries[index].model = model;
            sEntries[index].modelPicture = picture;
            return true;
        }
    }
    return false;
}

bool ModBlocks::ModelOf(const std::wstring& key, std::wstring* model, boost::shared_ptr<ModSurface>* picture)
{
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        if (sEntries[index].key == key && !sEntries[index].model.empty())
        {
            *model = sEntries[index].model;
            *picture = sEntries[index].modelPicture;
            return true;
        }
    }
    return false;
}

bool ModBlocks::DeclareItemIcon(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image)
{
    std::vector<int> argb;
    image->SnapshotArgb(&argb);
    Entry* entry = sOpen ? FindKindEntry(owner, name) : NULL;
    if (entry == NULL || !ModBlockKinds::OwnItem(entry->kind)
        || !ModAtlasIcons::Declare(ModAtlasIcons::eAtlas_Items, entry->key, image->Width(), image->Height(), argb))
        return false;
    entry->itemIcon = entry->key;
    return true;
}

bool ModBlocks::DeclareTexture(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image,
                               const std::wstring& side, const std::wstring& when)
{
    std::vector<int> argb;
    image->SnapshotArgb(&argb);
    const int width = image->Width();
    const int height = image->Height();
    Entry* kinded = sOpen ? FindKindEntry(owner, name) : NULL;
    if (kinded != NULL)
        return DeclareSlot(kinded, side, width, height, argb);
    Entry* entry = sOpen ? FindEntry(owner, name) : NULL;
    if (entry == NULL)
        return false;
    wchar_t index[16];
    swprintf(index, 16, L"%d", static_cast<int>(entry->faces.size()));
    const std::wstring key = entry->key + L"#" + index;
    if (!DeclareFace(owner, name, side, key, when))
        return false;
    if (!ModAtlasIcons::Declare(ModAtlasIcons::eAtlas_Terrain, key, width, height, argb))
    {
        entry->faces.pop_back();
        return false;
    }
    return true;
}

void ModBlocks::Create()
{
    std::sort(sEntries.begin(), sEntries.end(), ByKey);
    for (size_t index = 0; index < sEntries.size(); ++index)
    {
        Entry& entry = sEntries[index];
        MaterialKind kind;
        FindMaterial(entry.material, &kind);
        Block* block = NULL;
        Item* item = NULL;
        if (!entry.kind.empty())
        {
            ModBlockKinds::Made made;
            ModBlockKinds::Make(entry.kind, kind.material, &made);
            block = made.block;
            item = made.item;
            if (!entry.iconKey.empty())
                block->setIconName(entry.iconKey);
        }
        else
        {
            std::vector<ScriptedBlock::FaceSpec> faces = entry.faces;
            ScriptedBlock::FaceSpec base = {ScriptedBlock::eSide_All, -1, 0, entry.texture};
            faces.insert(faces.begin(), base);
            ScriptedBlock* scripted = entry.entity
                                          ? new ScriptedEntityBlock(kind.material, entry.properties, faces,
                                                                    entry.shapes, entry.slots)
                                          : new ScriptedBlock(kind.material, entry.properties, faces, entry.shapes);
            block = scripted;
            block->setIconName(entry.texture);
            if (entry.hasDrop)
                scripted->SetDrop(entry.dropItem, entry.dropMin, entry.dropMax);
            if (entry.rotates)
                scripted->SetRotates();
            if (entry.interactive)
                scripted->SetInteractive();
            if (entry.randomTick)
                scripted->setTicking(true);
            if (!entry.power.empty())
                scripted->SetPower(entry.power);
        }
        block->setDestroyTime(entry.hardness);
        block->setExplodeable(entry.resistance);
        block->setSoundType(kind.sound);
        block->setLightEmission(static_cast<float>(entry.light) / 15.0f);
        // a block's item takes its name from the block, so the name goes to the block; an item of another class (a door) has its own
        const int description = ModStrings::Add(entry.title);
        block->setNameAndDescriptionId(description, description);
        if (item == NULL)
            item = new ModItem<BlockItem>(block);
        else
        {
            item->setDescriptionId(description);
            if (!entry.itemIcon.empty())
                item->setIconName(entry.itemIcon);
        }
        entry.block = block;
        entry.item = item;
    }
    Assign();
}

void ModBlocks::Reset()
{
    sIds.Clear();
    Assign();
}

void ModBlocks::Merge(CompoundTag* tag)
{
    sIds.Merge(tag);
    Assign();
}

void ModBlocks::Load(CompoundTag* tag)
{
    sIds.Load(tag);
    Assign();
}

void ModBlocks::Save(CompoundTag* tag)
{
    sIds.Save(tag);
}

void ModBlocks::SaveDeclared(CompoundTag* tag)
{
    std::vector<std::wstring> keys;
    for (size_t index = 0; index < sEntries.size(); ++index)
        keys.push_back(sEntries[index].key);
    sIds.SaveOnly(tag, keys);
}

