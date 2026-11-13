#ifndef GAME_CORE_MODBLOCKKINDS_HPP
#define GAME_CORE_MODBLOCKKINDS_HPP

#include <string>
#include <vector>

class Block;
class Item;
class Material;

// the game's own block classes a mod can make more of, by name
namespace ModBlockKinds
{
struct Made
{
    Block* block;
    // the item that places the block, or NULL when the block uses a plain block item
    Item* item;
};

bool Known(const std::wstring& kind);
// the pictures the class reads (a door: upper and lower); empty for a class with one
std::vector<std::wstring> Slots(const std::wstring& kind);
// whether the kind places the block with an item of its own class (a door) instead of a plain block item
bool OwnItem(const std::wstring& kind);
bool Make(const std::wstring& kind, Material* material, Made* made);
}

#endif
