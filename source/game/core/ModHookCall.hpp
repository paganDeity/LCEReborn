#ifndef GAME_CORE_MODHOOKCALL_HPP
#define GAME_CORE_MODHOOKCALL_HPP

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "game/core/ModHandles.hpp"
#include "game/core/ModHookIds_generated.hpp"
#include "game/world/LevelSource.hpp"
#include "mod/ModValue.hpp"

class Block;
class DamageSource;
class Entity;
class Item;
class Level;
class SoundEvent;
class LevelChunk;
class ChunkPrimer;
class Gui;
class Font;

class ModHookCall
{
public:
    explicit ModHookCall(int hook);
    ~ModHookCall();

    void Add(const boost::shared_ptr<Entity>& entity);
    // a game object by pointer, as the handle of its class
    void AddObject(unsigned int kind, uint32_t id);
    // a mutable handle starts empty; the mod fills it in
    void AddNil();
    bool GetHandle(size_t index, uint32_t* id) const;
    void Add(int value);
    void Add(bool value);
    void Add(float value);
    void Add(const std::wstring& value);
    // raw bytes, not text: nothing is converted
    void Add(const std::string& value);
    void Add(const ModHandles::GuiInfo& gui);
    void Add(const ModHandles::WorldInfo& world);
    void AddDamage(const DamageSource* source);
    void AddLightLayer(LightLayer::variety layer);
    void AddBlock(Block* block);
    void AddItem(Item* item);
    void AddSound(const SoundEvent* sound);
    void AddEntityType(const boost::shared_ptr<Entity>& entity);
    bool Run();
    float GetFloat(size_t index, float fallback) const;
    int GetInt(size_t index, int fallback) const;
    bool GetBool(size_t index, bool fallback) const;
    std::string GetBytes(size_t index, const std::string& fallback) const;
    std::wstring GetString(size_t index, const std::wstring& fallback) const;

private:
    int m_hook;
    bool m_active;
    ModHandles::Scope* m_scope;
    std::vector<mod::Value> m_values;
};

#endif
