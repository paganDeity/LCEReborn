#include "game/core/ModHookCall.hpp"

#include "game/core/ModHooks.hpp"
#include "game/core/ModHost.hpp"
#include "game/core/DamageSource.hpp"
#include "game/core/ModText.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/client/audio/SoundEvents.hpp"
#include "game/entity/mob/EntityIO.hpp"
#include "game/item/Item.hpp"
#include "game/world/block/Block.hpp"
#include "mod/ModApiIds_generated.hpp"

ModHookCall::ModHookCall(int hook) : m_hook(hook), m_active(ModHooks::IsActive(hook)), m_scope(NULL)
{
    if (m_active)
        m_scope = new ModHandles::Scope();
}

ModHookCall::~ModHookCall()
{
    delete m_scope;
}

void ModHookCall::Add(const boost::shared_ptr<Entity>& entity)
{
    if (m_active)
        m_values.push_back(mod::Value::FromHandle(mod::api::eHandle_entity, ModHandles::Add(entity)));
}

void ModHookCall::AddNil()
{
    if (m_active)
        m_values.push_back(mod::Value());
}

bool ModHookCall::GetHandle(size_t index, uint32_t* id) const
{
    if (index >= m_values.size() || m_values[index].Type() != mod::Value::eType_Handle)
        return false;
    *id = m_values[index].Id();
    return true;
}

void ModHookCall::AddObject(unsigned int kind, uint32_t id)
{
    if (m_active)
        m_values.push_back(mod::Value::FromHandle(kind, id));
}

void ModHookCall::Add(int value)
{
    if (m_active)
        m_values.push_back(mod::Value::FromInt(value));
}

void ModHookCall::Add(bool value)
{
    if (m_active)
        m_values.push_back(mod::Value::FromBool(value));
}

void ModHookCall::Add(float value)
{
    if (m_active)
        m_values.push_back(mod::Value::FromFloat(value));
}

void ModHookCall::Add(const std::wstring& value)
{
    if (m_active)
        m_values.push_back(mod::Value::FromString(ModText::ToUtf8(value)));
}

void ModHookCall::Add(const std::string& value)
{
    if (m_active)
        m_values.push_back(mod::Value::FromString(value));
}

std::string ModHookCall::GetBytes(size_t index, const std::string& fallback) const
{
    return index < m_values.size() ? m_values[index].String() : fallback;
}

void ModHookCall::Add(const ModHandles::GuiInfo& gui)
{
    if (m_active)
        m_values.push_back(mod::Value::FromHandle(mod::api::eHandle_gui, ModHandles::AddGui(gui.gui, gui.font, gui.width, gui.height)));
}

void ModHookCall::Add(const ModHandles::WorldInfo& world)
{
    if (m_active)
        m_values.push_back(mod::Value::FromHandle(mod::api::eHandle_canvas, ModHandles::AddWorld(world)));
}

void ModHookCall::AddLightLayer(LightLayer::variety layer)
{
    if (m_active)
        m_values.push_back(mod::Value::FromInt(static_cast<int>(layer)));
}

void ModHookCall::AddDamage(const DamageSource* source)
{
    if (m_active)
        m_values.push_back(mod::Value::FromInt(source != NULL ? static_cast<int>(source->getMsgId()) : 0));
}

int ModHookCall::GetInt(size_t index, int fallback) const
{
    return index < m_values.size() ? static_cast<int>(m_values[index].Int()) : fallback;
}

bool ModHookCall::GetBool(size_t index, bool fallback) const
{
    return index < m_values.size() ? m_values[index].Bool() : fallback;
}

void ModHookCall::AddBlock(Block* block)
{
    if (m_active)
        m_values.push_back(mod::Value::FromString(ModText::ToUtf8(Block::blockRegistry->getKey(block).toString())));
}

bool ModHookCall::Run()
{
    if (!m_active)
        return true;
    return ModHost::Get().RunHook(m_hook, m_values);
}

float ModHookCall::GetFloat(size_t index, float fallback) const
{
    return index < m_values.size() ? static_cast<float>(m_values[index].Float()) : fallback;
}

void ModHookCall::AddItem(Item* item)
{
    if (m_active)
        m_values.push_back(mod::Value::FromString(ModText::ToUtf8(Item::itemRegistry.getKey(item).toString())));
}

void ModHookCall::AddSound(const SoundEvent* sound)
{
    if (m_active)
        m_values.push_back(mod::Value::FromString(ModText::ToUtf8(SoundEvents::getName(sound))));
}

void ModHookCall::AddEntityType(const boost::shared_ptr<Entity>& entity)
{
    if (m_active)
        m_values.push_back(mod::Value::FromString(ModText::ToUtf8(EntityIO::getKey(entity).toString())));
}

std::wstring ModHookCall::GetString(size_t index, const std::wstring& fallback) const
{
    return index < m_values.size() ? ModText::FromUtf8(m_values[index].String()) : fallback;
}
