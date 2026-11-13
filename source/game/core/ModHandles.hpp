#ifndef GAME_CORE_MODHANDLES_HPP
#define GAME_CORE_MODHANDLES_HPP

#include <stdint.h>

#include <string>

#include <boost/shared_ptr.hpp>

class Entity;
class Font;
class Gui;
class Level;
class MenuPainter;
class ModSurface;
class ModAudioStream;
class LevelChunk;
class ChunkPrimer;

namespace ModHandles
{
class Caller
{
public:
    explicit Caller(const std::string& mod);
    ~Caller();
    static const std::string& Mod();

private:
    const std::string* mPrevious;
};

class Scope
{
public:
    Scope();
    ~Scope();
};

// any game object by pointer: the handle is typed by the class, so a mod cannot pass a Block where a Level is wanted
uint32_t AddObject(void* pointer, const void* key);
// an object of the derived class may be used as the base class: how to turn its pointer into the base's
typedef void* (*Upcaster)(void* pointer);
void AddBase(const void* derived, const void* base, Upcaster upcast);
bool GetObject(uint32_t id, const void* key, void** pointer);
template <class T> const void* Key()
{
    static const char key = 0;
    return &key;
}
// an object the game shares ownership of (an item stack, a block entity): the handle keeps it alive for the call, and a call that takes a shared
// pointer gets the very owner back, so what a mod makes is never freed twice
// an owned object may be asked for as another class it is (a block entity that is also a container): how to turn its owner into the other's
typedef boost::shared_ptr<void> (*SharedCaster)(const boost::shared_ptr<void>& owner);
void AddSharedCast(const void* from, const void* to, SharedCaster cast);
// an object a mod keeps across calls (a menu navigator): its handle lasts until the mod frees it or is unloaded, and only that mod may use it; 0 when it has too many
uint32_t AddPersistentObject(const boost::shared_ptr<void>& owner, void* pointer, const void* key);
void ReleasePersistent(uint32_t id);
// free the persistent handle of an object the calling mod made, by the object (a surface frees itself this way)
void ReleasePersistentObject(const void* pointer);
template <class T> uint32_t AddPersistent(const boost::shared_ptr<T>& owner)
{
    return AddPersistentObject(owner, const_cast<void*>(static_cast<const void*>(owner.get())), Key<T>());
}
uint32_t AddSharedObject(const boost::shared_ptr<void>& owner, void* pointer, const void* key);
bool GetSharedObject(uint32_t id, const void* key, boost::shared_ptr<void>* owner);
template <class T> uint32_t AddShared(const boost::shared_ptr<T>& owner)
{
    return AddSharedObject(owner, const_cast<void*>(static_cast<const void*>(owner.get())), Key<T>());
}
template <class T> bool GetShared(uint32_t id, boost::shared_ptr<T>* owner)
{
    boost::shared_ptr<void> found;
    if (!GetSharedObject(id, Key<T>(), &found))
        return false;
    *owner = boost::static_pointer_cast<T>(found);
    return true;
}
template <class T> uint32_t Add(T* pointer)
{
    return AddObject(const_cast<void*>(static_cast<const void*>(pointer)), Key<T>());
}
template <class T> bool Get(uint32_t id, T** pointer)
{
    void* found = NULL;
    if (!GetObject(id, Key<T>(), &found))
        return false;
    *pointer = static_cast<T*>(found);
    return true;
}

uint32_t Add(const boost::shared_ptr<Entity>& entity);
uint32_t AddGui(Gui* gui, Font* font, int width, int height);
// the world while it is being drawn: where the camera is, and what a mod may draw in it. Positions are world coordinates; the game places them
// relative to the camera. A struct of plain members, so the game makes one with braces.
struct WorldInfo
{
    double cameraX;
    double cameraY;
    double cameraZ;

    // The camera's x in world coordinates.
    float CameraX() const;
    // The camera's y in world coordinates.
    float CameraY() const;
    // The camera's z in world coordinates.
    float CameraZ() const;
    // Draw a picture as a parallelogram from a corner (x, y, z) along two edges, (across_x, across_y, across_z) and (down_x, down_y, down_z), lit 0 to 15
    // (dark to full); false when the picture is not yours.
    bool Image(const boost::shared_ptr<ModSurface>& picture, double x, double y, double z, double acrossX, double acrossY, double acrossZ,
               double downX, double downY, double downZ, int light) const;
    // Draw triangles wearing a picture. vertices is 9 numbers per corner, three corners a triangle, as little-endian floats (string.pack("<f"), x y z, u v, red
    // green blue alpha); up to 30000 corners. False when the data is the wrong size or the picture is not yours.
    bool Triangles(const boost::shared_ptr<ModSurface>& picture, const std::string& vertices) const;
    // Draw flat-coloured triangles from the same vertex data as Triangles, whose u and v are ignored.
    bool ColoredTriangles(const std::string& vertices) const;
};
uint32_t AddWorld(const WorldInfo& world);
// the interface while it is being drawn, in interface pixels from the top left
struct GuiInfo
{
    Gui* gui;
    Font* font;
    int width;
    int height;

    // The width of the whole screen in interface pixels.
    int GetWidth() const;
    // The height of the whole screen in interface pixels.
    int GetHeight() const;
    // The game's menu painter for this gui, the same one the shader and debug menus draw with. Valid until the next call.
    MenuPainter* Painter() const;
    // Draw a surface you own, or part of it, at x, y, scaled to width by height, taking u, v to u2, v2 of the picture (0 to 1; 0, 0, 1, 1 is all of it).
    // False when the picture is not yours.
    bool Image(const boost::shared_ptr<ModSurface>& picture, int x, int y, int width, int height, float u, float v, float u2, float v2) const;
};

bool Get(uint32_t id, boost::shared_ptr<Entity>* entity);
bool Get(uint32_t id, GuiInfo* info);
bool Get(uint32_t id, WorldInfo* world);
}

#endif
