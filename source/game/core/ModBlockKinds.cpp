#include "game/core/ModBlockKinds.hpp"

#include "game/core/ModKindTable.hpp"

#include "game/item/DoorItem.hpp"
#include "game/world/block/Material.hpp"
#include "game/world/block/DoorBlock.hpp"
#include "game/world/block/FenceBlock.hpp"
#include "game/world/block/LadderBlock.hpp"
#include "game/world/block/LeverBlock.hpp"
#include "game/world/block/ModBlock_generated.hpp"
#include "game/world/block/PressurePlateBlock.hpp"
#include "game/world/block/StoneButtonBlock.hpp"
#include "game/world/block/WoodButtonBlock.hpp"
#include "game/world/block/TrapDoorBlock.hpp"

namespace
{
typedef ModBlockKinds::Made (*Factory)(Material* material);

ModBlockKinds::Made Plain(Block* block)
{
    ModBlockKinds::Made made = {block, NULL};
    return made;
}

ModBlockKinds::Made MakeDoor(Material* material)
{
    ModBlockKinds::Made made = {new ModBlock<DoorBlock>(material), NULL};
    made.item = new DoorItem(made.block);
    return made;
}

ModBlockKinds::Made MakeTrapDoor(Material* material)
{
    return Plain(new ModBlock<TrapDoorBlock>(material));
}

ModBlockKinds::Made MakeButton(Material* material)
{
    return material == Material::WOOD ? Plain(new ModBlock<WoodButtonBlock>()) : Plain(new ModBlock<StoneButtonBlock>());
}

ModBlockKinds::Made MakePressurePlate(Material* material)
{
    return Plain(new ModBlock<PressurePlateBlock>(L"pressurePlate", material,
                                                  material == Material::WOOD ? PressurePlateBlock::EVERYTHING
                                                                             : PressurePlateBlock::MOBS));
}

ModBlockKinds::Made MakeLever(Material*)
{
    return Plain(new ModBlock<LeverBlock>());
}

ModBlockKinds::Made MakeLadder(Material*)
{
    return Plain(new ModBlock<LadderBlock>());
}

ModBlockKinds::Made MakeFence(Material* material)
{
    return Plain(new ModBlock<FenceBlock>(L"fence", material, material->getColor()));
}

struct Kind
{
    const wchar_t* name;
    Factory make;
    // the pictures the class reads, comma separated, appended to the block's icon name; empty when it has one picture
    const wchar_t* slots;
    // the class places itself with an item of its own (a door) instead of a plain block item
    bool ownItem;
};

const Kind kKinds[] = {{L"door", MakeDoor, L"upper,lower", true},
                       {L"trapdoor", MakeTrapDoor, L"", false},
                       {L"button", MakeButton, L"", false},
                       {L"pressure_plate", MakePressurePlate, L"", false},
                       {L"lever", MakeLever, L"", false},
                       {L"ladder", MakeLadder, L"", false},
                       {L"fence", MakeFence, L"", false}};
}

bool ModBlockKinds::Known(const std::wstring& kind)
{
    return ModKindFind(kKinds, kind) != NULL;
}

bool ModBlockKinds::Make(const std::wstring& kind, Material* material, Made* made)
{
    const Kind* found = ModKindFind(kKinds, kind);
    if (found == NULL)
        return false;
    *made = found->make(material);
    return true;
}

std::vector<std::wstring> ModBlockKinds::Slots(const std::wstring& kind)
{
    std::vector<std::wstring> slots;
    const Kind* found = ModKindFind(kKinds, kind);
    if (found == NULL)
        return slots;
    const std::wstring list = found->slots;
    for (size_t start = 0; start < list.size();)
    {
        size_t end = list.find(L',', start);
        if (end == std::wstring::npos)
            end = list.size();
        slots.push_back(list.substr(start, end - start));
        start = end + 1;
    }
    return slots;
}

bool ModBlockKinds::OwnItem(const std::wstring& kind)
{
    const Kind* found = ModKindFind(kKinds, kind);
    return found != NULL && found->ownItem;
}
