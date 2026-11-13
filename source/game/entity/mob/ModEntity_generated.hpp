// Generated from game/entity/mob/Entity.hpp, game/entity/mob/LivingEntity.hpp, game/entity/mob/Mob.hpp by tools/hooks/hookgen.py. Do not edit.
#ifndef GAME_MODCLASS_ENTITY_GENERATED_HPP
#define GAME_MODCLASS_ENTITY_GENERATED_HPP

#include <stddef.h>

#include "game/core/ModEvents_generated.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModItemNames.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/generation/Player.hpp"
#include "game/entity/mob/LivingEntity.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/entity/mob/LivingEntity.hpp"
#include "game/entity/mob/Mob.hpp"

// every virtual method of Entity a mod can answer, on top of any game class derived from it
template <class Base>
class ModEntity : public Base
{
public:
    using Base::Base;

    virtual void onShared()
    {
        mLive = true;
        Base::onShared();
    }

    template <class B>
    auto base_Entity_fjDerivedCtorCalls(int) -> decltype(static_cast<B*>(this)->B::fjDerivedCtorCalls())
    {
        static_cast<B*>(this)->B::fjDerivedCtorCalls();
    }
    template <class B>
    void base_Entity_fjDerivedCtorCalls(long)
    {
        static_cast<Mob*>(this)->Mob::fjDerivedCtorCalls();
    }

    virtual void fjDerivedCtorCalls()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_fjDerivedCtorCalls))
        {
            if (!ModEvents::EntityFjDerivedCtorCalls(this->shared_from_this()))
                return;
        }
        base_Entity_fjDerivedCtorCalls<Base>(0);
    }

    template <class B>
    auto base_Entity_kill(int) -> decltype(static_cast<B*>(this)->B::kill())
    {
        static_cast<B*>(this)->B::kill();
    }
    template <class B>
    void base_Entity_kill(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::kill();
    }

    virtual void kill()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_kill))
        {
            if (!ModEvents::EntityKill(this->shared_from_this()))
                return;
        }
        base_Entity_kill<Base>(0);
    }

    template <class B>
    auto base_Entity_resetPos(int) -> decltype(static_cast<B*>(this)->B::resetPos())
    {
        static_cast<B*>(this)->B::resetPos();
    }
    template <class B>
    void base_Entity_resetPos(long)
    {
        static_cast<Entity*>(this)->Entity::resetPos();
    }

    virtual void resetPos()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_resetPos))
        {
            if (!ModEvents::EntityResetPos(this->shared_from_this()))
                return;
        }
        base_Entity_resetPos<Base>(0);
    }

    template <class B>
    auto base_Entity_remove(int) -> decltype(static_cast<B*>(this)->B::remove())
    {
        static_cast<B*>(this)->B::remove();
    }
    template <class B>
    void base_Entity_remove(long)
    {
        static_cast<Entity*>(this)->Entity::remove();
    }

    virtual void remove()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_remove))
        {
            if (!ModEvents::EntityRemove(this->shared_from_this()))
                return;
        }
        base_Entity_remove<Base>(0);
    }

    template <class B>
    auto base_Entity_setDropContainerContent(bool dropContainerContent, int) -> decltype(static_cast<B*>(this)->B::setDropContainerContent(dropContainerContent))
    {
        static_cast<B*>(this)->B::setDropContainerContent(dropContainerContent);
    }
    template <class B>
    void base_Entity_setDropContainerContent(bool dropContainerContent, long)
    {
        static_cast<Entity*>(this)->Entity::setDropContainerContent(dropContainerContent);
    }

    virtual void setDropContainerContent(bool dropContainerContent)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setDropContainerContent))
        {
            if (!ModEvents::EntitySetDropContainerContent(this->shared_from_this(), dropContainerContent))
                return;
        }
        base_Entity_setDropContainerContent<Base>(dropContainerContent, 0);
    }

    template <class B>
    auto base_Entity_setSize(float width, float height, int) -> decltype(static_cast<B*>(this)->B::setSize(width, height))
    {
        static_cast<B*>(this)->B::setSize(width, height);
    }
    template <class B>
    void base_Entity_setSize(float width, float height, long)
    {
        static_cast<Entity*>(this)->Entity::setSize(width, height);
    }

    virtual void setSize(float width, float height)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSize))
        {
            if (!ModEvents::EntitySetSize(this->shared_from_this(), width, height))
                return;
        }
        base_Entity_setSize<Base>(width, height, 0);
    }

    template <class B>
    auto base_Entity_setPos(double x, double y, double z, int) -> decltype(static_cast<B*>(this)->B::setPos(x, y, z))
    {
        static_cast<B*>(this)->B::setPos(x, y, z);
    }
    template <class B>
    void base_Entity_setPos(double x, double y, double z, long)
    {
        static_cast<Entity*>(this)->Entity::setPos(x, y, z);
    }

    virtual void setPos(double x, double y, double z)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setPos))
        {
            if (!ModEvents::EntitySetPos(this->shared_from_this(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)))
                return;
        }
        base_Entity_setPos<Base>(x, y, z, 0);
    }

    template <class B>
    auto base_Entity_tick(int) -> decltype(static_cast<B*>(this)->B::tick())
    {
        static_cast<B*>(this)->B::tick();
    }
    template <class B>
    void base_Entity_tick(long)
    {
        static_cast<Mob*>(this)->Mob::tick();
    }

    virtual void tick()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_tick))
        {
            if (!ModEvents::EntityTick(this->shared_from_this()))
                return;
        }
        base_Entity_tick<Base>(0);
    }

    template <class B>
    auto base_Entity_baseTick(int) -> decltype(static_cast<B*>(this)->B::baseTick())
    {
        static_cast<B*>(this)->B::baseTick();
    }
    template <class B>
    void base_Entity_baseTick(long)
    {
        static_cast<Mob*>(this)->Mob::baseTick();
    }

    virtual void baseTick()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_baseTick))
        {
            if (!ModEvents::EntityBaseTick(this->shared_from_this()))
                return;
        }
        base_Entity_baseTick<Base>(0);
    }

    template <class B>
    auto base_Entity_processDimensionDelay(int) -> decltype(static_cast<B*>(this)->B::processDimensionDelay())
    {
        static_cast<B*>(this)->B::processDimensionDelay();
    }
    template <class B>
    void base_Entity_processDimensionDelay(long)
    {
        static_cast<Entity*>(this)->Entity::processDimensionDelay();
    }

    virtual void processDimensionDelay()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_processDimensionDelay))
        {
            if (!ModEvents::EntityProcessDimensionDelay(this->shared_from_this()))
                return;
        }
        base_Entity_processDimensionDelay<Base>(0);
    }

    template <class B>
    auto base_Entity_getPortalWaitTime(int) -> decltype(static_cast<B*>(this)->B::getPortalWaitTime())
    {
        return static_cast<B*>(this)->B::getPortalWaitTime();
    }
    template <class B>
    int base_Entity_getPortalWaitTime(long)
    {
        return static_cast<Entity*>(this)->Entity::getPortalWaitTime();
    }

    virtual int getPortalWaitTime()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getPortalWaitTime))
        {
            int result = 0;
            if (!ModEvents::EntityGetPortalWaitTime(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getPortalWaitTime<Base>(0);
    }

    template <class B>
    auto base_Entity_setSecondsOnFire(int seconds, int) -> decltype(static_cast<B*>(this)->B::setSecondsOnFire(seconds))
    {
        static_cast<B*>(this)->B::setSecondsOnFire(seconds);
    }
    template <class B>
    void base_Entity_setSecondsOnFire(int seconds, long)
    {
        static_cast<Entity*>(this)->Entity::setSecondsOnFire(seconds);
    }

    virtual void setSecondsOnFire(int seconds)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSecondsOnFire))
        {
            if (!ModEvents::EntitySetSecondsOnFire(this->shared_from_this(), seconds))
                return;
        }
        base_Entity_setSecondsOnFire<Base>(seconds, 0);
    }

    template <class B>
    auto base_Entity_clearFire(int) -> decltype(static_cast<B*>(this)->B::clearFire())
    {
        static_cast<B*>(this)->B::clearFire();
    }
    template <class B>
    void base_Entity_clearFire(long)
    {
        static_cast<Entity*>(this)->Entity::clearFire();
    }

    virtual void clearFire()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_clearFire))
        {
            if (!ModEvents::EntityClearFire(this->shared_from_this()))
                return;
        }
        base_Entity_clearFire<Base>(0);
    }

    template <class B>
    auto base_Entity_outOfWorld(int) -> decltype(static_cast<B*>(this)->B::outOfWorld())
    {
        static_cast<B*>(this)->B::outOfWorld();
    }
    template <class B>
    void base_Entity_outOfWorld(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::outOfWorld();
    }

    virtual void outOfWorld()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_outOfWorld))
        {
            if (!ModEvents::EntityOutOfWorld(this->shared_from_this()))
                return;
        }
        base_Entity_outOfWorld<Base>(0);
    }

    template <class B>
    auto base_Entity_move(MoverType type, double x, double y, double z, bool pushedByPiston, int) -> decltype(static_cast<B*>(this)->B::move(type, x, y, z, pushedByPiston))
    {
        static_cast<B*>(this)->B::move(type, x, y, z, pushedByPiston);
    }
    template <class B>
    void base_Entity_move(MoverType type, double x, double y, double z, bool pushedByPiston, long)
    {
        static_cast<Entity*>(this)->Entity::move(type, x, y, z, pushedByPiston);
    }

    virtual void move(MoverType type, double x, double y, double z, bool pushedByPiston)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_move))
        {
            if (!ModEvents::EntityMove(this->shared_from_this(), static_cast<int>(type), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), pushedByPiston))
                return;
        }
        base_Entity_move<Base>(type, x, y, z, pushedByPiston, 0);
    }

    template <class B>
    auto base_Entity_setLocationFromBoundingbox(int) -> decltype(static_cast<B*>(this)->B::setLocationFromBoundingbox())
    {
        static_cast<B*>(this)->B::setLocationFromBoundingbox();
    }
    template <class B>
    void base_Entity_setLocationFromBoundingbox(long)
    {
        static_cast<Entity*>(this)->Entity::setLocationFromBoundingbox();
    }

    virtual void setLocationFromBoundingbox()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setLocationFromBoundingbox))
        {
            if (!ModEvents::EntitySetLocationFromBoundingbox(this->shared_from_this()))
                return;
        }
        base_Entity_setLocationFromBoundingbox<Base>(0);
    }

    template <class B>
    auto base_Entity_getSwimSound(int) -> decltype(static_cast<B*>(this)->B::getSwimSound())
    {
        return static_cast<B*>(this)->B::getSwimSound();
    }
    template <class B>
    const SoundEvent* base_Entity_getSwimSound(long)
    {
        return static_cast<Entity*>(this)->Entity::getSwimSound();
    }

    virtual const SoundEvent* getSwimSound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSwimSound))
        {
            SoundEvent* result = NULL;
            if (!ModEvents::EntityGetSwimSound(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getSwimSound<Base>(0);
    }

    template <class B>
    auto base_Entity_getSwimSplashSound(int) -> decltype(static_cast<B*>(this)->B::getSwimSplashSound())
    {
        return static_cast<B*>(this)->B::getSwimSplashSound();
    }
    template <class B>
    const SoundEvent* base_Entity_getSwimSplashSound(long)
    {
        return static_cast<Entity*>(this)->Entity::getSwimSplashSound();
    }

    virtual const SoundEvent* getSwimSplashSound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSwimSplashSound))
        {
            SoundEvent* result = NULL;
            if (!ModEvents::EntityGetSwimSplashSound(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getSwimSplashSound<Base>(0);
    }

    template <class B>
    auto base_Entity_getHeavySplashSound(int) -> decltype(static_cast<B*>(this)->B::getHeavySplashSound())
    {
        return static_cast<B*>(this)->B::getHeavySplashSound();
    }
    template <class B>
    const SoundEvent* base_Entity_getHeavySplashSound(long)
    {
        return static_cast<Entity*>(this)->Entity::getHeavySplashSound();
    }

    virtual const SoundEvent* getHeavySplashSound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getHeavySplashSound))
        {
            SoundEvent* result = NULL;
            if (!ModEvents::EntityGetHeavySplashSound(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getHeavySplashSound<Base>(0);
    }

    template <class B>
    auto base_Entity_checkInsideBlocks(int) -> decltype(static_cast<B*>(this)->B::checkInsideBlocks())
    {
        static_cast<B*>(this)->B::checkInsideBlocks();
    }
    template <class B>
    void base_Entity_checkInsideBlocks(long)
    {
        static_cast<Entity*>(this)->Entity::checkInsideBlocks();
    }

    virtual void checkInsideBlocks()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_checkInsideBlocks))
        {
            if (!ModEvents::EntityCheckInsideBlocks(this->shared_from_this()))
                return;
        }
        base_Entity_checkInsideBlocks<Base>(0);
    }

    template <class B>
    auto base_Entity_onInsideBlock(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::onInsideBlock(state))
    {
        static_cast<B*>(this)->B::onInsideBlock(state);
    }
    template <class B>
    void base_Entity_onInsideBlock(const BlockState* state, long)
    {
        static_cast<Entity*>(this)->Entity::onInsideBlock(state);
    }

    virtual void onInsideBlock(const BlockState* state)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onInsideBlock))
        {
            if (!ModEvents::EntityOnInsideBlock(this->shared_from_this(), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0)))
                return;
        }
        base_Entity_onInsideBlock<Base>(state, 0);
    }

    template <class B>
    auto base_Entity_playStepSound(const BlockPos& pos, Block* block, int) -> decltype(static_cast<B*>(this)->B::playStepSound(pos, block))
    {
        static_cast<B*>(this)->B::playStepSound(pos, block);
    }
    template <class B>
    void base_Entity_playStepSound(const BlockPos& pos, Block* block, long)
    {
        static_cast<Entity*>(this)->Entity::playStepSound(pos, block);
    }

    virtual void playStepSound(const BlockPos& pos, Block* block)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_playStepSound))
        {
            if (!ModEvents::EntityPlayStepSound(this->shared_from_this(), pos.x, pos.y, pos.z, block))
                return;
        }
        base_Entity_playStepSound<Base>(pos, block, 0);
    }

    template <class B>
    auto base_Entity_playUnderwaterSound(const BlockPos& pos, Block* block, int) -> decltype(static_cast<B*>(this)->B::playUnderwaterSound(pos, block))
    {
        static_cast<B*>(this)->B::playUnderwaterSound(pos, block);
    }
    template <class B>
    void base_Entity_playUnderwaterSound(const BlockPos& pos, Block* block, long)
    {
        static_cast<Entity*>(this)->Entity::playUnderwaterSound(pos, block);
    }

    virtual void playUnderwaterSound(const BlockPos& pos, Block* block)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_playUnderwaterSound))
        {
            if (!ModEvents::EntityPlayUnderwaterSound(this->shared_from_this(), pos.x, pos.y, pos.z, block))
                return;
        }
        base_Entity_playUnderwaterSound<Base>(pos, block, 0);
    }

    template <class B>
    auto base_Entity_playFlySound(float moveDist, int) -> decltype(static_cast<B*>(this)->B::playFlySound(moveDist))
    {
        return static_cast<B*>(this)->B::playFlySound(moveDist);
    }
    template <class B>
    float base_Entity_playFlySound(float moveDist, long)
    {
        return static_cast<Entity*>(this)->Entity::playFlySound(moveDist);
    }

    virtual float playFlySound(float moveDist)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_playFlySound))
        {
            float result = 0.0f;
            if (!ModEvents::EntityPlayFlySound(this->shared_from_this(), moveDist, result))
                return result;
        }
        return base_Entity_playFlySound<Base>(moveDist, 0);
    }

    template <class B>
    auto base_Entity_makeFlySound(int) -> decltype(static_cast<B*>(this)->B::makeFlySound())
    {
        return static_cast<B*>(this)->B::makeFlySound();
    }
    template <class B>
    bool base_Entity_makeFlySound(long)
    {
        return static_cast<Entity*>(this)->Entity::makeFlySound();
    }

    virtual bool makeFlySound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_makeFlySound))
        {
            bool result = false;
            if (!ModEvents::EntityMakeFlySound(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_makeFlySound<Base>(0);
    }

    template <class B>
    auto base_Entity_playSound(const SoundEvent* sound, float volume, float pitch, int) -> decltype(static_cast<B*>(this)->B::playSound(sound, volume, pitch))
    {
        static_cast<B*>(this)->B::playSound(sound, volume, pitch);
    }
    template <class B>
    void base_Entity_playSound(const SoundEvent* sound, float volume, float pitch, long)
    {
        static_cast<Entity*>(this)->Entity::playSound(sound, volume, pitch);
    }

    virtual void playSound(const SoundEvent* sound, float volume, float pitch)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_playSound))
        {
            if (!ModEvents::EntityPlaySound(this->shared_from_this(), const_cast<SoundEvent*>(sound), volume, pitch))
                return;
        }
        base_Entity_playSound<Base>(sound, volume, pitch, 0);
    }

    template <class B>
    auto base_Entity_isSilent(int) -> decltype(static_cast<B*>(this)->B::isSilent())
    {
        return static_cast<B*>(this)->B::isSilent();
    }
    template <class B>
    bool base_Entity_isSilent(long)
    {
        return static_cast<Entity*>(this)->Entity::isSilent();
    }

    virtual bool isSilent()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isSilent))
        {
            bool result = false;
            if (!ModEvents::EntityIsSilent(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isSilent<Base>(0);
    }

    template <class B>
    auto base_Entity_setSilent(bool silent, int) -> decltype(static_cast<B*>(this)->B::setSilent(silent))
    {
        static_cast<B*>(this)->B::setSilent(silent);
    }
    template <class B>
    void base_Entity_setSilent(bool silent, long)
    {
        static_cast<Entity*>(this)->Entity::setSilent(silent);
    }

    virtual void setSilent(bool silent)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSilent))
        {
            if (!ModEvents::EntitySetSilent(this->shared_from_this(), silent))
                return;
        }
        base_Entity_setSilent<Base>(silent, 0);
    }

    template <class B>
    auto base_Entity_isNoGravity(int) -> decltype(static_cast<B*>(this)->B::isNoGravity())
    {
        return static_cast<B*>(this)->B::isNoGravity();
    }
    template <class B>
    bool base_Entity_isNoGravity(long)
    {
        return static_cast<Entity*>(this)->Entity::isNoGravity();
    }

    virtual bool isNoGravity()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isNoGravity))
        {
            bool result = false;
            if (!ModEvents::EntityIsNoGravity(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isNoGravity<Base>(0);
    }

    template <class B>
    auto base_Entity_setNoGravity(bool noGravity, int) -> decltype(static_cast<B*>(this)->B::setNoGravity(noGravity))
    {
        static_cast<B*>(this)->B::setNoGravity(noGravity);
    }
    template <class B>
    void base_Entity_setNoGravity(bool noGravity, long)
    {
        static_cast<Entity*>(this)->Entity::setNoGravity(noGravity);
    }

    virtual void setNoGravity(bool noGravity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setNoGravity))
        {
            if (!ModEvents::EntitySetNoGravity(this->shared_from_this(), noGravity))
                return;
        }
        base_Entity_setNoGravity<Base>(noGravity, 0);
    }

    template <class B>
    auto base_Entity_isMoving(int) -> decltype(static_cast<B*>(this)->B::isMoving())
    {
        return static_cast<B*>(this)->B::isMoving();
    }
    template <class B>
    bool base_Entity_isMoving(long)
    {
        return static_cast<Entity*>(this)->Entity::isMoving();
    }

    virtual bool isMoving()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isMoving))
        {
            bool result = false;
            if (!ModEvents::EntityIsMoving(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isMoving<Base>(0);
    }

    template <class B>
    auto base_Entity_setMoving(bool moving, int) -> decltype(static_cast<B*>(this)->B::setMoving(moving))
    {
        static_cast<B*>(this)->B::setMoving(moving);
    }
    template <class B>
    void base_Entity_setMoving(bool moving, long)
    {
        static_cast<Entity*>(this)->Entity::setMoving(moving);
    }

    virtual void setMoving(bool moving)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setMoving))
        {
            if (!ModEvents::EntitySetMoving(this->shared_from_this(), moving))
                return;
        }
        base_Entity_setMoving<Base>(moving, 0);
    }

    template <class B>
    auto base_Entity_makeStepSound(int) -> decltype(static_cast<B*>(this)->B::makeStepSound())
    {
        return static_cast<B*>(this)->B::makeStepSound();
    }
    template <class B>
    bool base_Entity_makeStepSound(long)
    {
        return static_cast<Entity*>(this)->Entity::makeStepSound();
    }

    virtual bool makeStepSound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_makeStepSound))
        {
            bool result = false;
            if (!ModEvents::EntityMakeStepSound(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_makeStepSound<Base>(0);
    }

    template <class B>
    auto base_Entity_getCollideBox(int) -> decltype(static_cast<B*>(this)->B::getCollideBox())
    {
        return static_cast<B*>(this)->B::getCollideBox();
    }
    template <class B>
    AABB* base_Entity_getCollideBox(long)
    {
        return static_cast<Entity*>(this)->Entity::getCollideBox();
    }

    virtual AABB* getCollideBox()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getCollideBox))
        {
            AABB* result = NULL;
            if (!ModEvents::EntityGetCollideBox(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getCollideBox<Base>(0);
    }

    template <class B>
    auto base_Entity_burn(int damage, int) -> decltype(static_cast<B*>(this)->B::burn(damage))
    {
        static_cast<B*>(this)->B::burn(damage);
    }
    template <class B>
    void base_Entity_burn(int damage, long)
    {
        static_cast<Entity*>(this)->Entity::burn(damage);
    }

    virtual void burn(int damage)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_burn))
        {
            if (!ModEvents::EntityBurn(this->shared_from_this(), damage))
                return;
        }
        base_Entity_burn<Base>(damage, 0);
    }

    template <class B>
    auto base_Entity_causeFallDamage(float fallDistance, float damageMultiplier, int) -> decltype(static_cast<B*>(this)->B::causeFallDamage(fallDistance, damageMultiplier))
    {
        static_cast<B*>(this)->B::causeFallDamage(fallDistance, damageMultiplier);
    }
    template <class B>
    void base_Entity_causeFallDamage(float fallDistance, float damageMultiplier, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::causeFallDamage(fallDistance, damageMultiplier);
    }

    virtual void causeFallDamage(float fallDistance, float damageMultiplier)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_causeFallDamage))
        {
            if (!ModEvents::EntityCauseFallDamage(this->shared_from_this(), fallDistance, damageMultiplier))
                return;
        }
        base_Entity_causeFallDamage<Base>(fallDistance, damageMultiplier, 0);
    }

    template <class B>
    auto base_Entity_isInWater(int) -> decltype(static_cast<B*>(this)->B::isInWater())
    {
        return static_cast<B*>(this)->B::isInWater();
    }
    template <class B>
    bool base_Entity_isInWater(long)
    {
        return static_cast<Entity*>(this)->Entity::isInWater();
    }

    virtual bool isInWater()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInWater))
        {
            bool result = false;
            if (!ModEvents::EntityIsInWater(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isInWater<Base>(0);
    }

    template <class B>
    auto base_Entity_headInWater(int) -> decltype(static_cast<B*>(this)->B::headInWater())
    {
        return static_cast<B*>(this)->B::headInWater();
    }
    template <class B>
    bool base_Entity_headInWater(long)
    {
        return static_cast<Entity*>(this)->Entity::headInWater();
    }

    virtual bool headInWater()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_headInWater))
        {
            bool result = false;
            if (!ModEvents::EntityHeadInWater(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_headInWater<Base>(0);
    }

    template <class B>
    auto base_Entity_isOverWater(int) -> decltype(static_cast<B*>(this)->B::isOverWater())
    {
        return static_cast<B*>(this)->B::isOverWater();
    }
    template <class B>
    bool base_Entity_isOverWater(long)
    {
        return static_cast<Entity*>(this)->Entity::isOverWater();
    }

    virtual bool isOverWater()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isOverWater))
        {
            bool result = false;
            if (!ModEvents::EntityIsOverWater(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isOverWater<Base>(0);
    }

    template <class B>
    auto base_Entity_updateInWaterState(int) -> decltype(static_cast<B*>(this)->B::updateInWaterState())
    {
        return static_cast<B*>(this)->B::updateInWaterState();
    }
    template <class B>
    bool base_Entity_updateInWaterState(long)
    {
        return static_cast<Entity*>(this)->Entity::updateInWaterState();
    }

    virtual bool updateInWaterState()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_updateInWaterState))
        {
            bool result = false;
            if (!ModEvents::EntityUpdateInWaterState(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_updateInWaterState<Base>(0);
    }

    template <class B>
    auto base_Entity_doWaterSplashEffect(int) -> decltype(static_cast<B*>(this)->B::doWaterSplashEffect())
    {
        static_cast<B*>(this)->B::doWaterSplashEffect();
    }
    template <class B>
    void base_Entity_doWaterSplashEffect(long)
    {
        static_cast<Entity*>(this)->Entity::doWaterSplashEffect();
    }

    virtual void doWaterSplashEffect()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_doWaterSplashEffect))
        {
            if (!ModEvents::EntityDoWaterSplashEffect(this->shared_from_this()))
                return;
        }
        base_Entity_doWaterSplashEffect<Base>(0);
    }

    template <class B>
    auto base_Entity_updateSprintingState(int) -> decltype(static_cast<B*>(this)->B::updateSprintingState())
    {
        static_cast<B*>(this)->B::updateSprintingState();
    }
    template <class B>
    void base_Entity_updateSprintingState(long)
    {
        static_cast<Entity*>(this)->Entity::updateSprintingState();
    }

    virtual void updateSprintingState()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_updateSprintingState))
        {
            if (!ModEvents::EntityUpdateSprintingState(this->shared_from_this()))
                return;
        }
        base_Entity_updateSprintingState<Base>(0);
    }

    template <class B>
    auto base_Entity_doSprintParticleEffect(int) -> decltype(static_cast<B*>(this)->B::doSprintParticleEffect())
    {
        static_cast<B*>(this)->B::doSprintParticleEffect();
    }
    template <class B>
    void base_Entity_doSprintParticleEffect(long)
    {
        static_cast<Entity*>(this)->Entity::doSprintParticleEffect();
    }

    virtual void doSprintParticleEffect()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_doSprintParticleEffect))
        {
            if (!ModEvents::EntityDoSprintParticleEffect(this->shared_from_this()))
                return;
        }
        base_Entity_doSprintParticleEffect<Base>(0);
    }

    template <class B>
    auto base_Entity_isUnderLiquid(Material* material, int) -> decltype(static_cast<B*>(this)->B::isUnderLiquid(material))
    {
        return static_cast<B*>(this)->B::isUnderLiquid(material);
    }
    template <class B>
    bool base_Entity_isUnderLiquid(Material* material, long)
    {
        return static_cast<Entity*>(this)->Entity::isUnderLiquid(material);
    }

    virtual bool isUnderLiquid(Material* material)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isUnderLiquid))
        {
            bool result = false;
            if (!ModEvents::EntityIsUnderLiquid(this->shared_from_this(), const_cast<Material*>(material), result))
                return result;
        }
        return base_Entity_isUnderLiquid<Base>(material, 0);
    }

    template <class B>
    auto base_Entity_isInLiquid(Material* material, Vec3* pos, int) -> decltype(static_cast<B*>(this)->B::isInLiquid(material, pos))
    {
        return static_cast<B*>(this)->B::isInLiquid(material, pos);
    }
    template <class B>
    bool base_Entity_isInLiquid(Material* material, Vec3* pos, long)
    {
        return static_cast<Entity*>(this)->Entity::isInLiquid(material, pos);
    }

    virtual bool isInLiquid(Material* material, Vec3* pos)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInLiquid))
        {
            bool result = false;
            if (!ModEvents::EntityIsInLiquid(this->shared_from_this(), const_cast<Material*>(material), const_cast<Vec3*>(pos), result))
                return result;
        }
        return base_Entity_isInLiquid<Base>(material, pos, 0);
    }

    template <class B>
    auto base_Entity_getLightColor(int) -> decltype(static_cast<B*>(this)->B::getLightColor())
    {
        return static_cast<B*>(this)->B::getLightColor();
    }
    template <class B>
    int base_Entity_getLightColor(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getLightColor();
    }

    virtual int getLightColor()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getLightColor))
        {
            int result = 0;
            if (!ModEvents::EntityGetLightColor(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getLightColor<Base>(0);
    }

    template <class B>
    auto base_Entity_getBrightness(int) -> decltype(static_cast<B*>(this)->B::getBrightness())
    {
        return static_cast<B*>(this)->B::getBrightness();
    }
    template <class B>
    float base_Entity_getBrightness(long)
    {
        return static_cast<Entity*>(this)->Entity::getBrightness();
    }

    virtual float getBrightness()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getBrightness))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetBrightness(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getBrightness<Base>(0);
    }

    template <class B>
    auto base_Entity_setLevel(Level* level, int) -> decltype(static_cast<B*>(this)->B::setLevel(level))
    {
        static_cast<B*>(this)->B::setLevel(level);
    }
    template <class B>
    void base_Entity_setLevel(Level* level, long)
    {
        static_cast<Mob*>(this)->Mob::setLevel(level);
    }

    virtual void setLevel(Level* level)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setLevel))
        {
            if (!ModEvents::EntitySetLevel(this->shared_from_this(), level))
                return;
        }
        base_Entity_setLevel<Base>(level, 0);
    }

    template <class B>
    auto base_Entity_moveTo(double x, double y, double z, float yRot, float xRot, int) -> decltype(static_cast<B*>(this)->B::moveTo(x, y, z, yRot, xRot))
    {
        static_cast<B*>(this)->B::moveTo(x, y, z, yRot, xRot);
    }
    template <class B>
    void base_Entity_moveTo(double x, double y, double z, float yRot, float xRot, long)
    {
        static_cast<Entity*>(this)->Entity::moveTo(x, y, z, yRot, xRot);
    }

    virtual void moveTo(double x, double y, double z, float yRot, float xRot)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_moveTo))
        {
            if (!ModEvents::EntityMoveTo(this->shared_from_this(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), yRot, xRot))
                return;
        }
        base_Entity_moveTo<Base>(x, y, z, yRot, xRot, 0);
    }

    template <class B>
    auto base_Entity_playerTouch(boost::shared_ptr<Player> player, int) -> decltype(static_cast<B*>(this)->B::playerTouch(player))
    {
        static_cast<B*>(this)->B::playerTouch(player);
    }
    template <class B>
    void base_Entity_playerTouch(boost::shared_ptr<Player> player, long)
    {
        static_cast<Entity*>(this)->Entity::playerTouch(player);
    }

    virtual void playerTouch(boost::shared_ptr<Player> player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_playerTouch))
        {
            if (!ModEvents::EntityPlayerTouch(this->shared_from_this(), player))
                return;
        }
        base_Entity_playerTouch<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_push(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::push(entity))
    {
        static_cast<B*>(this)->B::push(entity);
    }
    template <class B>
    void base_Entity_push(boost::shared_ptr<Entity> entity, long)
    {
        static_cast<Entity*>(this)->Entity::push(entity);
    }

    virtual void push(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_push))
        {
            if (!ModEvents::EntityPush(this->shared_from_this(), entity))
                return;
        }
        base_Entity_push<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_push_2(double x, double y, double z, int) -> decltype(static_cast<B*>(this)->B::push(x, y, z))
    {
        static_cast<B*>(this)->B::push(x, y, z);
    }
    template <class B>
    void base_Entity_push_2(double x, double y, double z, long)
    {
        static_cast<Entity*>(this)->Entity::push(x, y, z);
    }

    virtual void push(double x, double y, double z)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_push_2))
        {
            if (!ModEvents::EntityPush2(this->shared_from_this(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)))
                return;
        }
        base_Entity_push_2<Base>(x, y, z, 0);
    }

    template <class B>
    auto base_Entity_markHurt(int) -> decltype(static_cast<B*>(this)->B::markHurt())
    {
        static_cast<B*>(this)->B::markHurt();
    }
    template <class B>
    void base_Entity_markHurt(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::markHurt();
    }

    virtual void markHurt()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_markHurt))
        {
            if (!ModEvents::EntityMarkHurt(this->shared_from_this()))
                return;
        }
        base_Entity_markHurt<Base>(0);
    }

    template <class B>
    auto base_Entity_hurt(const DamageSource* source, float damage, int) -> decltype(static_cast<B*>(this)->B::hurt(source, damage))
    {
        return static_cast<B*>(this)->B::hurt(source, damage);
    }
    template <class B>
    bool base_Entity_hurt(const DamageSource* source, float damage, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::hurt(source, damage);
    }

    virtual bool hurt(const DamageSource* source, float damage)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hurt))
        {
            bool result = false;
            if (!ModEvents::EntityHurt(this->shared_from_this(), const_cast<DamageSource*>(source), damage, result))
                return result;
        }
        return base_Entity_hurt<Base>(source, damage, 0);
    }

    template <class B>
    auto base_Entity_getViewVector(float partialTicks, int) -> decltype(static_cast<B*>(this)->B::getViewVector(partialTicks))
    {
        return static_cast<B*>(this)->B::getViewVector(partialTicks);
    }
    template <class B>
    Vec3* base_Entity_getViewVector(float partialTicks, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getViewVector(partialTicks);
    }

    virtual Vec3* getViewVector(float partialTicks)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getViewVector))
        {
            Vec3* result = NULL;
            if (!ModEvents::EntityGetViewVector(this->shared_from_this(), partialTicks, result))
                return result;
        }
        return base_Entity_getViewVector<Base>(partialTicks, 0);
    }

    template <class B>
    auto base_Entity_isPickable(int) -> decltype(static_cast<B*>(this)->B::isPickable())
    {
        return static_cast<B*>(this)->B::isPickable();
    }
    template <class B>
    bool base_Entity_isPickable(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isPickable();
    }

    virtual bool isPickable()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isPickable))
        {
            bool result = false;
            if (!ModEvents::EntityIsPickable(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isPickable<Base>(0);
    }

    template <class B>
    auto base_Entity_isPushable(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::isPushable(entity))
    {
        return static_cast<B*>(this)->B::isPushable(entity);
    }
    template <class B>
    bool base_Entity_isPushable(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isPushable(entity);
    }

    virtual bool isPushable(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isPushable))
        {
            bool result = false;
            if (!ModEvents::EntityIsPushable(this->shared_from_this(), entity, result))
                return result;
        }
        return base_Entity_isPushable<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_awardKillScore(boost::shared_ptr<Entity> killed, int score, const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::awardKillScore(killed, score, source))
    {
        static_cast<B*>(this)->B::awardKillScore(killed, score, source);
    }
    template <class B>
    void base_Entity_awardKillScore(boost::shared_ptr<Entity> killed, int score, const DamageSource* source, long)
    {
        static_cast<Entity*>(this)->Entity::awardKillScore(killed, score, source);
    }

    virtual void awardKillScore(boost::shared_ptr<Entity> killed, int score, const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_awardKillScore))
        {
            if (!ModEvents::EntityAwardKillScore(this->shared_from_this(), killed, score, const_cast<DamageSource*>(source)))
                return;
        }
        base_Entity_awardKillScore<Base>(killed, score, source, 0);
    }

    template <class B>
    auto base_Entity_shouldRender(double x, double y, double z, int) -> decltype(static_cast<B*>(this)->B::shouldRender(x, y, z))
    {
        return static_cast<B*>(this)->B::shouldRender(x, y, z);
    }
    template <class B>
    bool base_Entity_shouldRender(double x, double y, double z, long)
    {
        return static_cast<Entity*>(this)->Entity::shouldRender(x, y, z);
    }

    virtual bool shouldRender(double x, double y, double z)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_shouldRender))
        {
            bool result = false;
            if (!ModEvents::EntityShouldRender(this->shared_from_this(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), result))
                return result;
        }
        return base_Entity_shouldRender<Base>(x, y, z, 0);
    }

    template <class B>
    auto base_Entity_shouldRender_2(double distanceSqr, int) -> decltype(static_cast<B*>(this)->B::shouldRender(distanceSqr))
    {
        return static_cast<B*>(this)->B::shouldRender(distanceSqr);
    }
    template <class B>
    bool base_Entity_shouldRender_2(double distanceSqr, long)
    {
        return static_cast<Entity*>(this)->Entity::shouldRender(distanceSqr);
    }

    virtual bool shouldRender(double distanceSqr)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_shouldRender_2))
        {
            bool result = false;
            if (!ModEvents::EntityShouldRender2(this->shared_from_this(), static_cast<float>(distanceSqr), result))
                return result;
        }
        return base_Entity_shouldRender_2<Base>(distanceSqr, 0);
    }

    template <class B>
    auto base_Entity_shouldRenderAtSqrDistance(double distanceSqr, int) -> decltype(static_cast<B*>(this)->B::shouldRenderAtSqrDistance(distanceSqr))
    {
        return static_cast<B*>(this)->B::shouldRenderAtSqrDistance(distanceSqr);
    }
    template <class B>
    bool base_Entity_shouldRenderAtSqrDistance(double distanceSqr, long)
    {
        return static_cast<Entity*>(this)->Entity::shouldRenderAtSqrDistance(distanceSqr);
    }

    virtual bool shouldRenderAtSqrDistance(double distanceSqr)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_shouldRenderAtSqrDistance))
        {
            bool result = false;
            if (!ModEvents::EntityShouldRenderAtSqrDistance(this->shared_from_this(), static_cast<float>(distanceSqr), result))
                return result;
        }
        return base_Entity_shouldRenderAtSqrDistance<Base>(distanceSqr, 0);
    }

    template <class B>
    auto base_Entity_save(CompoundTag* tag, int) -> decltype(static_cast<B*>(this)->B::save(tag))
    {
        return static_cast<B*>(this)->B::save(tag);
    }
    template <class B>
    bool base_Entity_save(CompoundTag* tag, long)
    {
        return static_cast<Entity*>(this)->Entity::save(tag);
    }

    virtual bool save(CompoundTag* tag)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_save))
        {
            bool result = false;
            if (!ModEvents::EntitySave(this->shared_from_this(), const_cast<CompoundTag*>(tag), result))
                return result;
        }
        return base_Entity_save<Base>(tag, 0);
    }

    template <class B>
    auto base_Entity_load(CompoundTag* tag, int) -> decltype(static_cast<B*>(this)->B::load(tag))
    {
        static_cast<B*>(this)->B::load(tag);
    }
    template <class B>
    void base_Entity_load(CompoundTag* tag, long)
    {
        static_cast<Entity*>(this)->Entity::load(tag);
    }

    virtual void load(CompoundTag* tag)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_load))
        {
            if (!ModEvents::EntityLoad(this->shared_from_this(), const_cast<CompoundTag*>(tag)))
                return;
        }
        base_Entity_load<Base>(tag, 0);
    }

    template <class B>
    auto base_Entity_shouldTick(int) -> decltype(static_cast<B*>(this)->B::shouldTick())
    {
        return static_cast<B*>(this)->B::shouldTick();
    }
    template <class B>
    bool base_Entity_shouldTick(long)
    {
        return static_cast<Entity*>(this)->Entity::shouldTick();
    }

    virtual bool shouldTick()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_shouldTick))
        {
            bool result = false;
            if (!ModEvents::EntityShouldTick(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_shouldTick<Base>(0);
    }

    template <class B>
    auto base_Entity_repositionEntityAfterLoad(int) -> decltype(static_cast<B*>(this)->B::repositionEntityAfterLoad())
    {
        return static_cast<B*>(this)->B::repositionEntityAfterLoad();
    }
    template <class B>
    bool base_Entity_repositionEntityAfterLoad(long)
    {
        return static_cast<Entity*>(this)->Entity::repositionEntityAfterLoad();
    }

    virtual bool repositionEntityAfterLoad()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_repositionEntityAfterLoad))
        {
            bool result = false;
            if (!ModEvents::EntityRepositionEntityAfterLoad(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_repositionEntityAfterLoad<Base>(0);
    }

    template <class B>
    auto base_Entity_getAdditionContructionTag(int) -> decltype(static_cast<B*>(this)->B::getAdditionContructionTag())
    {
        return static_cast<B*>(this)->B::getAdditionContructionTag();
    }
    template <class B>
    CompoundTag* base_Entity_getAdditionContructionTag(long)
    {
        return static_cast<Entity*>(this)->Entity::getAdditionContructionTag();
    }

    virtual CompoundTag* getAdditionContructionTag()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAdditionContructionTag))
        {
            CompoundTag* result = NULL;
            if (!ModEvents::EntityGetAdditionContructionTag(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getAdditionContructionTag<Base>(0);
    }

    template <class B>
    auto base_Entity_isAlive(int) -> decltype(static_cast<B*>(this)->B::isAlive())
    {
        return static_cast<B*>(this)->B::isAlive();
    }
    template <class B>
    bool base_Entity_isAlive(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isAlive();
    }

    virtual bool isAlive()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isAlive))
        {
            bool result = false;
            if (!ModEvents::EntityIsAlive(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isAlive<Base>(0);
    }

    template <class B>
    auto base_Entity_isInWall(int) -> decltype(static_cast<B*>(this)->B::isInWall())
    {
        return static_cast<B*>(this)->B::isInWall();
    }
    template <class B>
    bool base_Entity_isInWall(long)
    {
        return static_cast<Entity*>(this)->Entity::isInWall();
    }

    virtual bool isInWall()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInWall))
        {
            bool result = false;
            if (!ModEvents::EntityIsInWall(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isInWall<Base>(0);
    }

    template <class B>
    auto base_Entity_interact(const boost::shared_ptr<Player>& player, InteractionHand::EInteractionHand hand, int) -> decltype(static_cast<B*>(this)->B::interact(player, hand))
    {
        return static_cast<B*>(this)->B::interact(player, hand);
    }
    template <class B>
    bool base_Entity_interact(const boost::shared_ptr<Player>& player, InteractionHand::EInteractionHand hand, long)
    {
        return static_cast<Mob*>(this)->Mob::interact(player, hand);
    }

    virtual bool interact(const boost::shared_ptr<Player>& player, InteractionHand::EInteractionHand hand)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_interact))
        {
            bool result = false;
            if (!ModEvents::EntityInteract(this->shared_from_this(), player, static_cast<int>(hand), result))
                return result;
        }
        return base_Entity_interact<Base>(player, hand, 0);
    }

    template <class B>
    auto base_Entity_getCollideAgainstBox(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::getCollideAgainstBox(entity))
    {
        return static_cast<B*>(this)->B::getCollideAgainstBox(entity);
    }
    template <class B>
    AABB* base_Entity_getCollideAgainstBox(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Entity*>(this)->Entity::getCollideAgainstBox(entity);
    }

    virtual AABB* getCollideAgainstBox(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getCollideAgainstBox))
        {
            AABB* result = NULL;
            if (!ModEvents::EntityGetCollideAgainstBox(this->shared_from_this(), entity, result))
                return result;
        }
        return base_Entity_getCollideAgainstBox<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_rideTick(int) -> decltype(static_cast<B*>(this)->B::rideTick())
    {
        static_cast<B*>(this)->B::rideTick();
    }
    template <class B>
    void base_Entity_rideTick(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::rideTick();
    }

    virtual void rideTick()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_rideTick))
        {
            if (!ModEvents::EntityRideTick(this->shared_from_this()))
                return;
        }
        base_Entity_rideTick<Base>(0);
    }

    template <class B>
    auto base_Entity_positionRider(const boost::shared_ptr<Entity>& passenger, int) -> decltype(static_cast<B*>(this)->B::positionRider(passenger))
    {
        static_cast<B*>(this)->B::positionRider(passenger);
    }
    template <class B>
    void base_Entity_positionRider(const boost::shared_ptr<Entity>& passenger, long)
    {
        static_cast<Entity*>(this)->Entity::positionRider(passenger);
    }

    virtual void positionRider(const boost::shared_ptr<Entity>& passenger)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_positionRider))
        {
            if (!ModEvents::EntityPositionRider(this->shared_from_this(), passenger))
                return;
        }
        base_Entity_positionRider<Base>(passenger, 0);
    }

    template <class B>
    auto base_Entity_onPassengerTurned(boost::shared_ptr<Entity> passenger, int) -> decltype(static_cast<B*>(this)->B::onPassengerTurned(passenger))
    {
        static_cast<B*>(this)->B::onPassengerTurned(passenger);
    }
    template <class B>
    void base_Entity_onPassengerTurned(boost::shared_ptr<Entity> passenger, long)
    {
        static_cast<Entity*>(this)->Entity::onPassengerTurned(passenger);
    }

    virtual void onPassengerTurned(boost::shared_ptr<Entity> passenger)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onPassengerTurned))
        {
            if (!ModEvents::EntityOnPassengerTurned(this->shared_from_this(), passenger))
                return;
        }
        base_Entity_onPassengerTurned<Base>(passenger, 0);
    }

    template <class B>
    auto base_Entity_showVehicleHealth(int) -> decltype(static_cast<B*>(this)->B::showVehicleHealth())
    {
        return static_cast<B*>(this)->B::showVehicleHealth();
    }
    template <class B>
    bool base_Entity_showVehicleHealth(long)
    {
        return static_cast<Entity*>(this)->Entity::showVehicleHealth();
    }

    virtual bool showVehicleHealth()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_showVehicleHealth))
        {
            bool result = false;
            if (!ModEvents::EntityShowVehicleHealth(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_showVehicleHealth<Base>(0);
    }

    template <class B>
    auto base_Entity_startRiding(boost::shared_ptr<Entity> vehicle, int) -> decltype(static_cast<B*>(this)->B::startRiding(vehicle))
    {
        return static_cast<B*>(this)->B::startRiding(vehicle);
    }
    template <class B>
    bool base_Entity_startRiding(boost::shared_ptr<Entity> vehicle, long)
    {
        return static_cast<Entity*>(this)->Entity::startRiding(vehicle);
    }

    virtual bool startRiding(boost::shared_ptr<Entity> vehicle)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_startRiding))
        {
            bool result = false;
            if (!ModEvents::EntityStartRiding(this->shared_from_this(), vehicle, result))
                return result;
        }
        return base_Entity_startRiding<Base>(vehicle, 0);
    }

    template <class B>
    auto base_Entity_startRiding_2(boost::shared_ptr<Entity> entity, bool force, int) -> decltype(static_cast<B*>(this)->B::startRiding(entity, force))
    {
        return static_cast<B*>(this)->B::startRiding(entity, force);
    }
    template <class B>
    bool base_Entity_startRiding_2(boost::shared_ptr<Entity> entity, bool force, long)
    {
        return static_cast<Mob*>(this)->Mob::startRiding(entity, force);
    }

    virtual bool startRiding(boost::shared_ptr<Entity> entity, bool force)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_startRiding_2))
        {
            bool result = false;
            if (!ModEvents::EntityStartRiding2(this->shared_from_this(), entity, force, result))
                return result;
        }
        return base_Entity_startRiding_2<Base>(entity, force, 0);
    }

    template <class B>
    auto base_Entity_HideUseTooltips(int) -> decltype(static_cast<B*>(this)->B::HideUseTooltips())
    {
        return static_cast<B*>(this)->B::HideUseTooltips();
    }
    template <class B>
    bool base_Entity_HideUseTooltips(long)
    {
        return static_cast<Entity*>(this)->Entity::HideUseTooltips();
    }

    virtual bool HideUseTooltips()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_HideUseTooltips))
        {
            bool result = false;
            if (!ModEvents::EntityHideUseTooltips(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_HideUseTooltips<Base>(0);
    }

    template <class B>
    auto base_Entity_canRide(boost::shared_ptr<Entity> vehicle, int) -> decltype(static_cast<B*>(this)->B::canRide(vehicle))
    {
        return static_cast<B*>(this)->B::canRide(vehicle);
    }
    template <class B>
    bool base_Entity_canRide(boost::shared_ptr<Entity> vehicle, long)
    {
        return static_cast<Entity*>(this)->Entity::canRide(vehicle);
    }

    virtual bool canRide(boost::shared_ptr<Entity> vehicle)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canRide))
        {
            bool result = false;
            if (!ModEvents::EntityCanRide(this->shared_from_this(), vehicle, result))
                return result;
        }
        return base_Entity_canRide<Base>(vehicle, 0);
    }

    template <class B>
    auto base_Entity_ejectPassengers(int) -> decltype(static_cast<B*>(this)->B::ejectPassengers())
    {
        static_cast<B*>(this)->B::ejectPassengers();
    }
    template <class B>
    void base_Entity_ejectPassengers(long)
    {
        static_cast<Entity*>(this)->Entity::ejectPassengers();
    }

    virtual void ejectPassengers()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_ejectPassengers))
        {
            if (!ModEvents::EntityEjectPassengers(this->shared_from_this()))
                return;
        }
        base_Entity_ejectPassengers<Base>(0);
    }

    template <class B>
    auto base_Entity_stopRiding(int) -> decltype(static_cast<B*>(this)->B::stopRiding())
    {
        static_cast<B*>(this)->B::stopRiding();
    }
    template <class B>
    void base_Entity_stopRiding(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::stopRiding();
    }

    virtual void stopRiding()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_stopRiding))
        {
            if (!ModEvents::EntityStopRiding(this->shared_from_this()))
                return;
        }
        base_Entity_stopRiding<Base>(0);
    }

    template <class B>
    auto base_Entity_stopRidingBeforeTeleport(int) -> decltype(static_cast<B*>(this)->B::stopRidingBeforeTeleport())
    {
        static_cast<B*>(this)->B::stopRidingBeforeTeleport();
    }
    template <class B>
    void base_Entity_stopRidingBeforeTeleport(long)
    {
        static_cast<Entity*>(this)->Entity::stopRidingBeforeTeleport();
    }

    virtual void stopRidingBeforeTeleport()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_stopRidingBeforeTeleport))
        {
            if (!ModEvents::EntityStopRidingBeforeTeleport(this->shared_from_this()))
                return;
        }
        base_Entity_stopRidingBeforeTeleport<Base>(0);
    }

    template <class B>
    auto base_Entity_addPassenger(boost::shared_ptr<Entity> passenger, int) -> decltype(static_cast<B*>(this)->B::addPassenger(passenger))
    {
        static_cast<B*>(this)->B::addPassenger(passenger);
    }
    template <class B>
    void base_Entity_addPassenger(boost::shared_ptr<Entity> passenger, long)
    {
        static_cast<Entity*>(this)->Entity::addPassenger(passenger);
    }

    virtual void addPassenger(boost::shared_ptr<Entity> passenger)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_addPassenger))
        {
            if (!ModEvents::EntityAddPassenger(this->shared_from_this(), passenger))
                return;
        }
        base_Entity_addPassenger<Base>(passenger, 0);
    }

    template <class B>
    auto base_Entity_removePassenger(boost::shared_ptr<Entity> passenger, int) -> decltype(static_cast<B*>(this)->B::removePassenger(passenger))
    {
        static_cast<B*>(this)->B::removePassenger(passenger);
    }
    template <class B>
    void base_Entity_removePassenger(boost::shared_ptr<Entity> passenger, long)
    {
        static_cast<Entity*>(this)->Entity::removePassenger(passenger);
    }

    virtual void removePassenger(boost::shared_ptr<Entity> passenger)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_removePassenger))
        {
            if (!ModEvents::EntityRemovePassenger(this->shared_from_this(), passenger))
                return;
        }
        base_Entity_removePassenger<Base>(passenger, 0);
    }

    template <class B>
    auto base_Entity_canAddPassenger(boost::shared_ptr<Entity> passenger, int) -> decltype(static_cast<B*>(this)->B::canAddPassenger(passenger))
    {
        return static_cast<B*>(this)->B::canAddPassenger(passenger);
    }
    template <class B>
    bool base_Entity_canAddPassenger(boost::shared_ptr<Entity> passenger, long)
    {
        return static_cast<Entity*>(this)->Entity::canAddPassenger(passenger);
    }

    virtual bool canAddPassenger(boost::shared_ptr<Entity> passenger)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canAddPassenger))
        {
            bool result = false;
            if (!ModEvents::EntityCanAddPassenger(this->shared_from_this(), passenger, result))
                return result;
        }
        return base_Entity_canAddPassenger<Base>(passenger, 0);
    }

    template <class B>
    auto base_Entity_lerpTo(double x, double y, double z, float yRot, float xRot, int steps, bool teleport, int) -> decltype(static_cast<B*>(this)->B::lerpTo(x, y, z, yRot, xRot, steps, teleport))
    {
        static_cast<B*>(this)->B::lerpTo(x, y, z, yRot, xRot, steps, teleport);
    }
    template <class B>
    void base_Entity_lerpTo(double x, double y, double z, float yRot, float xRot, int steps, bool teleport, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::lerpTo(x, y, z, yRot, xRot, steps, teleport);
    }

    virtual void lerpTo(double x, double y, double z, float yRot, float xRot, int steps, bool teleport)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_lerpTo))
        {
            if (!ModEvents::EntityLerpTo(this->shared_from_this(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), yRot, xRot, steps, teleport))
                return;
        }
        base_Entity_lerpTo<Base>(x, y, z, yRot, xRot, steps, teleport, 0);
    }

    template <class B>
    auto base_Entity_getPickRadius(int) -> decltype(static_cast<B*>(this)->B::getPickRadius())
    {
        return static_cast<B*>(this)->B::getPickRadius();
    }
    template <class B>
    float base_Entity_getPickRadius(long)
    {
        return static_cast<Entity*>(this)->Entity::getPickRadius();
    }

    virtual float getPickRadius()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getPickRadius))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetPickRadius(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getPickRadius<Base>(0);
    }

    template <class B>
    auto base_Entity_getLookAngle(int) -> decltype(static_cast<B*>(this)->B::getLookAngle())
    {
        return static_cast<B*>(this)->B::getLookAngle();
    }
    template <class B>
    Vec3* base_Entity_getLookAngle(long)
    {
        return static_cast<Entity*>(this)->Entity::getLookAngle();
    }

    virtual Vec3* getLookAngle()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getLookAngle))
        {
            Vec3* result = NULL;
            if (!ModEvents::EntityGetLookAngle(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getLookAngle<Base>(0);
    }

    template <class B>
    auto base_Entity_getForward(int) -> decltype(static_cast<B*>(this)->B::getForward())
    {
        return static_cast<B*>(this)->B::getForward();
    }
    template <class B>
    Vec3* base_Entity_getForward(long)
    {
        return static_cast<Entity*>(this)->Entity::getForward();
    }

    virtual Vec3* getForward()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getForward))
        {
            Vec3* result = NULL;
            if (!ModEvents::EntityGetForward(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getForward<Base>(0);
    }

    template <class B>
    auto base_Entity_handleInsidePortal(const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::handleInsidePortal(pos))
    {
        static_cast<B*>(this)->B::handleInsidePortal(pos);
    }
    template <class B>
    void base_Entity_handleInsidePortal(const BlockPos& pos, long)
    {
        static_cast<Entity*>(this)->Entity::handleInsidePortal(pos);
    }

    virtual void handleInsidePortal(const BlockPos& pos)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_handleInsidePortal))
        {
            if (!ModEvents::EntityHandleInsidePortal(this->shared_from_this(), pos.x, pos.y, pos.z))
                return;
        }
        base_Entity_handleInsidePortal<Base>(pos, 0);
    }

    template <class B>
    auto base_Entity_getDimensionChangingDelay(int) -> decltype(static_cast<B*>(this)->B::getDimensionChangingDelay())
    {
        return static_cast<B*>(this)->B::getDimensionChangingDelay();
    }
    template <class B>
    int base_Entity_getDimensionChangingDelay(long)
    {
        return static_cast<Entity*>(this)->Entity::getDimensionChangingDelay();
    }

    virtual int getDimensionChangingDelay()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDimensionChangingDelay))
        {
            int result = 0;
            if (!ModEvents::EntityGetDimensionChangingDelay(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getDimensionChangingDelay<Base>(0);
    }

    template <class B>
    auto base_Entity_lerpMotion(double xd, double yd, double zd, int) -> decltype(static_cast<B*>(this)->B::lerpMotion(xd, yd, zd))
    {
        static_cast<B*>(this)->B::lerpMotion(xd, yd, zd);
    }
    template <class B>
    void base_Entity_lerpMotion(double xd, double yd, double zd, long)
    {
        static_cast<Entity*>(this)->Entity::lerpMotion(xd, yd, zd);
    }

    virtual void lerpMotion(double xd, double yd, double zd)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_lerpMotion))
        {
            if (!ModEvents::EntityLerpMotion(this->shared_from_this(), static_cast<float>(xd), static_cast<float>(yd), static_cast<float>(zd)))
                return;
        }
        base_Entity_lerpMotion<Base>(xd, yd, zd, 0);
    }

    template <class B>
    auto base_Entity_handleEntityEvent(unsigned char event, int data, int) -> decltype(static_cast<B*>(this)->B::handleEntityEvent(event, data))
    {
        static_cast<B*>(this)->B::handleEntityEvent(event, data);
    }
    template <class B>
    void base_Entity_handleEntityEvent(unsigned char event, int data, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::handleEntityEvent(event, data);
    }

    virtual void handleEntityEvent(unsigned char event, int data)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_handleEntityEvent))
        {
            if (!ModEvents::EntityHandleEntityEvent(this->shared_from_this(), static_cast<int>(event), data))
                return;
        }
        base_Entity_handleEntityEvent<Base>(event, data, 0);
    }

    template <class B>
    auto base_Entity_animateHurt(int) -> decltype(static_cast<B*>(this)->B::animateHurt())
    {
        static_cast<B*>(this)->B::animateHurt();
    }
    template <class B>
    void base_Entity_animateHurt(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::animateHurt();
    }

    virtual void animateHurt()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_animateHurt))
        {
            if (!ModEvents::EntityAnimateHurt(this->shared_from_this()))
                return;
        }
        base_Entity_animateHurt<Base>(0);
    }

    template <class B>
    auto base_Entity_setItemSlot(const EquipmentSlot* slot, not_null_ptr<ItemInstance> item, int) -> decltype(static_cast<B*>(this)->B::setItemSlot(slot, item))
    {
        static_cast<B*>(this)->B::setItemSlot(slot, item);
    }
    template <class B>
    void base_Entity_setItemSlot(const EquipmentSlot* slot, not_null_ptr<ItemInstance> item, long)
    {
        static_cast<Mob*>(this)->Mob::setItemSlot(slot, item);
    }

    virtual void setItemSlot(const EquipmentSlot* slot, not_null_ptr<ItemInstance> item)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setItemSlot))
        {
            if (!ModEvents::EntitySetItemSlot(this->shared_from_this(), const_cast<EquipmentSlot*>(slot), const_cast<ItemInstance*>(item.get())))
                return;
        }
        base_Entity_setItemSlot<Base>(slot, item, 0);
    }

    template <class B>
    auto base_Entity_isOnFire(int) -> decltype(static_cast<B*>(this)->B::isOnFire())
    {
        return static_cast<B*>(this)->B::isOnFire();
    }
    template <class B>
    bool base_Entity_isOnFire(long)
    {
        return static_cast<Entity*>(this)->Entity::isOnFire();
    }

    virtual bool isOnFire()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isOnFire))
        {
            bool result = false;
            if (!ModEvents::EntityIsOnFire(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isOnFire<Base>(0);
    }

    template <class B>
    auto base_Entity_isPassenger(int) -> decltype(static_cast<B*>(this)->B::isPassenger())
    {
        return static_cast<B*>(this)->B::isPassenger();
    }
    template <class B>
    bool base_Entity_isPassenger(long)
    {
        return static_cast<Entity*>(this)->Entity::isPassenger();
    }

    virtual bool isPassenger()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isPassenger))
        {
            bool result = false;
            if (!ModEvents::EntityIsPassenger(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isPassenger<Base>(0);
    }

    template <class B>
    auto base_Entity_isVehicle(int) -> decltype(static_cast<B*>(this)->B::isVehicle())
    {
        return static_cast<B*>(this)->B::isVehicle();
    }
    template <class B>
    bool base_Entity_isVehicle(long)
    {
        return static_cast<Entity*>(this)->Entity::isVehicle();
    }

    virtual bool isVehicle()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isVehicle))
        {
            bool result = false;
            if (!ModEvents::EntityIsVehicle(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isVehicle<Base>(0);
    }

    template <class B>
    auto base_Entity_isSneaking(int) -> decltype(static_cast<B*>(this)->B::isSneaking())
    {
        return static_cast<B*>(this)->B::isSneaking();
    }
    template <class B>
    bool base_Entity_isSneaking(long)
    {
        return static_cast<Entity*>(this)->Entity::isSneaking();
    }

    virtual bool isSneaking()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isSneaking))
        {
            bool result = false;
            if (!ModEvents::EntityIsSneaking(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isSneaking<Base>(0);
    }

    template <class B>
    auto base_Entity_setSneaking(bool sneaking, int) -> decltype(static_cast<B*>(this)->B::setSneaking(sneaking))
    {
        static_cast<B*>(this)->B::setSneaking(sneaking);
    }
    template <class B>
    void base_Entity_setSneaking(bool sneaking, long)
    {
        static_cast<Entity*>(this)->Entity::setSneaking(sneaking);
    }

    virtual void setSneaking(bool sneaking)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSneaking))
        {
            if (!ModEvents::EntitySetSneaking(this->shared_from_this(), sneaking))
                return;
        }
        base_Entity_setSneaking<Base>(sneaking, 0);
    }

    template <class B>
    auto base_Entity_isIdle(int) -> decltype(static_cast<B*>(this)->B::isIdle())
    {
        return static_cast<B*>(this)->B::isIdle();
    }
    template <class B>
    bool base_Entity_isIdle(long)
    {
        return static_cast<Entity*>(this)->Entity::isIdle();
    }

    virtual bool isIdle()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isIdle))
        {
            bool result = false;
            if (!ModEvents::EntityIsIdle(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isIdle<Base>(0);
    }

    template <class B>
    auto base_Entity_setIsIdle(bool idle, int) -> decltype(static_cast<B*>(this)->B::setIsIdle(idle))
    {
        static_cast<B*>(this)->B::setIsIdle(idle);
    }
    template <class B>
    void base_Entity_setIsIdle(bool idle, long)
    {
        static_cast<Entity*>(this)->Entity::setIsIdle(idle);
    }

    virtual void setIsIdle(bool idle)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setIsIdle))
        {
            if (!ModEvents::EntitySetIsIdle(this->shared_from_this(), idle))
                return;
        }
        base_Entity_setIsIdle<Base>(idle, 0);
    }

    template <class B>
    auto base_Entity_isSprinting(int) -> decltype(static_cast<B*>(this)->B::isSprinting())
    {
        return static_cast<B*>(this)->B::isSprinting();
    }
    template <class B>
    bool base_Entity_isSprinting(long)
    {
        return static_cast<Entity*>(this)->Entity::isSprinting();
    }

    virtual bool isSprinting()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isSprinting))
        {
            bool result = false;
            if (!ModEvents::EntityIsSprinting(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isSprinting<Base>(0);
    }

    template <class B>
    auto base_Entity_setSprinting(bool sprinting, int) -> decltype(static_cast<B*>(this)->B::setSprinting(sprinting))
    {
        static_cast<B*>(this)->B::setSprinting(sprinting);
    }
    template <class B>
    void base_Entity_setSprinting(bool sprinting, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setSprinting(sprinting);
    }

    virtual void setSprinting(bool sprinting)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSprinting))
        {
            if (!ModEvents::EntitySetSprinting(this->shared_from_this(), sprinting))
                return;
        }
        base_Entity_setSprinting<Base>(sprinting, 0);
    }

    template <class B>
    auto base_Entity_isSwimming(int) -> decltype(static_cast<B*>(this)->B::isSwimming())
    {
        return static_cast<B*>(this)->B::isSwimming();
    }
    template <class B>
    bool base_Entity_isSwimming(long)
    {
        return static_cast<Entity*>(this)->Entity::isSwimming();
    }

    virtual bool isSwimming()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isSwimming))
        {
            bool result = false;
            if (!ModEvents::EntityIsSwimming(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isSwimming<Base>(0);
    }

    template <class B>
    auto base_Entity_isSwimmer(int) -> decltype(static_cast<B*>(this)->B::isSwimmer())
    {
        return static_cast<B*>(this)->B::isSwimmer();
    }
    template <class B>
    bool base_Entity_isSwimmer(long)
    {
        return static_cast<Entity*>(this)->Entity::isSwimmer();
    }

    virtual bool isSwimmer()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isSwimmer))
        {
            bool result = false;
            if (!ModEvents::EntityIsSwimmer(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isSwimmer<Base>(0);
    }

    template <class B>
    auto base_Entity_startSwimming(int) -> decltype(static_cast<B*>(this)->B::startSwimming())
    {
        static_cast<B*>(this)->B::startSwimming();
    }
    template <class B>
    void base_Entity_startSwimming(long)
    {
        static_cast<Entity*>(this)->Entity::startSwimming();
    }

    virtual void startSwimming()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_startSwimming))
        {
            if (!ModEvents::EntityStartSwimming(this->shared_from_this()))
                return;
        }
        base_Entity_startSwimming<Base>(0);
    }

    template <class B>
    auto base_Entity_stopSwimming(int) -> decltype(static_cast<B*>(this)->B::stopSwimming())
    {
        static_cast<B*>(this)->B::stopSwimming();
    }
    template <class B>
    void base_Entity_stopSwimming(long)
    {
        static_cast<Entity*>(this)->Entity::stopSwimming();
    }

    virtual void stopSwimming()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_stopSwimming))
        {
            if (!ModEvents::EntityStopSwimming(this->shared_from_this()))
                return;
        }
        base_Entity_stopSwimming<Base>(0);
    }

    template <class B>
    auto base_Entity_setSwimmer(bool swimmer, int) -> decltype(static_cast<B*>(this)->B::setSwimmer(swimmer))
    {
        static_cast<B*>(this)->B::setSwimmer(swimmer);
    }
    template <class B>
    void base_Entity_setSwimmer(bool swimmer, long)
    {
        static_cast<Entity*>(this)->Entity::setSwimmer(swimmer);
    }

    virtual void setSwimmer(bool swimmer)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSwimmer))
        {
            if (!ModEvents::EntitySetSwimmer(this->shared_from_this(), swimmer))
                return;
        }
        base_Entity_setSwimmer<Base>(swimmer, 0);
    }

    template <class B>
    auto base_Entity_setDamageNearbyMobs(bool damageNearbyMobs, int) -> decltype(static_cast<B*>(this)->B::setDamageNearbyMobs(damageNearbyMobs))
    {
        static_cast<B*>(this)->B::setDamageNearbyMobs(damageNearbyMobs);
    }
    template <class B>
    void base_Entity_setDamageNearbyMobs(bool damageNearbyMobs, long)
    {
        static_cast<Entity*>(this)->Entity::setDamageNearbyMobs(damageNearbyMobs);
    }

    virtual void setDamageNearbyMobs(bool damageNearbyMobs)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setDamageNearbyMobs))
        {
            if (!ModEvents::EntitySetDamageNearbyMobs(this->shared_from_this(), damageNearbyMobs))
                return;
        }
        base_Entity_setDamageNearbyMobs<Base>(damageNearbyMobs, 0);
    }

    template <class B>
    auto base_Entity_getDamageNearbyMobs(int) -> decltype(static_cast<B*>(this)->B::getDamageNearbyMobs())
    {
        return static_cast<B*>(this)->B::getDamageNearbyMobs();
    }
    template <class B>
    bool base_Entity_getDamageNearbyMobs(long)
    {
        return static_cast<Entity*>(this)->Entity::getDamageNearbyMobs();
    }

    virtual bool getDamageNearbyMobs()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDamageNearbyMobs))
        {
            bool result = false;
            if (!ModEvents::EntityGetDamageNearbyMobs(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getDamageNearbyMobs<Base>(0);
    }

    template <class B>
    auto base_Entity_isGlowing(int) -> decltype(static_cast<B*>(this)->B::isGlowing())
    {
        return static_cast<B*>(this)->B::isGlowing();
    }
    template <class B>
    bool base_Entity_isGlowing(long)
    {
        return static_cast<Entity*>(this)->Entity::isGlowing();
    }

    virtual bool isGlowing()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isGlowing))
        {
            bool result = false;
            if (!ModEvents::EntityIsGlowing(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isGlowing<Base>(0);
    }

    template <class B>
    auto base_Entity_setGlowing(bool glowing, int) -> decltype(static_cast<B*>(this)->B::setGlowing(glowing))
    {
        static_cast<B*>(this)->B::setGlowing(glowing);
    }
    template <class B>
    void base_Entity_setGlowing(bool glowing, long)
    {
        static_cast<Entity*>(this)->Entity::setGlowing(glowing);
    }

    virtual void setGlowing(bool glowing)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setGlowing))
        {
            if (!ModEvents::EntitySetGlowing(this->shared_from_this(), glowing))
                return;
        }
        base_Entity_setGlowing<Base>(glowing, 0);
    }

    template <class B>
    auto base_Entity_isInvisible(int) -> decltype(static_cast<B*>(this)->B::isInvisible())
    {
        return static_cast<B*>(this)->B::isInvisible();
    }
    template <class B>
    bool base_Entity_isInvisible(long)
    {
        return static_cast<Entity*>(this)->Entity::isInvisible();
    }

    virtual bool isInvisible()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInvisible))
        {
            bool result = false;
            if (!ModEvents::EntityIsInvisible(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isInvisible<Base>(0);
    }

    template <class B>
    auto base_Entity_isInvisibleTo(boost::shared_ptr<Player> player, int) -> decltype(static_cast<B*>(this)->B::isInvisibleTo(player))
    {
        return static_cast<B*>(this)->B::isInvisibleTo(player);
    }
    template <class B>
    bool base_Entity_isInvisibleTo(boost::shared_ptr<Player> player, long)
    {
        return static_cast<Entity*>(this)->Entity::isInvisibleTo(player);
    }

    virtual bool isInvisibleTo(boost::shared_ptr<Player> player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInvisibleTo))
        {
            bool result = false;
            if (!ModEvents::EntityIsInvisibleTo(this->shared_from_this(), player, result))
                return result;
        }
        return base_Entity_isInvisibleTo<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_getTeam(int) -> decltype(static_cast<B*>(this)->B::getTeam())
    {
        return static_cast<B*>(this)->B::getTeam();
    }
    template <class B>
    Team* base_Entity_getTeam(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getTeam();
    }

    virtual Team* getTeam()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getTeam))
        {
            Team* result = NULL;
            if (!ModEvents::EntityGetTeam(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getTeam<Base>(0);
    }

    template <class B>
    auto base_Entity_isAlliedTo(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::isAlliedTo(entity))
    {
        return static_cast<B*>(this)->B::isAlliedTo(entity);
    }
    template <class B>
    bool base_Entity_isAlliedTo(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Entity*>(this)->Entity::isAlliedTo(entity);
    }

    virtual bool isAlliedTo(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isAlliedTo))
        {
            bool result = false;
            if (!ModEvents::EntityIsAlliedTo(this->shared_from_this(), entity, result))
                return result;
        }
        return base_Entity_isAlliedTo<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_isAlliedTo_2(Team* team, int) -> decltype(static_cast<B*>(this)->B::isAlliedTo(team))
    {
        return static_cast<B*>(this)->B::isAlliedTo(team);
    }
    template <class B>
    bool base_Entity_isAlliedTo_2(Team* team, long)
    {
        return static_cast<Entity*>(this)->Entity::isAlliedTo(team);
    }

    virtual bool isAlliedTo(Team* team)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isAlliedTo_2))
        {
            bool result = false;
            if (!ModEvents::EntityIsAlliedTo2(this->shared_from_this(), const_cast<Team*>(team), result))
                return result;
        }
        return base_Entity_isAlliedTo_2<Base>(team, 0);
    }

    template <class B>
    auto base_Entity_setInvisible(bool invisible, int) -> decltype(static_cast<B*>(this)->B::setInvisible(invisible))
    {
        static_cast<B*>(this)->B::setInvisible(invisible);
    }
    template <class B>
    void base_Entity_setInvisible(bool invisible, long)
    {
        static_cast<Entity*>(this)->Entity::setInvisible(invisible);
    }

    virtual void setInvisible(bool invisible)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setInvisible))
        {
            if (!ModEvents::EntitySetInvisible(this->shared_from_this(), invisible))
                return;
        }
        base_Entity_setInvisible<Base>(invisible, 0);
    }

    template <class B>
    auto base_Entity_ShouldRenderShadow(int) -> decltype(static_cast<B*>(this)->B::ShouldRenderShadow())
    {
        return static_cast<B*>(this)->B::ShouldRenderShadow();
    }
    template <class B>
    bool base_Entity_ShouldRenderShadow(long)
    {
        return static_cast<Entity*>(this)->Entity::ShouldRenderShadow();
    }

    virtual bool ShouldRenderShadow()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_ShouldRenderShadow))
        {
            bool result = false;
            if (!ModEvents::EntityShouldRenderShadow(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_ShouldRenderShadow<Base>(0);
    }

    template <class B>
    auto base_Entity_canFly(int) -> decltype(static_cast<B*>(this)->B::canFly())
    {
        return static_cast<B*>(this)->B::canFly();
    }
    template <class B>
    bool base_Entity_canFly(long)
    {
        return static_cast<Entity*>(this)->Entity::canFly();
    }

    virtual bool canFly()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canFly))
        {
            bool result = false;
            if (!ModEvents::EntityCanFly(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_canFly<Base>(0);
    }

    template <class B>
    auto base_Entity_getAttachPos(Entity::EEntityLocation location, float partialTicks, int) -> decltype(static_cast<B*>(this)->B::getAttachPos(location, partialTicks))
    {
        return static_cast<B*>(this)->B::getAttachPos(location, partialTicks);
    }
    template <class B>
    Vec3* base_Entity_getAttachPos(Entity::EEntityLocation location, float partialTicks, long)
    {
        return static_cast<Entity*>(this)->Entity::getAttachPos(location, partialTicks);
    }

    virtual Vec3* getAttachPos(Entity::EEntityLocation location, float partialTicks)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAttachPos))
        {
            Vec3* result = NULL;
            if (!ModEvents::EntityGetAttachPos(this->shared_from_this(), static_cast<int>(location), partialTicks, result))
                return result;
        }
        return base_Entity_getAttachPos<Base>(location, partialTicks, 0);
    }

    template <class B>
    auto base_Entity_getMaxAirSupply(int) -> decltype(static_cast<B*>(this)->B::getMaxAirSupply())
    {
        return static_cast<B*>(this)->B::getMaxAirSupply();
    }
    template <class B>
    int base_Entity_getMaxAirSupply(long)
    {
        return static_cast<Entity*>(this)->Entity::getMaxAirSupply();
    }

    virtual int getMaxAirSupply()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMaxAirSupply))
        {
            int result = 0;
            if (!ModEvents::EntityGetMaxAirSupply(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMaxAirSupply<Base>(0);
    }

    template <class B>
    auto base_Entity_setAirSupply(int airSupply, int) -> decltype(static_cast<B*>(this)->B::setAirSupply(airSupply))
    {
        static_cast<B*>(this)->B::setAirSupply(airSupply);
    }
    template <class B>
    void base_Entity_setAirSupply(int airSupply, long)
    {
        static_cast<Entity*>(this)->Entity::setAirSupply(airSupply);
    }

    virtual void setAirSupply(int airSupply)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setAirSupply))
        {
            if (!ModEvents::EntitySetAirSupply(this->shared_from_this(), airSupply))
                return;
        }
        base_Entity_setAirSupply<Base>(airSupply, 0);
    }

    template <class B>
    auto base_Entity_thunderHit(const LightningBolt* lightningBolt, int) -> decltype(static_cast<B*>(this)->B::thunderHit(lightningBolt))
    {
        static_cast<B*>(this)->B::thunderHit(lightningBolt);
    }
    template <class B>
    void base_Entity_thunderHit(const LightningBolt* lightningBolt, long)
    {
        static_cast<Entity*>(this)->Entity::thunderHit(lightningBolt);
    }

    virtual void thunderHit(const LightningBolt* lightningBolt)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_thunderHit))
        {
            if (!ModEvents::EntityThunderHit(this->shared_from_this(), const_cast<LightningBolt*>(lightningBolt)))
                return;
        }
        base_Entity_thunderHit<Base>(lightningBolt, 0);
    }

    template <class B>
    auto base_Entity_onAboveBubbleCol(bool down, int) -> decltype(static_cast<B*>(this)->B::onAboveBubbleCol(down))
    {
        static_cast<B*>(this)->B::onAboveBubbleCol(down);
    }
    template <class B>
    void base_Entity_onAboveBubbleCol(bool down, long)
    {
        static_cast<Entity*>(this)->Entity::onAboveBubbleCol(down);
    }

    virtual void onAboveBubbleCol(bool down)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onAboveBubbleCol))
        {
            if (!ModEvents::EntityOnAboveBubbleCol(this->shared_from_this(), down))
                return;
        }
        base_Entity_onAboveBubbleCol<Base>(down, 0);
    }

    template <class B>
    auto base_Entity_killed(boost::shared_ptr<LivingEntity> killed, int) -> decltype(static_cast<B*>(this)->B::killed(killed))
    {
        static_cast<B*>(this)->B::killed(killed);
    }
    template <class B>
    void base_Entity_killed(boost::shared_ptr<LivingEntity> killed, long)
    {
        static_cast<Entity*>(this)->Entity::killed(killed);
    }

    virtual void killed(boost::shared_ptr<LivingEntity> killed)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_killed))
        {
            if (!ModEvents::EntityKilled(this->shared_from_this(), killed))
                return;
        }
        base_Entity_killed<Base>(killed, 0);
    }

    template <class B>
    auto base_Entity_checkInBlock(double x, double y, double z, int) -> decltype(static_cast<B*>(this)->B::checkInBlock(x, y, z))
    {
        return static_cast<B*>(this)->B::checkInBlock(x, y, z);
    }
    template <class B>
    bool base_Entity_checkInBlock(double x, double y, double z, long)
    {
        return static_cast<Entity*>(this)->Entity::checkInBlock(x, y, z);
    }

    virtual bool checkInBlock(double x, double y, double z)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_checkInBlock))
        {
            bool result = false;
            if (!ModEvents::EntityCheckInBlock(this->shared_from_this(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), result))
                return result;
        }
        return base_Entity_checkInBlock<Base>(x, y, z, 0);
    }

    template <class B>
    auto base_Entity_spawnEatParticles(const ItemInstance& itemInstance, int count, int) -> decltype(static_cast<B*>(this)->B::spawnEatParticles(itemInstance, count))
    {
        static_cast<B*>(this)->B::spawnEatParticles(itemInstance, count);
    }
    template <class B>
    void base_Entity_spawnEatParticles(const ItemInstance& itemInstance, int count, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::spawnEatParticles(itemInstance, count);
    }

    virtual void spawnEatParticles(const ItemInstance& itemInstance, int count)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_spawnEatParticles))
        {
            if (!ModEvents::EntitySpawnEatParticles(this->shared_from_this(), const_cast<ItemInstance*>(&itemInstance), count))
                return;
        }
        base_Entity_spawnEatParticles<Base>(itemInstance, count, 0);
    }

    template <class B>
    auto base_Entity_makeStuckInWeb(int) -> decltype(static_cast<B*>(this)->B::makeStuckInWeb())
    {
        static_cast<B*>(this)->B::makeStuckInWeb();
    }
    template <class B>
    void base_Entity_makeStuckInWeb(long)
    {
        static_cast<Entity*>(this)->Entity::makeStuckInWeb();
    }

    virtual void makeStuckInWeb()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_makeStuckInWeb))
        {
            if (!ModEvents::EntityMakeStuckInWeb(this->shared_from_this()))
                return;
        }
        base_Entity_makeStuckInWeb<Base>(0);
    }

    template <class B>
    auto base_Entity_getName(int) -> decltype(static_cast<B*>(this)->B::getName())
    {
        return static_cast<B*>(this)->B::getName();
    }
    template <class B>
    std::wstring base_Entity_getName(long)
    {
        return static_cast<Mob*>(this)->Mob::getName();
    }

    virtual std::wstring getName()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::EntityGetName(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getName<Base>(0);
    }

    template <class B>
    auto base_Entity_isInvulnerableTo(const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::isInvulnerableTo(source))
    {
        return static_cast<B*>(this)->B::isInvulnerableTo(source);
    }
    template <class B>
    bool base_Entity_isInvulnerableTo(const DamageSource* source, long)
    {
        return static_cast<Entity*>(this)->Entity::isInvulnerableTo(source);
    }

    virtual bool isInvulnerableTo(const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInvulnerableTo))
        {
            bool result = false;
            if (!ModEvents::EntityIsInvulnerableTo(this->shared_from_this(), const_cast<DamageSource*>(source), result))
                return result;
        }
        return base_Entity_isInvulnerableTo<Base>(source, 0);
    }

    template <class B>
    auto base_Entity_isInvulnerable(int) -> decltype(static_cast<B*>(this)->B::isInvulnerable())
    {
        return static_cast<B*>(this)->B::isInvulnerable();
    }
    template <class B>
    bool base_Entity_isInvulnerable(long)
    {
        return static_cast<Entity*>(this)->Entity::isInvulnerable();
    }

    virtual bool isInvulnerable()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInvulnerable))
        {
            bool result = false;
            if (!ModEvents::EntityIsInvulnerable(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isInvulnerable<Base>(0);
    }

    template <class B>
    auto base_Entity_setInvulnerable(bool invulnerable, int) -> decltype(static_cast<B*>(this)->B::setInvulnerable(invulnerable))
    {
        static_cast<B*>(this)->B::setInvulnerable(invulnerable);
    }
    template <class B>
    void base_Entity_setInvulnerable(bool invulnerable, long)
    {
        static_cast<Entity*>(this)->Entity::setInvulnerable(invulnerable);
    }

    virtual void setInvulnerable(bool invulnerable)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setInvulnerable))
        {
            if (!ModEvents::EntitySetInvulnerable(this->shared_from_this(), invulnerable))
                return;
        }
        base_Entity_setInvulnerable<Base>(invulnerable, 0);
    }

    template <class B>
    auto base_Entity_is(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::is(entity))
    {
        return static_cast<B*>(this)->B::is(entity);
    }
    template <class B>
    bool base_Entity_is(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Entity*>(this)->Entity::is(entity);
    }

    virtual bool is(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_is))
        {
            bool result = false;
            if (!ModEvents::EntityIs(this->shared_from_this(), entity, result))
                return result;
        }
        return base_Entity_is<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_getYHeadRot(int) -> decltype(static_cast<B*>(this)->B::getYHeadRot())
    {
        return static_cast<B*>(this)->B::getYHeadRot();
    }
    template <class B>
    float base_Entity_getYHeadRot(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getYHeadRot();
    }

    virtual float getYHeadRot()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getYHeadRot))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetYHeadRot(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getYHeadRot<Base>(0);
    }

    template <class B>
    auto base_Entity_setYHeadRot(float yHeadRot, int) -> decltype(static_cast<B*>(this)->B::setYHeadRot(yHeadRot))
    {
        static_cast<B*>(this)->B::setYHeadRot(yHeadRot);
    }
    template <class B>
    void base_Entity_setYHeadRot(float yHeadRot, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setYHeadRot(yHeadRot);
    }

    virtual void setYHeadRot(float yHeadRot)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setYHeadRot))
        {
            if (!ModEvents::EntitySetYHeadRot(this->shared_from_this(), yHeadRot))
                return;
        }
        base_Entity_setYHeadRot<Base>(yHeadRot, 0);
    }

    template <class B>
    auto base_Entity_setYBodyRot(float yBodyRot, int) -> decltype(static_cast<B*>(this)->B::setYBodyRot(yBodyRot))
    {
        static_cast<B*>(this)->B::setYBodyRot(yBodyRot);
    }
    template <class B>
    void base_Entity_setYBodyRot(float yBodyRot, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setYBodyRot(yBodyRot);
    }

    virtual void setYBodyRot(float yBodyRot)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setYBodyRot))
        {
            if (!ModEvents::EntitySetYBodyRot(this->shared_from_this(), yBodyRot))
                return;
        }
        base_Entity_setYBodyRot<Base>(yBodyRot, 0);
    }

    template <class B>
    auto base_Entity_isAttackable(int) -> decltype(static_cast<B*>(this)->B::isAttackable())
    {
        return static_cast<B*>(this)->B::isAttackable();
    }
    template <class B>
    bool base_Entity_isAttackable(long)
    {
        return static_cast<Entity*>(this)->Entity::isAttackable();
    }

    virtual bool isAttackable()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isAttackable))
        {
            bool result = false;
            if (!ModEvents::EntityIsAttackable(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isAttackable<Base>(0);
    }

    template <class B>
    auto base_Entity_skipAttackInteraction(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::skipAttackInteraction(entity))
    {
        return static_cast<B*>(this)->B::skipAttackInteraction(entity);
    }
    template <class B>
    bool base_Entity_skipAttackInteraction(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Entity*>(this)->Entity::skipAttackInteraction(entity);
    }

    virtual bool skipAttackInteraction(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_skipAttackInteraction))
        {
            bool result = false;
            if (!ModEvents::EntitySkipAttackInteraction(this->shared_from_this(), entity, result))
                return result;
        }
        return base_Entity_skipAttackInteraction<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_copyPosition(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::copyPosition(entity))
    {
        static_cast<B*>(this)->B::copyPosition(entity);
    }
    template <class B>
    void base_Entity_copyPosition(boost::shared_ptr<Entity> entity, long)
    {
        static_cast<Entity*>(this)->Entity::copyPosition(entity);
    }

    virtual void copyPosition(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_copyPosition))
        {
            if (!ModEvents::EntityCopyPosition(this->shared_from_this(), entity))
                return;
        }
        base_Entity_copyPosition<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_canChangeDimensions(int) -> decltype(static_cast<B*>(this)->B::canChangeDimensions())
    {
        return static_cast<B*>(this)->B::canChangeDimensions();
    }
    template <class B>
    bool base_Entity_canChangeDimensions(long)
    {
        return static_cast<Entity*>(this)->Entity::canChangeDimensions();
    }

    virtual bool canChangeDimensions()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canChangeDimensions))
        {
            bool result = false;
            if (!ModEvents::EntityCanChangeDimensions(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_canChangeDimensions<Base>(0);
    }

    template <class B>
    auto base_Entity_getBlockExplosionResistance(Explosion* explosion, Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getBlockExplosionResistance(explosion, level, pos))
    {
        return static_cast<B*>(this)->B::getBlockExplosionResistance(explosion, level, pos);
    }
    template <class B>
    float base_Entity_getBlockExplosionResistance(Explosion* explosion, Level* level, const BlockPos& pos, long)
    {
        return static_cast<Entity*>(this)->Entity::getBlockExplosionResistance(explosion, level, pos);
    }

    virtual float getBlockExplosionResistance(Explosion* explosion, Level* level, const BlockPos& pos)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getBlockExplosionResistance))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetBlockExplosionResistance(this->shared_from_this(), const_cast<Explosion*>(explosion), level, pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Entity_getBlockExplosionResistance<Base>(explosion, level, pos, 0);
    }

    template <class B>
    auto base_Entity_shouldBlockExplode(Explosion* explosion, Level* level, const BlockPos& pos, float power, int) -> decltype(static_cast<B*>(this)->B::shouldBlockExplode(explosion, level, pos, power))
    {
        return static_cast<B*>(this)->B::shouldBlockExplode(explosion, level, pos, power);
    }
    template <class B>
    bool base_Entity_shouldBlockExplode(Explosion* explosion, Level* level, const BlockPos& pos, float power, long)
    {
        return static_cast<Entity*>(this)->Entity::shouldBlockExplode(explosion, level, pos, power);
    }

    virtual bool shouldBlockExplode(Explosion* explosion, Level* level, const BlockPos& pos, float power)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_shouldBlockExplode))
        {
            bool result = false;
            if (!ModEvents::EntityShouldBlockExplode(this->shared_from_this(), const_cast<Explosion*>(explosion), level, pos.x, pos.y, pos.z, power, result))
                return result;
        }
        return base_Entity_shouldBlockExplode<Base>(explosion, level, pos, power, 0);
    }

    template <class B>
    auto base_Entity_getMaxFallDistance(int) -> decltype(static_cast<B*>(this)->B::getMaxFallDistance())
    {
        return static_cast<B*>(this)->B::getMaxFallDistance();
    }
    template <class B>
    int base_Entity_getMaxFallDistance(long)
    {
        return static_cast<Entity*>(this)->Entity::getMaxFallDistance();
    }

    virtual int getMaxFallDistance()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMaxFallDistance))
        {
            int result = 0;
            if (!ModEvents::EntityGetMaxFallDistance(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMaxFallDistance<Base>(0);
    }

    template <class B>
    auto base_Entity_getPortalEntranceOffset(int) -> decltype(static_cast<B*>(this)->B::getPortalEntranceOffset())
    {
        return static_cast<B*>(this)->B::getPortalEntranceOffset();
    }
    template <class B>
    Vec3* base_Entity_getPortalEntranceOffset(long)
    {
        return static_cast<Entity*>(this)->Entity::getPortalEntranceOffset();
    }

    virtual Vec3* getPortalEntranceOffset()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getPortalEntranceOffset))
        {
            Vec3* result = NULL;
            if (!ModEvents::EntityGetPortalEntranceOffset(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getPortalEntranceOffset<Base>(0);
    }

    template <class B>
    auto base_Entity_getPortalEntranceForwards(int) -> decltype(static_cast<B*>(this)->B::getPortalEntranceForwards())
    {
        return static_cast<B*>(this)->B::getPortalEntranceForwards();
    }
    template <class B>
    const Direction* base_Entity_getPortalEntranceForwards(long)
    {
        return static_cast<Entity*>(this)->Entity::getPortalEntranceForwards();
    }

    virtual const Direction* getPortalEntranceForwards()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getPortalEntranceForwards))
        {
            Direction* result = NULL;
            if (!ModEvents::EntityGetPortalEntranceForwards(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getPortalEntranceForwards<Base>(0);
    }

    template <class B>
    auto base_Entity_isIgnoringBlockTriggers(int) -> decltype(static_cast<B*>(this)->B::isIgnoringBlockTriggers())
    {
        return static_cast<B*>(this)->B::isIgnoringBlockTriggers();
    }
    template <class B>
    bool base_Entity_isIgnoringBlockTriggers(long)
    {
        return static_cast<Entity*>(this)->Entity::isIgnoringBlockTriggers();
    }

    virtual bool isIgnoringBlockTriggers()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isIgnoringBlockTriggers))
        {
            bool result = false;
            if (!ModEvents::EntityIsIgnoringBlockTriggers(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isIgnoringBlockTriggers<Base>(0);
    }

    template <class B>
    auto base_Entity_displayFireAnimation(int) -> decltype(static_cast<B*>(this)->B::displayFireAnimation())
    {
        return static_cast<B*>(this)->B::displayFireAnimation();
    }
    template <class B>
    bool base_Entity_displayFireAnimation(long)
    {
        return static_cast<Entity*>(this)->Entity::displayFireAnimation();
    }

    virtual bool displayFireAnimation()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_displayFireAnimation))
        {
            bool result = false;
            if (!ModEvents::EntityDisplayFireAnimation(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_displayFireAnimation<Base>(0);
    }

    template <class B>
    auto base_Entity_setUUID(const std::wstring& uuid, int) -> decltype(static_cast<B*>(this)->B::setUUID(uuid))
    {
        static_cast<B*>(this)->B::setUUID(uuid);
    }
    template <class B>
    void base_Entity_setUUID(const std::wstring& uuid, long)
    {
        static_cast<Entity*>(this)->Entity::setUUID(uuid);
    }

    virtual void setUUID(const std::wstring& uuid)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setUUID))
        {
            if (!ModEvents::EntitySetUUID(this->shared_from_this(), uuid))
                return;
        }
        base_Entity_setUUID<Base>(uuid, 0);
    }

    template <class B>
    auto base_Entity_getUUID(int) -> decltype(static_cast<B*>(this)->B::getUUID())
    {
        return static_cast<B*>(this)->B::getUUID();
    }
    template <class B>
    std::wstring base_Entity_getUUID(long)
    {
        return static_cast<Entity*>(this)->Entity::getUUID();
    }

    virtual std::wstring getUUID()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getUUID))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::EntityGetUUID(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getUUID<Base>(0);
    }

    template <class B>
    auto base_Entity_getStringUUID(int) -> decltype(static_cast<B*>(this)->B::getStringUUID())
    {
        return static_cast<B*>(this)->B::getStringUUID();
    }
    template <class B>
    std::wstring base_Entity_getStringUUID(long)
    {
        return static_cast<Entity*>(this)->Entity::getStringUUID();
    }

    virtual std::wstring getStringUUID()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getStringUUID))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::EntityGetStringUUID(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getStringUUID<Base>(0);
    }

    template <class B>
    auto base_Entity_isPushedByWater(int) -> decltype(static_cast<B*>(this)->B::isPushedByWater())
    {
        return static_cast<B*>(this)->B::isPushedByWater();
    }
    template <class B>
    bool base_Entity_isPushedByWater(long)
    {
        return static_cast<Entity*>(this)->Entity::isPushedByWater();
    }

    virtual bool isPushedByWater()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isPushedByWater))
        {
            bool result = false;
            if (!ModEvents::EntityIsPushedByWater(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isPushedByWater<Base>(0);
    }

    template <class B>
    auto base_Entity_setViewScale(double viewScale, int) -> decltype(static_cast<B*>(this)->B::setViewScale(viewScale))
    {
        static_cast<B*>(this)->B::setViewScale(viewScale);
    }
    template <class B>
    void base_Entity_setViewScale(double viewScale, long)
    {
        static_cast<Entity*>(this)->Entity::setViewScale(viewScale);
    }

    virtual void setViewScale(double viewScale)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setViewScale))
        {
            if (!ModEvents::EntitySetViewScale(this->shared_from_this(), static_cast<float>(viewScale)))
                return;
        }
        base_Entity_setViewScale<Base>(viewScale, 0);
    }

    template <class B>
    auto base_Entity_getDisplayName(int) -> decltype(static_cast<B*>(this)->B::getDisplayName())
    {
        return static_cast<B*>(this)->B::getDisplayName();
    }
    template <class B>
    std::wstring base_Entity_getDisplayName(long)
    {
        return static_cast<Entity*>(this)->Entity::getDisplayName();
    }

    virtual std::wstring getDisplayName()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDisplayName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::EntityGetDisplayName(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getDisplayName<Base>(0);
    }

    template <class B>
    auto base_Entity_getNetworkName(int) -> decltype(static_cast<B*>(this)->B::getNetworkName())
    {
        return static_cast<B*>(this)->B::getNetworkName();
    }
    template <class B>
    std::wstring base_Entity_getNetworkName(long)
    {
        return static_cast<Entity*>(this)->Entity::getNetworkName();
    }

    virtual std::wstring getNetworkName()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getNetworkName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::EntityGetNetworkName(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getNetworkName<Base>(0);
    }

    template <class B>
    auto base_Entity_setCustomName(const std::wstring& customName, const std::wstring& ownerUUID, bool namedByRestrictedPlayer, int) -> decltype(static_cast<B*>(this)->B::setCustomName(customName, ownerUUID, namedByRestrictedPlayer))
    {
        static_cast<B*>(this)->B::setCustomName(customName, ownerUUID, namedByRestrictedPlayer);
    }
    template <class B>
    void base_Entity_setCustomName(const std::wstring& customName, const std::wstring& ownerUUID, bool namedByRestrictedPlayer, long)
    {
        static_cast<Entity*>(this)->Entity::setCustomName(customName, ownerUUID, namedByRestrictedPlayer);
    }

    virtual void setCustomName(const std::wstring& customName, const std::wstring& ownerUUID, bool namedByRestrictedPlayer)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setCustomName))
        {
            if (!ModEvents::EntitySetCustomName(this->shared_from_this(), customName, ownerUUID, namedByRestrictedPlayer))
                return;
        }
        base_Entity_setCustomName<Base>(customName, ownerUUID, namedByRestrictedPlayer, 0);
    }

    template <class B>
    auto base_Entity_getCustomName(int) -> decltype(static_cast<B*>(this)->B::getCustomName())
    {
        return static_cast<B*>(this)->B::getCustomName();
    }
    template <class B>
    std::wstring base_Entity_getCustomName(long)
    {
        return static_cast<Entity*>(this)->Entity::getCustomName();
    }

    virtual std::wstring getCustomName()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getCustomName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::EntityGetCustomName(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getCustomName<Base>(0);
    }

    template <class B>
    auto base_Entity_hasCustomName(int) -> decltype(static_cast<B*>(this)->B::hasCustomName())
    {
        return static_cast<B*>(this)->B::hasCustomName();
    }
    template <class B>
    bool base_Entity_hasCustomName(long)
    {
        return static_cast<Entity*>(this)->Entity::hasCustomName();
    }

    virtual bool hasCustomName()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasCustomName))
        {
            bool result = false;
            if (!ModEvents::EntityHasCustomName(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_hasCustomName<Base>(0);
    }

    template <class B>
    auto base_Entity_setCustomNameVisible(bool customNameVisible, int) -> decltype(static_cast<B*>(this)->B::setCustomNameVisible(customNameVisible))
    {
        static_cast<B*>(this)->B::setCustomNameVisible(customNameVisible);
    }
    template <class B>
    void base_Entity_setCustomNameVisible(bool customNameVisible, long)
    {
        static_cast<Entity*>(this)->Entity::setCustomNameVisible(customNameVisible);
    }

    virtual void setCustomNameVisible(bool customNameVisible)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setCustomNameVisible))
        {
            if (!ModEvents::EntitySetCustomNameVisible(this->shared_from_this(), customNameVisible))
                return;
        }
        base_Entity_setCustomNameVisible<Base>(customNameVisible, 0);
    }

    template <class B>
    auto base_Entity_isCustomNameVisible(int) -> decltype(static_cast<B*>(this)->B::isCustomNameVisible())
    {
        return static_cast<B*>(this)->B::isCustomNameVisible();
    }
    template <class B>
    bool base_Entity_isCustomNameVisible(long)
    {
        return static_cast<Entity*>(this)->Entity::isCustomNameVisible();
    }

    virtual bool isCustomNameVisible()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isCustomNameVisible))
        {
            bool result = false;
            if (!ModEvents::EntityIsCustomNameVisible(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isCustomNameVisible<Base>(0);
    }

    template <class B>
    auto base_Entity_teleportTo(double x, double y, double z, int) -> decltype(static_cast<B*>(this)->B::teleportTo(x, y, z))
    {
        static_cast<B*>(this)->B::teleportTo(x, y, z);
    }
    template <class B>
    void base_Entity_teleportTo(double x, double y, double z, long)
    {
        static_cast<Entity*>(this)->Entity::teleportTo(x, y, z);
    }

    virtual void teleportTo(double x, double y, double z)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_teleportTo))
        {
            if (!ModEvents::EntityTeleportTo(this->shared_from_this(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)))
                return;
        }
        base_Entity_teleportTo<Base>(x, y, z, 0);
    }

    template <class B>
    auto base_Entity_shouldShowName(int) -> decltype(static_cast<B*>(this)->B::shouldShowName())
    {
        return static_cast<B*>(this)->B::shouldShowName();
    }
    template <class B>
    bool base_Entity_shouldShowName(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::shouldShowName();
    }

    virtual bool shouldShowName()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_shouldShowName))
        {
            bool result = false;
            if (!ModEvents::EntityShouldShowName(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_shouldShowName<Base>(0);
    }

    template <class B>
    auto base_Entity_onSyncedDataUpdated(const EntityDataAccessor_Base* accessor, int) -> decltype(static_cast<B*>(this)->B::onSyncedDataUpdated(accessor))
    {
        static_cast<B*>(this)->B::onSyncedDataUpdated(accessor);
    }
    template <class B>
    void base_Entity_onSyncedDataUpdated(const EntityDataAccessor_Base* accessor, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::onSyncedDataUpdated(accessor);
    }

    virtual void onSyncedDataUpdated(const EntityDataAccessor_Base* accessor)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onSyncedDataUpdated))
        {
            if (!ModEvents::EntityOnSyncedDataUpdated(this->shared_from_this(), const_cast<EntityDataAccessor_Base*>(accessor)))
                return;
        }
        base_Entity_onSyncedDataUpdated<Base>(accessor, 0);
    }

    template <class B>
    auto base_Entity_getDirection(int) -> decltype(static_cast<B*>(this)->B::getDirection())
    {
        return static_cast<B*>(this)->B::getDirection();
    }
    template <class B>
    const Direction* base_Entity_getDirection(long)
    {
        return static_cast<Entity*>(this)->Entity::getDirection();
    }

    virtual const Direction* getDirection()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDirection))
        {
            Direction* result = NULL;
            if (!ModEvents::EntityGetDirection(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getDirection<Base>(0);
    }

    template <class B>
    auto base_Entity_getMotionDirection(int) -> decltype(static_cast<B*>(this)->B::getMotionDirection())
    {
        return static_cast<B*>(this)->B::getMotionDirection();
    }
    template <class B>
    const Direction* base_Entity_getMotionDirection(long)
    {
        return static_cast<Entity*>(this)->Entity::getMotionDirection();
    }

    virtual const Direction* getMotionDirection()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMotionDirection))
        {
            Direction* result = NULL;
            if (!ModEvents::EntityGetMotionDirection(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMotionDirection<Base>(0);
    }

    template <class B>
    auto base_Entity_broadcastToPlayer(boost::shared_ptr<ServerPlayer> player, int) -> decltype(static_cast<B*>(this)->B::broadcastToPlayer(player))
    {
        return static_cast<B*>(this)->B::broadcastToPlayer(player);
    }
    template <class B>
    bool base_Entity_broadcastToPlayer(boost::shared_ptr<ServerPlayer> player, long)
    {
        return static_cast<Entity*>(this)->Entity::broadcastToPlayer(player);
    }

    virtual bool broadcastToPlayer(boost::shared_ptr<ServerPlayer> player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_broadcastToPlayer))
        {
            bool result = false;
            if (!ModEvents::EntityBroadcastToPlayer(this->shared_from_this(), player, result))
                return result;
        }
        return base_Entity_broadcastToPlayer<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_getBoundingBoxForCulling(int) -> decltype(static_cast<B*>(this)->B::getBoundingBoxForCulling())
    {
        return static_cast<B*>(this)->B::getBoundingBoxForCulling();
    }
    template <class B>
    AABB* base_Entity_getBoundingBoxForCulling(long)
    {
        return static_cast<Entity*>(this)->Entity::getBoundingBoxForCulling();
    }

    virtual AABB* getBoundingBoxForCulling()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getBoundingBoxForCulling))
        {
            AABB* result = NULL;
            if (!ModEvents::EntityGetBoundingBoxForCulling(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getBoundingBoxForCulling<Base>(0);
    }

    template <class B>
    auto base_Entity_setBoundingBox(AABB* boundingBox, int) -> decltype(static_cast<B*>(this)->B::setBoundingBox(boundingBox))
    {
        static_cast<B*>(this)->B::setBoundingBox(boundingBox);
    }
    template <class B>
    void base_Entity_setBoundingBox(AABB* boundingBox, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setBoundingBox(boundingBox);
    }

    virtual void setBoundingBox(AABB* boundingBox)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setBoundingBox))
        {
            if (!ModEvents::EntitySetBoundingBox(this->shared_from_this(), const_cast<AABB*>(boundingBox)))
                return;
        }
        base_Entity_setBoundingBox<Base>(boundingBox, 0);
    }

    template <class B>
    auto base_Entity_getEyeHeight(int) -> decltype(static_cast<B*>(this)->B::getEyeHeight())
    {
        return static_cast<B*>(this)->B::getEyeHeight();
    }
    template <class B>
    float base_Entity_getEyeHeight(long)
    {
        return static_cast<Entity*>(this)->Entity::getEyeHeight();
    }

    virtual float getEyeHeight()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getEyeHeight))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetEyeHeight(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getEyeHeight<Base>(0);
    }

    template <class B>
    auto base_Entity_setSlot(int slot, const not_null_ptr<ItemInstance>& itemInstance, int) -> decltype(static_cast<B*>(this)->B::setSlot(slot, itemInstance))
    {
        return static_cast<B*>(this)->B::setSlot(slot, itemInstance);
    }
    template <class B>
    bool base_Entity_setSlot(int slot, const not_null_ptr<ItemInstance>& itemInstance, long)
    {
        return static_cast<Entity*>(this)->Entity::setSlot(slot, itemInstance);
    }

    virtual bool setSlot(int slot, const not_null_ptr<ItemInstance>& itemInstance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSlot))
        {
            bool result = false;
            if (!ModEvents::EntitySetSlot(this->shared_from_this(), slot, const_cast<ItemInstance*>(itemInstance.get()), result))
                return result;
        }
        return base_Entity_setSlot<Base>(slot, itemInstance, 0);
    }

    template <class B>
    auto base_Entity_hasPermission(EGameCommand command, int) -> decltype(static_cast<B*>(this)->B::hasPermission(command))
    {
        return static_cast<B*>(this)->B::hasPermission(command);
    }
    template <class B>
    bool base_Entity_hasPermission(EGameCommand command, long)
    {
        return static_cast<Entity*>(this)->Entity::hasPermission(command);
    }

    virtual bool hasPermission(EGameCommand command)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasPermission))
        {
            bool result = false;
            if (!ModEvents::EntityHasPermission(this->shared_from_this(), static_cast<int>(command), result))
                return result;
        }
        return base_Entity_hasPermission<Base>(command, 0);
    }

    template <class B>
    auto base_Entity_getCommandSenderWorldPosition(int) -> decltype(static_cast<B*>(this)->B::getCommandSenderWorldPosition())
    {
        return static_cast<B*>(this)->B::getCommandSenderWorldPosition();
    }
    template <class B>
    Vec3* base_Entity_getCommandSenderWorldPosition(long)
    {
        return static_cast<Entity*>(this)->Entity::getCommandSenderWorldPosition();
    }

    virtual Vec3* getCommandSenderWorldPosition()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getCommandSenderWorldPosition))
        {
            Vec3* result = NULL;
            if (!ModEvents::EntityGetCommandSenderWorldPosition(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getCommandSenderWorldPosition<Base>(0);
    }

    template <class B>
    auto base_Entity_getCommandSenderWorld(int) -> decltype(static_cast<B*>(this)->B::getCommandSenderWorld())
    {
        return static_cast<B*>(this)->B::getCommandSenderWorld();
    }
    template <class B>
    Level* base_Entity_getCommandSenderWorld(long)
    {
        return static_cast<Entity*>(this)->Entity::getCommandSenderWorld();
    }

    virtual Level* getCommandSenderWorld()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getCommandSenderWorld))
        {
            Level* result = NULL;
            if (!ModEvents::EntityGetCommandSenderWorld(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getCommandSenderWorld<Base>(0);
    }

    template <class B>
    auto base_Entity_isSendMessageEnabled(int) -> decltype(static_cast<B*>(this)->B::isSendMessageEnabled())
    {
        return static_cast<B*>(this)->B::isSendMessageEnabled();
    }
    template <class B>
    bool base_Entity_isSendMessageEnabled(long)
    {
        return static_cast<Entity*>(this)->Entity::isSendMessageEnabled();
    }

    virtual bool isSendMessageEnabled()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isSendMessageEnabled))
        {
            bool result = false;
            if (!ModEvents::EntityIsSendMessageEnabled(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isSendMessageEnabled<Base>(0);
    }

    template <class B>
    auto base_Entity_setCommandStats(CommandStats::CommandStatType statType, int value, int) -> decltype(static_cast<B*>(this)->B::setCommandStats(statType, value))
    {
        static_cast<B*>(this)->B::setCommandStats(statType, value);
    }
    template <class B>
    void base_Entity_setCommandStats(CommandStats::CommandStatType statType, int value, long)
    {
        static_cast<Entity*>(this)->Entity::setCommandStats(statType, value);
    }

    virtual void setCommandStats(CommandStats::CommandStatType statType, int value)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setCommandStats))
        {
            if (!ModEvents::EntitySetCommandStats(this->shared_from_this(), static_cast<int>(statType), value))
                return;
        }
        base_Entity_setCommandStats<Base>(statType, value, 0);
    }

    template <class B>
    auto base_Entity_getServer(int) -> decltype(static_cast<B*>(this)->B::getServer())
    {
        return static_cast<B*>(this)->B::getServer();
    }
    template <class B>
    MinecraftServer* base_Entity_getServer(long)
    {
        return static_cast<Entity*>(this)->Entity::getServer();
    }

    virtual MinecraftServer* getServer()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getServer))
        {
            MinecraftServer* result = NULL;
            if (!ModEvents::EntityGetServer(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getServer<Base>(0);
    }

    template <class B>
    auto base_Entity_interactAt(const boost::shared_ptr<Player>& player, Vec3* pos, InteractionHand::EInteractionHand hand, int) -> decltype(static_cast<B*>(this)->B::interactAt(player, pos, hand))
    {
        return static_cast<B*>(this)->B::interactAt(player, pos, hand);
    }
    template <class B>
    InteractionResult::EInteractionResult base_Entity_interactAt(const boost::shared_ptr<Player>& player, Vec3* pos, InteractionHand::EInteractionHand hand, long)
    {
        return static_cast<Entity*>(this)->Entity::interactAt(player, pos, hand);
    }

    virtual InteractionResult::EInteractionResult interactAt(const boost::shared_ptr<Player>& player, Vec3* pos, InteractionHand::EInteractionHand hand)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_interactAt))
        {
            int result = 0;
            if (!ModEvents::EntityInteractAt(this->shared_from_this(), player, const_cast<Vec3*>(pos), static_cast<int>(hand), result))
                return static_cast<InteractionResult::EInteractionResult>(result);
        }
        return base_Entity_interactAt<Base>(player, pos, hand, 0);
    }

    template <class B>
    auto base_Entity_ignoreExplosion(int) -> decltype(static_cast<B*>(this)->B::ignoreExplosion())
    {
        return static_cast<B*>(this)->B::ignoreExplosion();
    }
    template <class B>
    bool base_Entity_ignoreExplosion(long)
    {
        return static_cast<Entity*>(this)->Entity::ignoreExplosion();
    }

    virtual bool ignoreExplosion()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_ignoreExplosion))
        {
            bool result = false;
            if (!ModEvents::EntityIgnoreExplosion(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_ignoreExplosion<Base>(0);
    }

    template <class B>
    auto base_Entity_startSeenByPlayer(boost::shared_ptr<ServerPlayer> player, int) -> decltype(static_cast<B*>(this)->B::startSeenByPlayer(player))
    {
        static_cast<B*>(this)->B::startSeenByPlayer(player);
    }
    template <class B>
    void base_Entity_startSeenByPlayer(boost::shared_ptr<ServerPlayer> player, long)
    {
        static_cast<Entity*>(this)->Entity::startSeenByPlayer(player);
    }

    virtual void startSeenByPlayer(boost::shared_ptr<ServerPlayer> player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_startSeenByPlayer))
        {
            if (!ModEvents::EntityStartSeenByPlayer(this->shared_from_this(), player))
                return;
        }
        base_Entity_startSeenByPlayer<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_stopSeenByPlayer(boost::shared_ptr<ServerPlayer> player, int) -> decltype(static_cast<B*>(this)->B::stopSeenByPlayer(player))
    {
        static_cast<B*>(this)->B::stopSeenByPlayer(player);
    }
    template <class B>
    void base_Entity_stopSeenByPlayer(boost::shared_ptr<ServerPlayer> player, long)
    {
        static_cast<Entity*>(this)->Entity::stopSeenByPlayer(player);
    }

    virtual void stopSeenByPlayer(boost::shared_ptr<ServerPlayer> player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_stopSeenByPlayer))
        {
            if (!ModEvents::EntityStopSeenByPlayer(this->shared_from_this(), player))
                return;
        }
        base_Entity_stopSeenByPlayer<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_rotate(Rotation* rotation, int) -> decltype(static_cast<B*>(this)->B::rotate(rotation))
    {
        return static_cast<B*>(this)->B::rotate(rotation);
    }
    template <class B>
    float base_Entity_rotate(Rotation* rotation, long)
    {
        return static_cast<Entity*>(this)->Entity::rotate(rotation);
    }

    virtual float rotate(Rotation* rotation)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_rotate))
        {
            float result = 0.0f;
            if (!ModEvents::EntityRotate(this->shared_from_this(), const_cast<Rotation*>(rotation), result))
                return result;
        }
        return base_Entity_rotate<Base>(rotation, 0);
    }

    template <class B>
    auto base_Entity_mirror(Mirror* mirror, int) -> decltype(static_cast<B*>(this)->B::mirror(mirror))
    {
        return static_cast<B*>(this)->B::mirror(mirror);
    }
    template <class B>
    float base_Entity_mirror(Mirror* mirror, long)
    {
        return static_cast<Entity*>(this)->Entity::mirror(mirror);
    }

    virtual float mirror(Mirror* mirror)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_mirror))
        {
            float result = 0.0f;
            if (!ModEvents::EntityMirror(this->shared_from_this(), const_cast<Mirror*>(mirror), result))
                return result;
        }
        return base_Entity_mirror<Base>(mirror, 0);
    }

    template <class B>
    auto base_Entity_onlyOpCanSetNbt(int) -> decltype(static_cast<B*>(this)->B::onlyOpCanSetNbt())
    {
        return static_cast<B*>(this)->B::onlyOpCanSetNbt();
    }
    template <class B>
    bool base_Entity_onlyOpCanSetNbt(long)
    {
        return static_cast<Entity*>(this)->Entity::onlyOpCanSetNbt();
    }

    virtual bool onlyOpCanSetNbt()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onlyOpCanSetNbt))
        {
            bool result = false;
            if (!ModEvents::EntityOnlyOpCanSetNbt(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_onlyOpCanSetNbt<Base>(0);
    }

    template <class B>
    auto base_Entity_checkAndResetTeleportedFlag(int) -> decltype(static_cast<B*>(this)->B::checkAndResetTeleportedFlag())
    {
        return static_cast<B*>(this)->B::checkAndResetTeleportedFlag();
    }
    template <class B>
    bool base_Entity_checkAndResetTeleportedFlag(long)
    {
        return static_cast<Entity*>(this)->Entity::checkAndResetTeleportedFlag();
    }

    virtual bool checkAndResetTeleportedFlag()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_checkAndResetTeleportedFlag))
        {
            bool result = false;
            if (!ModEvents::EntityCheckAndResetTeleportedFlag(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_checkAndResetTeleportedFlag<Base>(0);
    }

    template <class B>
    auto base_Entity_hasPassenger(boost::shared_ptr<Entity> passenger, int) -> decltype(static_cast<B*>(this)->B::hasPassenger(passenger))
    {
        return static_cast<B*>(this)->B::hasPassenger(passenger);
    }
    template <class B>
    bool base_Entity_hasPassenger(boost::shared_ptr<Entity> passenger, long)
    {
        return static_cast<Entity*>(this)->Entity::hasPassenger(passenger);
    }

    virtual bool hasPassenger(boost::shared_ptr<Entity> passenger)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasPassenger))
        {
            bool result = false;
            if (!ModEvents::EntityHasPassenger(this->shared_from_this(), passenger, result))
                return result;
        }
        return base_Entity_hasPassenger<Base>(passenger, 0);
    }

    template <class B>
    auto base_Entity_hasPassenger_2(unsigned long long type, int) -> decltype(static_cast<B*>(this)->B::hasPassenger(type))
    {
        return static_cast<B*>(this)->B::hasPassenger(type);
    }
    template <class B>
    bool base_Entity_hasPassenger_2(unsigned long long type, long)
    {
        return static_cast<Entity*>(this)->Entity::hasPassenger(type);
    }

    virtual bool hasPassenger(unsigned long long type)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasPassenger_2))
        {
            bool result = false;
            if (!ModEvents::EntityHasPassenger2(this->shared_from_this(), static_cast<int>(type), result))
                return result;
        }
        return base_Entity_hasPassenger_2<Base>(type, 0);
    }

    template <class B>
    auto base_Entity_isPassengerOfSameVehicle(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::isPassengerOfSameVehicle(entity))
    {
        return static_cast<B*>(this)->B::isPassengerOfSameVehicle(entity);
    }
    template <class B>
    bool base_Entity_isPassengerOfSameVehicle(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Entity*>(this)->Entity::isPassengerOfSameVehicle(entity);
    }

    virtual bool isPassengerOfSameVehicle(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isPassengerOfSameVehicle))
        {
            bool result = false;
            if (!ModEvents::EntityIsPassengerOfSameVehicle(this->shared_from_this(), entity, result))
                return result;
        }
        return base_Entity_isPassengerOfSameVehicle<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_hasIndirectPassenger(boost::shared_ptr<Entity> passenger, int) -> decltype(static_cast<B*>(this)->B::hasIndirectPassenger(passenger))
    {
        return static_cast<B*>(this)->B::hasIndirectPassenger(passenger);
    }
    template <class B>
    bool base_Entity_hasIndirectPassenger(boost::shared_ptr<Entity> passenger, long)
    {
        return static_cast<Entity*>(this)->Entity::hasIndirectPassenger(passenger);
    }

    virtual bool hasIndirectPassenger(boost::shared_ptr<Entity> passenger)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasIndirectPassenger))
        {
            bool result = false;
            if (!ModEvents::EntityHasIndirectPassenger(this->shared_from_this(), passenger, result))
                return result;
        }
        return base_Entity_hasIndirectPassenger<Base>(passenger, 0);
    }

    template <class B>
    auto base_Entity_hasIndirectPassenger_2(unsigned long long type, int) -> decltype(static_cast<B*>(this)->B::hasIndirectPassenger(type))
    {
        return static_cast<B*>(this)->B::hasIndirectPassenger(type);
    }
    template <class B>
    bool base_Entity_hasIndirectPassenger_2(unsigned long long type, long)
    {
        return static_cast<Entity*>(this)->Entity::hasIndirectPassenger(type);
    }

    virtual bool hasIndirectPassenger(unsigned long long type)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasIndirectPassenger_2))
        {
            bool result = false;
            if (!ModEvents::EntityHasIndirectPassenger2(this->shared_from_this(), static_cast<int>(type), result))
                return result;
        }
        return base_Entity_hasIndirectPassenger_2<Base>(type, 0);
    }

    template <class B>
    auto base_Entity_isControlledByLocalInstance(int) -> decltype(static_cast<B*>(this)->B::isControlledByLocalInstance())
    {
        return static_cast<B*>(this)->B::isControlledByLocalInstance();
    }
    template <class B>
    bool base_Entity_isControlledByLocalInstance(long)
    {
        return static_cast<Entity*>(this)->Entity::isControlledByLocalInstance();
    }

    virtual bool isControlledByLocalInstance()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isControlledByLocalInstance))
        {
            bool result = false;
            if (!ModEvents::EntityIsControlledByLocalInstance(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isControlledByLocalInstance<Base>(0);
    }

    template <class B>
    auto base_Entity_getPistonPushReaction(int) -> decltype(static_cast<B*>(this)->B::getPistonPushReaction())
    {
        return static_cast<B*>(this)->B::getPistonPushReaction();
    }
    template <class B>
    int base_Entity_getPistonPushReaction(long)
    {
        return static_cast<Entity*>(this)->Entity::getPistonPushReaction();
    }

    virtual int getPistonPushReaction()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getPistonPushReaction))
        {
            int result = 0;
            if (!ModEvents::EntityGetPistonPushReaction(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getPistonPushReaction<Base>(0);
    }

    template <class B>
    auto base_Entity_getSoundSource(int) -> decltype(static_cast<B*>(this)->B::getSoundSource())
    {
        return static_cast<B*>(this)->B::getSoundSource();
    }
    template <class B>
    SoundSource::ESoundSource base_Entity_getSoundSource(long)
    {
        return static_cast<Entity*>(this)->Entity::getSoundSource();
    }

    virtual SoundSource::ESoundSource getSoundSource()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSoundSource))
        {
            int result = 0;
            if (!ModEvents::EntityGetSoundSource(this->shared_from_this(), result))
                return static_cast<SoundSource::ESoundSource>(result);
        }
        return base_Entity_getSoundSource<Base>(0);
    }

    template <class B>
    auto base_Entity_isCreative(int) -> decltype(static_cast<B*>(this)->B::isCreative())
    {
        return static_cast<B*>(this)->B::isCreative();
    }
    template <class B>
    bool base_Entity_isCreative(long)
    {
        return static_cast<Entity*>(this)->Entity::isCreative();
    }

    virtual bool isCreative()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isCreative))
        {
            bool result = false;
            if (!ModEvents::EntityIsCreative(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isCreative<Base>(0);
    }

    template <class B>
    auto base_Entity_isDespawnProtected(int) -> decltype(static_cast<B*>(this)->B::isDespawnProtected())
    {
        return static_cast<B*>(this)->B::isDespawnProtected();
    }
    template <class B>
    bool base_Entity_isDespawnProtected(long)
    {
        return static_cast<Entity*>(this)->Entity::isDespawnProtected();
    }

    virtual bool isDespawnProtected()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isDespawnProtected))
        {
            bool result = false;
            if (!ModEvents::EntityIsDespawnProtected(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isDespawnProtected<Base>(0);
    }

    template <class B>
    auto base_Entity_setDespawnProtected(int) -> decltype(static_cast<B*>(this)->B::setDespawnProtected())
    {
        static_cast<B*>(this)->B::setDespawnProtected();
    }
    template <class B>
    void base_Entity_setDespawnProtected(long)
    {
        static_cast<Entity*>(this)->Entity::setDespawnProtected();
    }

    virtual void setDespawnProtected()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setDespawnProtected))
        {
            if (!ModEvents::EntitySetDespawnProtected(this->shared_from_this()))
                return;
        }
        base_Entity_setDespawnProtected<Base>(0);
    }

    template <class B>
    auto base_Entity_couldWander(int) -> decltype(static_cast<B*>(this)->B::couldWander())
    {
        return static_cast<B*>(this)->B::couldWander();
    }
    template <class B>
    bool base_Entity_couldWander(long)
    {
        return static_cast<Entity*>(this)->Entity::couldWander();
    }

    virtual bool couldWander()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_couldWander))
        {
            bool result = false;
            if (!ModEvents::EntityCouldWander(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_couldWander<Base>(0);
    }

    template <class B>
    auto base_Entity_canCreateParticles(int) -> decltype(static_cast<B*>(this)->B::canCreateParticles())
    {
        return static_cast<B*>(this)->B::canCreateParticles();
    }
    template <class B>
    bool base_Entity_canCreateParticles(long)
    {
        return static_cast<Entity*>(this)->Entity::canCreateParticles();
    }

    virtual bool canCreateParticles()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canCreateParticles))
        {
            bool result = false;
            if (!ModEvents::EntityCanCreateParticles(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_canCreateParticles<Base>(0);
    }

    template <class B>
    auto base_Entity_stopCurrentLerp(int) -> decltype(static_cast<B*>(this)->B::stopCurrentLerp())
    {
        static_cast<B*>(this)->B::stopCurrentLerp();
    }
    template <class B>
    void base_Entity_stopCurrentLerp(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::stopCurrentLerp();
    }

    virtual void stopCurrentLerp()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_stopCurrentLerp))
        {
            if (!ModEvents::EntityStopCurrentLerp(this->shared_from_this()))
                return;
        }
        base_Entity_stopCurrentLerp<Base>(0);
    }

    template <class B>
    auto base_Entity_DoAutoAttackOnTouch(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::DoAutoAttackOnTouch(entity))
    {
        static_cast<B*>(this)->B::DoAutoAttackOnTouch(entity);
    }
    template <class B>
    void base_Entity_DoAutoAttackOnTouch(boost::shared_ptr<Entity> entity, long)
    {
        static_cast<Entity*>(this)->Entity::DoAutoAttackOnTouch(entity);
    }

    virtual void DoAutoAttackOnTouch(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_DoAutoAttackOnTouch))
        {
            if (!ModEvents::EntityDoAutoAttackOnTouch(this->shared_from_this(), entity))
                return;
        }
        base_Entity_DoAutoAttackOnTouch<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_tickLeash(int) -> decltype(static_cast<B*>(this)->B::tickLeash())
    {
        static_cast<B*>(this)->B::tickLeash();
    }
    template <class B>
    void base_Entity_tickLeash(long)
    {
        static_cast<Entity*>(this)->Entity::tickLeash();
    }

    virtual void tickLeash()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_tickLeash))
        {
            if (!ModEvents::EntityTickLeash(this->shared_from_this()))
                return;
        }
        base_Entity_tickLeash<Base>(0);
    }

    template <class B>
    auto base_Entity_dropLeash(bool broadcast, bool dropItem, int) -> decltype(static_cast<B*>(this)->B::dropLeash(broadcast, dropItem))
    {
        static_cast<B*>(this)->B::dropLeash(broadcast, dropItem);
    }
    template <class B>
    void base_Entity_dropLeash(bool broadcast, bool dropItem, long)
    {
        static_cast<Entity*>(this)->Entity::dropLeash(broadcast, dropItem);
    }

    virtual void dropLeash(bool broadcast, bool dropItem)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_dropLeash))
        {
            if (!ModEvents::EntityDropLeash(this->shared_from_this(), broadcast, dropItem))
                return;
        }
        base_Entity_dropLeash<Base>(broadcast, dropItem, 0);
    }

    template <class B>
    auto base_Entity_canBeLeashed(const boost::shared_ptr<Player>& player, int) -> decltype(static_cast<B*>(this)->B::canBeLeashed(player))
    {
        return static_cast<B*>(this)->B::canBeLeashed(player);
    }
    template <class B>
    bool base_Entity_canBeLeashed(const boost::shared_ptr<Player>& player, long)
    {
        return static_cast<Entity*>(this)->Entity::canBeLeashed(player);
    }

    virtual bool canBeLeashed(const boost::shared_ptr<Player>& player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canBeLeashed))
        {
            bool result = false;
            if (!ModEvents::EntityCanBeLeashed(this->shared_from_this(), player, result))
                return result;
        }
        return base_Entity_canBeLeashed<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_isLeashed(int) -> decltype(static_cast<B*>(this)->B::isLeashed())
    {
        return static_cast<B*>(this)->B::isLeashed();
    }
    template <class B>
    bool base_Entity_isLeashed(long)
    {
        return static_cast<Entity*>(this)->Entity::isLeashed();
    }

    virtual bool isLeashed()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isLeashed))
        {
            bool result = false;
            if (!ModEvents::EntityIsLeashed(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isLeashed<Base>(0);
    }

    template <class B>
    auto base_Entity_isLeashedToPlayer(const boost::shared_ptr<Player>& player, int) -> decltype(static_cast<B*>(this)->B::isLeashedToPlayer(player))
    {
        return static_cast<B*>(this)->B::isLeashedToPlayer(player);
    }
    template <class B>
    bool base_Entity_isLeashedToPlayer(const boost::shared_ptr<Player>& player, long)
    {
        return static_cast<Entity*>(this)->Entity::isLeashedToPlayer(player);
    }

    virtual bool isLeashedToPlayer(const boost::shared_ptr<Player>& player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isLeashedToPlayer))
        {
            bool result = false;
            if (!ModEvents::EntityIsLeashedToPlayer(this->shared_from_this(), player, result))
                return result;
        }
        return base_Entity_isLeashedToPlayer<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_setLeashedTo(boost::shared_ptr<Entity> leashHolder, bool broadcast, int) -> decltype(static_cast<B*>(this)->B::setLeashedTo(leashHolder, broadcast))
    {
        static_cast<B*>(this)->B::setLeashedTo(leashHolder, broadcast);
    }
    template <class B>
    void base_Entity_setLeashedTo(boost::shared_ptr<Entity> leashHolder, bool broadcast, long)
    {
        static_cast<Entity*>(this)->Entity::setLeashedTo(leashHolder, broadcast);
    }

    virtual void setLeashedTo(boost::shared_ptr<Entity> leashHolder, bool broadcast)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setLeashedTo))
        {
            if (!ModEvents::EntitySetLeashedTo(this->shared_from_this(), leashHolder, broadcast))
                return;
        }
        base_Entity_setLeashedTo<Base>(leashHolder, broadcast, 0);
    }

    template <class B>
    auto base_Entity_restoreLeashFromSave(int) -> decltype(static_cast<B*>(this)->B::restoreLeashFromSave())
    {
        static_cast<B*>(this)->B::restoreLeashFromSave();
    }
    template <class B>
    void base_Entity_restoreLeashFromSave(long)
    {
        static_cast<Entity*>(this)->Entity::restoreLeashFromSave();
    }

    virtual void restoreLeashFromSave()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_restoreLeashFromSave))
        {
            if (!ModEvents::EntityRestoreLeashFromSave(this->shared_from_this()))
                return;
        }
        base_Entity_restoreLeashFromSave<Base>(0);
    }

    template <class B>
    auto base_Entity_defineSynchedData(int) -> decltype(static_cast<B*>(this)->B::defineSynchedData())
    {
        static_cast<B*>(this)->B::defineSynchedData();
    }
    template <class B>
    void base_Entity_defineSynchedData(long)
    {
        static_cast<Mob*>(this)->Mob::defineSynchedData();
    }

    virtual void defineSynchedData()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_defineSynchedData))
        {
            if (!ModEvents::EntityDefineSynchedData(this->shared_from_this()))
                return;
        }
        base_Entity_defineSynchedData<Base>(0);
    }

    template <class B>
    auto base_Entity_readAdditionalSaveData(CompoundTag* tag, int) -> decltype(static_cast<B*>(this)->B::readAdditionalSaveData(tag))
    {
        static_cast<B*>(this)->B::readAdditionalSaveData(tag);
    }
    template <class B>
    void base_Entity_readAdditionalSaveData(CompoundTag* tag, long)
    {
        static_cast<Mob*>(this)->Mob::readAdditionalSaveData(tag);
    }

    virtual void readAdditionalSaveData(CompoundTag* tag)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_readAdditionalSaveData))
        {
            if (!ModEvents::EntityReadAdditionalSaveData(this->shared_from_this(), const_cast<CompoundTag*>(tag)))
                return;
        }
        base_Entity_readAdditionalSaveData<Base>(tag, 0);
    }

    template <class B>
    auto base_Entity_addAdditonalSaveData(CompoundTag* tag, int) -> decltype(static_cast<B*>(this)->B::addAdditonalSaveData(tag))
    {
        static_cast<B*>(this)->B::addAdditonalSaveData(tag);
    }
    template <class B>
    void base_Entity_addAdditonalSaveData(CompoundTag* tag, long)
    {
        static_cast<Mob*>(this)->Mob::addAdditonalSaveData(tag);
    }

    virtual void addAdditonalSaveData(CompoundTag* tag)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_addAdditonalSaveData))
        {
            if (!ModEvents::EntityAddAdditonalSaveData(this->shared_from_this(), const_cast<CompoundTag*>(tag)))
                return;
        }
        base_Entity_addAdditonalSaveData<Base>(tag, 0);
    }

    template <class B>
    auto base_Entity_registerAttributes(int) -> decltype(static_cast<B*>(this)->B::registerAttributes())
    {
        static_cast<B*>(this)->B::registerAttributes();
    }
    template <class B>
    void base_Entity_registerAttributes(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::registerAttributes();
    }

    virtual void registerAttributes()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_registerAttributes))
        {
            if (!ModEvents::EntityRegisterAttributes(this->shared_from_this()))
                return;
        }
        base_Entity_registerAttributes<Base>(0);
    }

    template <class B>
    auto base_Entity_isWaterMob(int) -> decltype(static_cast<B*>(this)->B::isWaterMob())
    {
        return static_cast<B*>(this)->B::isWaterMob();
    }
    template <class B>
    bool base_Entity_isWaterMob(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isWaterMob();
    }

    virtual bool isWaterMob()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isWaterMob))
        {
            bool result = false;
            if (!ModEvents::EntityIsWaterMob(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isWaterMob<Base>(0);
    }

    template <class B>
    auto base_Entity_breathesWater(int) -> decltype(static_cast<B*>(this)->B::breathesWater())
    {
        return static_cast<B*>(this)->B::breathesWater();
    }
    template <class B>
    bool base_Entity_breathesWater(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::breathesWater();
    }

    virtual bool breathesWater()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_breathesWater))
        {
            bool result = false;
            if (!ModEvents::EntityBreathesWater(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_breathesWater<Base>(0);
    }

    template <class B>
    auto base_Entity_breathesAir(int) -> decltype(static_cast<B*>(this)->B::breathesAir())
    {
        return static_cast<B*>(this)->B::breathesAir();
    }
    template <class B>
    bool base_Entity_breathesAir(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::breathesAir();
    }

    virtual bool breathesAir()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_breathesAir))
        {
            bool result = false;
            if (!ModEvents::EntityBreathesAir(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_breathesAir<Base>(0);
    }

    template <class B>
    auto base_Entity_breathesLava(int) -> decltype(static_cast<B*>(this)->B::breathesLava())
    {
        return static_cast<B*>(this)->B::breathesLava();
    }
    template <class B>
    bool base_Entity_breathesLava(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::breathesLava();
    }

    virtual bool breathesLava()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_breathesLava))
        {
            bool result = false;
            if (!ModEvents::EntityBreathesLava(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_breathesLava<Base>(0);
    }

    template <class B>
    auto base_Entity_breathesSolids(int) -> decltype(static_cast<B*>(this)->B::breathesSolids())
    {
        return static_cast<B*>(this)->B::breathesSolids();
    }
    template <class B>
    bool base_Entity_breathesSolids(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::breathesSolids();
    }

    virtual bool breathesSolids()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_breathesSolids))
        {
            bool result = false;
            if (!ModEvents::EntityBreathesSolids(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_breathesSolids<Base>(0);
    }

    template <class B>
    auto base_Entity_GetSuffocateTime(int) -> decltype(static_cast<B*>(this)->B::GetSuffocateTime())
    {
        return static_cast<B*>(this)->B::GetSuffocateTime();
    }
    template <class B>
    int base_Entity_GetSuffocateTime(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::GetSuffocateTime();
    }

    virtual int GetSuffocateTime()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_GetSuffocateTime))
        {
            int result = 0;
            if (!ModEvents::EntityGetSuffocateTime(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_GetSuffocateTime<Base>(0);
    }

    template <class B>
    auto base_Entity_GetInhaleTime(int) -> decltype(static_cast<B*>(this)->B::GetInhaleTime())
    {
        return static_cast<B*>(this)->B::GetInhaleTime();
    }
    template <class B>
    float base_Entity_GetInhaleTime(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::GetInhaleTime();
    }

    virtual float GetInhaleTime()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_GetInhaleTime))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetInhaleTime(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_GetInhaleTime<Base>(0);
    }

    template <class B>
    auto base_Entity_generatesBubbles(int) -> decltype(static_cast<B*>(this)->B::generatesBubbles())
    {
        return static_cast<B*>(this)->B::generatesBubbles();
    }
    template <class B>
    bool base_Entity_generatesBubbles(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::generatesBubbles();
    }

    virtual bool generatesBubbles()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_generatesBubbles))
        {
            bool result = false;
            if (!ModEvents::EntityGeneratesBubbles(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_generatesBubbles<Base>(0);
    }

    template <class B>
    auto base_Entity_onChangedBlock(const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::onChangedBlock(pos))
    {
        static_cast<B*>(this)->B::onChangedBlock(pos);
    }
    template <class B>
    void base_Entity_onChangedBlock(const BlockPos& pos, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::onChangedBlock(pos);
    }

    virtual void onChangedBlock(const BlockPos& pos)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onChangedBlock))
        {
            if (!ModEvents::EntityOnChangedBlock(this->shared_from_this(), pos.x, pos.y, pos.z))
                return;
        }
        base_Entity_onChangedBlock<Base>(pos, 0);
    }

    template <class B>
    auto base_Entity_isBaby(int) -> decltype(static_cast<B*>(this)->B::isBaby())
    {
        return static_cast<B*>(this)->B::isBaby();
    }
    template <class B>
    bool base_Entity_isBaby(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isBaby();
    }

    virtual bool isBaby()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isBaby))
        {
            bool result = false;
            if (!ModEvents::EntityIsBaby(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isBaby<Base>(0);
    }

    template <class B>
    auto base_Entity_tickDeath(int) -> decltype(static_cast<B*>(this)->B::tickDeath())
    {
        static_cast<B*>(this)->B::tickDeath();
    }
    template <class B>
    void base_Entity_tickDeath(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::tickDeath();
    }

    virtual void tickDeath()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_tickDeath))
        {
            if (!ModEvents::EntityTickDeath(this->shared_from_this()))
                return;
        }
        base_Entity_tickDeath<Base>(0);
    }

    template <class B>
    auto base_Entity_shouldDropExperience(int) -> decltype(static_cast<B*>(this)->B::shouldDropExperience())
    {
        return static_cast<B*>(this)->B::shouldDropExperience();
    }
    template <class B>
    bool base_Entity_shouldDropExperience(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::shouldDropExperience();
    }

    virtual bool shouldDropExperience()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_shouldDropExperience))
        {
            bool result = false;
            if (!ModEvents::EntityShouldDropExperience(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_shouldDropExperience<Base>(0);
    }

    template <class B>
    auto base_Entity_decreaseAirSupply(int airSupply, int) -> decltype(static_cast<B*>(this)->B::decreaseAirSupply(airSupply))
    {
        return static_cast<B*>(this)->B::decreaseAirSupply(airSupply);
    }
    template <class B>
    int base_Entity_decreaseAirSupply(int airSupply, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::decreaseAirSupply(airSupply);
    }

    virtual int decreaseAirSupply(int airSupply)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_decreaseAirSupply))
        {
            int result = 0;
            if (!ModEvents::EntityDecreaseAirSupply(this->shared_from_this(), airSupply, result))
                return result;
        }
        return base_Entity_decreaseAirSupply<Base>(airSupply, 0);
    }

    template <class B>
    auto base_Entity_getExperienceReward(boost::shared_ptr<Player> player, int) -> decltype(static_cast<B*>(this)->B::getExperienceReward(player))
    {
        return static_cast<B*>(this)->B::getExperienceReward(player);
    }
    template <class B>
    int base_Entity_getExperienceReward(boost::shared_ptr<Player> player, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getExperienceReward(player);
    }

    virtual int getExperienceReward(boost::shared_ptr<Player> player)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getExperienceReward))
        {
            int result = 0;
            if (!ModEvents::EntityGetExperienceReward(this->shared_from_this(), player, result))
                return result;
        }
        return base_Entity_getExperienceReward<Base>(player, 0);
    }

    template <class B>
    auto base_Entity_isAlwaysExperienceDropper(int) -> decltype(static_cast<B*>(this)->B::isAlwaysExperienceDropper())
    {
        return static_cast<B*>(this)->B::isAlwaysExperienceDropper();
    }
    template <class B>
    bool base_Entity_isAlwaysExperienceDropper(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isAlwaysExperienceDropper();
    }

    virtual bool isAlwaysExperienceDropper()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isAlwaysExperienceDropper))
        {
            bool result = false;
            if (!ModEvents::EntityIsAlwaysExperienceDropper(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isAlwaysExperienceDropper<Base>(0);
    }

    template <class B>
    auto base_Entity_getRandom(int) -> decltype(static_cast<B*>(this)->B::getRandom())
    {
        return static_cast<B*>(this)->B::getRandom();
    }
    template <class B>
    Random* base_Entity_getRandom(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getRandom();
    }

    virtual Random* getRandom()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getRandom))
        {
            Random* result = NULL;
            if (!ModEvents::EntityGetRandom(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getRandom<Base>(0);
    }

    template <class B>
    auto base_Entity_getLastHurtByMobTimestamp(int) -> decltype(static_cast<B*>(this)->B::getLastHurtByMobTimestamp())
    {
        return static_cast<B*>(this)->B::getLastHurtByMobTimestamp();
    }
    template <class B>
    int base_Entity_getLastHurtByMobTimestamp(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getLastHurtByMobTimestamp();
    }

    virtual int getLastHurtByMobTimestamp()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getLastHurtByMobTimestamp))
        {
            int result = 0;
            if (!ModEvents::EntityGetLastHurtByMobTimestamp(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getLastHurtByMobTimestamp<Base>(0);
    }

    template <class B>
    auto base_Entity_setLastHurtByMob(boost::shared_ptr<LivingEntity> attacker, int) -> decltype(static_cast<B*>(this)->B::setLastHurtByMob(attacker))
    {
        static_cast<B*>(this)->B::setLastHurtByMob(attacker);
    }
    template <class B>
    void base_Entity_setLastHurtByMob(boost::shared_ptr<LivingEntity> attacker, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setLastHurtByMob(attacker);
    }

    virtual void setLastHurtByMob(boost::shared_ptr<LivingEntity> attacker)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setLastHurtByMob))
        {
            if (!ModEvents::EntitySetLastHurtByMob(this->shared_from_this(), attacker))
                return;
        }
        base_Entity_setLastHurtByMob<Base>(attacker, 0);
    }

    template <class B>
    auto base_Entity_getLastHurtMobTimestamp(int) -> decltype(static_cast<B*>(this)->B::getLastHurtMobTimestamp())
    {
        return static_cast<B*>(this)->B::getLastHurtMobTimestamp();
    }
    template <class B>
    int base_Entity_getLastHurtMobTimestamp(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getLastHurtMobTimestamp();
    }

    virtual int getLastHurtMobTimestamp()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getLastHurtMobTimestamp))
        {
            int result = 0;
            if (!ModEvents::EntityGetLastHurtMobTimestamp(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getLastHurtMobTimestamp<Base>(0);
    }

    template <class B>
    auto base_Entity_setLastHurtMob(boost::shared_ptr<Entity> target, int) -> decltype(static_cast<B*>(this)->B::setLastHurtMob(target))
    {
        static_cast<B*>(this)->B::setLastHurtMob(target);
    }
    template <class B>
    void base_Entity_setLastHurtMob(boost::shared_ptr<Entity> target, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setLastHurtMob(target);
    }

    virtual void setLastHurtMob(boost::shared_ptr<Entity> target)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setLastHurtMob))
        {
            if (!ModEvents::EntitySetLastHurtMob(this->shared_from_this(), target))
                return;
        }
        base_Entity_setLastHurtMob<Base>(target, 0);
    }

    template <class B>
    auto base_Entity_getNoActionTime(int) -> decltype(static_cast<B*>(this)->B::getNoActionTime())
    {
        return static_cast<B*>(this)->B::getNoActionTime();
    }
    template <class B>
    int base_Entity_getNoActionTime(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getNoActionTime();
    }

    virtual int getNoActionTime()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getNoActionTime))
        {
            int result = 0;
            if (!ModEvents::EntityGetNoActionTime(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getNoActionTime<Base>(0);
    }

    template <class B>
    auto base_Entity_tickEffects(int) -> decltype(static_cast<B*>(this)->B::tickEffects())
    {
        static_cast<B*>(this)->B::tickEffects();
    }
    template <class B>
    void base_Entity_tickEffects(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::tickEffects();
    }

    virtual void tickEffects()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_tickEffects))
        {
            if (!ModEvents::EntityTickEffects(this->shared_from_this()))
                return;
        }
        base_Entity_tickEffects<Base>(0);
    }

    template <class B>
    auto base_Entity_updateInvisibilityStatus(int) -> decltype(static_cast<B*>(this)->B::updateInvisibilityStatus())
    {
        static_cast<B*>(this)->B::updateInvisibilityStatus();
    }
    template <class B>
    void base_Entity_updateInvisibilityStatus(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::updateInvisibilityStatus();
    }

    virtual void updateInvisibilityStatus()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_updateInvisibilityStatus))
        {
            if (!ModEvents::EntityUpdateInvisibilityStatus(this->shared_from_this()))
                return;
        }
        base_Entity_updateInvisibilityStatus<Base>(0);
    }

    template <class B>
    auto base_Entity_removeEffectParticles(int) -> decltype(static_cast<B*>(this)->B::removeEffectParticles())
    {
        static_cast<B*>(this)->B::removeEffectParticles();
    }
    template <class B>
    void base_Entity_removeEffectParticles(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::removeEffectParticles();
    }

    virtual void removeEffectParticles()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_removeEffectParticles))
        {
            if (!ModEvents::EntityRemoveEffectParticles(this->shared_from_this()))
                return;
        }
        base_Entity_removeEffectParticles<Base>(0);
    }

    template <class B>
    auto base_Entity_removeAllEffects(int) -> decltype(static_cast<B*>(this)->B::removeAllEffects())
    {
        static_cast<B*>(this)->B::removeAllEffects();
    }
    template <class B>
    void base_Entity_removeAllEffects(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::removeAllEffects();
    }

    virtual void removeAllEffects()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_removeAllEffects))
        {
            if (!ModEvents::EntityRemoveAllEffects(this->shared_from_this()))
                return;
        }
        base_Entity_removeAllEffects<Base>(0);
    }

    template <class B>
    auto base_Entity_hasEffect(MobEffect* effect, int) -> decltype(static_cast<B*>(this)->B::hasEffect(effect))
    {
        return static_cast<B*>(this)->B::hasEffect(effect);
    }
    template <class B>
    bool base_Entity_hasEffect(MobEffect* effect, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::hasEffect(effect);
    }

    virtual bool hasEffect(MobEffect* effect)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasEffect))
        {
            bool result = false;
            if (!ModEvents::EntityHasEffect(this->shared_from_this(), const_cast<MobEffect*>(effect), result))
                return result;
        }
        return base_Entity_hasEffect<Base>(effect, 0);
    }

    template <class B>
    auto base_Entity_getEffect(MobEffect* effect, int) -> decltype(static_cast<B*>(this)->B::getEffect(effect))
    {
        return static_cast<B*>(this)->B::getEffect(effect);
    }
    template <class B>
    MobEffectInstance* base_Entity_getEffect(MobEffect* effect, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getEffect(effect);
    }

    virtual MobEffectInstance* getEffect(MobEffect* effect)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getEffect))
        {
            MobEffectInstance* result = NULL;
            if (!ModEvents::EntityGetEffect(this->shared_from_this(), const_cast<MobEffect*>(effect), result))
                return result;
        }
        return base_Entity_getEffect<Base>(effect, 0);
    }

    template <class B>
    auto base_Entity_addEffect(MobEffectInstance* effectInstance, const boost::shared_ptr<Entity>& source, int) -> decltype(static_cast<B*>(this)->B::addEffect(effectInstance, source))
    {
        static_cast<B*>(this)->B::addEffect(effectInstance, source);
    }
    template <class B>
    void base_Entity_addEffect(MobEffectInstance* effectInstance, const boost::shared_ptr<Entity>& source, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::addEffect(effectInstance, source);
    }

    virtual void addEffect(MobEffectInstance* effectInstance, const boost::shared_ptr<Entity>& source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_addEffect))
        {
            if (!ModEvents::EntityAddEffect(this->shared_from_this(), const_cast<MobEffectInstance*>(effectInstance), source))
                return;
        }
        base_Entity_addEffect<Base>(effectInstance, source, 0);
    }

    template <class B>
    auto base_Entity_addEffectNoUpdate(MobEffectInstance* effectInstance, int) -> decltype(static_cast<B*>(this)->B::addEffectNoUpdate(effectInstance))
    {
        static_cast<B*>(this)->B::addEffectNoUpdate(effectInstance);
    }
    template <class B>
    void base_Entity_addEffectNoUpdate(MobEffectInstance* effectInstance, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::addEffectNoUpdate(effectInstance);
    }

    virtual void addEffectNoUpdate(MobEffectInstance* effectInstance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_addEffectNoUpdate))
        {
            if (!ModEvents::EntityAddEffectNoUpdate(this->shared_from_this(), const_cast<MobEffectInstance*>(effectInstance)))
                return;
        }
        base_Entity_addEffectNoUpdate<Base>(effectInstance, 0);
    }

    template <class B>
    auto base_Entity_canBeAffected(MobEffectInstance* effectInstance, int) -> decltype(static_cast<B*>(this)->B::canBeAffected(effectInstance))
    {
        return static_cast<B*>(this)->B::canBeAffected(effectInstance);
    }
    template <class B>
    bool base_Entity_canBeAffected(MobEffectInstance* effectInstance, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::canBeAffected(effectInstance);
    }

    virtual bool canBeAffected(MobEffectInstance* effectInstance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canBeAffected))
        {
            bool result = false;
            if (!ModEvents::EntityCanBeAffected(this->shared_from_this(), const_cast<MobEffectInstance*>(effectInstance), result))
                return result;
        }
        return base_Entity_canBeAffected<Base>(effectInstance, 0);
    }

    template <class B>
    auto base_Entity_isInvertedHealAndHarm(int) -> decltype(static_cast<B*>(this)->B::isInvertedHealAndHarm())
    {
        return static_cast<B*>(this)->B::isInvertedHealAndHarm();
    }
    template <class B>
    bool base_Entity_isInvertedHealAndHarm(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isInvertedHealAndHarm();
    }

    virtual bool isInvertedHealAndHarm()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isInvertedHealAndHarm))
        {
            bool result = false;
            if (!ModEvents::EntityIsInvertedHealAndHarm(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isInvertedHealAndHarm<Base>(0);
    }

    template <class B>
    auto base_Entity_removeEffectNoUpdate(MobEffect* effect, int) -> decltype(static_cast<B*>(this)->B::removeEffectNoUpdate(effect))
    {
        return static_cast<B*>(this)->B::removeEffectNoUpdate(effect);
    }
    template <class B>
    MobEffectInstance* base_Entity_removeEffectNoUpdate(MobEffect* effect, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::removeEffectNoUpdate(effect);
    }

    virtual MobEffectInstance* removeEffectNoUpdate(MobEffect* effect)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_removeEffectNoUpdate))
        {
            MobEffectInstance* result = NULL;
            if (!ModEvents::EntityRemoveEffectNoUpdate(this->shared_from_this(), const_cast<MobEffect*>(effect), result))
                return result;
        }
        return base_Entity_removeEffectNoUpdate<Base>(effect, 0);
    }

    template <class B>
    auto base_Entity_removeEffect(MobEffect* effect, int) -> decltype(static_cast<B*>(this)->B::removeEffect(effect))
    {
        static_cast<B*>(this)->B::removeEffect(effect);
    }
    template <class B>
    void base_Entity_removeEffect(MobEffect* effect, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::removeEffect(effect);
    }

    virtual void removeEffect(MobEffect* effect)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_removeEffect))
        {
            if (!ModEvents::EntityRemoveEffect(this->shared_from_this(), const_cast<MobEffect*>(effect)))
                return;
        }
        base_Entity_removeEffect<Base>(effect, 0);
    }

    template <class B>
    auto base_Entity_onEffectAdded(MobEffectInstance* effectInstance, int) -> decltype(static_cast<B*>(this)->B::onEffectAdded(effectInstance))
    {
        static_cast<B*>(this)->B::onEffectAdded(effectInstance);
    }
    template <class B>
    void base_Entity_onEffectAdded(MobEffectInstance* effectInstance, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::onEffectAdded(effectInstance);
    }

    virtual void onEffectAdded(MobEffectInstance* effectInstance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onEffectAdded))
        {
            if (!ModEvents::EntityOnEffectAdded(this->shared_from_this(), const_cast<MobEffectInstance*>(effectInstance)))
                return;
        }
        base_Entity_onEffectAdded<Base>(effectInstance, 0);
    }

    template <class B>
    auto base_Entity_onEffectUpdated(MobEffectInstance* effectInstance, bool reapply, int) -> decltype(static_cast<B*>(this)->B::onEffectUpdated(effectInstance, reapply))
    {
        static_cast<B*>(this)->B::onEffectUpdated(effectInstance, reapply);
    }
    template <class B>
    void base_Entity_onEffectUpdated(MobEffectInstance* effectInstance, bool reapply, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::onEffectUpdated(effectInstance, reapply);
    }

    virtual void onEffectUpdated(MobEffectInstance* effectInstance, bool reapply)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onEffectUpdated))
        {
            if (!ModEvents::EntityOnEffectUpdated(this->shared_from_this(), const_cast<MobEffectInstance*>(effectInstance), reapply))
                return;
        }
        base_Entity_onEffectUpdated<Base>(effectInstance, reapply, 0);
    }

    template <class B>
    auto base_Entity_onEffectRemoved(MobEffectInstance* effectInstance, int) -> decltype(static_cast<B*>(this)->B::onEffectRemoved(effectInstance))
    {
        static_cast<B*>(this)->B::onEffectRemoved(effectInstance);
    }
    template <class B>
    void base_Entity_onEffectRemoved(MobEffectInstance* effectInstance, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::onEffectRemoved(effectInstance);
    }

    virtual void onEffectRemoved(MobEffectInstance* effectInstance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onEffectRemoved))
        {
            if (!ModEvents::EntityOnEffectRemoved(this->shared_from_this(), const_cast<MobEffectInstance*>(effectInstance)))
                return;
        }
        base_Entity_onEffectRemoved<Base>(effectInstance, 0);
    }

    template <class B>
    auto base_Entity_heal(float amount, int) -> decltype(static_cast<B*>(this)->B::heal(amount))
    {
        static_cast<B*>(this)->B::heal(amount);
    }
    template <class B>
    void base_Entity_heal(float amount, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::heal(amount);
    }

    virtual void heal(float amount)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_heal))
        {
            if (!ModEvents::EntityHeal(this->shared_from_this(), amount))
                return;
        }
        base_Entity_heal<Base>(amount, 0);
    }

    template <class B>
    auto base_Entity_getHealth(int) -> decltype(static_cast<B*>(this)->B::getHealth())
    {
        return static_cast<B*>(this)->B::getHealth();
    }
    template <class B>
    float base_Entity_getHealth(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getHealth();
    }

    virtual float getHealth()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getHealth))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetHealth(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getHealth<Base>(0);
    }

    template <class B>
    auto base_Entity_setHealth(float health, int) -> decltype(static_cast<B*>(this)->B::setHealth(health))
    {
        static_cast<B*>(this)->B::setHealth(health);
    }
    template <class B>
    void base_Entity_setHealth(float health, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setHealth(health);
    }

    virtual void setHealth(float health)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setHealth))
        {
            if (!ModEvents::EntitySetHealth(this->shared_from_this(), health))
                return;
        }
        base_Entity_setHealth<Base>(health, 0);
    }

    template <class B>
    auto base_Entity_getLastDamageSource(int) -> decltype(static_cast<B*>(this)->B::getLastDamageSource())
    {
        return static_cast<B*>(this)->B::getLastDamageSource();
    }
    template <class B>
    const DamageSource* base_Entity_getLastDamageSource(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getLastDamageSource();
    }

    virtual const DamageSource* getLastDamageSource()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getLastDamageSource))
        {
            DamageSource* result = NULL;
            if (!ModEvents::EntityGetLastDamageSource(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getLastDamageSource<Base>(0);
    }

    template <class B>
    auto base_Entity_blockUsingShield(const boost::shared_ptr<LivingEntity>& attacker, int) -> decltype(static_cast<B*>(this)->B::blockUsingShield(attacker))
    {
        static_cast<B*>(this)->B::blockUsingShield(attacker);
    }
    template <class B>
    void base_Entity_blockUsingShield(const boost::shared_ptr<LivingEntity>& attacker, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::blockUsingShield(attacker);
    }

    virtual void blockUsingShield(const boost::shared_ptr<LivingEntity>& attacker)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_blockUsingShield))
        {
            if (!ModEvents::EntityBlockUsingShield(this->shared_from_this(), attacker))
                return;
        }
        base_Entity_blockUsingShield<Base>(attacker, 0);
    }

    template <class B>
    auto base_Entity_playHurtSound(const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::playHurtSound(source))
    {
        static_cast<B*>(this)->B::playHurtSound(source);
    }
    template <class B>
    void base_Entity_playHurtSound(const DamageSource* source, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::playHurtSound(source);
    }

    virtual void playHurtSound(const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_playHurtSound))
        {
            if (!ModEvents::EntityPlayHurtSound(this->shared_from_this(), const_cast<DamageSource*>(source)))
                return;
        }
        base_Entity_playHurtSound<Base>(source, 0);
    }

    template <class B>
    auto base_Entity_checkTotemDeathProtection(const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::checkTotemDeathProtection(source))
    {
        return static_cast<B*>(this)->B::checkTotemDeathProtection(source);
    }
    template <class B>
    bool base_Entity_checkTotemDeathProtection(const DamageSource* source, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::checkTotemDeathProtection(source);
    }

    virtual bool checkTotemDeathProtection(const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_checkTotemDeathProtection))
        {
            bool result = false;
            if (!ModEvents::EntityCheckTotemDeathProtection(this->shared_from_this(), const_cast<DamageSource*>(source), result))
                return result;
        }
        return base_Entity_checkTotemDeathProtection<Base>(source, 0);
    }

    template <class B>
    auto base_Entity_isDamageSourceBlocked(const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::isDamageSourceBlocked(source))
    {
        return static_cast<B*>(this)->B::isDamageSourceBlocked(source);
    }
    template <class B>
    bool base_Entity_isDamageSourceBlocked(const DamageSource* source, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isDamageSourceBlocked(source);
    }

    virtual bool isDamageSourceBlocked(const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isDamageSourceBlocked))
        {
            bool result = false;
            if (!ModEvents::EntityIsDamageSourceBlocked(this->shared_from_this(), const_cast<DamageSource*>(source), result))
                return result;
        }
        return base_Entity_isDamageSourceBlocked<Base>(source, 0);
    }

    template <class B>
    auto base_Entity_breakItem(not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::breakItem(itemInstance))
    {
        static_cast<B*>(this)->B::breakItem(itemInstance);
    }
    template <class B>
    void base_Entity_breakItem(not_null_ptr<ItemInstance> itemInstance, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::breakItem(itemInstance);
    }

    virtual void breakItem(not_null_ptr<ItemInstance> itemInstance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_breakItem))
        {
            if (!ModEvents::EntityBreakItem(this->shared_from_this(), const_cast<ItemInstance*>(itemInstance.get())))
                return;
        }
        base_Entity_breakItem<Base>(itemInstance, 0);
    }

    template <class B>
    auto base_Entity_die(const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::die(source))
    {
        static_cast<B*>(this)->B::die(source);
    }
    template <class B>
    void base_Entity_die(const DamageSource* source, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::die(source);
    }

    virtual void die(const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_die))
        {
            if (!ModEvents::EntityDie(this->shared_from_this(), const_cast<DamageSource*>(source)))
                return;
        }
        base_Entity_die<Base>(source, 0);
    }

    template <class B>
    auto base_Entity_dropEquipment(bool hitByPlayer, int lootingLevel, int) -> decltype(static_cast<B*>(this)->B::dropEquipment(hitByPlayer, lootingLevel))
    {
        static_cast<B*>(this)->B::dropEquipment(hitByPlayer, lootingLevel);
    }
    template <class B>
    void base_Entity_dropEquipment(bool hitByPlayer, int lootingLevel, long)
    {
        static_cast<Mob*>(this)->Mob::dropEquipment(hitByPlayer, lootingLevel);
    }

    virtual void dropEquipment(bool hitByPlayer, int lootingLevel)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_dropEquipment))
        {
            if (!ModEvents::EntityDropEquipment(this->shared_from_this(), hitByPlayer, lootingLevel))
                return;
        }
        base_Entity_dropEquipment<Base>(hitByPlayer, lootingLevel, 0);
    }

    template <class B>
    auto base_Entity_knockback(boost::shared_ptr<Entity> attacker, float strength, double x, double z, int) -> decltype(static_cast<B*>(this)->B::knockback(attacker, strength, x, z))
    {
        static_cast<B*>(this)->B::knockback(attacker, strength, x, z);
    }
    template <class B>
    void base_Entity_knockback(boost::shared_ptr<Entity> attacker, float strength, double x, double z, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::knockback(attacker, strength, x, z);
    }

    virtual void knockback(boost::shared_ptr<Entity> attacker, float strength, double x, double z)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_knockback))
        {
            if (!ModEvents::EntityKnockback(this->shared_from_this(), attacker, strength, static_cast<float>(x), static_cast<float>(z)))
                return;
        }
        base_Entity_knockback<Base>(attacker, strength, x, z, 0);
    }

    template <class B>
    auto base_Entity_getHurtSound(const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::getHurtSound(source))
    {
        return static_cast<B*>(this)->B::getHurtSound(source);
    }
    template <class B>
    const SoundEvent* base_Entity_getHurtSound(const DamageSource* source, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getHurtSound(source);
    }

    virtual const SoundEvent* getHurtSound(const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getHurtSound))
        {
            SoundEvent* result = NULL;
            if (!ModEvents::EntityGetHurtSound(this->shared_from_this(), const_cast<DamageSource*>(source), result))
                return result;
        }
        return base_Entity_getHurtSound<Base>(source, 0);
    }

    template <class B>
    auto base_Entity_getDeathSound(int) -> decltype(static_cast<B*>(this)->B::getDeathSound())
    {
        return static_cast<B*>(this)->B::getDeathSound();
    }
    template <class B>
    const SoundEvent* base_Entity_getDeathSound(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getDeathSound();
    }

    virtual const SoundEvent* getDeathSound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDeathSound))
        {
            SoundEvent* result = NULL;
            if (!ModEvents::EntityGetDeathSound(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getDeathSound<Base>(0);
    }

    template <class B>
    auto base_Entity_getFallDamageSound(int distance, int) -> decltype(static_cast<B*>(this)->B::getFallDamageSound(distance))
    {
        return static_cast<B*>(this)->B::getFallDamageSound(distance);
    }
    template <class B>
    const SoundEvent* base_Entity_getFallDamageSound(int distance, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getFallDamageSound(distance);
    }

    virtual const SoundEvent* getFallDamageSound(int distance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getFallDamageSound))
        {
            SoundEvent* result = NULL;
            if (!ModEvents::EntityGetFallDamageSound(this->shared_from_this(), distance, result))
                return result;
        }
        return base_Entity_getFallDamageSound<Base>(distance, 0);
    }

    template <class B>
    auto base_Entity_dropAllDeathLoot(bool hitByPlayer, int lootingLevel, const DamageSource* source, int) -> decltype(static_cast<B*>(this)->B::dropAllDeathLoot(hitByPlayer, lootingLevel, source))
    {
        static_cast<B*>(this)->B::dropAllDeathLoot(hitByPlayer, lootingLevel, source);
    }
    template <class B>
    void base_Entity_dropAllDeathLoot(bool hitByPlayer, int lootingLevel, const DamageSource* source, long)
    {
        static_cast<Mob*>(this)->Mob::dropAllDeathLoot(hitByPlayer, lootingLevel, source);
    }

    virtual void dropAllDeathLoot(bool hitByPlayer, int lootingLevel, const DamageSource* source)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_dropAllDeathLoot))
        {
            if (!ModEvents::EntityDropAllDeathLoot(this->shared_from_this(), hitByPlayer, lootingLevel, const_cast<DamageSource*>(source)))
                return;
        }
        base_Entity_dropAllDeathLoot<Base>(hitByPlayer, lootingLevel, source, 0);
    }

    template <class B>
    auto base_Entity_dropDeathLoot(bool hitByPlayer, int lootingLevel, int) -> decltype(static_cast<B*>(this)->B::dropDeathLoot(hitByPlayer, lootingLevel))
    {
        static_cast<B*>(this)->B::dropDeathLoot(hitByPlayer, lootingLevel);
    }
    template <class B>
    void base_Entity_dropDeathLoot(bool hitByPlayer, int lootingLevel, long)
    {
        static_cast<Mob*>(this)->Mob::dropDeathLoot(hitByPlayer, lootingLevel);
    }

    virtual void dropDeathLoot(bool hitByPlayer, int lootingLevel)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_dropDeathLoot))
        {
            if (!ModEvents::EntityDropDeathLoot(this->shared_from_this(), hitByPlayer, lootingLevel))
                return;
        }
        base_Entity_dropDeathLoot<Base>(hitByPlayer, lootingLevel, 0);
    }

    template <class B>
    auto base_Entity_onLadder(int) -> decltype(static_cast<B*>(this)->B::onLadder())
    {
        return static_cast<B*>(this)->B::onLadder();
    }
    template <class B>
    bool base_Entity_onLadder(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::onLadder();
    }

    virtual bool onLadder()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onLadder))
        {
            bool result = false;
            if (!ModEvents::EntityOnLadder(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_onLadder<Base>(0);
    }

    template <class B>
    auto base_Entity_isShootable(int) -> decltype(static_cast<B*>(this)->B::isShootable())
    {
        return static_cast<B*>(this)->B::isShootable();
    }
    template <class B>
    bool base_Entity_isShootable(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isShootable();
    }

    virtual bool isShootable()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isShootable))
        {
            bool result = false;
            if (!ModEvents::EntityIsShootable(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isShootable<Base>(0);
    }

    template <class B>
    auto base_Entity_getArmorValue(int) -> decltype(static_cast<B*>(this)->B::getArmorValue())
    {
        return static_cast<B*>(this)->B::getArmorValue();
    }
    template <class B>
    int base_Entity_getArmorValue(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getArmorValue();
    }

    virtual int getArmorValue()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getArmorValue))
        {
            int result = 0;
            if (!ModEvents::EntityGetArmorValue(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getArmorValue<Base>(0);
    }

    template <class B>
    auto base_Entity_hurtArmor(float damage, int) -> decltype(static_cast<B*>(this)->B::hurtArmor(damage))
    {
        static_cast<B*>(this)->B::hurtArmor(damage);
    }
    template <class B>
    void base_Entity_hurtArmor(float damage, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::hurtArmor(damage);
    }

    virtual void hurtArmor(float damage)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hurtArmor))
        {
            if (!ModEvents::EntityHurtArmor(this->shared_from_this(), damage))
                return;
        }
        base_Entity_hurtArmor<Base>(damage, 0);
    }

    template <class B>
    auto base_Entity_hurtCurrentlyUsedShield(float damage, int) -> decltype(static_cast<B*>(this)->B::hurtCurrentlyUsedShield(damage))
    {
        static_cast<B*>(this)->B::hurtCurrentlyUsedShield(damage);
    }
    template <class B>
    void base_Entity_hurtCurrentlyUsedShield(float damage, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::hurtCurrentlyUsedShield(damage);
    }

    virtual void hurtCurrentlyUsedShield(float damage)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hurtCurrentlyUsedShield))
        {
            if (!ModEvents::EntityHurtCurrentlyUsedShield(this->shared_from_this(), damage))
                return;
        }
        base_Entity_hurtCurrentlyUsedShield<Base>(damage, 0);
    }

    template <class B>
    auto base_Entity_getDamageAfterArmorAbsorb(const DamageSource* source, float damage, int) -> decltype(static_cast<B*>(this)->B::getDamageAfterArmorAbsorb(source, damage))
    {
        return static_cast<B*>(this)->B::getDamageAfterArmorAbsorb(source, damage);
    }
    template <class B>
    float base_Entity_getDamageAfterArmorAbsorb(const DamageSource* source, float damage, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getDamageAfterArmorAbsorb(source, damage);
    }

    virtual float getDamageAfterArmorAbsorb(const DamageSource* source, float damage)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDamageAfterArmorAbsorb))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetDamageAfterArmorAbsorb(this->shared_from_this(), const_cast<DamageSource*>(source), damage, result))
                return result;
        }
        return base_Entity_getDamageAfterArmorAbsorb<Base>(source, damage, 0);
    }

    template <class B>
    auto base_Entity_getDamageAfterMagicAbsorb(const DamageSource* source, float damage, int) -> decltype(static_cast<B*>(this)->B::getDamageAfterMagicAbsorb(source, damage))
    {
        return static_cast<B*>(this)->B::getDamageAfterMagicAbsorb(source, damage);
    }
    template <class B>
    float base_Entity_getDamageAfterMagicAbsorb(const DamageSource* source, float damage, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getDamageAfterMagicAbsorb(source, damage);
    }

    virtual float getDamageAfterMagicAbsorb(const DamageSource* source, float damage)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDamageAfterMagicAbsorb))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetDamageAfterMagicAbsorb(this->shared_from_this(), const_cast<DamageSource*>(source), damage, result))
                return result;
        }
        return base_Entity_getDamageAfterMagicAbsorb<Base>(source, damage, 0);
    }

    template <class B>
    auto base_Entity_actuallyHurt(const DamageSource* source, float damage, int) -> decltype(static_cast<B*>(this)->B::actuallyHurt(source, damage))
    {
        static_cast<B*>(this)->B::actuallyHurt(source, damage);
    }
    template <class B>
    void base_Entity_actuallyHurt(const DamageSource* source, float damage, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::actuallyHurt(source, damage);
    }

    virtual void actuallyHurt(const DamageSource* source, float damage)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_actuallyHurt))
        {
            if (!ModEvents::EntityActuallyHurt(this->shared_from_this(), const_cast<DamageSource*>(source), damage))
                return;
        }
        base_Entity_actuallyHurt<Base>(source, damage, 0);
    }

    template <class B>
    auto base_Entity_getCombatTracker(int) -> decltype(static_cast<B*>(this)->B::getCombatTracker())
    {
        return static_cast<B*>(this)->B::getCombatTracker();
    }
    template <class B>
    CombatTracker* base_Entity_getCombatTracker(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getCombatTracker();
    }

    virtual CombatTracker* getCombatTracker()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getCombatTracker))
        {
            CombatTracker* result = NULL;
            if (!ModEvents::EntityGetCombatTracker(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getCombatTracker<Base>(0);
    }

    template <class B>
    auto base_Entity_getMaxHealth(int) -> decltype(static_cast<B*>(this)->B::getMaxHealth())
    {
        return static_cast<B*>(this)->B::getMaxHealth();
    }
    template <class B>
    float base_Entity_getMaxHealth(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getMaxHealth();
    }

    virtual float getMaxHealth()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMaxHealth))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetMaxHealth(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMaxHealth<Base>(0);
    }

    template <class B>
    auto base_Entity_getArrowCount(int) -> decltype(static_cast<B*>(this)->B::getArrowCount())
    {
        return static_cast<B*>(this)->B::getArrowCount();
    }
    template <class B>
    int base_Entity_getArrowCount(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getArrowCount();
    }

    virtual int getArrowCount()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getArrowCount))
        {
            int result = 0;
            if (!ModEvents::EntityGetArrowCount(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getArrowCount<Base>(0);
    }

    template <class B>
    auto base_Entity_setArrowCount(int arrowCount, int) -> decltype(static_cast<B*>(this)->B::setArrowCount(arrowCount))
    {
        static_cast<B*>(this)->B::setArrowCount(arrowCount);
    }
    template <class B>
    void base_Entity_setArrowCount(int arrowCount, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setArrowCount(arrowCount);
    }

    virtual void setArrowCount(int arrowCount)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setArrowCount))
        {
            if (!ModEvents::EntitySetArrowCount(this->shared_from_this(), arrowCount))
                return;
        }
        base_Entity_setArrowCount<Base>(arrowCount, 0);
    }

    template <class B>
    auto base_Entity_swing(InteractionHand::EInteractionHand hand, int) -> decltype(static_cast<B*>(this)->B::swing(hand))
    {
        return static_cast<B*>(this)->B::swing(hand);
    }
    template <class B>
    bool base_Entity_swing(InteractionHand::EInteractionHand hand, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::swing(hand);
    }

    virtual bool swing(InteractionHand::EInteractionHand hand)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_swing))
        {
            bool result = false;
            if (!ModEvents::EntitySwing(this->shared_from_this(), static_cast<int>(hand), result))
                return result;
        }
        return base_Entity_swing<Base>(hand, 0);
    }

    template <class B>
    auto base_Entity_updateSwingTime(int) -> decltype(static_cast<B*>(this)->B::updateSwingTime())
    {
        static_cast<B*>(this)->B::updateSwingTime();
    }
    template <class B>
    void base_Entity_updateSwingTime(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::updateSwingTime();
    }

    virtual void updateSwingTime()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_updateSwingTime))
        {
            if (!ModEvents::EntityUpdateSwingTime(this->shared_from_this()))
                return;
        }
        base_Entity_updateSwingTime<Base>(0);
    }

    template <class B>
    auto base_Entity_getAttribute(Attribute* attribute, int) -> decltype(static_cast<B*>(this)->B::getAttribute(attribute))
    {
        return static_cast<B*>(this)->B::getAttribute(attribute);
    }
    template <class B>
    AttributeInstance* base_Entity_getAttribute(Attribute* attribute, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getAttribute(attribute);
    }

    virtual AttributeInstance* getAttribute(Attribute* attribute)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAttribute))
        {
            AttributeInstance* result = NULL;
            if (!ModEvents::EntityGetAttribute(this->shared_from_this(), const_cast<Attribute*>(attribute), result))
                return result;
        }
        return base_Entity_getAttribute<Base>(attribute, 0);
    }

    template <class B>
    auto base_Entity_getAttributes(int) -> decltype(static_cast<B*>(this)->B::getAttributes())
    {
        return static_cast<B*>(this)->B::getAttributes();
    }
    template <class B>
    BaseAttributeMap* base_Entity_getAttributes(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getAttributes();
    }

    virtual BaseAttributeMap* getAttributes()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAttributes))
        {
            BaseAttributeMap* result = NULL;
            if (!ModEvents::EntityGetAttributes(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getAttributes<Base>(0);
    }

    template <class B>
    auto base_Entity_getMobType(int) -> decltype(static_cast<B*>(this)->B::getMobType())
    {
        return static_cast<B*>(this)->B::getMobType();
    }
    template <class B>
    EMobType base_Entity_getMobType(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getMobType();
    }

    virtual EMobType getMobType()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMobType))
        {
            int result = 0;
            if (!ModEvents::EntityGetMobType(this->shared_from_this(), result))
                return static_cast<EMobType>(result);
        }
        return base_Entity_getMobType<Base>(0);
    }

    template <class B>
    auto base_Entity_setItemInHand(InteractionHand::EInteractionHand hand, not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::setItemInHand(hand, itemInstance))
    {
        static_cast<B*>(this)->B::setItemInHand(hand, itemInstance);
    }
    template <class B>
    void base_Entity_setItemInHand(InteractionHand::EInteractionHand hand, not_null_ptr<ItemInstance> itemInstance, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setItemInHand(hand, itemInstance);
    }

    virtual void setItemInHand(InteractionHand::EInteractionHand hand, not_null_ptr<ItemInstance> itemInstance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setItemInHand))
        {
            if (!ModEvents::EntitySetItemInHand(this->shared_from_this(), static_cast<int>(hand), const_cast<ItemInstance*>(itemInstance.get())))
                return;
        }
        base_Entity_setItemInHand<Base>(hand, itemInstance, 0);
    }

    template <class B>
    auto base_Entity_hasItemInSlot(const EquipmentSlot* slot, int) -> decltype(static_cast<B*>(this)->B::hasItemInSlot(slot))
    {
        return static_cast<B*>(this)->B::hasItemInSlot(slot);
    }
    template <class B>
    bool base_Entity_hasItemInSlot(const EquipmentSlot* slot, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::hasItemInSlot(slot);
    }

    virtual bool hasItemInSlot(const EquipmentSlot* slot)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_hasItemInSlot))
        {
            bool result = false;
            if (!ModEvents::EntityHasItemInSlot(this->shared_from_this(), const_cast<EquipmentSlot*>(slot), result))
                return result;
        }
        return base_Entity_hasItemInSlot<Base>(slot, 0);
    }

    template <class B>
    auto base_Entity_getItemInHandIcon(not_null_ptr<ItemInstance> itemInstance, int frame, int) -> decltype(static_cast<B*>(this)->B::getItemInHandIcon(itemInstance, frame))
    {
        return static_cast<B*>(this)->B::getItemInHandIcon(itemInstance, frame);
    }
    template <class B>
    TextureAtlasSprite* base_Entity_getItemInHandIcon(not_null_ptr<ItemInstance> itemInstance, int frame, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getItemInHandIcon(itemInstance, frame);
    }

    virtual TextureAtlasSprite* getItemInHandIcon(not_null_ptr<ItemInstance> itemInstance, int frame)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getItemInHandIcon))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::EntityGetItemInHandIcon(this->shared_from_this(), const_cast<ItemInstance*>(itemInstance.get()), frame, result))
                return result;
        }
        return base_Entity_getItemInHandIcon<Base>(itemInstance, frame, 0);
    }

    template <class B>
    auto base_Entity_getSoundVolume(int) -> decltype(static_cast<B*>(this)->B::getSoundVolume())
    {
        return static_cast<B*>(this)->B::getSoundVolume();
    }
    template <class B>
    float base_Entity_getSoundVolume(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getSoundVolume();
    }

    virtual float getSoundVolume()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSoundVolume))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetSoundVolume(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getSoundVolume<Base>(0);
    }

    template <class B>
    auto base_Entity_getVoicePitch(int) -> decltype(static_cast<B*>(this)->B::getVoicePitch())
    {
        return static_cast<B*>(this)->B::getVoicePitch();
    }
    template <class B>
    float base_Entity_getVoicePitch(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getVoicePitch();
    }

    virtual float getVoicePitch()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getVoicePitch))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetVoicePitch(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getVoicePitch<Base>(0);
    }

    template <class B>
    auto base_Entity_isImmobile(int) -> decltype(static_cast<B*>(this)->B::isImmobile())
    {
        return static_cast<B*>(this)->B::isImmobile();
    }
    template <class B>
    bool base_Entity_isImmobile(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isImmobile();
    }

    virtual bool isImmobile()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isImmobile))
        {
            bool result = false;
            if (!ModEvents::EntityIsImmobile(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isImmobile<Base>(0);
    }

    template <class B>
    auto base_Entity_findStandUpPosition(boost::shared_ptr<Entity> vehicle, int) -> decltype(static_cast<B*>(this)->B::findStandUpPosition(vehicle))
    {
        static_cast<B*>(this)->B::findStandUpPosition(vehicle);
    }
    template <class B>
    void base_Entity_findStandUpPosition(boost::shared_ptr<Entity> vehicle, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::findStandUpPosition(vehicle);
    }

    virtual void findStandUpPosition(boost::shared_ptr<Entity> vehicle)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_findStandUpPosition))
        {
            if (!ModEvents::EntityFindStandUpPosition(this->shared_from_this(), vehicle))
                return;
        }
        base_Entity_findStandUpPosition<Base>(vehicle, 0);
    }

    template <class B>
    auto base_Entity_getJumpPower(int) -> decltype(static_cast<B*>(this)->B::getJumpPower())
    {
        return static_cast<B*>(this)->B::getJumpPower();
    }
    template <class B>
    float base_Entity_getJumpPower(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getJumpPower();
    }

    virtual float getJumpPower()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getJumpPower))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetJumpPower(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getJumpPower<Base>(0);
    }

    template <class B>
    auto base_Entity_jumpFromGround(int) -> decltype(static_cast<B*>(this)->B::jumpFromGround())
    {
        static_cast<B*>(this)->B::jumpFromGround();
    }
    template <class B>
    void base_Entity_jumpFromGround(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::jumpFromGround();
    }

    virtual void jumpFromGround()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_jumpFromGround))
        {
            if (!ModEvents::EntityJumpFromGround(this->shared_from_this()))
                return;
        }
        base_Entity_jumpFromGround<Base>(0);
    }

    template <class B>
    auto base_Entity_jumpInWater(int) -> decltype(static_cast<B*>(this)->B::jumpInWater())
    {
        static_cast<B*>(this)->B::jumpInWater();
    }
    template <class B>
    void base_Entity_jumpInWater(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::jumpInWater();
    }

    virtual void jumpInWater()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_jumpInWater))
        {
            if (!ModEvents::EntityJumpInWater(this->shared_from_this()))
                return;
        }
        base_Entity_jumpInWater<Base>(0);
    }

    template <class B>
    auto base_Entity_goDownInWater(int) -> decltype(static_cast<B*>(this)->B::goDownInWater())
    {
        static_cast<B*>(this)->B::goDownInWater();
    }
    template <class B>
    void base_Entity_goDownInWater(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::goDownInWater();
    }

    virtual void goDownInWater()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_goDownInWater))
        {
            if (!ModEvents::EntityGoDownInWater(this->shared_from_this()))
                return;
        }
        base_Entity_goDownInWater<Base>(0);
    }

    template <class B>
    auto base_Entity_jumpInLava(int) -> decltype(static_cast<B*>(this)->B::jumpInLava())
    {
        static_cast<B*>(this)->B::jumpInLava();
    }
    template <class B>
    void base_Entity_jumpInLava(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::jumpInLava();
    }

    virtual void jumpInLava()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_jumpInLava))
        {
            if (!ModEvents::EntityJumpInLava(this->shared_from_this()))
                return;
        }
        base_Entity_jumpInLava<Base>(0);
    }

    template <class B>
    auto base_Entity_getWaterSlowDown(int) -> decltype(static_cast<B*>(this)->B::getWaterSlowDown())
    {
        return static_cast<B*>(this)->B::getWaterSlowDown();
    }
    template <class B>
    float base_Entity_getWaterSlowDown(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getWaterSlowDown();
    }

    virtual float getWaterSlowDown()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getWaterSlowDown))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetWaterSlowDown(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getWaterSlowDown<Base>(0);
    }

    template <class B>
    auto base_Entity_travel(float xxa, float yya, float zza, int) -> decltype(static_cast<B*>(this)->B::travel(xxa, yya, zza))
    {
        static_cast<B*>(this)->B::travel(xxa, yya, zza);
    }
    template <class B>
    void base_Entity_travel(float xxa, float yya, float zza, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::travel(xxa, yya, zza);
    }

    virtual void travel(float xxa, float yya, float zza)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_travel))
        {
            if (!ModEvents::EntityTravel(this->shared_from_this(), xxa, yya, zza))
                return;
        }
        base_Entity_travel<Base>(xxa, yya, zza, 0);
    }

    template <class B>
    auto base_Entity_updateModelAnimation(int) -> decltype(static_cast<B*>(this)->B::updateModelAnimation())
    {
        static_cast<B*>(this)->B::updateModelAnimation();
    }
    template <class B>
    void base_Entity_updateModelAnimation(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::updateModelAnimation();
    }

    virtual void updateModelAnimation()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_updateModelAnimation))
        {
            if (!ModEvents::EntityUpdateModelAnimation(this->shared_from_this()))
                return;
        }
        base_Entity_updateModelAnimation<Base>(0);
    }

    template <class B>
    auto base_Entity_getSpeed(int) -> decltype(static_cast<B*>(this)->B::getSpeed())
    {
        return static_cast<B*>(this)->B::getSpeed();
    }
    template <class B>
    float base_Entity_getSpeed(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getSpeed();
    }

    virtual float getSpeed()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSpeed))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetSpeed(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getSpeed<Base>(0);
    }

    template <class B>
    auto base_Entity_setSpeed(float speed, int) -> decltype(static_cast<B*>(this)->B::setSpeed(speed))
    {
        static_cast<B*>(this)->B::setSpeed(speed);
    }
    template <class B>
    void base_Entity_setSpeed(float speed, long)
    {
        static_cast<Mob*>(this)->Mob::setSpeed(speed);
    }

    virtual void setSpeed(float speed)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setSpeed))
        {
            if (!ModEvents::EntitySetSpeed(this->shared_from_this(), speed))
                return;
        }
        base_Entity_setSpeed<Base>(speed, 0);
    }

    template <class B>
    auto base_Entity_doHurtTarget(boost::shared_ptr<Entity> target, int) -> decltype(static_cast<B*>(this)->B::doHurtTarget(target))
    {
        return static_cast<B*>(this)->B::doHurtTarget(target);
    }
    template <class B>
    bool base_Entity_doHurtTarget(boost::shared_ptr<Entity> target, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::doHurtTarget(target);
    }

    virtual bool doHurtTarget(boost::shared_ptr<Entity> target)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_doHurtTarget))
        {
            bool result = false;
            if (!ModEvents::EntityDoHurtTarget(this->shared_from_this(), target, result))
                return result;
        }
        return base_Entity_doHurtTarget<Base>(target, 0);
    }

    template <class B>
    auto base_Entity_isSleeping(int) -> decltype(static_cast<B*>(this)->B::isSleeping())
    {
        return static_cast<B*>(this)->B::isSleeping();
    }
    template <class B>
    bool base_Entity_isSleeping(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isSleeping();
    }

    virtual bool isSleeping()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isSleeping))
        {
            bool result = false;
            if (!ModEvents::EntityIsSleeping(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isSleeping<Base>(0);
    }

    template <class B>
    auto base_Entity_tickHeadTurn(float yBodyRot, float headTurnDelta, int) -> decltype(static_cast<B*>(this)->B::tickHeadTurn(yBodyRot, headTurnDelta))
    {
        return static_cast<B*>(this)->B::tickHeadTurn(yBodyRot, headTurnDelta);
    }
    template <class B>
    float base_Entity_tickHeadTurn(float yBodyRot, float headTurnDelta, long)
    {
        return static_cast<Mob*>(this)->Mob::tickHeadTurn(yBodyRot, headTurnDelta);
    }

    virtual float tickHeadTurn(float yBodyRot, float headTurnDelta)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_tickHeadTurn))
        {
            float result = 0.0f;
            if (!ModEvents::EntityTickHeadTurn(this->shared_from_this(), yBodyRot, headTurnDelta, result))
                return result;
        }
        return base_Entity_tickHeadTurn<Base>(yBodyRot, headTurnDelta, 0);
    }

    template <class B>
    auto base_Entity_aiStep(int) -> decltype(static_cast<B*>(this)->B::aiStep())
    {
        static_cast<B*>(this)->B::aiStep();
    }
    template <class B>
    void base_Entity_aiStep(long)
    {
        static_cast<Mob*>(this)->Mob::aiStep();
    }

    virtual void aiStep()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_aiStep))
        {
            if (!ModEvents::EntityAiStep(this->shared_from_this()))
                return;
        }
        base_Entity_aiStep<Base>(0);
    }

    template <class B>
    auto base_Entity_serverAiStep(int) -> decltype(static_cast<B*>(this)->B::serverAiStep())
    {
        static_cast<B*>(this)->B::serverAiStep();
    }
    template <class B>
    void base_Entity_serverAiStep(long)
    {
        static_cast<Mob*>(this)->Mob::serverAiStep();
    }

    virtual void serverAiStep()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_serverAiStep))
        {
            if (!ModEvents::EntityServerAiStep(this->shared_from_this()))
                return;
        }
        base_Entity_serverAiStep<Base>(0);
    }

    template <class B>
    auto base_Entity_pushEntities(int) -> decltype(static_cast<B*>(this)->B::pushEntities())
    {
        static_cast<B*>(this)->B::pushEntities();
    }
    template <class B>
    void base_Entity_pushEntities(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::pushEntities();
    }

    virtual void pushEntities()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_pushEntities))
        {
            if (!ModEvents::EntityPushEntities(this->shared_from_this()))
                return;
        }
        base_Entity_pushEntities<Base>(0);
    }

    template <class B>
    auto base_Entity_doPush(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::doPush(entity))
    {
        static_cast<B*>(this)->B::doPush(entity);
    }
    template <class B>
    void base_Entity_doPush(boost::shared_ptr<Entity> entity, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::doPush(entity);
    }

    virtual void doPush(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_doPush))
        {
            if (!ModEvents::EntityDoPush(this->shared_from_this(), entity))
                return;
        }
        base_Entity_doPush<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_setJumping(bool jumping, int) -> decltype(static_cast<B*>(this)->B::setJumping(jumping))
    {
        static_cast<B*>(this)->B::setJumping(jumping);
    }
    template <class B>
    void base_Entity_setJumping(bool jumping, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setJumping(jumping);
    }

    virtual void setJumping(bool jumping)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setJumping))
        {
            if (!ModEvents::EntitySetJumping(this->shared_from_this(), jumping))
                return;
        }
        base_Entity_setJumping<Base>(jumping, 0);
    }

    template <class B>
    auto base_Entity_take(boost::shared_ptr<Entity> entity, int count, int) -> decltype(static_cast<B*>(this)->B::take(entity, count))
    {
        static_cast<B*>(this)->B::take(entity, count);
    }
    template <class B>
    void base_Entity_take(boost::shared_ptr<Entity> entity, int count, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::take(entity, count);
    }

    virtual void take(boost::shared_ptr<Entity> entity, int count)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_take))
        {
            if (!ModEvents::EntityTake(this->shared_from_this(), entity, count))
                return;
        }
        base_Entity_take<Base>(entity, count, 0);
    }

    template <class B>
    auto base_Entity_canSee(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::canSee(entity))
    {
        return static_cast<B*>(this)->B::canSee(entity);
    }
    template <class B>
    bool base_Entity_canSee(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::canSee(entity);
    }

    virtual bool canSee(boost::shared_ptr<Entity> entity)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canSee))
        {
            bool result = false;
            if (!ModEvents::EntityCanSee(this->shared_from_this(), entity, result))
                return result;
        }
        return base_Entity_canSee<Base>(entity, 0);
    }

    template <class B>
    auto base_Entity_getAttackAnim(float partialTicks, int) -> decltype(static_cast<B*>(this)->B::getAttackAnim(partialTicks))
    {
        return static_cast<B*>(this)->B::getAttackAnim(partialTicks);
    }
    template <class B>
    float base_Entity_getAttackAnim(float partialTicks, long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getAttackAnim(partialTicks);
    }

    virtual float getAttackAnim(float partialTicks)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAttackAnim))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetAttackAnim(this->shared_from_this(), partialTicks, result))
                return result;
        }
        return base_Entity_getAttackAnim<Base>(partialTicks, 0);
    }

    template <class B>
    auto base_Entity_getSweptVolume(int) -> decltype(static_cast<B*>(this)->B::getSweptVolume())
    {
        return static_cast<B*>(this)->B::getSweptVolume();
    }
    template <class B>
    AABB* base_Entity_getSweptVolume(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getSweptVolume();
    }

    virtual AABB* getSweptVolume()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSweptVolume))
        {
            AABB* result = NULL;
            if (!ModEvents::EntityGetSweptVolume(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getSweptVolume<Base>(0);
    }

    template <class B>
    auto base_Entity_isEffectiveAi(int) -> decltype(static_cast<B*>(this)->B::isEffectiveAi())
    {
        return static_cast<B*>(this)->B::isEffectiveAi();
    }
    template <class B>
    bool base_Entity_isEffectiveAi(long)
    {
        return static_cast<Mob*>(this)->Mob::isEffectiveAi();
    }

    virtual bool isEffectiveAi()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isEffectiveAi))
        {
            bool result = false;
            if (!ModEvents::EntityIsEffectiveAi(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isEffectiveAi<Base>(0);
    }

    template <class B>
    auto base_Entity_getAbsorptionAmount(int) -> decltype(static_cast<B*>(this)->B::getAbsorptionAmount())
    {
        return static_cast<B*>(this)->B::getAbsorptionAmount();
    }
    template <class B>
    float base_Entity_getAbsorptionAmount(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getAbsorptionAmount();
    }

    virtual float getAbsorptionAmount()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAbsorptionAmount))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetAbsorptionAmount(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getAbsorptionAmount<Base>(0);
    }

    template <class B>
    auto base_Entity_setAbsorptionAmount(float absorptionAmount, int) -> decltype(static_cast<B*>(this)->B::setAbsorptionAmount(absorptionAmount))
    {
        static_cast<B*>(this)->B::setAbsorptionAmount(absorptionAmount);
    }
    template <class B>
    void base_Entity_setAbsorptionAmount(float absorptionAmount, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setAbsorptionAmount(absorptionAmount);
    }

    virtual void setAbsorptionAmount(float absorptionAmount)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setAbsorptionAmount))
        {
            if (!ModEvents::EntitySetAbsorptionAmount(this->shared_from_this(), absorptionAmount))
                return;
        }
        base_Entity_setAbsorptionAmount<Base>(absorptionAmount, 0);
    }

    template <class B>
    auto base_Entity_onEnterCombat(int) -> decltype(static_cast<B*>(this)->B::onEnterCombat())
    {
        static_cast<B*>(this)->B::onEnterCombat();
    }
    template <class B>
    void base_Entity_onEnterCombat(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::onEnterCombat();
    }

    virtual void onEnterCombat()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onEnterCombat))
        {
            if (!ModEvents::EntityOnEnterCombat(this->shared_from_this()))
                return;
        }
        base_Entity_onEnterCombat<Base>(0);
    }

    template <class B>
    auto base_Entity_onLeaveCombat(int) -> decltype(static_cast<B*>(this)->B::onLeaveCombat())
    {
        static_cast<B*>(this)->B::onLeaveCombat();
    }
    template <class B>
    void base_Entity_onLeaveCombat(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::onLeaveCombat();
    }

    virtual void onLeaveCombat()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_onLeaveCombat))
        {
            if (!ModEvents::EntityOnLeaveCombat(this->shared_from_this()))
                return;
        }
        base_Entity_onLeaveCombat<Base>(0);
    }

    template <class B>
    auto base_Entity_updateEffectVisibility(int) -> decltype(static_cast<B*>(this)->B::updateEffectVisibility())
    {
        static_cast<B*>(this)->B::updateEffectVisibility();
    }
    template <class B>
    void base_Entity_updateEffectVisibility(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::updateEffectVisibility();
    }

    virtual void updateEffectVisibility()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_updateEffectVisibility))
        {
            if (!ModEvents::EntityUpdateEffectVisibility(this->shared_from_this()))
                return;
        }
        base_Entity_updateEffectVisibility<Base>(0);
    }

    template <class B>
    auto base_Entity_isUsingItem(int) -> decltype(static_cast<B*>(this)->B::isUsingItem())
    {
        return static_cast<B*>(this)->B::isUsingItem();
    }
    template <class B>
    bool base_Entity_isUsingItem(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isUsingItem();
    }

    virtual bool isUsingItem()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isUsingItem))
        {
            bool result = false;
            if (!ModEvents::EntityIsUsingItem(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isUsingItem<Base>(0);
    }

    template <class B>
    auto base_Entity_getUsedItemHand(int) -> decltype(static_cast<B*>(this)->B::getUsedItemHand())
    {
        return static_cast<B*>(this)->B::getUsedItemHand();
    }
    template <class B>
    InteractionHand::EInteractionHand base_Entity_getUsedItemHand(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getUsedItemHand();
    }

    virtual InteractionHand::EInteractionHand getUsedItemHand()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getUsedItemHand))
        {
            int result = 0;
            if (!ModEvents::EntityGetUsedItemHand(this->shared_from_this(), result))
                return static_cast<InteractionHand::EInteractionHand>(result);
        }
        return base_Entity_getUsedItemHand<Base>(0);
    }

    template <class B>
    auto base_Entity_updatingUsingItem(int) -> decltype(static_cast<B*>(this)->B::updatingUsingItem())
    {
        static_cast<B*>(this)->B::updatingUsingItem();
    }
    template <class B>
    void base_Entity_updatingUsingItem(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::updatingUsingItem();
    }

    virtual void updatingUsingItem()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_updatingUsingItem))
        {
            if (!ModEvents::EntityUpdatingUsingItem(this->shared_from_this()))
                return;
        }
        base_Entity_updatingUsingItem<Base>(0);
    }

    template <class B>
    auto base_Entity_startUsingItem(InteractionHand::EInteractionHand hand, int) -> decltype(static_cast<B*>(this)->B::startUsingItem(hand))
    {
        static_cast<B*>(this)->B::startUsingItem(hand);
    }
    template <class B>
    void base_Entity_startUsingItem(InteractionHand::EInteractionHand hand, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::startUsingItem(hand);
    }

    virtual void startUsingItem(InteractionHand::EInteractionHand hand)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_startUsingItem))
        {
            if (!ModEvents::EntityStartUsingItem(this->shared_from_this(), static_cast<int>(hand)))
                return;
        }
        base_Entity_startUsingItem<Base>(hand, 0);
    }

    template <class B>
    auto base_Entity_spawnItemUseParticles(not_null_ptr<ItemInstance> itemInstance, int count, int) -> decltype(static_cast<B*>(this)->B::spawnItemUseParticles(itemInstance, count))
    {
        static_cast<B*>(this)->B::spawnItemUseParticles(itemInstance, count);
    }
    template <class B>
    void base_Entity_spawnItemUseParticles(not_null_ptr<ItemInstance> itemInstance, int count, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::spawnItemUseParticles(itemInstance, count);
    }

    virtual void spawnItemUseParticles(not_null_ptr<ItemInstance> itemInstance, int count)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_spawnItemUseParticles))
        {
            if (!ModEvents::EntitySpawnItemUseParticles(this->shared_from_this(), const_cast<ItemInstance*>(itemInstance.get()), count))
                return;
        }
        base_Entity_spawnItemUseParticles<Base>(itemInstance, count, 0);
    }

    template <class B>
    auto base_Entity_completeUsingItem(int) -> decltype(static_cast<B*>(this)->B::completeUsingItem())
    {
        static_cast<B*>(this)->B::completeUsingItem();
    }
    template <class B>
    void base_Entity_completeUsingItem(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::completeUsingItem();
    }

    virtual void completeUsingItem()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_completeUsingItem))
        {
            if (!ModEvents::EntityCompleteUsingItem(this->shared_from_this()))
                return;
        }
        base_Entity_completeUsingItem<Base>(0);
    }

    template <class B>
    auto base_Entity_getTicksUsingItem(int) -> decltype(static_cast<B*>(this)->B::getTicksUsingItem())
    {
        return static_cast<B*>(this)->B::getTicksUsingItem();
    }
    template <class B>
    int base_Entity_getTicksUsingItem(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getTicksUsingItem();
    }

    virtual int getTicksUsingItem()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getTicksUsingItem))
        {
            int result = 0;
            if (!ModEvents::EntityGetTicksUsingItem(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getTicksUsingItem<Base>(0);
    }

    template <class B>
    auto base_Entity_releaseUsingItem(int) -> decltype(static_cast<B*>(this)->B::releaseUsingItem())
    {
        static_cast<B*>(this)->B::releaseUsingItem();
    }
    template <class B>
    void base_Entity_releaseUsingItem(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::releaseUsingItem();
    }

    virtual void releaseUsingItem()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_releaseUsingItem))
        {
            if (!ModEvents::EntityReleaseUsingItem(this->shared_from_this()))
                return;
        }
        base_Entity_releaseUsingItem<Base>(0);
    }

    template <class B>
    auto base_Entity_releaseUsingItem_2(int useDuration, int) -> decltype(static_cast<B*>(this)->B::releaseUsingItem(useDuration))
    {
        static_cast<B*>(this)->B::releaseUsingItem(useDuration);
    }
    template <class B>
    void base_Entity_releaseUsingItem_2(int useDuration, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::releaseUsingItem(useDuration);
    }

    virtual void releaseUsingItem(int useDuration)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_releaseUsingItem_2))
        {
            if (!ModEvents::EntityReleaseUsingItem2(this->shared_from_this(), useDuration))
                return;
        }
        base_Entity_releaseUsingItem_2<Base>(useDuration, 0);
    }

    template <class B>
    auto base_Entity_stopUsingItem(int) -> decltype(static_cast<B*>(this)->B::stopUsingItem())
    {
        static_cast<B*>(this)->B::stopUsingItem();
    }
    template <class B>
    void base_Entity_stopUsingItem(long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::stopUsingItem();
    }

    virtual void stopUsingItem()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_stopUsingItem))
        {
            if (!ModEvents::EntityStopUsingItem(this->shared_from_this()))
                return;
        }
        base_Entity_stopUsingItem<Base>(0);
    }

    template <class B>
    auto base_Entity_isBlocking(int) -> decltype(static_cast<B*>(this)->B::isBlocking())
    {
        return static_cast<B*>(this)->B::isBlocking();
    }
    template <class B>
    bool base_Entity_isBlocking(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isBlocking();
    }

    virtual bool isBlocking()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isBlocking))
        {
            bool result = false;
            if (!ModEvents::EntityIsBlocking(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isBlocking<Base>(0);
    }

    template <class B>
    auto base_Entity_isFallFlying(int) -> decltype(static_cast<B*>(this)->B::isFallFlying())
    {
        return static_cast<B*>(this)->B::isFallFlying();
    }
    template <class B>
    bool base_Entity_isFallFlying(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isFallFlying();
    }

    virtual bool isFallFlying()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isFallFlying))
        {
            bool result = false;
            if (!ModEvents::EntityIsFallFlying(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isFallFlying<Base>(0);
    }

    template <class B>
    auto base_Entity_getFallFlyingTicks(int) -> decltype(static_cast<B*>(this)->B::getFallFlyingTicks())
    {
        return static_cast<B*>(this)->B::getFallFlyingTicks();
    }
    template <class B>
    int base_Entity_getFallFlyingTicks(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::getFallFlyingTicks();
    }

    virtual int getFallFlyingTicks()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getFallFlyingTicks))
        {
            int result = 0;
            if (!ModEvents::EntityGetFallFlyingTicks(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getFallFlyingTicks<Base>(0);
    }

    template <class B>
    auto base_Entity_isAffectedByPotions(int) -> decltype(static_cast<B*>(this)->B::isAffectedByPotions())
    {
        return static_cast<B*>(this)->B::isAffectedByPotions();
    }
    template <class B>
    bool base_Entity_isAffectedByPotions(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::isAffectedByPotions();
    }

    virtual bool isAffectedByPotions()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isAffectedByPotions))
        {
            bool result = false;
            if (!ModEvents::EntityIsAffectedByPotions(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isAffectedByPotions<Base>(0);
    }

    template <class B>
    auto base_Entity_attackable(int) -> decltype(static_cast<B*>(this)->B::attackable())
    {
        return static_cast<B*>(this)->B::attackable();
    }
    template <class B>
    bool base_Entity_attackable(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::attackable();
    }

    virtual bool attackable()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_attackable))
        {
            bool result = false;
            if (!ModEvents::EntityAttackable(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_attackable<Base>(0);
    }

    template <class B>
    auto base_Entity_setRecordPlayingNearby(const BlockPos& pos, bool playing, int) -> decltype(static_cast<B*>(this)->B::setRecordPlayingNearby(pos, playing))
    {
        static_cast<B*>(this)->B::setRecordPlayingNearby(pos, playing);
    }
    template <class B>
    void base_Entity_setRecordPlayingNearby(const BlockPos& pos, bool playing, long)
    {
        static_cast<LivingEntity*>(this)->LivingEntity::setRecordPlayingNearby(pos, playing);
    }

    virtual void setRecordPlayingNearby(const BlockPos& pos, bool playing)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setRecordPlayingNearby))
        {
            if (!ModEvents::EntitySetRecordPlayingNearby(this->shared_from_this(), pos.x, pos.y, pos.z, playing))
                return;
        }
        base_Entity_setRecordPlayingNearby<Base>(pos, playing, 0);
    }

    template <class B>
    auto base_Entity_IsCreativeFlying(int) -> decltype(static_cast<B*>(this)->B::IsCreativeFlying())
    {
        return static_cast<B*>(this)->B::IsCreativeFlying();
    }
    template <class B>
    bool base_Entity_IsCreativeFlying(long)
    {
        return static_cast<LivingEntity*>(this)->LivingEntity::IsCreativeFlying();
    }

    virtual bool IsCreativeFlying()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_IsCreativeFlying))
        {
            bool result = false;
            if (!ModEvents::EntityIsCreativeFlying(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_IsCreativeFlying<Base>(0);
    }

    template <class B>
    auto base_Entity_getMainArm(int) -> decltype(static_cast<B*>(this)->B::getMainArm())
    {
        return static_cast<B*>(this)->B::getMainArm();
    }
    template <class B>
    const HumanoidArm* base_Entity_getMainArm(long)
    {
        return static_cast<Mob*>(this)->Mob::getMainArm();
    }

    virtual const HumanoidArm* getMainArm()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMainArm))
        {
            HumanoidArm* result = NULL;
            if (!ModEvents::EntityGetMainArm(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMainArm<Base>(0);
    }

    template <class B>
    auto base_Entity_registerGoals(int) -> decltype(static_cast<B*>(this)->B::registerGoals())
    {
        static_cast<B*>(this)->B::registerGoals();
    }
    template <class B>
    void base_Entity_registerGoals(long)
    {
        static_cast<Mob*>(this)->Mob::registerGoals();
    }

    virtual void registerGoals()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_registerGoals))
        {
            if (!ModEvents::EntityRegisterGoals(this->shared_from_this()))
                return;
        }
        base_Entity_registerGoals<Base>(0);
    }

    template <class B>
    auto base_Entity_createNavigation(Level* level, int) -> decltype(static_cast<B*>(this)->B::createNavigation(level))
    {
        return static_cast<B*>(this)->B::createNavigation(level);
    }
    template <class B>
    PathNavigation* base_Entity_createNavigation(Level* level, long)
    {
        return static_cast<Mob*>(this)->Mob::createNavigation(level);
    }

    virtual PathNavigation* createNavigation(Level* level)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_createNavigation))
        {
            PathNavigation* result = NULL;
            if (!ModEvents::EntityCreateNavigation(this->shared_from_this(), level, result))
                return result;
        }
        return base_Entity_createNavigation<Base>(level, 0);
    }

    template <class B>
    auto base_Entity_getPathfindingMalus(const BlockPathTypes* type, int) -> decltype(static_cast<B*>(this)->B::getPathfindingMalus(type))
    {
        return static_cast<B*>(this)->B::getPathfindingMalus(type);
    }
    template <class B>
    float base_Entity_getPathfindingMalus(const BlockPathTypes* type, long)
    {
        return static_cast<Mob*>(this)->Mob::getPathfindingMalus(type);
    }

    virtual float getPathfindingMalus(const BlockPathTypes* type)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getPathfindingMalus))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetPathfindingMalus(this->shared_from_this(), const_cast<BlockPathTypes*>(type), result))
                return result;
        }
        return base_Entity_getPathfindingMalus<Base>(type, 0);
    }

    template <class B>
    auto base_Entity_setPathfindingMalus(const BlockPathTypes* type, float malus, int) -> decltype(static_cast<B*>(this)->B::setPathfindingMalus(type, malus))
    {
        static_cast<B*>(this)->B::setPathfindingMalus(type, malus);
    }
    template <class B>
    void base_Entity_setPathfindingMalus(const BlockPathTypes* type, float malus, long)
    {
        static_cast<Mob*>(this)->Mob::setPathfindingMalus(type, malus);
    }

    virtual void setPathfindingMalus(const BlockPathTypes* type, float malus)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setPathfindingMalus))
        {
            if (!ModEvents::EntitySetPathfindingMalus(this->shared_from_this(), const_cast<BlockPathTypes*>(type), malus))
                return;
        }
        base_Entity_setPathfindingMalus<Base>(type, malus, 0);
    }

    template <class B>
    auto base_Entity_createBodyControl(int) -> decltype(static_cast<B*>(this)->B::createBodyControl())
    {
        return static_cast<B*>(this)->B::createBodyControl();
    }
    template <class B>
    BodyControl* base_Entity_createBodyControl(long)
    {
        return static_cast<Mob*>(this)->Mob::createBodyControl();
    }

    virtual BodyControl* createBodyControl()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_createBodyControl))
        {
            BodyControl* result = NULL;
            if (!ModEvents::EntityCreateBodyControl(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_createBodyControl<Base>(0);
    }

    template <class B>
    auto base_Entity_getLookControl(int) -> decltype(static_cast<B*>(this)->B::getLookControl())
    {
        return static_cast<B*>(this)->B::getLookControl();
    }
    template <class B>
    LookControl* base_Entity_getLookControl(long)
    {
        return static_cast<Mob*>(this)->Mob::getLookControl();
    }

    virtual LookControl* getLookControl()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getLookControl))
        {
            LookControl* result = NULL;
            if (!ModEvents::EntityGetLookControl(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getLookControl<Base>(0);
    }

    template <class B>
    auto base_Entity_getMoveControl(int) -> decltype(static_cast<B*>(this)->B::getMoveControl())
    {
        return static_cast<B*>(this)->B::getMoveControl();
    }
    template <class B>
    MoveControl* base_Entity_getMoveControl(long)
    {
        return static_cast<Mob*>(this)->Mob::getMoveControl();
    }

    virtual MoveControl* getMoveControl()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMoveControl))
        {
            MoveControl* result = NULL;
            if (!ModEvents::EntityGetMoveControl(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMoveControl<Base>(0);
    }

    template <class B>
    auto base_Entity_getJumpControl(int) -> decltype(static_cast<B*>(this)->B::getJumpControl())
    {
        return static_cast<B*>(this)->B::getJumpControl();
    }
    template <class B>
    JumpControl* base_Entity_getJumpControl(long)
    {
        return static_cast<Mob*>(this)->Mob::getJumpControl();
    }

    virtual JumpControl* getJumpControl()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getJumpControl))
        {
            JumpControl* result = NULL;
            if (!ModEvents::EntityGetJumpControl(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getJumpControl<Base>(0);
    }

    template <class B>
    auto base_Entity_getNavigation(int) -> decltype(static_cast<B*>(this)->B::getNavigation())
    {
        return static_cast<B*>(this)->B::getNavigation();
    }
    template <class B>
    PathNavigation* base_Entity_getNavigation(long)
    {
        return static_cast<Mob*>(this)->Mob::getNavigation();
    }

    virtual PathNavigation* getNavigation()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getNavigation))
        {
            PathNavigation* result = NULL;
            if (!ModEvents::EntityGetNavigation(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getNavigation<Base>(0);
    }

    template <class B>
    auto base_Entity_getSensing(int) -> decltype(static_cast<B*>(this)->B::getSensing())
    {
        return static_cast<B*>(this)->B::getSensing();
    }
    template <class B>
    Sensing* base_Entity_getSensing(long)
    {
        return static_cast<Mob*>(this)->Mob::getSensing();
    }

    virtual Sensing* getSensing()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSensing))
        {
            Sensing* result = NULL;
            if (!ModEvents::EntityGetSensing(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getSensing<Base>(0);
    }

    template <class B>
    auto base_Entity_setTarget(boost::shared_ptr<LivingEntity> target, int) -> decltype(static_cast<B*>(this)->B::setTarget(target))
    {
        static_cast<B*>(this)->B::setTarget(target);
    }
    template <class B>
    void base_Entity_setTarget(boost::shared_ptr<LivingEntity> target, long)
    {
        static_cast<Mob*>(this)->Mob::setTarget(target);
    }

    virtual void setTarget(boost::shared_ptr<LivingEntity> target)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setTarget))
        {
            if (!ModEvents::EntitySetTarget(this->shared_from_this(), target))
                return;
        }
        base_Entity_setTarget<Base>(target, 0);
    }

    template <class B>
    auto base_Entity_canAttackType(unsigned long long type, int) -> decltype(static_cast<B*>(this)->B::canAttackType(type))
    {
        return static_cast<B*>(this)->B::canAttackType(type);
    }
    template <class B>
    bool base_Entity_canAttackType(unsigned long long type, long)
    {
        return static_cast<Mob*>(this)->Mob::canAttackType(type);
    }

    virtual bool canAttackType(unsigned long long type)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canAttackType))
        {
            bool result = false;
            if (!ModEvents::EntityCanAttackType(this->shared_from_this(), static_cast<int>(type), result))
                return result;
        }
        return base_Entity_canAttackType<Base>(type, 0);
    }

    template <class B>
    auto base_Entity_ate(int) -> decltype(static_cast<B*>(this)->B::ate())
    {
        static_cast<B*>(this)->B::ate();
    }
    template <class B>
    void base_Entity_ate(long)
    {
        static_cast<Mob*>(this)->Mob::ate();
    }

    virtual void ate()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_ate))
        {
            if (!ModEvents::EntityAte(this->shared_from_this()))
                return;
        }
        base_Entity_ate<Base>(0);
    }

    template <class B>
    auto base_Entity_getAmbientSoundInterval(int) -> decltype(static_cast<B*>(this)->B::getAmbientSoundInterval())
    {
        return static_cast<B*>(this)->B::getAmbientSoundInterval();
    }
    template <class B>
    int base_Entity_getAmbientSoundInterval(long)
    {
        return static_cast<Mob*>(this)->Mob::getAmbientSoundInterval();
    }

    virtual int getAmbientSoundInterval()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAmbientSoundInterval))
        {
            int result = 0;
            if (!ModEvents::EntityGetAmbientSoundInterval(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getAmbientSoundInterval<Base>(0);
    }

    template <class B>
    auto base_Entity_playAmbientSound(int) -> decltype(static_cast<B*>(this)->B::playAmbientSound())
    {
        static_cast<B*>(this)->B::playAmbientSound();
    }
    template <class B>
    void base_Entity_playAmbientSound(long)
    {
        static_cast<Mob*>(this)->Mob::playAmbientSound();
    }

    virtual void playAmbientSound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_playAmbientSound))
        {
            if (!ModEvents::EntityPlayAmbientSound(this->shared_from_this()))
                return;
        }
        base_Entity_playAmbientSound<Base>(0);
    }

    template <class B>
    auto base_Entity_resetAmbientSoundTime(int) -> decltype(static_cast<B*>(this)->B::resetAmbientSoundTime())
    {
        static_cast<B*>(this)->B::resetAmbientSoundTime();
    }
    template <class B>
    void base_Entity_resetAmbientSoundTime(long)
    {
        static_cast<Mob*>(this)->Mob::resetAmbientSoundTime();
    }

    virtual void resetAmbientSoundTime()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_resetAmbientSoundTime))
        {
            if (!ModEvents::EntityResetAmbientSoundTime(this->shared_from_this()))
                return;
        }
        base_Entity_resetAmbientSoundTime<Base>(0);
    }

    template <class B>
    auto base_Entity_spawnAnim(int) -> decltype(static_cast<B*>(this)->B::spawnAnim())
    {
        static_cast<B*>(this)->B::spawnAnim();
    }
    template <class B>
    void base_Entity_spawnAnim(long)
    {
        static_cast<Mob*>(this)->Mob::spawnAnim();
    }

    virtual void spawnAnim()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_spawnAnim))
        {
            if (!ModEvents::EntitySpawnAnim(this->shared_from_this()))
                return;
        }
        base_Entity_spawnAnim<Base>(0);
    }

    template <class B>
    auto base_Entity_getAmbientSound(int) -> decltype(static_cast<B*>(this)->B::getAmbientSound())
    {
        return static_cast<B*>(this)->B::getAmbientSound();
    }
    template <class B>
    const SoundEvent* base_Entity_getAmbientSound(long)
    {
        return static_cast<Mob*>(this)->Mob::getAmbientSound();
    }

    virtual const SoundEvent* getAmbientSound()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getAmbientSound))
        {
            SoundEvent* result = NULL;
            if (!ModEvents::EntityGetAmbientSound(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getAmbientSound<Base>(0);
    }

    template <class B>
    auto base_Entity_getDeathLoot(int) -> decltype(static_cast<B*>(this)->B::getDeathLoot())
    {
        return static_cast<B*>(this)->B::getDeathLoot();
    }
    template <class B>
    Item* base_Entity_getDeathLoot(long)
    {
        return static_cast<Mob*>(this)->Mob::getDeathLoot();
    }

    virtual Item* getDeathLoot()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDeathLoot))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::EntityGetDeathLoot(this->shared_from_this(), result))
                return ModItemNames::Find(result);
        }
        return base_Entity_getDeathLoot<Base>(0);
    }

    template <class B>
    auto base_Entity_getDefaultLootTable(int) -> decltype(static_cast<B*>(this)->B::getDefaultLootTable())
    {
        return static_cast<B*>(this)->B::getDefaultLootTable();
    }
    template <class B>
    ResourceLocation* base_Entity_getDefaultLootTable(long)
    {
        return static_cast<Mob*>(this)->Mob::getDefaultLootTable();
    }

    virtual ResourceLocation* getDefaultLootTable()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDefaultLootTable))
        {
            ResourceLocation* result = NULL;
            if (!ModEvents::EntityGetDefaultLootTable(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getDefaultLootTable<Base>(0);
    }

    template <class B>
    auto base_Entity_getDefaultGearTable(int) -> decltype(static_cast<B*>(this)->B::getDefaultGearTable())
    {
        return static_cast<B*>(this)->B::getDefaultGearTable();
    }
    template <class B>
    ResourceLocation* base_Entity_getDefaultGearTable(long)
    {
        return static_cast<Mob*>(this)->Mob::getDefaultGearTable();
    }

    virtual ResourceLocation* getDefaultGearTable()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getDefaultGearTable))
        {
            ResourceLocation* result = NULL;
            if (!ModEvents::EntityGetDefaultGearTable(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getDefaultGearTable<Base>(0);
    }

    template <class B>
    auto base_Entity_setZza(float zza, int) -> decltype(static_cast<B*>(this)->B::setZza(zza))
    {
        static_cast<B*>(this)->B::setZza(zza);
    }
    template <class B>
    void base_Entity_setZza(float zza, long)
    {
        static_cast<Mob*>(this)->Mob::setZza(zza);
    }

    virtual void setZza(float zza)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setZza))
        {
            if (!ModEvents::EntitySetZza(this->shared_from_this(), zza))
                return;
        }
        base_Entity_setZza<Base>(zza, 0);
    }

    template <class B>
    auto base_Entity_setYya(float yya, int) -> decltype(static_cast<B*>(this)->B::setYya(yya))
    {
        static_cast<B*>(this)->B::setYya(yya);
    }
    template <class B>
    void base_Entity_setYya(float yya, long)
    {
        static_cast<Mob*>(this)->Mob::setYya(yya);
    }

    virtual void setYya(float yya)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setYya))
        {
            if (!ModEvents::EntitySetYya(this->shared_from_this(), yya))
                return;
        }
        base_Entity_setYya<Base>(yya, 0);
    }

    template <class B>
    auto base_Entity_setXxa(float xxa, int) -> decltype(static_cast<B*>(this)->B::setXxa(xxa))
    {
        static_cast<B*>(this)->B::setXxa(xxa);
    }
    template <class B>
    void base_Entity_setXxa(float xxa, long)
    {
        static_cast<Mob*>(this)->Mob::setXxa(xxa);
    }

    virtual void setXxa(float xxa)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setXxa))
        {
            if (!ModEvents::EntitySetXxa(this->shared_from_this(), xxa))
                return;
        }
        base_Entity_setXxa<Base>(xxa, 0);
    }

    template <class B>
    auto base_Entity_pickUpItem(boost::shared_ptr<ItemEntity> item, int) -> decltype(static_cast<B*>(this)->B::pickUpItem(item))
    {
        static_cast<B*>(this)->B::pickUpItem(item);
    }
    template <class B>
    void base_Entity_pickUpItem(boost::shared_ptr<ItemEntity> item, long)
    {
        static_cast<Mob*>(this)->Mob::pickUpItem(item);
    }

    virtual void pickUpItem(boost::shared_ptr<ItemEntity> item)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_pickUpItem))
        {
            if (!ModEvents::EntityPickUpItem(this->shared_from_this(), const_cast<ItemEntity*>(item.get())))
                return;
        }
        base_Entity_pickUpItem<Base>(item, 0);
    }

    template <class B>
    auto base_Entity_canHoldItem(not_null_ptr<ItemInstance> item, int) -> decltype(static_cast<B*>(this)->B::canHoldItem(item))
    {
        return static_cast<B*>(this)->B::canHoldItem(item);
    }
    template <class B>
    bool base_Entity_canHoldItem(not_null_ptr<ItemInstance> item, long)
    {
        return static_cast<Mob*>(this)->Mob::canHoldItem(item);
    }

    virtual bool canHoldItem(not_null_ptr<ItemInstance> item)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canHoldItem))
        {
            bool result = false;
            if (!ModEvents::EntityCanHoldItem(this->shared_from_this(), const_cast<ItemInstance*>(item.get()), result))
                return result;
        }
        return base_Entity_canHoldItem<Base>(item, 0);
    }

    template <class B>
    auto base_Entity_removeWhenFarAway(int) -> decltype(static_cast<B*>(this)->B::removeWhenFarAway())
    {
        return static_cast<B*>(this)->B::removeWhenFarAway();
    }
    template <class B>
    bool base_Entity_removeWhenFarAway(long)
    {
        return static_cast<Mob*>(this)->Mob::removeWhenFarAway();
    }

    virtual bool removeWhenFarAway()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_removeWhenFarAway))
        {
            bool result = false;
            if (!ModEvents::EntityRemoveWhenFarAway(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_removeWhenFarAway<Base>(0);
    }

    template <class B>
    auto base_Entity_checkDespawn(int) -> decltype(static_cast<B*>(this)->B::checkDespawn())
    {
        static_cast<B*>(this)->B::checkDespawn();
    }
    template <class B>
    void base_Entity_checkDespawn(long)
    {
        static_cast<Mob*>(this)->Mob::checkDespawn();
    }

    virtual void checkDespawn()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_checkDespawn))
        {
            if (!ModEvents::EntityCheckDespawn(this->shared_from_this()))
                return;
        }
        base_Entity_checkDespawn<Base>(0);
    }

    template <class B>
    auto base_Entity_customServerAiStep(int) -> decltype(static_cast<B*>(this)->B::customServerAiStep())
    {
        static_cast<B*>(this)->B::customServerAiStep();
    }
    template <class B>
    void base_Entity_customServerAiStep(long)
    {
        static_cast<Mob*>(this)->Mob::customServerAiStep();
    }

    virtual void customServerAiStep()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_customServerAiStep))
        {
            if (!ModEvents::EntityCustomServerAiStep(this->shared_from_this()))
                return;
        }
        base_Entity_customServerAiStep<Base>(0);
    }

    template <class B>
    auto base_Entity_getMaxHeadXRot(int) -> decltype(static_cast<B*>(this)->B::getMaxHeadXRot())
    {
        return static_cast<B*>(this)->B::getMaxHeadXRot();
    }
    template <class B>
    int base_Entity_getMaxHeadXRot(long)
    {
        return static_cast<Mob*>(this)->Mob::getMaxHeadXRot();
    }

    virtual int getMaxHeadXRot()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMaxHeadXRot))
        {
            int result = 0;
            if (!ModEvents::EntityGetMaxHeadXRot(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMaxHeadXRot<Base>(0);
    }

    template <class B>
    auto base_Entity_getMaxHeadYRot(int) -> decltype(static_cast<B*>(this)->B::getMaxHeadYRot())
    {
        return static_cast<B*>(this)->B::getMaxHeadYRot();
    }
    template <class B>
    int base_Entity_getMaxHeadYRot(long)
    {
        return static_cast<Mob*>(this)->Mob::getMaxHeadYRot();
    }

    virtual int getMaxHeadYRot()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMaxHeadYRot))
        {
            int result = 0;
            if (!ModEvents::EntityGetMaxHeadYRot(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMaxHeadYRot<Base>(0);
    }

    template <class B>
    auto base_Entity_checkSpawnRules(int) -> decltype(static_cast<B*>(this)->B::checkSpawnRules())
    {
        return static_cast<B*>(this)->B::checkSpawnRules();
    }
    template <class B>
    bool base_Entity_checkSpawnRules(long)
    {
        return static_cast<Mob*>(this)->Mob::checkSpawnRules();
    }

    virtual bool checkSpawnRules()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_checkSpawnRules))
        {
            bool result = false;
            if (!ModEvents::EntityCheckSpawnRules(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_checkSpawnRules<Base>(0);
    }

    template <class B>
    auto base_Entity_checkSpawnObstruction(int) -> decltype(static_cast<B*>(this)->B::checkSpawnObstruction())
    {
        return static_cast<B*>(this)->B::checkSpawnObstruction();
    }
    template <class B>
    bool base_Entity_checkSpawnObstruction(long)
    {
        return static_cast<Mob*>(this)->Mob::checkSpawnObstruction();
    }

    virtual bool checkSpawnObstruction()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_checkSpawnObstruction))
        {
            bool result = false;
            if (!ModEvents::EntityCheckSpawnObstruction(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_checkSpawnObstruction<Base>(0);
    }

    template <class B>
    auto base_Entity_getSizeScale(int) -> decltype(static_cast<B*>(this)->B::getSizeScale())
    {
        return static_cast<B*>(this)->B::getSizeScale();
    }
    template <class B>
    float base_Entity_getSizeScale(long)
    {
        return static_cast<Mob*>(this)->Mob::getSizeScale();
    }

    virtual float getSizeScale()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getSizeScale))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetSizeScale(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getSizeScale<Base>(0);
    }

    template <class B>
    auto base_Entity_getHeadSizeScale(int) -> decltype(static_cast<B*>(this)->B::getHeadSizeScale())
    {
        return static_cast<B*>(this)->B::getHeadSizeScale();
    }
    template <class B>
    float base_Entity_getHeadSizeScale(long)
    {
        return static_cast<Mob*>(this)->Mob::getHeadSizeScale();
    }

    virtual float getHeadSizeScale()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getHeadSizeScale))
        {
            float result = 0.0f;
            if (!ModEvents::EntityGetHeadSizeScale(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getHeadSizeScale<Base>(0);
    }

    template <class B>
    auto base_Entity_getMaxSpawnClusterSize(int) -> decltype(static_cast<B*>(this)->B::getMaxSpawnClusterSize())
    {
        return static_cast<B*>(this)->B::getMaxSpawnClusterSize();
    }
    template <class B>
    int base_Entity_getMaxSpawnClusterSize(long)
    {
        return static_cast<Mob*>(this)->Mob::getMaxSpawnClusterSize();
    }

    virtual int getMaxSpawnClusterSize()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_getMaxSpawnClusterSize))
        {
            int result = 0;
            if (!ModEvents::EntityGetMaxSpawnClusterSize(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_getMaxSpawnClusterSize<Base>(0);
    }

    template <class B>
    auto base_Entity_populateDefaultEquipmentSlots(const DifficultyInstance& difficulty, int) -> decltype(static_cast<B*>(this)->B::populateDefaultEquipmentSlots(difficulty))
    {
        static_cast<B*>(this)->B::populateDefaultEquipmentSlots(difficulty);
    }
    template <class B>
    void base_Entity_populateDefaultEquipmentSlots(const DifficultyInstance& difficulty, long)
    {
        static_cast<Mob*>(this)->Mob::populateDefaultEquipmentSlots(difficulty);
    }

    virtual void populateDefaultEquipmentSlots(const DifficultyInstance& difficulty)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_populateDefaultEquipmentSlots))
        {
            if (!ModEvents::EntityPopulateDefaultEquipmentSlots(this->shared_from_this(), const_cast<DifficultyInstance*>(&difficulty)))
                return;
        }
        base_Entity_populateDefaultEquipmentSlots<Base>(difficulty, 0);
    }

    template <class B>
    auto base_Entity_populateDefaultEquipmentEnchantments(const DifficultyInstance& difficulty, int) -> decltype(static_cast<B*>(this)->B::populateDefaultEquipmentEnchantments(difficulty))
    {
        static_cast<B*>(this)->B::populateDefaultEquipmentEnchantments(difficulty);
    }
    template <class B>
    void base_Entity_populateDefaultEquipmentEnchantments(const DifficultyInstance& difficulty, long)
    {
        static_cast<Mob*>(this)->Mob::populateDefaultEquipmentEnchantments(difficulty);
    }

    virtual void populateDefaultEquipmentEnchantments(const DifficultyInstance& difficulty)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_populateDefaultEquipmentEnchantments))
        {
            if (!ModEvents::EntityPopulateDefaultEquipmentEnchantments(this->shared_from_this(), const_cast<DifficultyInstance*>(&difficulty)))
                return;
        }
        base_Entity_populateDefaultEquipmentEnchantments<Base>(difficulty, 0);
    }

    template <class B>
    auto base_Entity_finalizeMobSpawn(const DifficultyInstance& difficulty, MobGroupData* data, const std::wstring& dataString, int) -> decltype(static_cast<B*>(this)->B::finalizeMobSpawn(difficulty, data, dataString))
    {
        return static_cast<B*>(this)->B::finalizeMobSpawn(difficulty, data, dataString);
    }
    template <class B>
    MobGroupData* base_Entity_finalizeMobSpawn(const DifficultyInstance& difficulty, MobGroupData* data, const std::wstring& dataString, long)
    {
        return static_cast<Mob*>(this)->Mob::finalizeMobSpawn(difficulty, data, dataString);
    }

    virtual MobGroupData* finalizeMobSpawn(const DifficultyInstance& difficulty, MobGroupData* data, const std::wstring& dataString)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_finalizeMobSpawn))
        {
            MobGroupData* result = NULL;
            if (!ModEvents::EntityFinalizeMobSpawn(this->shared_from_this(), const_cast<DifficultyInstance*>(&difficulty), const_cast<MobGroupData*>(data), dataString, result))
                return result;
        }
        return base_Entity_finalizeMobSpawn<Base>(difficulty, data, dataString, 0);
    }

    template <class B>
    auto base_Entity_finalizeSpawnEggSpawn(int type, int) -> decltype(static_cast<B*>(this)->B::finalizeSpawnEggSpawn(type))
    {
        static_cast<B*>(this)->B::finalizeSpawnEggSpawn(type);
    }
    template <class B>
    void base_Entity_finalizeSpawnEggSpawn(int type, long)
    {
        static_cast<Mob*>(this)->Mob::finalizeSpawnEggSpawn(type);
    }

    virtual void finalizeSpawnEggSpawn(int type)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_finalizeSpawnEggSpawn))
        {
            if (!ModEvents::EntityFinalizeSpawnEggSpawn(this->shared_from_this(), type))
                return;
        }
        base_Entity_finalizeSpawnEggSpawn<Base>(type, 0);
    }

    template <class B>
    auto base_Entity_canBeControlledByRider(int) -> decltype(static_cast<B*>(this)->B::canBeControlledByRider())
    {
        return static_cast<B*>(this)->B::canBeControlledByRider();
    }
    template <class B>
    bool base_Entity_canBeControlledByRider(long)
    {
        return static_cast<Mob*>(this)->Mob::canBeControlledByRider();
    }

    virtual bool canBeControlledByRider()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canBeControlledByRider))
        {
            bool result = false;
            if (!ModEvents::EntityCanBeControlledByRider(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_canBeControlledByRider<Base>(0);
    }

    template <class B>
    auto base_Entity_setDropChance(const EquipmentSlot* slot, float chance, int) -> decltype(static_cast<B*>(this)->B::setDropChance(slot, chance))
    {
        static_cast<B*>(this)->B::setDropChance(slot, chance);
    }
    template <class B>
    void base_Entity_setDropChance(const EquipmentSlot* slot, float chance, long)
    {
        static_cast<Mob*>(this)->Mob::setDropChance(slot, chance);
    }

    virtual void setDropChance(const EquipmentSlot* slot, float chance)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setDropChance))
        {
            if (!ModEvents::EntitySetDropChance(this->shared_from_this(), const_cast<EquipmentSlot*>(slot), chance))
                return;
        }
        base_Entity_setDropChance<Base>(slot, chance, 0);
    }

    template <class B>
    auto base_Entity_canPickUpLoot(int) -> decltype(static_cast<B*>(this)->B::canPickUpLoot())
    {
        return static_cast<B*>(this)->B::canPickUpLoot();
    }
    template <class B>
    bool base_Entity_canPickUpLoot(long)
    {
        return static_cast<Mob*>(this)->Mob::canPickUpLoot();
    }

    virtual bool canPickUpLoot()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_canPickUpLoot))
        {
            bool result = false;
            if (!ModEvents::EntityCanPickUpLoot(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_canPickUpLoot<Base>(0);
    }

    template <class B>
    auto base_Entity_setCanPickUpLoot(bool canPickUp, int) -> decltype(static_cast<B*>(this)->B::setCanPickUpLoot(canPickUp))
    {
        static_cast<B*>(this)->B::setCanPickUpLoot(canPickUp);
    }
    template <class B>
    void base_Entity_setCanPickUpLoot(bool canPickUp, long)
    {
        static_cast<Mob*>(this)->Mob::setCanPickUpLoot(canPickUp);
    }

    virtual void setCanPickUpLoot(bool canPickUp)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setCanPickUpLoot))
        {
            if (!ModEvents::EntitySetCanPickUpLoot(this->shared_from_this(), canPickUp))
                return;
        }
        base_Entity_setCanPickUpLoot<Base>(canPickUp, 0);
    }

    template <class B>
    auto base_Entity_isPersistenceRequired(int) -> decltype(static_cast<B*>(this)->B::isPersistenceRequired())
    {
        return static_cast<B*>(this)->B::isPersistenceRequired();
    }
    template <class B>
    bool base_Entity_isPersistenceRequired(long)
    {
        return static_cast<Mob*>(this)->Mob::isPersistenceRequired();
    }

    virtual bool isPersistenceRequired()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isPersistenceRequired))
        {
            bool result = false;
            if (!ModEvents::EntityIsPersistenceRequired(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isPersistenceRequired<Base>(0);
    }

    template <class B>
    auto base_Entity_mobInteract(const boost::shared_ptr<Player>& player, InteractionHand::EInteractionHand hand, int) -> decltype(static_cast<B*>(this)->B::mobInteract(player, hand))
    {
        return static_cast<B*>(this)->B::mobInteract(player, hand);
    }
    template <class B>
    bool base_Entity_mobInteract(const boost::shared_ptr<Player>& player, InteractionHand::EInteractionHand hand, long)
    {
        return static_cast<Mob*>(this)->Mob::mobInteract(player, hand);
    }

    virtual bool mobInteract(const boost::shared_ptr<Player>& player, InteractionHand::EInteractionHand hand)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_mobInteract))
        {
            bool result = false;
            if (!ModEvents::EntityMobInteract(this->shared_from_this(), player, static_cast<int>(hand), result))
                return result;
        }
        return base_Entity_mobInteract<Base>(player, hand, 0);
    }

    template <class B>
    auto base_Entity_underwaterEjectRider(int) -> decltype(static_cast<B*>(this)->B::underwaterEjectRider())
    {
        static_cast<B*>(this)->B::underwaterEjectRider();
    }
    template <class B>
    void base_Entity_underwaterEjectRider(long)
    {
        static_cast<Mob*>(this)->Mob::underwaterEjectRider();
    }

    virtual void underwaterEjectRider()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_underwaterEjectRider))
        {
            if (!ModEvents::EntityUnderwaterEjectRider(this->shared_from_this()))
                return;
        }
        base_Entity_underwaterEjectRider<Base>(0);
    }

    template <class B>
    auto base_Entity_setNoAi(bool noAi, int) -> decltype(static_cast<B*>(this)->B::setNoAi(noAi))
    {
        static_cast<B*>(this)->B::setNoAi(noAi);
    }
    template <class B>
    void base_Entity_setNoAi(bool noAi, long)
    {
        static_cast<Mob*>(this)->Mob::setNoAi(noAi);
    }

    virtual void setNoAi(bool noAi)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setNoAi))
        {
            if (!ModEvents::EntitySetNoAi(this->shared_from_this(), noAi))
                return;
        }
        base_Entity_setNoAi<Base>(noAi, 0);
    }

    template <class B>
    auto base_Entity_setLeftHanded(bool leftHanded, int) -> decltype(static_cast<B*>(this)->B::setLeftHanded(leftHanded))
    {
        static_cast<B*>(this)->B::setLeftHanded(leftHanded);
    }
    template <class B>
    void base_Entity_setLeftHanded(bool leftHanded, long)
    {
        static_cast<Mob*>(this)->Mob::setLeftHanded(leftHanded);
    }

    virtual void setLeftHanded(bool leftHanded)
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_setLeftHanded))
        {
            if (!ModEvents::EntitySetLeftHanded(this->shared_from_this(), leftHanded))
                return;
        }
        base_Entity_setLeftHanded<Base>(leftHanded, 0);
    }

    template <class B>
    auto base_Entity_isNoAi(int) -> decltype(static_cast<B*>(this)->B::isNoAi())
    {
        return static_cast<B*>(this)->B::isNoAi();
    }
    template <class B>
    bool base_Entity_isNoAi(long)
    {
        return static_cast<Mob*>(this)->Mob::isNoAi();
    }

    virtual bool isNoAi()
    {
        if (mLive && ModHooks::IsActive(kModHook_Entity_isNoAi))
        {
            bool result = false;
            if (!ModEvents::EntityIsNoAi(this->shared_from_this(), result))
                return result;
        }
        return base_Entity_isNoAi<Base>(0);
    }

    // not answerable yet, a parameter or result type has no conversion: GetHitTooltip, GetInteractTooltip, GetType, changeDimension, checkFallDamage, fillIndirectPassengers, getActiveEffects, getAllSlots, getArmorSlots, getCommandSenderBlockPosition, getCommandSenderEntity, getControllingPassenger, getHandSlots, getIndirectPassengers, getItemInHand, getItemSlot, getKillCredit, getLastHurtByMob, getLastHurtMob, getLeashHolder, getMainHandItem, getOffhandItem, getPassengers, getPortalEntranceBlock, getRideHeight, getRidingHeight, getRootVehicle, getRotationVector, getSubEntities, getTarget, getVehicle, getViewScale, sendMessage, spawnAtLocation, updateDeltaAfterMove

private:
    bool mLive = false;
};

#endif
