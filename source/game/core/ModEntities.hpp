#ifndef GAME_CORE_MODENTITIES_HPP
#define GAME_CORE_MODENTITIES_HPP

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

class EntityRenderDispatcher;
class ModSurface;
class Textures;

class ModEntities
{
public:
    struct SpawnRule
    {
        std::wstring category;
        int weight;
        int minimum;
        int maximum;
    };

    struct Spec
    {
        std::wstring key;
        std::wstring base;
        double health;
        double speed;
        unsigned long long type;
        unsigned long long baseType;
        int id;
        // the picture the mob wears, laid out as the base mob's own texture is; none while it wears the base's
        boost::shared_ptr<ModSurface> texture;
        // the model of boxes the mob is drawn with, empty while it is drawn as its base mob is
        std::wstring model;
        // what players see for the mob, a translation key like a block's title; empty for none, and then the mob has no spawn egg
        std::wstring title;
        // where the mob spawns by itself: in every biome, as the game's own mobs of a category do
        std::vector<SpawnRule> spawns;
    };

    static void Open();
    static void Close();
    // Add a kind of mob, only from Item_staticCtor_after: base is pig, cow, chicken, sheep, zombie, skeleton, spider or creeper, or a
    // mob of a mod already declared (mod:name, or just name for one of yours), which the new one is made from the same class as, and any
    // Entity_* hook answers a method of the game class, and the new mob is that mob (its model, sounds, drops and behaviour) under your
    // own name mod:name, so /summon mod:name makes one, it is saved by name, and entity_spawn reports it with that name so you can clear
    // its goals and give it new ones. health is its health in half hearts (1 to 1024) and speed multiplies the base mob's walking speed
    // (0.1 to 10). At most 16 a mod set. Its network number is 200 plus its place in the sorted list of mod mobs, so a client must have
    // the same mod mobs as the server.
    static bool Declare(const std::string& owner, const std::wstring& name, const std::wstring& base, double health,
                 double speed);
    // Give a mob you declared with registry.entity a picture of its own, only from Item_staticCtor_after, laid out as the base mob's own
    // texture is (a pig's picture for a pig-based mob, so nothing needs working out), at any size the base's layout scales to. A mob
    // derived from it wears the same picture unless it gets its own. False when the mob is not yours or was not declared yet.
    static bool DeclareTexture(const std::string& owner, const std::wstring& name, const boost::shared_ptr<ModSurface>& image);
    // the texture a mod mob wears, or -1 for the one its renderer picks; call it while drawing
    static int TextureOf(unsigned long long type, Textures* textures);
    // Give a mob you declared with registry.entity a model of boxes of its own, only from Item_staticCtor_after, in place of its base mob's, and give it a
    // picture with registry.entity_texture. The text is a `texture <width> <height>` line then a `part` line for each box:
    // `part <name> <x> <y> <z> <u> <v> <dx> <dy> <dz> <width> <height> <depth> [head|legA|legB]`. The part pivots at x y z (sixteenths of a block, y down, 24 is the
    // ground), its box starts dx dy dz from the pivot and is width by height by depth, its picture starts at u v of the texture, and the last word moves it as the
    // game's mobs move: the head turns with the mob's, legA and legB swing against each other. A mob derived from it has the same model. False when the mob is
    // not yours or the text is not a model.
    static bool DeclareModel(const std::string& owner, const std::wstring& name, const std::wstring& model);
    // Name a mob you declared with registry.entity, only from Item_staticCtor_after, and give it a spawn egg: the egg is the game's own, in the creative menu and
    // for /give, in the colours of the mob it is made from, and its name says "Spawn" and the title (a key translated by registry.translate, or the text itself).
    static bool DeclareName(const std::string& owner, const std::wstring& name, const std::wstring& title);
    // Let a mob you declared with registry.entity spawn by itself, only from Item_staticCtor_after, in every biome: category is monster (at night and in the dark),
    // creature (on grass in daylight), water_creature or ambient; weight is its share of that category's chances (the game's pig has 10), and a group is minimum to
    // maximum mobs (1 to 8). Chunks the world already made keep what they have.
    static bool DeclareSpawn(const std::string& owner, const std::wstring& name, const std::wstring& category, int weight, int minimum, int maximum);
    static void Register();
    static void BindRenderers(EntityRenderDispatcher* dispatcher);
    static const Spec* Find(unsigned long long type);
    // the mod mobs declared, by name, for the registry sync
    static std::vector<std::wstring> Keys();
    static bool Has(const std::wstring& key);
};

#endif
