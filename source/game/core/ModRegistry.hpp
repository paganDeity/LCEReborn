#ifndef GAME_CORE_MODREGISTRY_HPP
#define GAME_CORE_MODREGISTRY_HPP

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

class CompoundTag;
class ModSurface;

class ModRegistry
{
public:
    // content may be registered between Open and Close, which wrap the game's own item registration: a mod registers by
    // hooking Item_staticCtor_after
    static void Open();
    static void Close();
    // Add an item to the game, only from Item_staticCtor_after. Its id is your mod's id and name joined by a colon (mod:name; name is 1
    // to 32 of a-z 0-9 _), the same text /give takes. title is what players see, icon is the name of a game item whose picture it uses
    // (stick, diamond, ...), stack is the most per stack, 1 to 64. The number the game gives it is chosen for each world and saved with
    // that world, so it stays the same while mods come and go. False if the call came too late, the name is taken or a limit was
    // reached.
    static bool DeclareItem(const std::string& owner, const std::wstring& name, const std::wstring& title,
                     const std::wstring& icon, int stackSize);
    // Declare a dimension, only from Item_staticCtor_after. id is its number (2 to 100, the game's own are -1, 0 and 1) and is what
    // level:getDimension, entity:changeDimension and the packets carry; the dimension is named mod_name. It behaves like the overworld
    // (terrain from generate_terrain, which a mod hooks and tells apart by the level it is given) and every Dimension_* hook can change
    // that; its chunks are saved in the world under DIM<id>. At most 8 dimensions in all.
    static bool DeclareDimension(const std::string& owner, const std::wstring& name, int id);
    // Declare an item made from one of the game's own item classes, so it works the way that class does and a mod can change any of its
    // methods with the Item_* hooks. kind food (a nutrition in half drumsticks, b saturation, c non-zero if it is meat), or sword,
    // pickaxe, axe, shovel, hoe (a mining level, b uses, c speed, d damage bonus: the item's own tier). icon is as in registry.item.
    // kind can also be the name of an item already registered (mod:name, or just name for one of yours): the new item is made as that
    // one is, with the same stack size.
    static bool DeclareItemOf(const std::string& owner, const std::wstring& name, const std::wstring& kind,
                       const std::wstring& title, const std::wstring& icon, float a, float b, float c, float d);
    // Give an item you registered a picture of its own, only from Item_staticCtor_after: the pixels of the surface (any size, scaled to
    // the icon size) become the item's icon. Make the surface with surface.decode of a PNG. It also gives a picture to the item a block
    // made with block_of places when that block's class has its own item (a door). The game adds rows to the end of its item picture
    // sheet for these. False when the item is not yours or was not registered yet.
    static bool DeclareItemIcon(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image);
    static void Reset();
    static void Load(CompoundTag* tag);
    static void Save(CompoundTag* tag);
    static std::vector<unsigned char> WriteSync();
    static bool ApplySync(const std::vector<unsigned char>& data, std::vector<std::wstring>* missing);
};

#endif
