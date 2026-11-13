#include "game/core/ModEntities.hpp"
#include "game/core/ModText.hpp"
#include "game/core/ModStrings.hpp"
#include "game/entity/mob/MobCategory.hpp"
#include "game/world/biome/Biome.hpp"
#include "game/entity/mob/EntityIO.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/client/render/MobRenderer.hpp"
#include "game/client/render/ModModel.hpp"
#include "game/core/ModSurfaces.hpp"

#include <algorithm>
#include <vector>

#include "game/client/render/EntityRenderDispatcher.hpp"
#include "game/core/ModEntityBases_generated.hpp"
#include "game/entity/mob/EntityIO.hpp"

namespace
{
const size_t kMax = 16;
const int kFirstId = 200;
const int kPlaceShift = 40;

bool sOpen = false;
std::vector<ModEntities::Spec> sSpecs;

bool ByKey(const ModEntities::Spec& a, const ModEntities::Spec& b)
{
    return a.key < b.key;
}

struct Base
{
    const wchar_t* name;
    unsigned long long type;
    EntityIO::Creator creator;
};

const Base kBases[] = {LCE_MOD_ENTITY_BASES};

bool BaseOf(const std::wstring& name, unsigned long long* type, EntityIO::Creator* creator)
{
    for (size_t index = 0; index < sizeof(kBases) / sizeof(kBases[0]); ++index)
    {
        if (name == kBases[index].name)
        {
            *type = kBases[index].type;
            *creator = kBases[index].creator;
            return true;
        }
    }
    return false;
}
}

void ModEntities::Open()
{
    sOpen = true;
}

void ModEntities::Close()
{
    sOpen = false;
}

bool ModEntities::Declare(const std::string& owner, const std::wstring& name, const std::wstring& base, double health,
             double speed)
{
    unsigned long long type;
    EntityIO::Creator creator;
    std::wstring made = base;
    boost::shared_ptr<ModSurface> inherited;
    std::wstring inheritedModel;
    if (!BaseOf(base, &type, &creator))
    {
        // a mob already declared is a base too: the new one is made from the same game class
        const std::wstring parent = base.find(L':') != std::wstring::npos ? base : std::wstring(owner.begin(), owner.end()) + L":" + base;
        for (size_t index = 0; index < sSpecs.size(); ++index)
        {
            if (sSpecs[index].key == parent)
            {
                made = sSpecs[index].base;
                inherited = sSpecs[index].texture;
                inheritedModel = sSpecs[index].model;
            }
        }
    }
    if (!sOpen || sSpecs.size() >= kMax || !ModText::IsName(name) || !BaseOf(made, &type, &creator)
        || health < 1.0 || health > 1024.0 || speed < 0.1 || speed > 10.0)
        return false;
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        if (sSpecs[index].key == key)
            return false;
    }
    Spec spec = {key, made, health, speed, 0, type, 0, inherited, inheritedModel, std::wstring(), std::vector<SpawnRule>()};
    sSpecs.push_back(spec);
    return true;
}

bool ModEntities::DeclareTexture(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image)
{
    std::vector<int> argb;
    image->SnapshotArgb(&argb);
    const int width = image->Width();
    const int height = image->Height();
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    if (!sOpen || width < 1 || height < 1 || width > 1024 || height > 1024
        || argb.size() != static_cast<size_t>(width) * height)
        return false;
    std::string rgba;
    for (size_t index = 0; index < argb.size(); ++index)
    {
        const unsigned int pixel = static_cast<unsigned int>(argb[index]);
        rgba += static_cast<char>((pixel >> 16) & 0xFF);
        rgba += static_cast<char>((pixel >> 8) & 0xFF);
        rgba += static_cast<char>(pixel & 0xFF);
        rgba += static_cast<char>((pixel >> 24) & 0xFF);
    }
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        if (sSpecs[index].key != key)
            continue;
        boost::shared_ptr<ModSurface> surface(new ModSurface(owner, width, height));
        if (!surface->Write(0, 0, width, height, rgba))
            return false;
        sSpecs[index].texture = surface;
        return true;
    }
    return false;
}

bool ModEntities::DeclareSpawn(const std::string& owner, const std::wstring& name, const std::wstring& category, int weight, int minimum,
                               int maximum)
{
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    if (!sOpen || weight < 1 || weight > 1000 || minimum < 1 || maximum < minimum || maximum > 8
        || (category != L"monster" && category != L"creature" && category != L"water_creature" && category != L"ambient"))
        return false;
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        if (sSpecs[index].key == key && sSpecs[index].spawns.size() < 4)
        {
            SpawnRule rule = {category, weight, minimum, maximum};
            sSpecs[index].spawns.push_back(rule);
            return true;
        }
    }
    return false;
}

bool ModEntities::DeclareName(const std::string& owner, const std::wstring& name, const std::wstring& title)
{
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    if (!sOpen || title.empty() || title.size() > 256)
        return false;
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        if (sSpecs[index].key == key)
        {
            sSpecs[index].title = title;
            return true;
        }
    }
    return false;
}

bool ModEntities::DeclareModel(const std::string& owner, const std::wstring& name, const std::wstring& model)
{
    const std::wstring key = std::wstring(owner.begin(), owner.end()) + L":" + name;
    int width = 0;
    int height = 0;
    std::vector<ModModel::PartSpec> parts;
    std::wstring problem;
    if (!sOpen || !ModModel::Parse(model, &width, &height, &parts, &problem))
        return false;
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        if (sSpecs[index].key == key)
        {
            sSpecs[index].model = model;
            return true;
        }
    }
    return false;
}

int ModEntities::TextureOf(unsigned long long type, Textures* textures)
{
    const Spec* spec = Find(type);
    return spec != NULL && spec->texture ? spec->texture->Texture(textures) : -1;
}

void ModEntities::Register()
{
    std::sort(sSpecs.begin(), sSpecs.end(), ByKey);
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        Spec& spec = sSpecs[index];
        EntityIO::Creator creator;
        BaseOf(spec.base, &spec.baseType, &creator);
        spec.type = spec.baseType | (1ULL << (kPlaceShift + index));
        spec.id = kFirstId + static_cast<int>(index);
        EntityIO::Register(creator, spec.id, spec.key, spec.type, spec.key);
        for (size_t rule = 0; rule < spec.spawns.size(); ++rule)
        {
            const SpawnRule& spawn = spec.spawns[rule];
            MobCategory* category = spawn.category == L"monster"          ? MobCategory::MONSTER
                                    : spawn.category == L"creature"       ? MobCategory::CREATURE
                                    : spawn.category == L"water_creature" ? MobCategory::WATER_CREATURE
                                                                           : MobCategory::AMBIENT;
            for (int biomeId = 0; biomeId <= Biome::MAX_BIOME_ID; ++biomeId)
            {
                Biome* biome = Biome::biomes[biomeId];
                std::vector<Biome::MobSpawnerData*>* list = biome != NULL ? biome->getMobs(category) : NULL;
                if (list != NULL)
                    list->push_back(new Biome::MobSpawnerData(spawn.weight, spec.type, spawn.minimum, spawn.maximum));
            }
        }
        if (!spec.title.empty())
        {
            // the egg takes the colours of the mob this one is made from
            EntityIO::SpawnInfo::const_iterator base = EntityIO::sSpawnInfo.find(ResourceLocation(spec.base));
            if (base != EntityIO::sSpawnInfo.end())
                EntityIO::addSpawnInfo(ModStrings::Add(spec.title), spec.key, base->second->colour, base->second->secondaryColour,
                                       base->second->icon);
        }
    }
}

void ModEntities::BindRenderers(EntityRenderDispatcher* dispatcher)
{
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        EntityRenderDispatcher::iterator base = dispatcher->find(sSpecs[index].baseType);
        if (!sSpecs[index].model.empty())
            (*dispatcher)[sSpecs[index].type] = new MobRenderer(dispatcher, new ModModel(sSpecs[index].model), 0.5f);
        else if (base != dispatcher->end())
            (*dispatcher)[sSpecs[index].type] = base->second;
    }
}

std::vector<std::wstring> ModEntities::Keys()
{
    std::vector<std::wstring> keys;
    for (size_t index = 0; index < sSpecs.size(); ++index)
        keys.push_back(sSpecs[index].key);
    return keys;
}

bool ModEntities::Has(const std::wstring& key)
{
    for (size_t index = 0; index < sSpecs.size(); ++index)
    {
        if (sSpecs[index].key == key)
            return true;
    }
    return false;
}

const ModEntities::Spec* ModEntities::Find(unsigned long long type)
{
    // the table is resolved once in Register and indexed by the mob's place in it, which its type carries as one bit above bit 40,
    // so a lookup is a shift and an index and the render and tick paths that ask every frame do not scan
    const unsigned long long place = type >> kPlaceShift;
    if (place == 0 || (place & (place - 1)) != 0)
        return NULL;
    size_t index = 0;
    for (unsigned long long bits = place; bits > 1; bits >>= 1)
        ++index;
    if (index >= sSpecs.size() || sSpecs[index].type != type)
        return NULL;
    return &sSpecs[index];
}

