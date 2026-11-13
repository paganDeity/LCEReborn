// Generated from game/item/Item.hpp by tools/hooks/hookgen.py. Do not edit.
#ifndef GAME_MODCLASS_ITEM_GENERATED_HPP
#define GAME_MODCLASS_ITEM_GENERATED_HPP

#include <stddef.h>

#include "game/core/ModEvents_generated.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModItemNames.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/generation/Player.hpp"
#include "game/entity/mob/LivingEntity.hpp"
#include "game/item/Item.hpp"

// every virtual method of Item a mod can answer, on top of any game class derived from it
template <class Base>
class ModItem : public Base
{
public:
    using Base::Base;

    template <class B>
    auto base_Item_verifyTagAfterLoad(CompoundTag* tag, int) -> decltype(static_cast<B*>(this)->B::verifyTagAfterLoad(tag))
    {
        return static_cast<B*>(this)->B::verifyTagAfterLoad(tag);
    }
    template <class B>
    bool base_Item_verifyTagAfterLoad(CompoundTag* tag, long)
    {
        return static_cast<Item*>(this)->Item::verifyTagAfterLoad(tag);
    }

    virtual bool verifyTagAfterLoad(CompoundTag* tag)
    {
        if (ModHooks::IsActive(kModHook_Item_verifyTagAfterLoad))
        {
            bool result = false;
            if (!ModEvents::ItemVerifyTagAfterLoad(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<CompoundTag*>(tag), result))
                return result;
        }
        return base_Item_verifyTagAfterLoad<Base>(tag, 0);
    }

    template <class B>
    auto base_Item_useOn(boost::shared_ptr<Player> player, Level* level, const BlockPos& pos, InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY, float hitZ, bool simulate, int) -> decltype(static_cast<B*>(this)->B::useOn(player, level, pos, hand, direction, hitX, hitY, hitZ, simulate))
    {
        return static_cast<B*>(this)->B::useOn(player, level, pos, hand, direction, hitX, hitY, hitZ, simulate);
    }
    template <class B>
    InteractionResult::EInteractionResult base_Item_useOn(boost::shared_ptr<Player> player, Level* level, const BlockPos& pos, InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY, float hitZ, bool simulate, long)
    {
        return static_cast<Item*>(this)->Item::useOn(player, level, pos, hand, direction, hitX, hitY, hitZ, simulate);
    }

    virtual InteractionResult::EInteractionResult useOn(boost::shared_ptr<Player> player, Level* level, const BlockPos& pos, InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY, float hitZ, bool simulate)
    {
        if (ModHooks::IsActive(kModHook_Item_useOn))
        {
            int result = 0;
            if (!ModEvents::ItemUseOn(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), player, level, pos.x, pos.y, pos.z, static_cast<int>(hand), (direction != NULL ? direction->get3DDataValue() : -1), hitX, hitY, hitZ, simulate, result))
                return static_cast<InteractionResult::EInteractionResult>(result);
        }
        return base_Item_useOn<Base>(player, level, pos, hand, direction, hitX, hitY, hitZ, simulate, 0);
    }

    template <class B>
    auto base_Item_getDestroySpeed(not_null_ptr<ItemInstance> itemInstance, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getDestroySpeed(itemInstance, state))
    {
        return static_cast<B*>(this)->B::getDestroySpeed(itemInstance, state);
    }
    template <class B>
    float base_Item_getDestroySpeed(not_null_ptr<ItemInstance> itemInstance, const BlockState* state, long)
    {
        return static_cast<Item*>(this)->Item::getDestroySpeed(itemInstance, state);
    }

    virtual float getDestroySpeed(not_null_ptr<ItemInstance> itemInstance, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Item_getDestroySpeed))
        {
            float result = 0.0f;
            if (!ModEvents::ItemGetDestroySpeed(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Item_getDestroySpeed<Base>(itemInstance, state, 0);
    }

    template <class B>
    auto base_Item_TestUse(Level* level, boost::shared_ptr<Player> player, InteractionHand::EInteractionHand hand, int) -> decltype(static_cast<B*>(this)->B::TestUse(level, player, hand))
    {
        return static_cast<B*>(this)->B::TestUse(level, player, hand);
    }
    template <class B>
    bool base_Item_TestUse(Level* level, boost::shared_ptr<Player> player, InteractionHand::EInteractionHand hand, long)
    {
        return static_cast<Item*>(this)->Item::TestUse(level, player, hand);
    }

    virtual bool TestUse(Level* level, boost::shared_ptr<Player> player, InteractionHand::EInteractionHand hand)
    {
        if (ModHooks::IsActive(kModHook_Item_TestUse))
        {
            bool result = false;
            if (!ModEvents::ItemTestUse(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), level, player, static_cast<int>(hand), result))
                return result;
        }
        return base_Item_TestUse<Base>(level, player, hand, 0);
    }

    template <class B>
    auto base_Item_getMaxStackSize(int) -> decltype(static_cast<B*>(this)->B::getMaxStackSize())
    {
        return static_cast<B*>(this)->B::getMaxStackSize();
    }
    template <class B>
    int base_Item_getMaxStackSize(long)
    {
        return static_cast<Item*>(this)->Item::getMaxStackSize();
    }

    virtual int getMaxStackSize()
    {
        if (ModHooks::IsActive(kModHook_Item_getMaxStackSize))
        {
            int result = 0;
            if (!ModEvents::ItemGetMaxStackSize(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_getMaxStackSize<Base>(0);
    }

    template <class B>
    auto base_Item_getLevelDataForAuxValue(int auxValue, int) -> decltype(static_cast<B*>(this)->B::getLevelDataForAuxValue(auxValue))
    {
        return static_cast<B*>(this)->B::getLevelDataForAuxValue(auxValue);
    }
    template <class B>
    int base_Item_getLevelDataForAuxValue(int auxValue, long)
    {
        return static_cast<Item*>(this)->Item::getLevelDataForAuxValue(auxValue);
    }

    virtual int getLevelDataForAuxValue(int auxValue)
    {
        if (ModHooks::IsActive(kModHook_Item_getLevelDataForAuxValue))
        {
            int result = 0;
            if (!ModEvents::ItemGetLevelDataForAuxValue(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), auxValue, result))
                return result;
        }
        return base_Item_getLevelDataForAuxValue<Base>(auxValue, 0);
    }

    template <class B>
    auto base_Item_hurtEnemy(not_null_ptr<ItemInstance> itemInstance, boost::shared_ptr<LivingEntity> target, boost::shared_ptr<LivingEntity> attacker, int) -> decltype(static_cast<B*>(this)->B::hurtEnemy(itemInstance, target, attacker))
    {
        return static_cast<B*>(this)->B::hurtEnemy(itemInstance, target, attacker);
    }
    template <class B>
    bool base_Item_hurtEnemy(not_null_ptr<ItemInstance> itemInstance, boost::shared_ptr<LivingEntity> target, boost::shared_ptr<LivingEntity> attacker, long)
    {
        return static_cast<Item*>(this)->Item::hurtEnemy(itemInstance, target, attacker);
    }

    virtual bool hurtEnemy(not_null_ptr<ItemInstance> itemInstance, boost::shared_ptr<LivingEntity> target, boost::shared_ptr<LivingEntity> attacker)
    {
        if (ModHooks::IsActive(kModHook_Item_hurtEnemy))
        {
            bool result = false;
            if (!ModEvents::ItemHurtEnemy(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), target, attacker, result))
                return result;
        }
        return base_Item_hurtEnemy<Base>(itemInstance, target, attacker, 0);
    }

    template <class B>
    auto base_Item_mineBlock(not_null_ptr<ItemInstance> itemInstance, Level* level, const BlockState* state, const BlockPos& pos, boost::shared_ptr<LivingEntity> entity, int) -> decltype(static_cast<B*>(this)->B::mineBlock(itemInstance, level, state, pos, entity))
    {
        return static_cast<B*>(this)->B::mineBlock(itemInstance, level, state, pos, entity);
    }
    template <class B>
    bool base_Item_mineBlock(not_null_ptr<ItemInstance> itemInstance, Level* level, const BlockState* state, const BlockPos& pos, boost::shared_ptr<LivingEntity> entity, long)
    {
        return static_cast<Item*>(this)->Item::mineBlock(itemInstance, level, state, pos, entity);
    }

    virtual bool mineBlock(not_null_ptr<ItemInstance> itemInstance, Level* level, const BlockState* state, const BlockPos& pos, boost::shared_ptr<LivingEntity> entity)
    {
        if (ModHooks::IsActive(kModHook_Item_mineBlock))
        {
            bool result = false;
            if (!ModEvents::ItemMineBlock(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), level, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), pos.x, pos.y, pos.z, entity, result))
                return result;
        }
        return base_Item_mineBlock<Base>(itemInstance, level, state, pos, entity, 0);
    }

    template <class B>
    auto base_Item_getAttackDamage(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::getAttackDamage(entity))
    {
        return static_cast<B*>(this)->B::getAttackDamage(entity);
    }
    template <class B>
    int base_Item_getAttackDamage(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Item*>(this)->Item::getAttackDamage(entity);
    }

    virtual int getAttackDamage(boost::shared_ptr<Entity> entity)
    {
        if (ModHooks::IsActive(kModHook_Item_getAttackDamage))
        {
            int result = 0;
            if (!ModEvents::ItemGetAttackDamage(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), entity, result))
                return result;
        }
        return base_Item_getAttackDamage<Base>(entity, 0);
    }

    template <class B>
    auto base_Item_canDestroySpecial(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::canDestroySpecial(state))
    {
        return static_cast<B*>(this)->B::canDestroySpecial(state);
    }
    template <class B>
    bool base_Item_canDestroySpecial(const BlockState* state, long)
    {
        return static_cast<Item*>(this)->Item::canDestroySpecial(state);
    }

    virtual bool canDestroySpecial(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Item_canDestroySpecial))
        {
            bool result = false;
            if (!ModEvents::ItemCanDestroySpecial(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Item_canDestroySpecial<Base>(state, 0);
    }

    template <class B>
    auto base_Item_interactEnemy(not_null_ptr<ItemInstance> itemInstance, boost::shared_ptr<Player> player, boost::shared_ptr<LivingEntity> target, InteractionHand::EInteractionHand hand, int) -> decltype(static_cast<B*>(this)->B::interactEnemy(itemInstance, player, target, hand))
    {
        return static_cast<B*>(this)->B::interactEnemy(itemInstance, player, target, hand);
    }
    template <class B>
    bool base_Item_interactEnemy(not_null_ptr<ItemInstance> itemInstance, boost::shared_ptr<Player> player, boost::shared_ptr<LivingEntity> target, InteractionHand::EInteractionHand hand, long)
    {
        return static_cast<Item*>(this)->Item::interactEnemy(itemInstance, player, target, hand);
    }

    virtual bool interactEnemy(not_null_ptr<ItemInstance> itemInstance, boost::shared_ptr<Player> player, boost::shared_ptr<LivingEntity> target, InteractionHand::EInteractionHand hand)
    {
        if (ModHooks::IsActive(kModHook_Item_interactEnemy))
        {
            bool result = false;
            if (!ModEvents::ItemInteractEnemy(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), player, target, static_cast<int>(hand), result))
                return result;
        }
        return base_Item_interactEnemy<Base>(itemInstance, player, target, hand, 0);
    }

    template <class B>
    auto base_Item_isHandEquipped(int) -> decltype(static_cast<B*>(this)->B::isHandEquipped())
    {
        return static_cast<B*>(this)->B::isHandEquipped();
    }
    template <class B>
    bool base_Item_isHandEquipped(long)
    {
        return static_cast<Item*>(this)->Item::isHandEquipped();
    }

    virtual bool isHandEquipped()
    {
        if (ModHooks::IsActive(kModHook_Item_isHandEquipped))
        {
            bool result = false;
            if (!ModEvents::ItemIsHandEquipped(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_isHandEquipped<Base>(0);
    }

    template <class B>
    auto base_Item_isMirroredArt(int) -> decltype(static_cast<B*>(this)->B::isMirroredArt())
    {
        return static_cast<B*>(this)->B::isMirroredArt();
    }
    template <class B>
    bool base_Item_isMirroredArt(long)
    {
        return static_cast<Item*>(this)->Item::isMirroredArt();
    }

    virtual bool isMirroredArt()
    {
        if (ModHooks::IsActive(kModHook_Item_isMirroredArt))
        {
            bool result = false;
            if (!ModEvents::ItemIsMirroredArt(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_isMirroredArt<Base>(0);
    }

    template <class B>
    auto base_Item_getDescriptionId(int auxValue, int) -> decltype(static_cast<B*>(this)->B::getDescriptionId(auxValue))
    {
        return static_cast<B*>(this)->B::getDescriptionId(auxValue);
    }
    template <class B>
    int base_Item_getDescriptionId(int auxValue, long)
    {
        return static_cast<Item*>(this)->Item::getDescriptionId(auxValue);
    }

    virtual int getDescriptionId(int auxValue)
    {
        if (ModHooks::IsActive(kModHook_Item_getDescriptionId))
        {
            int result = 0;
            if (!ModEvents::ItemGetDescriptionId(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), auxValue, result))
                return result;
        }
        return base_Item_getDescriptionId<Base>(auxValue, 0);
    }

    template <class B>
    auto base_Item_getDescriptionId_2(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::getDescriptionId(itemInstance))
    {
        return static_cast<B*>(this)->B::getDescriptionId(itemInstance);
    }
    template <class B>
    int base_Item_getDescriptionId_2(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::getDescriptionId(itemInstance);
    }

    virtual int getDescriptionId(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_getDescriptionId_2))
        {
            int result = 0;
            if (!ModEvents::ItemGetDescriptionId2(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getDescriptionId_2<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_getUseDescriptionId(int) -> decltype(static_cast<B*>(this)->B::getUseDescriptionId())
    {
        return static_cast<B*>(this)->B::getUseDescriptionId();
    }
    template <class B>
    int base_Item_getUseDescriptionId(long)
    {
        return static_cast<Item*>(this)->Item::getUseDescriptionId();
    }

    virtual int getUseDescriptionId()
    {
        if (ModHooks::IsActive(kModHook_Item_getUseDescriptionId))
        {
            int result = 0;
            if (!ModEvents::ItemGetUseDescriptionId(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_getUseDescriptionId<Base>(0);
    }

    template <class B>
    auto base_Item_getUseDescriptionId_2(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::getUseDescriptionId(itemInstance))
    {
        return static_cast<B*>(this)->B::getUseDescriptionId(itemInstance);
    }
    template <class B>
    int base_Item_getUseDescriptionId_2(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::getUseDescriptionId(itemInstance);
    }

    virtual int getUseDescriptionId(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_getUseDescriptionId_2))
        {
            int result = 0;
            if (!ModEvents::ItemGetUseDescriptionId2(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getUseDescriptionId_2<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_shouldOverrideMultiplayerNBT(int) -> decltype(static_cast<B*>(this)->B::shouldOverrideMultiplayerNBT())
    {
        return static_cast<B*>(this)->B::shouldOverrideMultiplayerNBT();
    }
    template <class B>
    bool base_Item_shouldOverrideMultiplayerNBT(long)
    {
        return static_cast<Item*>(this)->Item::shouldOverrideMultiplayerNBT();
    }

    virtual bool shouldOverrideMultiplayerNBT()
    {
        if (ModHooks::IsActive(kModHook_Item_shouldOverrideMultiplayerNBT))
        {
            bool result = false;
            if (!ModEvents::ItemShouldOverrideMultiplayerNBT(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_shouldOverrideMultiplayerNBT<Base>(0);
    }

    template <class B>
    auto base_Item_getColor(not_null_ptr<ItemInstance> itemInstance, int layer, int) -> decltype(static_cast<B*>(this)->B::getColor(itemInstance, layer))
    {
        return static_cast<B*>(this)->B::getColor(itemInstance, layer);
    }
    template <class B>
    int base_Item_getColor(not_null_ptr<ItemInstance> itemInstance, int layer, long)
    {
        return static_cast<Item*>(this)->Item::getColor(itemInstance, layer);
    }

    virtual int getColor(not_null_ptr<ItemInstance> itemInstance, int layer)
    {
        if (ModHooks::IsActive(kModHook_Item_getColor))
        {
            int result = 0;
            if (!ModEvents::ItemGetColor(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), layer, result))
                return result;
        }
        return base_Item_getColor<Base>(itemInstance, layer, 0);
    }

    template <class B>
    auto base_Item_inventoryTick(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<Entity> entity, int slot, bool selected, int) -> decltype(static_cast<B*>(this)->B::inventoryTick(itemInstance, level, entity, slot, selected))
    {
        static_cast<B*>(this)->B::inventoryTick(itemInstance, level, entity, slot, selected);
    }
    template <class B>
    void base_Item_inventoryTick(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<Entity> entity, int slot, bool selected, long)
    {
        static_cast<Item*>(this)->Item::inventoryTick(itemInstance, level, entity, slot, selected);
    }

    virtual void inventoryTick(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<Entity> entity, int slot, bool selected)
    {
        if (ModHooks::IsActive(kModHook_Item_inventoryTick))
        {
            if (!ModEvents::ItemInventoryTick(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), level, entity, slot, selected))
                return;
        }
        base_Item_inventoryTick<Base>(itemInstance, level, entity, slot, selected, 0);
    }

    template <class B>
    auto base_Item_onCraftedBy(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<Player> player, int) -> decltype(static_cast<B*>(this)->B::onCraftedBy(itemInstance, level, player))
    {
        static_cast<B*>(this)->B::onCraftedBy(itemInstance, level, player);
    }
    template <class B>
    void base_Item_onCraftedBy(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<Player> player, long)
    {
        static_cast<Item*>(this)->Item::onCraftedBy(itemInstance, level, player);
    }

    virtual void onCraftedBy(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<Player> player)
    {
        if (ModHooks::IsActive(kModHook_Item_onCraftedBy))
        {
            if (!ModEvents::ItemOnCraftedBy(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), level, player))
                return;
        }
        base_Item_onCraftedBy<Base>(itemInstance, level, player, 0);
    }

    template <class B>
    auto base_Item_isComplex(int) -> decltype(static_cast<B*>(this)->B::isComplex())
    {
        return static_cast<B*>(this)->B::isComplex();
    }
    template <class B>
    bool base_Item_isComplex(long)
    {
        return static_cast<Item*>(this)->Item::isComplex();
    }

    virtual bool isComplex()
    {
        if (ModHooks::IsActive(kModHook_Item_isComplex))
        {
            bool result = false;
            if (!ModEvents::ItemIsComplex(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_isComplex<Base>(0);
    }

    template <class B>
    auto base_Item_getUseAnimation(not_null_ptr<const ItemInstance> itemInstance, int) const -> decltype(static_cast<const B*>(this)->B::getUseAnimation(itemInstance))
    {
        return static_cast<const B*>(this)->B::getUseAnimation(itemInstance);
    }
    template <class B>
    int base_Item_getUseAnimation(not_null_ptr<const ItemInstance> itemInstance, long) const
    {
        return static_cast<const Item*>(this)->Item::getUseAnimation(itemInstance);
    }

    virtual int getUseAnimation(not_null_ptr<const ItemInstance> itemInstance) const
    {
        if (ModHooks::IsActive(kModHook_Item_getUseAnimation))
        {
            int result = 0;
            if (!ModEvents::ItemGetUseAnimation(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getUseAnimation<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_getUseDuration(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::getUseDuration(itemInstance))
    {
        return static_cast<B*>(this)->B::getUseDuration(itemInstance);
    }
    template <class B>
    int base_Item_getUseDuration(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::getUseDuration(itemInstance);
    }

    virtual int getUseDuration(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_getUseDuration))
        {
            int result = 0;
            if (!ModEvents::ItemGetUseDuration(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getUseDuration<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_releaseUsing(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<LivingEntity> entity, int timeLeft, int) -> decltype(static_cast<B*>(this)->B::releaseUsing(itemInstance, level, entity, timeLeft))
    {
        static_cast<B*>(this)->B::releaseUsing(itemInstance, level, entity, timeLeft);
    }
    template <class B>
    void base_Item_releaseUsing(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<LivingEntity> entity, int timeLeft, long)
    {
        static_cast<Item*>(this)->Item::releaseUsing(itemInstance, level, entity, timeLeft);
    }

    virtual void releaseUsing(not_null_ptr<ItemInstance> itemInstance, Level* level, boost::shared_ptr<LivingEntity> entity, int timeLeft)
    {
        if (ModHooks::IsActive(kModHook_Item_releaseUsing))
        {
            if (!ModEvents::ItemReleaseUsing(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), level, entity, timeLeft))
                return;
        }
        base_Item_releaseUsing<Base>(itemInstance, level, entity, timeLeft, 0);
    }

    template <class B>
    auto base_Item_getName(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::getName(itemInstance))
    {
        return static_cast<B*>(this)->B::getName(itemInstance);
    }
    template <class B>
    std::wstring base_Item_getName(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::getName(itemInstance);
    }

    virtual std::wstring getName(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_getName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::ItemGetName(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getName<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_isFoil(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::isFoil(itemInstance))
    {
        return static_cast<B*>(this)->B::isFoil(itemInstance);
    }
    template <class B>
    bool base_Item_isFoil(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::isFoil(itemInstance);
    }

    virtual bool isFoil(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_isFoil))
        {
            bool result = false;
            if (!ModEvents::ItemIsFoil(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_isFoil<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_getRarity(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::getRarity(itemInstance))
    {
        return static_cast<B*>(this)->B::getRarity(itemInstance);
    }
    template <class B>
    const Rarity* base_Item_getRarity(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::getRarity(itemInstance);
    }

    virtual const Rarity* getRarity(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_getRarity))
        {
            Rarity* result = NULL;
            if (!ModEvents::ItemGetRarity(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getRarity<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_isEnchantable(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::isEnchantable(itemInstance))
    {
        return static_cast<B*>(this)->B::isEnchantable(itemInstance);
    }
    template <class B>
    bool base_Item_isEnchantable(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::isEnchantable(itemInstance);
    }

    virtual bool isEnchantable(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_isEnchantable))
        {
            bool result = false;
            if (!ModEvents::ItemIsEnchantable(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_isEnchantable<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_getEnchantmentValue(int) -> decltype(static_cast<B*>(this)->B::getEnchantmentValue())
    {
        return static_cast<B*>(this)->B::getEnchantmentValue();
    }
    template <class B>
    int base_Item_getEnchantmentValue(long)
    {
        return static_cast<Item*>(this)->Item::getEnchantmentValue();
    }

    virtual int getEnchantmentValue()
    {
        if (ModHooks::IsActive(kModHook_Item_getEnchantmentValue))
        {
            int result = 0;
            if (!ModEvents::ItemGetEnchantmentValue(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_getEnchantmentValue<Base>(0);
    }

    template <class B>
    auto base_Item_mayBePlacedInAdventureMode(int) -> decltype(static_cast<B*>(this)->B::mayBePlacedInAdventureMode())
    {
        return static_cast<B*>(this)->B::mayBePlacedInAdventureMode();
    }
    template <class B>
    bool base_Item_mayBePlacedInAdventureMode(long)
    {
        return static_cast<Item*>(this)->Item::mayBePlacedInAdventureMode();
    }

    virtual bool mayBePlacedInAdventureMode()
    {
        if (ModHooks::IsActive(kModHook_Item_mayBePlacedInAdventureMode))
        {
            bool result = false;
            if (!ModEvents::ItemMayBePlacedInAdventureMode(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_mayBePlacedInAdventureMode<Base>(0);
    }

    template <class B>
    auto base_Item_isValidRepairItem(not_null_ptr<ItemInstance> itemInstance, not_null_ptr<ItemInstance> repairItem, int) -> decltype(static_cast<B*>(this)->B::isValidRepairItem(itemInstance, repairItem))
    {
        return static_cast<B*>(this)->B::isValidRepairItem(itemInstance, repairItem);
    }
    template <class B>
    bool base_Item_isValidRepairItem(not_null_ptr<ItemInstance> itemInstance, not_null_ptr<ItemInstance> repairItem, long)
    {
        return static_cast<Item*>(this)->Item::isValidRepairItem(itemInstance, repairItem);
    }

    virtual bool isValidRepairItem(not_null_ptr<ItemInstance> itemInstance, not_null_ptr<ItemInstance> repairItem)
    {
        if (ModHooks::IsActive(kModHook_Item_isValidRepairItem))
        {
            bool result = false;
            if (!ModEvents::ItemIsValidRepairItem(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), const_cast<ItemInstance*>(repairItem.get()), result))
                return result;
        }
        return base_Item_isValidRepairItem<Base>(itemInstance, repairItem, 0);
    }

    template <class B>
    auto base_Item_registerIcons(IconRegister* iconRegister, int) -> decltype(static_cast<B*>(this)->B::registerIcons(iconRegister))
    {
        static_cast<B*>(this)->B::registerIcons(iconRegister);
    }
    template <class B>
    void base_Item_registerIcons(IconRegister* iconRegister, long)
    {
        static_cast<Item*>(this)->Item::registerIcons(iconRegister);
    }

    virtual void registerIcons(IconRegister* iconRegister)
    {
        if (ModHooks::IsActive(kModHook_Item_registerIcons))
        {
            if (!ModEvents::ItemRegisterIcons(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<IconRegister*>(iconRegister)))
                return;
        }
        base_Item_registerIcons<Base>(iconRegister, 0);
    }

    template <class B>
    auto base_Item_hasMultipleSpriteLayers(int) -> decltype(static_cast<B*>(this)->B::hasMultipleSpriteLayers())
    {
        return static_cast<B*>(this)->B::hasMultipleSpriteLayers();
    }
    template <class B>
    bool base_Item_hasMultipleSpriteLayers(long)
    {
        return static_cast<Item*>(this)->Item::hasMultipleSpriteLayers();
    }

    virtual bool hasMultipleSpriteLayers()
    {
        if (ModHooks::IsActive(kModHook_Item_hasMultipleSpriteLayers))
        {
            bool result = false;
            if (!ModEvents::ItemHasMultipleSpriteLayers(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_hasMultipleSpriteLayers<Base>(0);
    }

    template <class B>
    auto base_Item_getLayerIcon(int auxValue, int layer, not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::getLayerIcon(auxValue, layer, itemInstance))
    {
        return static_cast<B*>(this)->B::getLayerIcon(auxValue, layer, itemInstance);
    }
    template <class B>
    TextureAtlasSprite* base_Item_getLayerIcon(int auxValue, int layer, not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::getLayerIcon(auxValue, layer, itemInstance);
    }

    virtual TextureAtlasSprite* getLayerIcon(int auxValue, int layer, not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_getLayerIcon))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::ItemGetLayerIcon(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), auxValue, layer, const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getLayerIcon<Base>(auxValue, layer, itemInstance, 0);
    }

    template <class B>
    auto base_Item_getIconType(int) -> decltype(static_cast<B*>(this)->B::getIconType())
    {
        return static_cast<B*>(this)->B::getIconType();
    }
    template <class B>
    int base_Item_getIconType(long)
    {
        return static_cast<Item*>(this)->Item::getIconType();
    }

    virtual int getIconType()
    {
        if (ModHooks::IsActive(kModHook_Item_getIconType))
        {
            int result = 0;
            if (!ModEvents::ItemGetIconType(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_getIconType<Base>(0);
    }

    template <class B>
    auto base_Item_getIcon(int auxValue, int) -> decltype(static_cast<B*>(this)->B::getIcon(auxValue))
    {
        return static_cast<B*>(this)->B::getIcon(auxValue);
    }
    template <class B>
    TextureAtlasSprite* base_Item_getIcon(int auxValue, long)
    {
        return static_cast<Item*>(this)->Item::getIcon(auxValue);
    }

    virtual TextureAtlasSprite* getIcon(int auxValue)
    {
        if (ModHooks::IsActive(kModHook_Item_getIcon))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::ItemGetIcon(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), auxValue, result))
                return result;
        }
        return base_Item_getIcon<Base>(auxValue, 0);
    }

    template <class B>
    auto base_Item_getIcon_2(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::getIcon(itemInstance))
    {
        return static_cast<B*>(this)->B::getIcon(itemInstance);
    }
    template <class B>
    TextureAtlasSprite* base_Item_getIcon_2(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::getIcon(itemInstance);
    }

    virtual TextureAtlasSprite* getIcon(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_getIcon_2))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::ItemGetIcon2(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_getIcon_2<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Item_GetArmorType(int) -> decltype(static_cast<B*>(this)->B::GetArmorType())
    {
        return static_cast<B*>(this)->B::GetArmorType();
    }
    template <class B>
    int base_Item_GetArmorType(long)
    {
        return static_cast<Item*>(this)->Item::GetArmorType();
    }

    virtual int GetArmorType()
    {
        if (ModHooks::IsActive(kModHook_Item_GetArmorType))
        {
            int result = 0;
            if (!ModEvents::ItemGetArmorType(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_GetArmorType<Base>(0);
    }

    template <class B>
    auto base_Item_GetOverrideCount(int) -> decltype(static_cast<B*>(this)->B::GetOverrideCount())
    {
        return static_cast<B*>(this)->B::GetOverrideCount();
    }
    template <class B>
    int base_Item_GetOverrideCount(long)
    {
        return static_cast<Item*>(this)->Item::GetOverrideCount();
    }

    virtual int GetOverrideCount()
    {
        if (ModHooks::IsActive(kModHook_Item_GetOverrideCount))
        {
            int result = 0;
            if (!ModEvents::ItemGetOverrideCount(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_GetOverrideCount<Base>(0);
    }

    template <class B>
    auto base_Item_GetOverrideCountColour(int) -> decltype(static_cast<B*>(this)->B::GetOverrideCountColour())
    {
        return static_cast<B*>(this)->B::GetOverrideCountColour();
    }
    template <class B>
    int base_Item_GetOverrideCountColour(long)
    {
        return static_cast<Item*>(this)->Item::GetOverrideCountColour();
    }

    virtual int GetOverrideCountColour()
    {
        if (ModHooks::IsActive(kModHook_Item_GetOverrideCountColour))
        {
            int result = 0;
            if (!ModEvents::ItemGetOverrideCountColour(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), result))
                return result;
        }
        return base_Item_GetOverrideCountColour<Base>(0);
    }

    template <class B>
    auto base_Item_GetOverrideCountIcon(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::GetOverrideCountIcon(itemInstance))
    {
        return static_cast<B*>(this)->B::GetOverrideCountIcon(itemInstance);
    }
    template <class B>
    TextureAtlasSprite* base_Item_GetOverrideCountIcon(not_null_ptr<ItemInstance> itemInstance, long)
    {
        return static_cast<Item*>(this)->Item::GetOverrideCountIcon(itemInstance);
    }

    virtual TextureAtlasSprite* GetOverrideCountIcon(not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Item_GetOverrideCountIcon))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::ItemGetOverrideCountIcon(static_cast<Item*>(const_cast<ModItem<Base>*>(this)), const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Item_GetOverrideCountIcon<Base>(itemInstance, 0);
    }

    // not answerable yet, a parameter or result type has no conversion: GetUseTooltip, appendHoverText, finishUsingItem, getDefaultAttributeModifiers, getDefaultInstance, use
};

#endif
