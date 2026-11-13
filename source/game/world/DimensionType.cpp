#include "game/world/DimensionType.hpp"

#include <vector>

#include "game/world/Dimension.hpp"
#include "game/world/NetherDimension.hpp"
#include "game/world/NormalDimension.hpp"
#include "game/world/ScriptedDimension.hpp"
#include "game/world/TheEndDimension.hpp"

namespace
{
// every type, in the order it was made, so a type's index is its place here
std::vector<const DimensionType*>& Types()
{
    static std::vector<const DimensionType*> types;
    return types;
}
}

const DimensionType* DimensionType::OVERWORLD = new DimensionType(0, L"Overworld", L"");
const DimensionType* DimensionType::NETHER = new DimensionType(-1, L"Nether", L"_nether");
const DimensionType* DimensionType::THE_END = new DimensionType(1, L"The End", L"_end");

DimensionType::DimensionType(int id, const std::wstring& name, const std::wstring& fileSuffix)
{
    this->id = id;
    this->name = name;
    this->fileSuffix = fileSuffix;
    this->index = static_cast<int>(Types().size());
    Types().push_back(this);
}

int DimensionType::getIndex() const
{
    return index;
}

bool DimensionType::isModded() const
{
    return index >= 3;
}

int DimensionType::getClientSlot() const
{
    return index < 3 ? index : 0;
}

int DimensionType::count()
{
    return static_cast<int>(Types().size());
}

const DimensionType* DimensionType::at(int index)
{
    return index >= 0 && index < count() ? Types()[index] : OVERWORLD;
}

const DimensionType* DimensionType::Register(int id, const std::wstring& name)
{
    if (id < 2 || id > 100 || name.empty() || count() >= kMaxDimensions)
        return NULL;
    for (size_t position = 0; position < Types().size(); ++position)
    {
        if (Types()[position]->getId() == id || Types()[position]->getName() == name)
            return NULL;
    }
    return new DimensionType(id, name, L"_" + name);
}

int DimensionType::getId() const
{
    return id;
}

std::wstring DimensionType::getName() const
{
    return name;
}

std::wstring DimensionType::getFileSuffix() const
{
    return fileSuffix;
}

Dimension* DimensionType::create() const
{
    if (isModded())
        return new ScriptedDimension(this);

    if (id == 1)
    {
        return new TheEndDimension();
    }

    if (id == -1)
    {
        return new NetherDimension();
    }

    return new NormalDimension();
}

const DimensionType* DimensionType::getById(int id)
{
    for (size_t position = 0; position < Types().size(); ++position)
    {
        if (Types()[position]->getId() == id)
            return Types()[position];
    }
    return OVERWORLD;
}

const DimensionType* DimensionType::getByName(const std::wstring& name)
{
    for (size_t position = 0; position < Types().size(); ++position)
    {
        if (Types()[position]->getName() == name)
            return Types()[position];
    }
    return OVERWORLD;
}
