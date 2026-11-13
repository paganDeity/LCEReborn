#ifndef GAME_CORE_MODBLOCKS_HPP
#define GAME_CORE_MODBLOCKS_HPP

#include <stddef.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

class CompoundTag;
class ModSurface;
class Block;

class ModBlocks
{
public:
    struct PaletteEntry
    {
        int id;
        std::wstring name;
    };

    static bool Any();
    static void BuildPalette(const unsigned short* elements, const unsigned short* extraElements, size_t count,
                      std::vector<PaletteEntry>* palette);
    // the block number each saved number loads as: through the chunk's palette for mod blocks, and air for a number
    // no block is registered under
    static void BuildRemap(const std::vector<PaletteEntry>& palette, std::vector<unsigned short>* table);
    // which block numbers a chunk may hold, read once: blocks are only registered while the game starts
    static const std::vector<bool>& RegisteredIds();
    static void Open();
    static void Close();
    // Add a plain full block, only from Item_staticCtor_after. Its id is mod:name, the text /setblock and /give take. title is its name,
    // texture is the name of a game block whose picture it wears (stone, planks, ...), material is stone, wood, dirt, grass, sand,
    // glass, metal, wool, ice, clay or snow (it sets how the block sounds and what it counts as), hardness is how long it takes to break
    // (-1 never, 1.5 is stone), resistance to explosions, light 0 to 15 what it gives off. The block drops itself, and is saved in
    // chunks by name.
    static bool Declare(const std::string& owner, const std::wstring& name, const std::wstring& title,
                 const std::wstring& texture, const std::wstring& material, float hardness, float resistance, int light);
    // Declare a block made from one of the game's own block classes, so it works the way that class does and a mod can change any of its
    // methods with the Block_* hooks. kind is door, trapdoor, button, pressure_plate, lever, ladder or fence, or the name of a block
    // already registered (mod:name, or just name for one of yours): the new block is made the same way and starts with that block's
    // properties, pictures, shapes, drops and settings, which later calls change for the new block only. material, hardness, resistance
    // and light are as in registry.block. The block looks like the game's own until a hook draws it differently.
    static bool DeclareKind(const std::string& owner, const std::wstring& name, const std::wstring& kind,
                     const std::wstring& title, const std::wstring& material, float hardness, float resistance, int light);
    // Give a block you registered a property with count values (0 to count-1), only from Item_staticCtor_after, before its textures. A
    // block's properties multiply to at most 16 combinations. A property called facing (4 values: north, east, south, west; or 6 with up
    // and down) is set from the way the player looks when the block is placed, so the block faces the player. Read and change it with
    // level:get_state and level:set_state.
    static bool DeclareProperty(const std::string& owner, const std::wstring& name, const std::wstring& property, int count);
    // Choose what a block drops when it is broken, only from Item_staticCtor_after: min to max (0 to 64) of an item or block by name, a
    // random number in that range. An empty name or max of 0 drops nothing. Without this a block drops itself.
    static bool DeclareDrop(const std::string& owner, const std::wstring& name, const std::wstring& item, int minimum,
                     int maximum);
    // Give a block the game's slow random ticks, only from Item_staticCtor_after, so Block_tick is called for it now and then at random
    // positions like grass growing. Everything else a block reports (a neighbour changed, placed, removed, an entity stepped on it or is
    // inside it, a tick you asked for with level:schedule_tick) is a hook on the game class and needs no declaring:
    // Block_neighborChanged, Block_onPlace, Block_onRemove, Block_stepOn, Block_entityInside, Block_tick, each given the block's name as
    // self.
    static bool DeclareRandomTicks(const std::string& owner, const std::wstring& name);
    // Make a block a redstone power source whose strength (0 to 15) is the value of one of its properties, so set_state changes the
    // signal. Needs a property of up to 16 values.
    static bool DeclarePower(const std::string& owner, const std::wstring& name, const std::wstring& property);
    // Make a block take the click when a player right-clicks it with anything in hand, like a chest does, only from
    // Item_staticCtor_after: block_use is called (and cancelling it is not needed), and nothing is placed against the block unless the
    // player sneaks. Without this a block only sees block_use when an item is used on it, which is when the player holds something to
    // place or use, and an empty hand does nothing.
    static bool DeclareInteractive(const std::string& owner, const std::wstring& name);
    // Make the shapes and the north, south, east, west and sides pictures of a block turn with its facing property (4 or more values;
    // value 0 is the way you drew them, north), so you draw one shape and one set of pictures instead of one per facing. False when the
    // block has no facing property.
    static bool DeclareRotates(const std::string& owner, const std::wstring& name);
    // Give a block you registered a small store of its own for every block placed, only from Item_staticCtor_after: up to 64 keys of up
    // to 32 characters, each holding text up to 1024 bytes, kept with the chunk and sent to the players who can see the block, so a
    // client mod can read it with level:data_get too (every data_set sends an update; the game batches them per tick). slots is 0 for
    // none, or 27 or 54 (the two sizes of the game's chest window) for an inventory the block holds, shown in the game's chest window
    // with entity:open_container and read and written with level:get_item and level:set_item. Read and write it with level:data_get and
    // level:data_set. Every placed block of this kind is ticked once a game tick on the server: hook ScriptedBlockEntity_tick and read
    // its place with self:get_mPos_x(), get_mPos_y(), get_mPos_z() and its level with self:get_mLevel().
    static bool DeclareEntity(const std::string& owner, const std::wstring& name, int slots);
    // Give a block a shape that is not a full cube, only from Item_staticCtor_after, after its properties. boxes is up to 16 boxes
    // joined by semicolons, each x0,y0,z0,x1,y1,z1 in sixteenths of a block (0 to 16, the low corner below the high), for instance
    // "0,0,0,16,8,16" for a slab or "6,0,6,10,16,10;0,12,6,16,14,10" for a post with an arm. The boxes are what you collide with, what
    // is drawn (each wears the block's side pictures) and what is outlined (their bounding box). when is empty or property=value for a
    // shape used only in that state; a state beats none. A block with a shape does not hide the faces of its neighbours.
    static bool DeclareShape(const std::string& owner, const std::wstring& name, const std::wstring& boxes,
                      const std::wstring& when);
    // Choose the picture of one side of a block, only from Item_staticCtor_after: texture is the name of a game block whose picture to
    // use. side is down, up, north, south, west, east, sides (the four around) or all; when is empty, or property=value to use it only
    // in that state. The most specific match wins: a state beats none, a named side beats sides, and sides beat all. Properties must be
    // added first.
    static bool DeclareFace(const std::string& owner, const std::wstring& name, const std::wstring& side,
                     const std::wstring& texture, const std::wstring& when);
    // the icon of the item a block of its own class places (a door), which a plain block item does not have
    static bool DeclareItemIcon(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image);
    // The same as block_face with a picture of your own: the pixels of the surface (any size, scaled to the block texture size) are
    // added as new rows at the end of the game's terrain picture sheet. For a block made with block_of, side is not a side but a slot:
    // the picture the game's class reads, which the class defines once for every block of it (door: upper and lower, both needed; the
    // other kinds have one picture, slot all). The block then draws as the class draws, with your pictures, and a block derived from it
    // wears the same ones unless it supplies its own.
    static bool DeclareTexture(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image,
                               const std::wstring& side, const std::wstring& when);
    // Draw a block you gave a store (registry.block_entity) with a model of boxes of its own, only from Item_staticCtor_after, wearing a surface of yours:
    // `texture <width> <height>` then a `part <name> <x> <y> <z> <u> <v> <dx> <dy> <dz> <width> <height> <depth> [head|legA|legB]` line for each box, as
    // registry.entity_model takes it. The model is drawn at the block's centre, its pivots in sixteenths of a block with 24 as the ground, in addition to the
    // block's own shape. False when the block is not yours, has no store or the text is not a model. A block derived from it has the same model.
    static bool DeclareModel(const std::string& owner, const std::wstring& name, const std::wstring& model, const boost::shared_ptr<ModSurface>& picture);
    // the model of a block by its key, as the renderer asks
    static bool ModelOf(const std::wstring& key, std::wstring* model, boost::shared_ptr<ModSurface>* picture);
    static Block* Find(const std::wstring& key);
    static void Missing(CompoundTag* tag, std::vector<std::wstring>* names);
    static void Create();
    static void Reset();
    static void Load(CompoundTag* tag);
    static void Merge(CompoundTag* tag);
    static void Save(CompoundTag* tag);
    static void SaveDeclared(CompoundTag* tag);
};

#endif
