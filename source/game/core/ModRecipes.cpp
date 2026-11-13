#include "game/core/ModRecipes.hpp"

#include "game/core/ModItemNames.hpp"

#include <map>
#include <vector>

#include "game/item/FurnaceRecipes.hpp"
#include "game/item/Item.hpp"
#include "game/item/ItemInstance.hpp"
#include "game/item/Recipe.hpp"
#include "game/item/Recipes.hpp"
#include "game/item/ShapedRecipe.hpp"
#include "game/item/ShapelessRecipe.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/block/Block.hpp"
#include "platform/pc/Log.hpp"

static const Logger sLog("ModRecipes");

namespace
{
const size_t kMaxRecipes = 4096;
const size_t kMaxSide = 3;

struct Placement
{
    int group;
    std::wstring like;
};

struct Shaped
{
    Placement placement;
    std::wstring result;
    int count;
    std::vector<std::wstring> rows;
    std::map<wchar_t, std::wstring> key;
};

struct Shapeless
{
    Placement placement;
    std::wstring result;
    int count;
    std::vector<std::wstring> ingredients;
};

struct Smelting
{
    std::wstring input;
    std::wstring result;
    float experience;
};

bool sOpen = false;
std::vector<Shaped> sShaped;
std::vector<Shapeless> sShapeless;
std::vector<Smelting> sSmelting;

std::vector<std::wstring> Split(const std::wstring& text, wchar_t separator)
{
    std::vector<std::wstring> parts;
    std::wstring current;
    for (size_t index = 0; index < text.size(); ++index)
    {
        if (text[index] == separator)
        {
            parts.push_back(current);
            current.clear();
        }
        else
            current += text[index];
    }
    parts.push_back(current);
    return parts;
}

bool GroupOf(const std::wstring& name, int* group)
{
    static const wchar_t* const names[7] = {L"structures", L"tools", L"food", L"armour", L"mechanisms", L"vehicles", L"misc"};
    for (int index = 0; index < 7; ++index)
    {
        if (name == names[index])
        {
            *group = index;
            return true;
        }
    }
    return false;
}

int BaseTypeOf(const Placement& placement, ItemInstance* result)
{
    ItemInstance* like = NULL;
    if (!placement.like.empty() && ModItemNames::Resolve(placement.like, 1, 0, &like))
    {
        const int type = like->getItem()->getBaseItemType();
        delete like;
        return type;
    }
    return result->getItem()->getBaseItemType();
}

size_t Total()
{
    return sShaped.size() + sShapeless.size() + sSmelting.size();
}
}

void ModRecipes::Open()
{
    sOpen = true;
}

void ModRecipes::Close()
{
    sOpen = false;
}

bool ModRecipes::DeclareShaped(const std::wstring& result, int count, const std::wstring& rows, const std::wstring& key,
                   const std::wstring& group, const std::wstring& like)
{
    if (!sOpen || Total() >= kMaxRecipes || count < 1 || count > 64)
        return false;
    Shaped recipe;
    if (!GroupOf(group, &recipe.placement.group))
        return false;
    recipe.placement.like = like;
    recipe.result = result;
    recipe.count = count;
    recipe.rows = Split(rows, L'/');
    if (recipe.rows.empty() || recipe.rows.size() > kMaxSide)
        return false;
    for (size_t index = 0; index < recipe.rows.size(); ++index)
    {
        if (recipe.rows[index].empty() || recipe.rows[index].size() > kMaxSide
            || recipe.rows[index].size() != recipe.rows[0].size())
            return false;
    }
    const std::vector<std::wstring> pairs = Split(key, L',');
    for (size_t index = 0; index < pairs.size(); ++index)
    {
        if (pairs[index].size() < 3 || pairs[index][1] != L'=')
            return false;
        recipe.key[pairs[index][0]] = pairs[index].substr(2);
    }
    sShaped.push_back(recipe);
    return true;
}

bool ModRecipes::DeclareShapeless(const std::wstring& result, int count, const std::wstring& ingredients,
                      const std::wstring& group, const std::wstring& like)
{
    if (!sOpen || Total() >= kMaxRecipes || count < 1 || count > 64)
        return false;
    Shapeless recipe;
    if (!GroupOf(group, &recipe.placement.group))
        return false;
    recipe.placement.like = like;
    recipe.result = result;
    recipe.count = count;
    recipe.ingredients = Split(ingredients, L',');
    if (recipe.ingredients.empty() || recipe.ingredients.size() > kMaxSide * kMaxSide)
        return false;
    sShapeless.push_back(recipe);
    return true;
}

bool ModRecipes::DeclareSmelting(const std::wstring& input, const std::wstring& result, float experience)
{
    if (!sOpen || Total() >= kMaxRecipes || experience < 0.0f)
        return false;
    Smelting recipe = {input, result, experience};
    sSmelting.push_back(recipe);
    return true;
}

void ModRecipes::Finish()
{
    Recipes* recipes = Recipes::getInstance();
    const size_t before = recipes->getRecipes(Recipes::eRecipeCategory_Crafting)->size();
    for (size_t index = 0; index < sShaped.size(); ++index)
    {
        const Shaped& entry = sShaped[index];
        ItemInstance* result = NULL;
        const int width = static_cast<int>(entry.rows[0].size());
        const int height = static_cast<int>(entry.rows.size());
        ItemInstance** ingredients = new ItemInstance*[width * height];
        bool good = ModItemNames::Resolve(entry.result, entry.count, 0, &result);
        for (int slot = 0; good && slot < width * height; ++slot)
        {
            const wchar_t symbol = entry.rows[slot / width][slot % width];
            ingredients[slot] = ItemInstance::EMPTY.get();
            if (symbol == L' ')
                continue;
            std::map<wchar_t, std::wstring>::const_iterator named = entry.key.find(symbol);
            good = named != entry.key.end() && ModItemNames::Resolve(named->second, 1, -1, &ingredients[slot]);
        }
        if (!good)
        {
            LOG_WARN(sLog, "A shaped recipe for a mod was skipped: an item name was not found");
            delete[] ingredients;
            continue;
        }
        ShapedRecipe* recipe = new ShapedRecipe(width, height, ingredients, result, entry.placement.group);
        recipe->setBaseItemType(BaseTypeOf(entry.placement, result));
        recipes->addCustomRecipe(recipe, Recipes::eRecipeCategory_Crafting);
    }
    for (size_t index = 0; index < sShapeless.size(); ++index)
    {
        const Shapeless& entry = sShapeless[index];
        ItemInstance* result = NULL;
        std::vector<ItemInstance*>* ingredients = new std::vector<ItemInstance*>();
        bool good = ModItemNames::Resolve(entry.result, entry.count, 0, &result);
        for (size_t slot = 0; good && slot < entry.ingredients.size(); ++slot)
        {
            ItemInstance* ingredient = NULL;
            good = ModItemNames::Resolve(entry.ingredients[slot], 1, -1, &ingredient);
            if (good)
                ingredients->push_back(ingredient);
        }
        if (!good)
        {
            LOG_WARN(sLog, "A shapeless recipe for a mod was skipped: an item name was not found");
            delete ingredients;
            continue;
        }
        ShapelessRecipe* recipe = new ShapelessRecipe(result, ingredients, static_cast<Recipe::_eGroupType>(entry.placement.group));
        recipe->setBaseItemType(BaseTypeOf(entry.placement, result));
        recipes->addCustomRecipe(recipe, Recipes::eRecipeCategory_Crafting);
    }
    for (size_t index = 0; index < sSmelting.size(); ++index)
    {
        const Smelting& entry = sSmelting[index];
        ItemInstance* input = NULL;
        ItemInstance* result = NULL;
        if (!ModItemNames::Resolve(entry.input, 1, -1, &input) || !ModItemNames::Resolve(entry.result, 1, 0, &result))
        {
            LOG_WARN(sLog, "A smelting recipe for a mod was skipped: an item name was not found");
            continue;
        }
        FurnaceRecipes::getInstance()->addFurnaceRecipe(input->getItem(), result, entry.experience);
    }
    recipes->buildRecipeIngredientsArray();
    if (Total() > 0)
        LOG_INFO(sLog, "Mods added %d crafting recipes and %d smelting recipes",
                 static_cast<int>(recipes->getRecipes(Recipes::eRecipeCategory_Crafting)->size() - before),
                 static_cast<int>(sSmelting.size()));
}

