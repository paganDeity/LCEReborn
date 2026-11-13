// Generated from game/world/block/Block.hpp by tools/hooks/hookgen.py. Do not edit.
#ifndef GAME_MODCLASS_BLOCK_GENERATED_HPP
#define GAME_MODCLASS_BLOCK_GENERATED_HPP

#include <stddef.h>

#include "game/core/ModEvents_generated.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModItemNames.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/generation/Player.hpp"
#include "game/entity/mob/LivingEntity.hpp"
#include "game/world/block/Block.hpp"

// every virtual method of Block a mod can answer, on top of any game class derived from it
template <class Base>
class ModBlock : public Base
{
public:
    using Base::Base;

    template <class B>
    auto base_Block_isTopSolidBlocking(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::isTopSolidBlocking(state))
    {
        return static_cast<B*>(this)->B::isTopSolidBlocking(state);
    }
    template <class B>
    bool base_Block_isTopSolidBlocking(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::isTopSolidBlocking(state);
    }

    virtual bool isTopSolidBlocking(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_isTopSolidBlocking))
        {
            bool result = false;
            if (!ModEvents::BlockIsTopSolidBlocking(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isTopSolidBlocking<Base>(state, 0);
    }

    template <class B>
    auto base_Block_canProvideSupport(const BlockState* state, const Direction* direction, EBlockSupportType supportType, int) -> decltype(static_cast<B*>(this)->B::canProvideSupport(state, direction, supportType))
    {
        return static_cast<B*>(this)->B::canProvideSupport(state, direction, supportType);
    }
    template <class B>
    bool base_Block_canProvideSupport(const BlockState* state, const Direction* direction, EBlockSupportType supportType, long)
    {
        return static_cast<Block*>(this)->Block::canProvideSupport(state, direction, supportType);
    }

    virtual bool canProvideSupport(const BlockState* state, const Direction* direction, EBlockSupportType supportType)
    {
        if (ModHooks::IsActive(kModHook_Block_canProvideSupport))
        {
            bool result = false;
            if (!ModEvents::BlockCanProvideSupport(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), (direction != NULL ? direction->get3DDataValue() : -1), static_cast<int>(supportType), result))
                return result;
        }
        return base_Block_canProvideSupport<Base>(state, direction, supportType, 0);
    }

    template <class B>
    auto base_Block_isValidSpawn(const BlockState* state, boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::isValidSpawn(state, entity))
    {
        return static_cast<B*>(this)->B::isValidSpawn(state, entity);
    }
    template <class B>
    bool base_Block_isValidSpawn(const BlockState* state, boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Block*>(this)->Block::isValidSpawn(state, entity);
    }

    virtual bool isValidSpawn(const BlockState* state, boost::shared_ptr<Entity> entity)
    {
        if (ModHooks::IsActive(kModHook_Block_isValidSpawn))
        {
            bool result = false;
            if (!ModEvents::BlockIsValidSpawn(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), entity, result))
                return result;
        }
        return base_Block_isValidSpawn<Base>(state, entity, 0);
    }

    template <class B>
    auto base_Block_getLightEmission(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getLightEmission(state))
    {
        return static_cast<B*>(this)->B::getLightEmission(state);
    }
    template <class B>
    int base_Block_getLightEmission(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getLightEmission(state);
    }

    virtual int getLightEmission(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getLightEmission))
        {
            int result = 0;
            if (!ModEvents::BlockGetLightEmission(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getLightEmission<Base>(state, 0);
    }

    template <class B>
    auto base_Block_ParticlesSurviveWithin(int) -> decltype(static_cast<B*>(this)->B::ParticlesSurviveWithin())
    {
        return static_cast<B*>(this)->B::ParticlesSurviveWithin();
    }
    template <class B>
    bool base_Block_ParticlesSurviveWithin(long)
    {
        return static_cast<Block*>(this)->Block::ParticlesSurviveWithin();
    }

    virtual bool ParticlesSurviveWithin()
    {
        if (ModHooks::IsActive(kModHook_Block_ParticlesSurviveWithin))
        {
            bool result = false;
            if (!ModEvents::BlockParticlesSurviveWithin(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_ParticlesSurviveWithin<Base>(0);
    }

    template <class B>
    auto base_Block_getMapColor(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getMapColor(state, levelSource, pos))
    {
        return static_cast<B*>(this)->B::getMapColor(state, levelSource, pos);
    }
    template <class B>
    const MaterialColor* base_Block_getMapColor(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getMapColor(state, levelSource, pos);
    }

    virtual const MaterialColor* getMapColor(const BlockState* state, LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getMapColor))
        {
            MaterialColor* result = NULL;
            if (!ModEvents::BlockGetMapColor(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getMapColor<Base>(state, levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_getBlockState(int data, int) -> decltype(static_cast<B*>(this)->B::getBlockState(data))
    {
        return static_cast<B*>(this)->B::getBlockState(data);
    }
    template <class B>
    const BlockState* base_Block_getBlockState(int data, long)
    {
        return static_cast<Block*>(this)->Block::getBlockState(data);
    }

    virtual const BlockState* getBlockState(int data)
    {
        if (ModHooks::IsActive(kModHook_Block_getBlockState))
        {
            int result = 0;
            if (!ModEvents::BlockGetBlockState(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), data, result))
                return this->getBlockState(result);
        }
        return base_Block_getBlockState<Base>(data, 0);
    }

    template <class B>
    auto base_Block_convertBlockStateToLegacyData(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::convertBlockStateToLegacyData(state))
    {
        return static_cast<B*>(this)->B::convertBlockStateToLegacyData(state);
    }
    template <class B>
    int base_Block_convertBlockStateToLegacyData(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::convertBlockStateToLegacyData(state);
    }

    virtual int convertBlockStateToLegacyData(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_convertBlockStateToLegacyData))
        {
            int result = 0;
            if (!ModEvents::BlockConvertBlockStateToLegacyData(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_convertBlockStateToLegacyData<Base>(state, 0);
    }

    template <class B>
    auto base_Block_setPropertyValueFromString(const BlockState* state, const Property* property, const std::wstring& value, int) -> decltype(static_cast<B*>(this)->B::setPropertyValueFromString(state, property, value))
    {
        return static_cast<B*>(this)->B::setPropertyValueFromString(state, property, value);
    }
    template <class B>
    const BlockState* base_Block_setPropertyValueFromString(const BlockState* state, const Property* property, const std::wstring& value, long)
    {
        return static_cast<Block*>(this)->Block::setPropertyValueFromString(state, property, value);
    }

    virtual const BlockState* setPropertyValueFromString(const BlockState* state, const Property* property, const std::wstring& value)
    {
        if (ModHooks::IsActive(kModHook_Block_setPropertyValueFromString))
        {
            int result = 0;
            if (!ModEvents::BlockSetPropertyValueFromString(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), const_cast<Property*>(property), value, result))
                return this->getBlockState(result);
        }
        return base_Block_setPropertyValueFromString<Base>(state, property, value, 0);
    }

    template <class B>
    auto base_Block_fillVirtualBlockStateProperties(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::fillVirtualBlockStateProperties(state, levelSource, pos))
    {
        return static_cast<B*>(this)->B::fillVirtualBlockStateProperties(state, levelSource, pos);
    }
    template <class B>
    const BlockState* base_Block_fillVirtualBlockStateProperties(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::fillVirtualBlockStateProperties(state, levelSource, pos);
    }

    virtual const BlockState* fillVirtualBlockStateProperties(const BlockState* state, LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_fillVirtualBlockStateProperties))
        {
            int result = 0;
            if (!ModEvents::BlockFillVirtualBlockStateProperties(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return this->getBlockState(result);
        }
        return base_Block_fillVirtualBlockStateProperties<Base>(state, levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_rotate(const BlockState* state, Rotation* rotation, int) -> decltype(static_cast<B*>(this)->B::rotate(state, rotation))
    {
        return static_cast<B*>(this)->B::rotate(state, rotation);
    }
    template <class B>
    const BlockState* base_Block_rotate(const BlockState* state, Rotation* rotation, long)
    {
        return static_cast<Block*>(this)->Block::rotate(state, rotation);
    }

    virtual const BlockState* rotate(const BlockState* state, Rotation* rotation)
    {
        if (ModHooks::IsActive(kModHook_Block_rotate))
        {
            int result = 0;
            if (!ModEvents::BlockRotate(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), const_cast<Rotation*>(rotation), result))
                return this->getBlockState(result);
        }
        return base_Block_rotate<Base>(state, rotation, 0);
    }

    template <class B>
    auto base_Block_mirror(const BlockState* state, Mirror* mirror, int) -> decltype(static_cast<B*>(this)->B::mirror(state, mirror))
    {
        return static_cast<B*>(this)->B::mirror(state, mirror);
    }
    template <class B>
    const BlockState* base_Block_mirror(const BlockState* state, Mirror* mirror, long)
    {
        return static_cast<Block*>(this)->Block::mirror(state, mirror);
    }

    virtual const BlockState* mirror(const BlockState* state, Mirror* mirror)
    {
        if (ModHooks::IsActive(kModHook_Block_mirror))
        {
            int result = 0;
            if (!ModEvents::BlockMirror(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), const_cast<Mirror*>(mirror), result))
                return this->getBlockState(result);
        }
        return base_Block_mirror<Base>(state, mirror, 0);
    }

    template <class B>
    auto base_Block_DerivedInit(int) -> decltype(static_cast<B*>(this)->B::DerivedInit())
    {
        static_cast<B*>(this)->B::DerivedInit();
    }
    template <class B>
    void base_Block_DerivedInit(long)
    {
        static_cast<Block*>(this)->Block::DerivedInit();
    }

    virtual void DerivedInit()
    {
        if (ModHooks::IsActive(kModHook_Block_DerivedInit))
        {
            if (!ModEvents::BlockDerivedInit(static_cast<Block*>(const_cast<ModBlock<Base>*>(this))))
                return;
        }
        base_Block_DerivedInit<Base>(0);
    }

    template <class B>
    auto base_Block_sendBlockData(unsigned char blockData, int) -> decltype(static_cast<B*>(this)->B::sendBlockData(blockData))
    {
        return static_cast<B*>(this)->B::sendBlockData(blockData);
    }
    template <class B>
    Block* base_Block_sendBlockData(unsigned char blockData, long)
    {
        return static_cast<Block*>(this)->Block::sendBlockData(blockData);
    }

    virtual Block* sendBlockData(unsigned char blockData)
    {
        if (ModHooks::IsActive(kModHook_Block_sendBlockData))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSendBlockData(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), static_cast<int>(blockData), result))
                return result;
        }
        return base_Block_sendBlockData<Base>(blockData, 0);
    }

    template <class B>
    auto base_Block_setSoundType(const SoundType* soundType, int) -> decltype(static_cast<B*>(this)->B::setSoundType(soundType))
    {
        return static_cast<B*>(this)->B::setSoundType(soundType);
    }
    template <class B>
    Block* base_Block_setSoundType(const SoundType* soundType, long)
    {
        return static_cast<Block*>(this)->Block::setSoundType(soundType);
    }

    virtual Block* setSoundType(const SoundType* soundType)
    {
        if (ModHooks::IsActive(kModHook_Block_setSoundType))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetSoundType(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), const_cast<SoundType*>(soundType), result))
                return result;
        }
        return base_Block_setSoundType<Base>(soundType, 0);
    }

    template <class B>
    auto base_Block_setLightBlock(int lightBlock, int) -> decltype(static_cast<B*>(this)->B::setLightBlock(lightBlock))
    {
        return static_cast<B*>(this)->B::setLightBlock(lightBlock);
    }
    template <class B>
    Block* base_Block_setLightBlock(int lightBlock, long)
    {
        return static_cast<Block*>(this)->Block::setLightBlock(lightBlock);
    }

    virtual Block* setLightBlock(int lightBlock)
    {
        if (ModHooks::IsActive(kModHook_Block_setLightBlock))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetLightBlock(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), lightBlock, result))
                return result;
        }
        return base_Block_setLightBlock<Base>(lightBlock, 0);
    }

    template <class B>
    auto base_Block_setLightEmission(float lightEmission, int) -> decltype(static_cast<B*>(this)->B::setLightEmission(lightEmission))
    {
        return static_cast<B*>(this)->B::setLightEmission(lightEmission);
    }
    template <class B>
    Block* base_Block_setLightEmission(float lightEmission, long)
    {
        return static_cast<Block*>(this)->Block::setLightEmission(lightEmission);
    }

    virtual Block* setLightEmission(float lightEmission)
    {
        if (ModHooks::IsActive(kModHook_Block_setLightEmission))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetLightEmission(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), lightEmission, result))
                return result;
        }
        return base_Block_setLightEmission<Base>(lightEmission, 0);
    }

    template <class B>
    auto base_Block_setExplodeable(float explosionResistance, int) -> decltype(static_cast<B*>(this)->B::setExplodeable(explosionResistance))
    {
        return static_cast<B*>(this)->B::setExplodeable(explosionResistance);
    }
    template <class B>
    Block* base_Block_setExplodeable(float explosionResistance, long)
    {
        return static_cast<Block*>(this)->Block::setExplodeable(explosionResistance);
    }

    virtual Block* setExplodeable(float explosionResistance)
    {
        if (ModHooks::IsActive(kModHook_Block_setExplodeable))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetExplodeable(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), explosionResistance, result))
                return result;
        }
        return base_Block_setExplodeable<Base>(explosionResistance, 0);
    }

    template <class B>
    auto base_Block_isSolidBlockingCube(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::isSolidBlockingCube(state))
    {
        return static_cast<B*>(this)->B::isSolidBlockingCube(state);
    }
    template <class B>
    bool base_Block_isSolidBlockingCube(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::isSolidBlockingCube(state);
    }

    virtual bool isSolidBlockingCube(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_isSolidBlockingCube))
        {
            bool result = false;
            if (!ModEvents::BlockIsSolidBlockingCube(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isSolidBlockingCube<Base>(state, 0);
    }

    template <class B>
    auto base_Block_isSolidBlockingCubeAndNotSignalSource(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::isSolidBlockingCubeAndNotSignalSource(state))
    {
        return static_cast<B*>(this)->B::isSolidBlockingCubeAndNotSignalSource(state);
    }
    template <class B>
    bool base_Block_isSolidBlockingCubeAndNotSignalSource(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::isSolidBlockingCubeAndNotSignalSource(state);
    }

    virtual bool isSolidBlockingCubeAndNotSignalSource(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_isSolidBlockingCubeAndNotSignalSource))
        {
            bool result = false;
            if (!ModEvents::BlockIsSolidBlockingCubeAndNotSignalSource(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isSolidBlockingCubeAndNotSignalSource<Base>(state, 0);
    }

    template <class B>
    auto base_Block_isViewBlocking(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::isViewBlocking(state))
    {
        return static_cast<B*>(this)->B::isViewBlocking(state);
    }
    template <class B>
    bool base_Block_isViewBlocking(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::isViewBlocking(state);
    }

    virtual bool isViewBlocking(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_isViewBlocking))
        {
            bool result = false;
            if (!ModEvents::BlockIsViewBlocking(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isViewBlocking<Base>(state, 0);
    }

    template <class B>
    auto base_Block_isCubeShaped(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::isCubeShaped(state))
    {
        return static_cast<B*>(this)->B::isCubeShaped(state);
    }
    template <class B>
    bool base_Block_isCubeShaped(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::isCubeShaped(state);
    }

    virtual bool isCubeShaped(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_isCubeShaped))
        {
            bool result = false;
            if (!ModEvents::BlockIsCubeShaped(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isCubeShaped<Base>(state, 0);
    }

    template <class B>
    auto base_Block_hasCustomBreakingProgress(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::hasCustomBreakingProgress(state))
    {
        return static_cast<B*>(this)->B::hasCustomBreakingProgress(state);
    }
    template <class B>
    bool base_Block_hasCustomBreakingProgress(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::hasCustomBreakingProgress(state);
    }

    virtual bool hasCustomBreakingProgress(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_hasCustomBreakingProgress))
        {
            bool result = false;
            if (!ModEvents::BlockHasCustomBreakingProgress(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_hasCustomBreakingProgress<Base>(state, 0);
    }

    template <class B>
    auto base_Block_isPathfindable(LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::isPathfindable(levelSource, pos))
    {
        return static_cast<B*>(this)->B::isPathfindable(levelSource, pos);
    }
    template <class B>
    bool base_Block_isPathfindable(LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::isPathfindable(levelSource, pos);
    }

    virtual bool isPathfindable(LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_isPathfindable))
        {
            bool result = false;
            if (!ModEvents::BlockIsPathfindable(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_isPathfindable<Base>(levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_getRenderShape(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getRenderShape(state))
    {
        return static_cast<B*>(this)->B::getRenderShape(state);
    }
    template <class B>
    EBlockRenderShape base_Block_getRenderShape(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getRenderShape(state);
    }

    virtual EBlockRenderShape getRenderShape(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getRenderShape))
        {
            int result = 0;
            if (!ModEvents::BlockGetRenderShape(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return static_cast<EBlockRenderShape>(result);
        }
        return base_Block_getRenderShape<Base>(state, 0);
    }

    template <class B>
    auto base_Block_hasInHandRenderOffset(int) -> decltype(static_cast<B*>(this)->B::hasInHandRenderOffset())
    {
        return static_cast<B*>(this)->B::hasInHandRenderOffset();
    }
    template <class B>
    bool base_Block_hasInHandRenderOffset(long)
    {
        return static_cast<Block*>(this)->Block::hasInHandRenderOffset();
    }

    virtual bool hasInHandRenderOffset()
    {
        if (ModHooks::IsActive(kModHook_Block_hasInHandRenderOffset))
        {
            bool result = false;
            if (!ModEvents::BlockHasInHandRenderOffset(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_hasInHandRenderOffset<Base>(0);
    }

    template <class B>
    auto base_Block_mayReplaceWithPlace(LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::mayReplaceWithPlace(levelSource, pos))
    {
        return static_cast<B*>(this)->B::mayReplaceWithPlace(levelSource, pos);
    }
    template <class B>
    bool base_Block_mayReplaceWithPlace(LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::mayReplaceWithPlace(levelSource, pos);
    }

    virtual bool mayReplaceWithPlace(LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_mayReplaceWithPlace))
        {
            bool result = false;
            if (!ModEvents::BlockMayReplaceWithPlace(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_mayReplaceWithPlace<Base>(levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_isTransparent(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::isTransparent(state))
    {
        return static_cast<B*>(this)->B::isTransparent(state);
    }
    template <class B>
    bool base_Block_isTransparent(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::isTransparent(state);
    }

    virtual bool isTransparent(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_isTransparent))
        {
            bool result = false;
            if (!ModEvents::BlockIsTransparent(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isTransparent<Base>(state, 0);
    }

    template <class B>
    auto base_Block_setDestroyTime(float destroyTime, int) -> decltype(static_cast<B*>(this)->B::setDestroyTime(destroyTime))
    {
        return static_cast<B*>(this)->B::setDestroyTime(destroyTime);
    }
    template <class B>
    Block* base_Block_setDestroyTime(float destroyTime, long)
    {
        return static_cast<Block*>(this)->Block::setDestroyTime(destroyTime);
    }

    virtual Block* setDestroyTime(float destroyTime)
    {
        if (ModHooks::IsActive(kModHook_Block_setDestroyTime))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetDestroyTime(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), destroyTime, result))
                return result;
        }
        return base_Block_setDestroyTime<Base>(destroyTime, 0);
    }

    template <class B>
    auto base_Block_setIndestructible(int) -> decltype(static_cast<B*>(this)->B::setIndestructible())
    {
        return static_cast<B*>(this)->B::setIndestructible();
    }
    template <class B>
    Block* base_Block_setIndestructible(long)
    {
        return static_cast<Block*>(this)->Block::setIndestructible();
    }

    virtual Block* setIndestructible()
    {
        if (ModHooks::IsActive(kModHook_Block_setIndestructible))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetIndestructible(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_setIndestructible<Base>(0);
    }

    template <class B>
    auto base_Block_isIndestructible(int) -> decltype(static_cast<B*>(this)->B::isIndestructible())
    {
        return static_cast<B*>(this)->B::isIndestructible();
    }
    template <class B>
    bool base_Block_isIndestructible(long)
    {
        return static_cast<Block*>(this)->Block::isIndestructible();
    }

    virtual bool isIndestructible()
    {
        if (ModHooks::IsActive(kModHook_Block_isIndestructible))
        {
            bool result = false;
            if (!ModEvents::BlockIsIndestructible(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isIndestructible<Base>(0);
    }

    template <class B>
    auto base_Block_getDestroySpeed(const BlockState* state, Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getDestroySpeed(state, level, pos))
    {
        return static_cast<B*>(this)->B::getDestroySpeed(state, level, pos);
    }
    template <class B>
    float base_Block_getDestroySpeed(const BlockState* state, Level* level, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getDestroySpeed(state, level, pos);
    }

    virtual float getDestroySpeed(const BlockState* state, Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getDestroySpeed))
        {
            float result = 0.0f;
            if (!ModEvents::BlockGetDestroySpeed(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), level, pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getDestroySpeed<Base>(state, level, pos, 0);
    }

    template <class B>
    auto base_Block_setTicking(bool ticking, int) -> decltype(static_cast<B*>(this)->B::setTicking(ticking))
    {
        return static_cast<B*>(this)->B::setTicking(ticking);
    }
    template <class B>
    Block* base_Block_setTicking(bool ticking, long)
    {
        return static_cast<Block*>(this)->Block::setTicking(ticking);
    }

    virtual Block* setTicking(bool ticking)
    {
        if (ModHooks::IsActive(kModHook_Block_setTicking))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetTicking(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), ticking, result))
                return result;
        }
        return base_Block_setTicking<Base>(ticking, 0);
    }

    template <class B>
    auto base_Block_disableMipmap(int) -> decltype(static_cast<B*>(this)->B::disableMipmap())
    {
        return static_cast<B*>(this)->B::disableMipmap();
    }
    template <class B>
    Block* base_Block_disableMipmap(long)
    {
        return static_cast<Block*>(this)->Block::disableMipmap();
    }

    virtual Block* disableMipmap()
    {
        if (ModHooks::IsActive(kModHook_Block_disableMipmap))
        {
            Block* result = NULL;
            if (!ModEvents::BlockDisableMipmap(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_disableMipmap<Base>(0);
    }

    template <class B>
    auto base_Block_setSemiTransparent(int) -> decltype(static_cast<B*>(this)->B::setSemiTransparent())
    {
        return static_cast<B*>(this)->B::setSemiTransparent();
    }
    template <class B>
    Block* base_Block_setSemiTransparent(long)
    {
        return static_cast<Block*>(this)->Block::setSemiTransparent();
    }

    virtual Block* setSemiTransparent()
    {
        if (ModHooks::IsActive(kModHook_Block_setSemiTransparent))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetSemiTransparent(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_setSemiTransparent<Base>(0);
    }

    template <class B>
    auto base_Block_isTicking(int) -> decltype(static_cast<B*>(this)->B::isTicking())
    {
        return static_cast<B*>(this)->B::isTicking();
    }
    template <class B>
    bool base_Block_isTicking(long)
    {
        return static_cast<Block*>(this)->Block::isTicking();
    }

    virtual bool isTicking()
    {
        if (ModHooks::IsActive(kModHook_Block_isTicking))
        {
            bool result = false;
            if (!ModEvents::BlockIsTicking(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isTicking<Base>(0);
    }

    template <class B>
    auto base_Block_isSlabBlock(int) -> decltype(static_cast<B*>(this)->B::isSlabBlock())
    {
        return static_cast<B*>(this)->B::isSlabBlock();
    }
    template <class B>
    bool base_Block_isSlabBlock(long)
    {
        return static_cast<Block*>(this)->Block::isSlabBlock();
    }

    virtual bool isSlabBlock()
    {
        if (ModHooks::IsActive(kModHook_Block_isSlabBlock))
        {
            bool result = false;
            if (!ModEvents::BlockIsSlabBlock(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isSlabBlock<Base>(0);
    }

    template <class B>
    auto base_Block_isWaterBlocking(int) const -> decltype(static_cast<const B*>(this)->B::isWaterBlocking())
    {
        return static_cast<const B*>(this)->B::isWaterBlocking();
    }
    template <class B>
    bool base_Block_isWaterBlocking(long) const
    {
        return static_cast<const Block*>(this)->Block::isWaterBlocking();
    }

    virtual bool isWaterBlocking() const
    {
        if (ModHooks::IsActive(kModHook_Block_isWaterBlocking))
        {
            bool result = false;
            if (!ModEvents::BlockIsWaterBlocking(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isWaterBlocking<Base>(0);
    }

    template <class B>
    auto base_Block_canContainLiquid(const BlockState* state, int) const -> decltype(static_cast<const B*>(this)->B::canContainLiquid(state))
    {
        return static_cast<const B*>(this)->B::canContainLiquid(state);
    }
    template <class B>
    bool base_Block_canContainLiquid(const BlockState* state, long) const
    {
        return static_cast<const Block*>(this)->Block::canContainLiquid(state);
    }

    virtual bool canContainLiquid(const BlockState* state) const
    {
        if (ModHooks::IsActive(kModHook_Block_canContainLiquid))
        {
            bool result = false;
            if (!ModEvents::BlockCanContainLiquid(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_canContainLiquid<Base>(state, 0);
    }

    template <class B>
    auto base_Block_isStrippable(int) -> decltype(static_cast<B*>(this)->B::isStrippable())
    {
        return static_cast<B*>(this)->B::isStrippable();
    }
    template <class B>
    bool base_Block_isStrippable(long)
    {
        return static_cast<Block*>(this)->Block::isStrippable();
    }

    virtual bool isStrippable()
    {
        if (ModHooks::IsActive(kModHook_Block_isStrippable))
        {
            bool result = false;
            if (!ModEvents::BlockIsStrippable(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isStrippable<Base>(0);
    }

    template <class B>
    auto base_Block_getStrippedBlock(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getStrippedBlock(state))
    {
        return static_cast<B*>(this)->B::getStrippedBlock(state);
    }
    template <class B>
    const BlockState* base_Block_getStrippedBlock(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getStrippedBlock(state);
    }

    virtual const BlockState* getStrippedBlock(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getStrippedBlock))
        {
            int result = 0;
            if (!ModEvents::BlockGetStrippedBlock(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return this->getBlockState(result);
        }
        return base_Block_getStrippedBlock<Base>(state, 0);
    }

    template <class B>
    auto base_Block_liquidCanFlowIntoFromDirection(LevelSource* levelSource, const BlockPos& pos, const Direction* direction, int) const -> decltype(static_cast<const B*>(this)->B::liquidCanFlowIntoFromDirection(levelSource, pos, direction))
    {
        return static_cast<const B*>(this)->B::liquidCanFlowIntoFromDirection(levelSource, pos, direction);
    }
    template <class B>
    bool base_Block_liquidCanFlowIntoFromDirection(LevelSource* levelSource, const BlockPos& pos, const Direction* direction, long) const
    {
        return static_cast<const Block*>(this)->Block::liquidCanFlowIntoFromDirection(levelSource, pos, direction);
    }

    virtual bool liquidCanFlowIntoFromDirection(LevelSource* levelSource, const BlockPos& pos, const Direction* direction) const
    {
        if (ModHooks::IsActive(kModHook_Block_liquidCanFlowIntoFromDirection))
        {
            bool result = false;
            if (!ModEvents::BlockLiquidCanFlowIntoFromDirection(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), result))
                return result;
        }
        return base_Block_liquidCanFlowIntoFromDirection<Base>(levelSource, pos, direction, 0);
    }

    template <class B>
    auto base_Block_getShape(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getShape(state, levelSource, pos))
    {
        return static_cast<B*>(this)->B::getShape(state, levelSource, pos);
    }
    template <class B>
    const AABB* base_Block_getShape(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getShape(state, levelSource, pos);
    }

    virtual const AABB* getShape(const BlockState* state, LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getShape))
        {
            AABB* result = NULL;
            if (!ModEvents::BlockGetShape(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getShape<Base>(state, levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_getLightColor(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int lightValue, int) -> decltype(static_cast<B*>(this)->B::getLightColor(state, levelSource, pos, lightValue))
    {
        return static_cast<B*>(this)->B::getLightColor(state, levelSource, pos, lightValue);
    }
    template <class B>
    int base_Block_getLightColor(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int lightValue, long)
    {
        return static_cast<Block*>(this)->Block::getLightColor(state, levelSource, pos, lightValue);
    }

    virtual int getLightColor(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int lightValue)
    {
        if (ModHooks::IsActive(kModHook_Block_getLightColor))
        {
            int result = 0;
            if (!ModEvents::BlockGetLightColor(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, lightValue, result))
                return result;
        }
        return base_Block_getLightColor<Base>(state, levelSource, pos, lightValue, 0);
    }

    template <class B>
    auto base_Block_shouldRenderFace(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction, int) -> decltype(static_cast<B*>(this)->B::shouldRenderFace(state, levelSource, pos, direction))
    {
        return static_cast<B*>(this)->B::shouldRenderFace(state, levelSource, pos, direction);
    }
    template <class B>
    bool base_Block_shouldRenderFace(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction, long)
    {
        return static_cast<Block*>(this)->Block::shouldRenderFace(state, levelSource, pos, direction);
    }

    virtual bool shouldRenderFace(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction)
    {
        if (ModHooks::IsActive(kModHook_Block_shouldRenderFace))
        {
            bool result = false;
            if (!ModEvents::BlockShouldRenderFace(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), result))
                return result;
        }
        return base_Block_shouldRenderFace<Base>(state, levelSource, pos, direction, 0);
    }

    template <class B>
    auto base_Block_isSolidFace(LevelSource* levelSource, const BlockPos& pos, const Direction* direction, int) -> decltype(static_cast<B*>(this)->B::isSolidFace(levelSource, pos, direction))
    {
        return static_cast<B*>(this)->B::isSolidFace(levelSource, pos, direction);
    }
    template <class B>
    bool base_Block_isSolidFace(LevelSource* levelSource, const BlockPos& pos, const Direction* direction, long)
    {
        return static_cast<Block*>(this)->Block::isSolidFace(levelSource, pos, direction);
    }

    virtual bool isSolidFace(LevelSource* levelSource, const BlockPos& pos, const Direction* direction)
    {
        if (ModHooks::IsActive(kModHook_Block_isSolidFace))
        {
            bool result = false;
            if (!ModEvents::BlockIsSolidFace(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), result))
                return result;
        }
        return base_Block_isSolidFace<Base>(levelSource, pos, direction, 0);
    }

    template <class B>
    auto base_Block_getBlockFaceShape(LevelSource* levelSource, const BlockState* state, const BlockPos& pos, const Direction* direction, int) -> decltype(static_cast<B*>(this)->B::getBlockFaceShape(levelSource, state, pos, direction))
    {
        return static_cast<B*>(this)->B::getBlockFaceShape(levelSource, state, pos, direction);
    }
    template <class B>
    EBlockFaceShape base_Block_getBlockFaceShape(LevelSource* levelSource, const BlockState* state, const BlockPos& pos, const Direction* direction, long)
    {
        return static_cast<Block*>(this)->Block::getBlockFaceShape(levelSource, state, pos, direction);
    }

    virtual EBlockFaceShape getBlockFaceShape(LevelSource* levelSource, const BlockState* state, const BlockPos& pos, const Direction* direction)
    {
        if (ModHooks::IsActive(kModHook_Block_getBlockFaceShape))
        {
            int result = 0;
            if (!ModEvents::BlockGetBlockFaceShape(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), result))
                return static_cast<EBlockFaceShape>(result);
        }
        return base_Block_getBlockFaceShape<Base>(levelSource, state, pos, direction, 0);
    }

    template <class B>
    auto base_Block_getOutlineAABB(const BlockState* state, Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getOutlineAABB(state, level, pos))
    {
        return static_cast<B*>(this)->B::getOutlineAABB(state, level, pos);
    }
    template <class B>
    const AABB* base_Block_getOutlineAABB(const BlockState* state, Level* level, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getOutlineAABB(state, level, pos);
    }

    virtual const AABB* getOutlineAABB(const BlockState* state, Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getOutlineAABB))
        {
            AABB* result = NULL;
            if (!ModEvents::BlockGetOutlineAABB(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), level, pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getOutlineAABB<Base>(state, level, pos, 0);
    }

    template <class B>
    auto base_Block_getClipAABB(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getClipAABB(state, levelSource, pos))
    {
        return static_cast<B*>(this)->B::getClipAABB(state, levelSource, pos);
    }
    template <class B>
    const AABB* base_Block_getClipAABB(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getClipAABB(state, levelSource, pos);
    }

    virtual const AABB* getClipAABB(const BlockState* state, LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getClipAABB))
        {
            AABB* result = NULL;
            if (!ModEvents::BlockGetClipAABB(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getClipAABB<Base>(state, levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_isSolidRender(const BlockState* state, int) const -> decltype(static_cast<const B*>(this)->B::isSolidRender(state))
    {
        return static_cast<const B*>(this)->B::isSolidRender(state);
    }
    template <class B>
    bool base_Block_isSolidRender(const BlockState* state, long) const
    {
        return static_cast<const Block*>(this)->Block::isSolidRender(state);
    }

    virtual bool isSolidRender(const BlockState* state) const
    {
        if (ModHooks::IsActive(kModHook_Block_isSolidRender))
        {
            bool result = false;
            if (!ModEvents::BlockIsSolidRender(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isSolidRender<Base>(state, 0);
    }

    template <class B>
    auto base_Block_mayPick(const BlockState* state, bool includeLiquid, int) -> decltype(static_cast<B*>(this)->B::mayPick(state, includeLiquid))
    {
        return static_cast<B*>(this)->B::mayPick(state, includeLiquid);
    }
    template <class B>
    bool base_Block_mayPick(const BlockState* state, bool includeLiquid, long)
    {
        return static_cast<Block*>(this)->Block::mayPick(state, includeLiquid);
    }

    virtual bool mayPick(const BlockState* state, bool includeLiquid)
    {
        if (ModHooks::IsActive(kModHook_Block_mayPick))
        {
            bool result = false;
            if (!ModEvents::BlockMayPick(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), includeLiquid, result))
                return result;
        }
        return base_Block_mayPick<Base>(state, includeLiquid, 0);
    }

    template <class B>
    auto base_Block_mayPick_2(int) -> decltype(static_cast<B*>(this)->B::mayPick())
    {
        return static_cast<B*>(this)->B::mayPick();
    }
    template <class B>
    bool base_Block_mayPick_2(long)
    {
        return static_cast<Block*>(this)->Block::mayPick();
    }

    virtual bool mayPick()
    {
        if (ModHooks::IsActive(kModHook_Block_mayPick_2))
        {
            bool result = false;
            if (!ModEvents::BlockMayPick2(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_mayPick_2<Base>(0);
    }

    template <class B>
    auto base_Block_randomTick(Level* level, const BlockPos& pos, const BlockState* state, Random* random, int) -> decltype(static_cast<B*>(this)->B::randomTick(level, pos, state, random))
    {
        static_cast<B*>(this)->B::randomTick(level, pos, state, random);
    }
    template <class B>
    void base_Block_randomTick(Level* level, const BlockPos& pos, const BlockState* state, Random* random, long)
    {
        static_cast<Block*>(this)->Block::randomTick(level, pos, state, random);
    }

    virtual void randomTick(Level* level, const BlockPos& pos, const BlockState* state, Random* random)
    {
        if (ModHooks::IsActive(kModHook_Block_randomTick))
        {
            if (!ModEvents::BlockRandomTick(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0)))
                return;
        }
        base_Block_randomTick<Base>(level, pos, state, random, 0);
    }

    template <class B>
    auto base_Block_tick(Level* level, const BlockPos& pos, const BlockState* state, Random* random, int) -> decltype(static_cast<B*>(this)->B::tick(level, pos, state, random))
    {
        static_cast<B*>(this)->B::tick(level, pos, state, random);
    }
    template <class B>
    void base_Block_tick(Level* level, const BlockPos& pos, const BlockState* state, Random* random, long)
    {
        static_cast<Block*>(this)->Block::tick(level, pos, state, random);
    }

    virtual void tick(Level* level, const BlockPos& pos, const BlockState* state, Random* random)
    {
        if (ModHooks::IsActive(kModHook_Block_tick))
        {
            if (!ModEvents::BlockTick(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0)))
                return;
        }
        base_Block_tick<Base>(level, pos, state, random, 0);
    }

    template <class B>
    auto base_Block_animateTick(const BlockState* state, Level* level, const BlockPos& pos, Random* random, int) -> decltype(static_cast<B*>(this)->B::animateTick(state, level, pos, random))
    {
        static_cast<B*>(this)->B::animateTick(state, level, pos, random);
    }
    template <class B>
    void base_Block_animateTick(const BlockState* state, Level* level, const BlockPos& pos, Random* random, long)
    {
        static_cast<Block*>(this)->Block::animateTick(state, level, pos, random);
    }

    virtual void animateTick(const BlockState* state, Level* level, const BlockPos& pos, Random* random)
    {
        if (ModHooks::IsActive(kModHook_Block_animateTick))
        {
            if (!ModEvents::BlockAnimateTick(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), level, pos.x, pos.y, pos.z))
                return;
        }
        base_Block_animateTick<Base>(state, level, pos, random, 0);
    }

    template <class B>
    auto base_Block_destroy(Level* level, const BlockPos& pos, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::destroy(level, pos, state))
    {
        static_cast<B*>(this)->B::destroy(level, pos, state);
    }
    template <class B>
    void base_Block_destroy(Level* level, const BlockPos& pos, const BlockState* state, long)
    {
        static_cast<Block*>(this)->Block::destroy(level, pos, state);
    }

    virtual void destroy(Level* level, const BlockPos& pos, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_destroy))
        {
            if (!ModEvents::BlockDestroy(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0)))
                return;
        }
        base_Block_destroy<Base>(level, pos, state, 0);
    }

    template <class B>
    auto base_Block_neighborChanged(const BlockState* state, Level* level, const BlockPos& pos, Block* block, const BlockPos& fromPos, int) -> decltype(static_cast<B*>(this)->B::neighborChanged(state, level, pos, block, fromPos))
    {
        static_cast<B*>(this)->B::neighborChanged(state, level, pos, block, fromPos);
    }
    template <class B>
    void base_Block_neighborChanged(const BlockState* state, Level* level, const BlockPos& pos, Block* block, const BlockPos& fromPos, long)
    {
        static_cast<Block*>(this)->Block::neighborChanged(state, level, pos, block, fromPos);
    }

    virtual void neighborChanged(const BlockState* state, Level* level, const BlockPos& pos, Block* block, const BlockPos& fromPos)
    {
        if (ModHooks::IsActive(kModHook_Block_neighborChanged))
        {
            if (!ModEvents::BlockNeighborChanged(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), level, pos.x, pos.y, pos.z, block, fromPos.x, fromPos.y, fromPos.z))
                return;
        }
        base_Block_neighborChanged<Base>(state, level, pos, block, fromPos, 0);
    }

    template <class B>
    auto base_Block_addLights(Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::addLights(level, pos))
    {
        static_cast<B*>(this)->B::addLights(level, pos);
    }
    template <class B>
    void base_Block_addLights(Level* level, const BlockPos& pos, long)
    {
        static_cast<Block*>(this)->Block::addLights(level, pos);
    }

    virtual void addLights(Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_addLights))
        {
            if (!ModEvents::BlockAddLights(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z))
                return;
        }
        base_Block_addLights<Base>(level, pos, 0);
    }

    template <class B>
    auto base_Block_getTickDelay(Level* level, int) -> decltype(static_cast<B*>(this)->B::getTickDelay(level))
    {
        return static_cast<B*>(this)->B::getTickDelay(level);
    }
    template <class B>
    int base_Block_getTickDelay(Level* level, long)
    {
        return static_cast<Block*>(this)->Block::getTickDelay(level);
    }

    virtual int getTickDelay(Level* level)
    {
        if (ModHooks::IsActive(kModHook_Block_getTickDelay))
        {
            int result = 0;
            if (!ModEvents::BlockGetTickDelay(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, result))
                return result;
        }
        return base_Block_getTickDelay<Base>(level, 0);
    }

    template <class B>
    auto base_Block_onPlace(Level* level, const BlockPos& pos, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::onPlace(level, pos, state))
    {
        static_cast<B*>(this)->B::onPlace(level, pos, state);
    }
    template <class B>
    void base_Block_onPlace(Level* level, const BlockPos& pos, const BlockState* state, long)
    {
        static_cast<Block*>(this)->Block::onPlace(level, pos, state);
    }

    virtual void onPlace(Level* level, const BlockPos& pos, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_onPlace))
        {
            if (!ModEvents::BlockOnPlace(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0)))
                return;
        }
        base_Block_onPlace<Base>(level, pos, state, 0);
    }

    template <class B>
    auto base_Block_onRemove(Level* level, const BlockPos& pos, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::onRemove(level, pos, state))
    {
        static_cast<B*>(this)->B::onRemove(level, pos, state);
    }
    template <class B>
    void base_Block_onRemove(Level* level, const BlockPos& pos, const BlockState* state, long)
    {
        static_cast<Block*>(this)->Block::onRemove(level, pos, state);
    }

    virtual void onRemove(Level* level, const BlockPos& pos, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_onRemove))
        {
            if (!ModEvents::BlockOnRemove(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0)))
                return;
        }
        base_Block_onRemove<Base>(level, pos, state, 0);
    }

    template <class B>
    auto base_Block_getResourceCount(Random* random, int) -> decltype(static_cast<B*>(this)->B::getResourceCount(random))
    {
        return static_cast<B*>(this)->B::getResourceCount(random);
    }
    template <class B>
    int base_Block_getResourceCount(Random* random, long)
    {
        return static_cast<Block*>(this)->Block::getResourceCount(random);
    }

    virtual int getResourceCount(Random* random)
    {
        if (ModHooks::IsActive(kModHook_Block_getResourceCount))
        {
            int result = 0;
            if (!ModEvents::BlockGetResourceCount(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getResourceCount<Base>(random, 0);
    }

    template <class B>
    auto base_Block_getResource(const BlockState* state, Random* random, int lootBonus, int) -> decltype(static_cast<B*>(this)->B::getResource(state, random, lootBonus))
    {
        return static_cast<B*>(this)->B::getResource(state, random, lootBonus);
    }
    template <class B>
    Item* base_Block_getResource(const BlockState* state, Random* random, int lootBonus, long)
    {
        return static_cast<Block*>(this)->Block::getResource(state, random, lootBonus);
    }

    virtual Item* getResource(const BlockState* state, Random* random, int lootBonus)
    {
        if (ModHooks::IsActive(kModHook_Block_getResource))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::BlockGetResource(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), lootBonus, result))
                return ModItemNames::Find(result);
        }
        return base_Block_getResource<Base>(state, random, lootBonus, 0);
    }

    template <class B>
    auto base_Block_getDestroyProgress(const BlockState* state, boost::shared_ptr<Player> player, Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getDestroyProgress(state, player, level, pos))
    {
        return static_cast<B*>(this)->B::getDestroyProgress(state, player, level, pos);
    }
    template <class B>
    float base_Block_getDestroyProgress(const BlockState* state, boost::shared_ptr<Player> player, Level* level, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getDestroyProgress(state, player, level, pos);
    }

    virtual float getDestroyProgress(const BlockState* state, boost::shared_ptr<Player> player, Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getDestroyProgress))
        {
            float result = 0.0f;
            if (!ModEvents::BlockGetDestroyProgress(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), player, level, pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getDestroyProgress<Base>(state, player, level, pos, 0);
    }

    template <class B>
    auto base_Block_spawnResources(Level* level, const BlockPos& pos, const BlockState* state, int lootBonus, int) -> decltype(static_cast<B*>(this)->B::spawnResources(level, pos, state, lootBonus))
    {
        static_cast<B*>(this)->B::spawnResources(level, pos, state, lootBonus);
    }
    template <class B>
    void base_Block_spawnResources(Level* level, const BlockPos& pos, const BlockState* state, int lootBonus, long)
    {
        static_cast<Block*>(this)->Block::spawnResources(level, pos, state, lootBonus);
    }

    virtual void spawnResources(Level* level, const BlockPos& pos, const BlockState* state, int lootBonus)
    {
        if (ModHooks::IsActive(kModHook_Block_spawnResources))
        {
            if (!ModEvents::BlockSpawnResources(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), lootBonus))
                return;
        }
        base_Block_spawnResources<Base>(level, pos, state, lootBonus, 0);
    }

    template <class B>
    auto base_Block_spawnResources_2(Level* level, const BlockPos& pos, const BlockState* state, float chance, int lootBonus, int) -> decltype(static_cast<B*>(this)->B::spawnResources(level, pos, state, chance, lootBonus))
    {
        static_cast<B*>(this)->B::spawnResources(level, pos, state, chance, lootBonus);
    }
    template <class B>
    void base_Block_spawnResources_2(Level* level, const BlockPos& pos, const BlockState* state, float chance, int lootBonus, long)
    {
        static_cast<Block*>(this)->Block::spawnResources(level, pos, state, chance, lootBonus);
    }

    virtual void spawnResources(Level* level, const BlockPos& pos, const BlockState* state, float chance, int lootBonus)
    {
        if (ModHooks::IsActive(kModHook_Block_spawnResources_2))
        {
            if (!ModEvents::BlockSpawnResources2(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), chance, lootBonus))
                return;
        }
        base_Block_spawnResources_2<Base>(level, pos, state, chance, lootBonus, 0);
    }

    template <class B>
    auto base_Block_popExperience(Level* level, const BlockPos& pos, int amount, int) -> decltype(static_cast<B*>(this)->B::popExperience(level, pos, amount))
    {
        static_cast<B*>(this)->B::popExperience(level, pos, amount);
    }
    template <class B>
    void base_Block_popExperience(Level* level, const BlockPos& pos, int amount, long)
    {
        static_cast<Block*>(this)->Block::popExperience(level, pos, amount);
    }

    virtual void popExperience(Level* level, const BlockPos& pos, int amount)
    {
        if (ModHooks::IsActive(kModHook_Block_popExperience))
        {
            if (!ModEvents::BlockPopExperience(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, amount))
                return;
        }
        base_Block_popExperience<Base>(level, pos, amount, 0);
    }

    template <class B>
    auto base_Block_getSpawnResourcesAuxValue(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getSpawnResourcesAuxValue(state))
    {
        return static_cast<B*>(this)->B::getSpawnResourcesAuxValue(state);
    }
    template <class B>
    int base_Block_getSpawnResourcesAuxValue(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getSpawnResourcesAuxValue(state);
    }

    virtual int getSpawnResourcesAuxValue(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getSpawnResourcesAuxValue))
        {
            int result = 0;
            if (!ModEvents::BlockGetSpawnResourcesAuxValue(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getSpawnResourcesAuxValue<Base>(state, 0);
    }

    template <class B>
    auto base_Block_getExplosionResistance(boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::getExplosionResistance(entity))
    {
        return static_cast<B*>(this)->B::getExplosionResistance(entity);
    }
    template <class B>
    float base_Block_getExplosionResistance(boost::shared_ptr<Entity> entity, long)
    {
        return static_cast<Block*>(this)->Block::getExplosionResistance(entity);
    }

    virtual float getExplosionResistance(boost::shared_ptr<Entity> entity)
    {
        if (ModHooks::IsActive(kModHook_Block_getExplosionResistance))
        {
            float result = 0.0f;
            if (!ModEvents::BlockGetExplosionResistance(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), entity, result))
                return result;
        }
        return base_Block_getExplosionResistance<Base>(entity, 0);
    }

    template <class B>
    auto base_Block_clip(const BlockState* state, Level* level, const BlockPos& pos, Vec3* from, Vec3* to, int) -> decltype(static_cast<B*>(this)->B::clip(state, level, pos, from, to))
    {
        return static_cast<B*>(this)->B::clip(state, level, pos, from, to);
    }
    template <class B>
    HitResult* base_Block_clip(const BlockState* state, Level* level, const BlockPos& pos, Vec3* from, Vec3* to, long)
    {
        return static_cast<Block*>(this)->Block::clip(state, level, pos, from, to);
    }

    virtual HitResult* clip(const BlockState* state, Level* level, const BlockPos& pos, Vec3* from, Vec3* to)
    {
        if (ModHooks::IsActive(kModHook_Block_clip))
        {
            HitResult* result = NULL;
            if (!ModEvents::BlockClip(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), level, pos.x, pos.y, pos.z, const_cast<Vec3*>(from), const_cast<Vec3*>(to), result))
                return result;
        }
        return base_Block_clip<Base>(state, level, pos, from, to, 0);
    }

    template <class B>
    auto base_Block_clip_2(const BlockPos& pos, Vec3* from, Vec3* to, const AABB* box, int) -> decltype(static_cast<B*>(this)->B::clip(pos, from, to, box))
    {
        return static_cast<B*>(this)->B::clip(pos, from, to, box);
    }
    template <class B>
    HitResult* base_Block_clip_2(const BlockPos& pos, Vec3* from, Vec3* to, const AABB* box, long)
    {
        return static_cast<Block*>(this)->Block::clip(pos, from, to, box);
    }

    virtual HitResult* clip(const BlockPos& pos, Vec3* from, Vec3* to, const AABB* box)
    {
        if (ModHooks::IsActive(kModHook_Block_clip_2))
        {
            HitResult* result = NULL;
            if (!ModEvents::BlockClip2(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), pos.x, pos.y, pos.z, const_cast<Vec3*>(from), const_cast<Vec3*>(to), const_cast<AABB*>(box), result))
                return result;
        }
        return base_Block_clip_2<Base>(pos, from, to, box, 0);
    }

    template <class B>
    auto base_Block_wasExploded(Level* level, const BlockPos& pos, Explosion* explosion, int) -> decltype(static_cast<B*>(this)->B::wasExploded(level, pos, explosion))
    {
        static_cast<B*>(this)->B::wasExploded(level, pos, explosion);
    }
    template <class B>
    void base_Block_wasExploded(Level* level, const BlockPos& pos, Explosion* explosion, long)
    {
        static_cast<Block*>(this)->Block::wasExploded(level, pos, explosion);
    }

    virtual void wasExploded(Level* level, const BlockPos& pos, Explosion* explosion)
    {
        if (ModHooks::IsActive(kModHook_Block_wasExploded))
        {
            if (!ModEvents::BlockWasExploded(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, const_cast<Explosion*>(explosion)))
                return;
        }
        base_Block_wasExploded<Base>(level, pos, explosion, 0);
    }

    template <class B>
    auto base_Block_getRenderLayer(int) -> decltype(static_cast<B*>(this)->B::getRenderLayer())
    {
        return static_cast<B*>(this)->B::getRenderLayer();
    }
    template <class B>
    int base_Block_getRenderLayer(long)
    {
        return static_cast<Block*>(this)->Block::getRenderLayer();
    }

    virtual int getRenderLayer()
    {
        if (ModHooks::IsActive(kModHook_Block_getRenderLayer))
        {
            int result = 0;
            if (!ModEvents::BlockGetRenderLayer(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getRenderLayer<Base>(0);
    }

    template <class B>
    auto base_Block_mayPlace(Level* level, const BlockPos& pos, const Direction* direction, int) -> decltype(static_cast<B*>(this)->B::mayPlace(level, pos, direction))
    {
        return static_cast<B*>(this)->B::mayPlace(level, pos, direction);
    }
    template <class B>
    bool base_Block_mayPlace(Level* level, const BlockPos& pos, const Direction* direction, long)
    {
        return static_cast<Block*>(this)->Block::mayPlace(level, pos, direction);
    }

    virtual bool mayPlace(Level* level, const BlockPos& pos, const Direction* direction)
    {
        if (ModHooks::IsActive(kModHook_Block_mayPlace))
        {
            bool result = false;
            if (!ModEvents::BlockMayPlace(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), result))
                return result;
        }
        return base_Block_mayPlace<Base>(level, pos, direction, 0);
    }

    template <class B>
    auto base_Block_mayPlace_2(Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::mayPlace(level, pos))
    {
        return static_cast<B*>(this)->B::mayPlace(level, pos);
    }
    template <class B>
    bool base_Block_mayPlace_2(Level* level, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::mayPlace(level, pos);
    }

    virtual bool mayPlace(Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_mayPlace_2))
        {
            bool result = false;
            if (!ModEvents::BlockMayPlace2(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_mayPlace_2<Base>(level, pos, 0);
    }

    template <class B>
    auto base_Block_mayPlaceOn(Level* level, const BlockPos& pos, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::mayPlaceOn(level, pos, state))
    {
        return static_cast<B*>(this)->B::mayPlaceOn(level, pos, state);
    }
    template <class B>
    bool base_Block_mayPlaceOn(Level* level, const BlockPos& pos, const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::mayPlaceOn(level, pos, state);
    }

    virtual bool mayPlaceOn(Level* level, const BlockPos& pos, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_mayPlaceOn))
        {
            bool result = false;
            if (!ModEvents::BlockMayPlaceOn(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_mayPlaceOn<Base>(level, pos, state, 0);
    }

    template <class B>
    auto base_Block_TestUse(int) -> decltype(static_cast<B*>(this)->B::TestUse())
    {
        return static_cast<B*>(this)->B::TestUse();
    }
    template <class B>
    bool base_Block_TestUse(long)
    {
        return static_cast<Block*>(this)->Block::TestUse();
    }

    virtual bool TestUse()
    {
        if (ModHooks::IsActive(kModHook_Block_TestUse))
        {
            bool result = false;
            if (!ModEvents::BlockTestUse(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_TestUse<Base>(0);
    }

    template <class B>
    auto base_Block_TestUse_2(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player, int) -> decltype(static_cast<B*>(this)->B::TestUse(level, pos, state, player))
    {
        return static_cast<B*>(this)->B::TestUse(level, pos, state, player);
    }
    template <class B>
    bool base_Block_TestUse_2(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player, long)
    {
        return static_cast<Block*>(this)->Block::TestUse(level, pos, state, player);
    }

    virtual bool TestUse(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player)
    {
        if (ModHooks::IsActive(kModHook_Block_TestUse_2))
        {
            bool result = false;
            if (!ModEvents::BlockTestUse2(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), player, result))
                return result;
        }
        return base_Block_TestUse_2<Base>(level, pos, state, player, 0);
    }

    template <class B>
    auto base_Block_use(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player, InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY, float hitZ, bool clientSide, int) -> decltype(static_cast<B*>(this)->B::use(level, pos, state, player, hand, direction, hitX, hitY, hitZ, clientSide))
    {
        return static_cast<B*>(this)->B::use(level, pos, state, player, hand, direction, hitX, hitY, hitZ, clientSide);
    }
    template <class B>
    bool base_Block_use(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player, InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY, float hitZ, bool clientSide, long)
    {
        return static_cast<Block*>(this)->Block::use(level, pos, state, player, hand, direction, hitX, hitY, hitZ, clientSide);
    }

    virtual bool use(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player, InteractionHand::EInteractionHand hand, const Direction* direction, float hitX, float hitY, float hitZ, bool clientSide)
    {
        if (ModHooks::IsActive(kModHook_Block_use))
        {
            bool result = false;
            if (!ModEvents::BlockUse(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), player, static_cast<int>(hand), (direction != NULL ? direction->get3DDataValue() : -1), hitX, hitY, hitZ, clientSide, result))
                return result;
        }
        return base_Block_use<Base>(level, pos, state, player, hand, direction, hitX, hitY, hitZ, clientSide, 0);
    }

    template <class B>
    auto base_Block_stepOn(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::stepOn(level, pos, entity))
    {
        static_cast<B*>(this)->B::stepOn(level, pos, entity);
    }
    template <class B>
    void base_Block_stepOn(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, long)
    {
        static_cast<Block*>(this)->Block::stepOn(level, pos, entity);
    }

    virtual void stepOn(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity)
    {
        if (ModHooks::IsActive(kModHook_Block_stepOn))
        {
            if (!ModEvents::BlockStepOn(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, entity))
                return;
        }
        base_Block_stepOn<Base>(level, pos, entity, 0);
    }

    template <class B>
    auto base_Block_getStateForPlacement(Level* level, const BlockPos& pos, const Direction* direction, float hitX, float hitY, float hitZ, int auxValue, boost::shared_ptr<LivingEntity> placer, int) -> decltype(static_cast<B*>(this)->B::getStateForPlacement(level, pos, direction, hitX, hitY, hitZ, auxValue, placer))
    {
        return static_cast<B*>(this)->B::getStateForPlacement(level, pos, direction, hitX, hitY, hitZ, auxValue, placer);
    }
    template <class B>
    const BlockState* base_Block_getStateForPlacement(Level* level, const BlockPos& pos, const Direction* direction, float hitX, float hitY, float hitZ, int auxValue, boost::shared_ptr<LivingEntity> placer, long)
    {
        return static_cast<Block*>(this)->Block::getStateForPlacement(level, pos, direction, hitX, hitY, hitZ, auxValue, placer);
    }

    virtual const BlockState* getStateForPlacement(Level* level, const BlockPos& pos, const Direction* direction, float hitX, float hitY, float hitZ, int auxValue, boost::shared_ptr<LivingEntity> placer)
    {
        if (ModHooks::IsActive(kModHook_Block_getStateForPlacement))
        {
            int result = 0;
            if (!ModEvents::BlockGetStateForPlacement(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), hitX, hitY, hitZ, auxValue, placer, result))
                return this->getBlockState(result);
        }
        return base_Block_getStateForPlacement<Base>(level, pos, direction, hitX, hitY, hitZ, auxValue, placer, 0);
    }

    template <class B>
    auto base_Block_prepareRender(Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::prepareRender(level, pos))
    {
        static_cast<B*>(this)->B::prepareRender(level, pos);
    }
    template <class B>
    void base_Block_prepareRender(Level* level, const BlockPos& pos, long)
    {
        static_cast<Block*>(this)->Block::prepareRender(level, pos);
    }

    virtual void prepareRender(Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_prepareRender))
        {
            if (!ModEvents::BlockPrepareRender(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z))
                return;
        }
        base_Block_prepareRender<Base>(level, pos, 0);
    }

    template <class B>
    auto base_Block_attack(Level* level, const BlockPos& pos, boost::shared_ptr<Player> player, int) -> decltype(static_cast<B*>(this)->B::attack(level, pos, player))
    {
        static_cast<B*>(this)->B::attack(level, pos, player);
    }
    template <class B>
    void base_Block_attack(Level* level, const BlockPos& pos, boost::shared_ptr<Player> player, long)
    {
        static_cast<Block*>(this)->Block::attack(level, pos, player);
    }

    virtual void attack(Level* level, const BlockPos& pos, boost::shared_ptr<Player> player)
    {
        if (ModHooks::IsActive(kModHook_Block_attack))
        {
            if (!ModEvents::BlockAttack(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, player))
                return;
        }
        base_Block_attack<Base>(level, pos, player, 0);
    }

    template <class B>
    auto base_Block_handleEntityInside(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, Vec3* motion, int) -> decltype(static_cast<B*>(this)->B::handleEntityInside(level, pos, entity, motion))
    {
        return static_cast<B*>(this)->B::handleEntityInside(level, pos, entity, motion);
    }
    template <class B>
    Vec3* base_Block_handleEntityInside(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, Vec3* motion, long)
    {
        return static_cast<Block*>(this)->Block::handleEntityInside(level, pos, entity, motion);
    }

    virtual Vec3* handleEntityInside(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, Vec3* motion)
    {
        if (ModHooks::IsActive(kModHook_Block_handleEntityInside))
        {
            Vec3* result = NULL;
            if (!ModEvents::BlockHandleEntityInside(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, entity, const_cast<Vec3*>(motion), result))
                return result;
        }
        return base_Block_handleEntityInside<Base>(level, pos, entity, motion, 0);
    }

    template <class B>
    auto base_Block_getColor(int) const -> decltype(static_cast<const B*>(this)->B::getColor())
    {
        return static_cast<const B*>(this)->B::getColor();
    }
    template <class B>
    int base_Block_getColor(long) const
    {
        return static_cast<const Block*>(this)->Block::getColor();
    }

    virtual int getColor() const
    {
        if (ModHooks::IsActive(kModHook_Block_getColor))
        {
            int result = 0;
            if (!ModEvents::BlockGetColor(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getColor<Base>(0);
    }

    template <class B>
    auto base_Block_getColor_2(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getColor(state))
    {
        return static_cast<B*>(this)->B::getColor(state);
    }
    template <class B>
    int base_Block_getColor_2(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getColor(state);
    }

    virtual int getColor(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getColor_2))
        {
            int result = 0;
            if (!ModEvents::BlockGetColor2(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getColor_2<Base>(state, 0);
    }

    template <class B>
    auto base_Block_getColor_3(LevelSource* levelSource, const BlockPos& pos, int tintIndex, int) -> decltype(static_cast<B*>(this)->B::getColor(levelSource, pos, tintIndex))
    {
        return static_cast<B*>(this)->B::getColor(levelSource, pos, tintIndex);
    }
    template <class B>
    int base_Block_getColor_3(LevelSource* levelSource, const BlockPos& pos, int tintIndex, long)
    {
        return static_cast<Block*>(this)->Block::getColor(levelSource, pos, tintIndex);
    }

    virtual int getColor(LevelSource* levelSource, const BlockPos& pos, int tintIndex)
    {
        if (ModHooks::IsActive(kModHook_Block_getColor_3))
        {
            int result = 0;
            if (!ModEvents::BlockGetColor3(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, tintIndex, result))
                return result;
        }
        return base_Block_getColor_3<Base>(levelSource, pos, tintIndex, 0);
    }

    template <class B>
    auto base_Block_getColor_4(LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getColor(levelSource, pos))
    {
        return static_cast<B*>(this)->B::getColor(levelSource, pos);
    }
    template <class B>
    int base_Block_getColor_4(LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getColor(levelSource, pos);
    }

    virtual int getColor(LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getColor_4))
        {
            int result = 0;
            if (!ModEvents::BlockGetColor4(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getColor_4<Base>(levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_getColor_5(LevelSource* levelSource, const BlockPos& pos, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getColor(levelSource, pos, state))
    {
        return static_cast<B*>(this)->B::getColor(levelSource, pos, state);
    }
    template <class B>
    int base_Block_getColor_5(LevelSource* levelSource, const BlockPos& pos, const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getColor(levelSource, pos, state);
    }

    virtual int getColor(LevelSource* levelSource, const BlockPos& pos, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getColor_5))
        {
            int result = 0;
            if (!ModEvents::BlockGetColor5(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getColor_5<Base>(levelSource, pos, state, 0);
    }

    template <class B>
    auto base_Block_getSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction, int) -> decltype(static_cast<B*>(this)->B::getSignal(state, levelSource, pos, direction))
    {
        return static_cast<B*>(this)->B::getSignal(state, levelSource, pos, direction);
    }
    template <class B>
    int base_Block_getSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction, long)
    {
        return static_cast<Block*>(this)->Block::getSignal(state, levelSource, pos, direction);
    }

    virtual int getSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction)
    {
        if (ModHooks::IsActive(kModHook_Block_getSignal))
        {
            int result = 0;
            if (!ModEvents::BlockGetSignal(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), result))
                return result;
        }
        return base_Block_getSignal<Base>(state, levelSource, pos, direction, 0);
    }

    template <class B>
    auto base_Block_isSignalSource(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::isSignalSource(state))
    {
        return static_cast<B*>(this)->B::isSignalSource(state);
    }
    template <class B>
    bool base_Block_isSignalSource(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::isSignalSource(state);
    }

    virtual bool isSignalSource(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_isSignalSource))
        {
            bool result = false;
            if (!ModEvents::BlockIsSignalSource(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_isSignalSource<Base>(state, 0);
    }

    template <class B>
    auto base_Block_entityInside(Level* level, const BlockPos& pos, const BlockState* state, const boost::shared_ptr<Entity>& entity, int) -> decltype(static_cast<B*>(this)->B::entityInside(level, pos, state, entity))
    {
        static_cast<B*>(this)->B::entityInside(level, pos, state, entity);
    }
    template <class B>
    void base_Block_entityInside(Level* level, const BlockPos& pos, const BlockState* state, const boost::shared_ptr<Entity>& entity, long)
    {
        static_cast<Block*>(this)->Block::entityInside(level, pos, state, entity);
    }

    virtual void entityInside(Level* level, const BlockPos& pos, const BlockState* state, const boost::shared_ptr<Entity>& entity)
    {
        if (ModHooks::IsActive(kModHook_Block_entityInside))
        {
            if (!ModEvents::BlockEntityInside(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), entity))
                return;
        }
        base_Block_entityInside<Base>(level, pos, state, entity, 0);
    }

    template <class B>
    auto base_Block_getDirectSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction, int) -> decltype(static_cast<B*>(this)->B::getDirectSignal(state, levelSource, pos, direction))
    {
        return static_cast<B*>(this)->B::getDirectSignal(state, levelSource, pos, direction);
    }
    template <class B>
    int base_Block_getDirectSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction, long)
    {
        return static_cast<Block*>(this)->Block::getDirectSignal(state, levelSource, pos, direction);
    }

    virtual int getDirectSignal(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, const Direction* direction)
    {
        if (ModHooks::IsActive(kModHook_Block_getDirectSignal))
        {
            int result = 0;
            if (!ModEvents::BlockGetDirectSignal(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), result))
                return result;
        }
        return base_Block_getDirectSignal<Base>(state, levelSource, pos, direction, 0);
    }

    template <class B>
    auto base_Block_waterSpreadCausesSpawn(int) -> decltype(static_cast<B*>(this)->B::waterSpreadCausesSpawn())
    {
        return static_cast<B*>(this)->B::waterSpreadCausesSpawn();
    }
    template <class B>
    bool base_Block_waterSpreadCausesSpawn(long)
    {
        return static_cast<Block*>(this)->Block::waterSpreadCausesSpawn();
    }

    virtual bool waterSpreadCausesSpawn()
    {
        if (ModHooks::IsActive(kModHook_Block_waterSpreadCausesSpawn))
        {
            bool result = false;
            if (!ModEvents::BlockWaterSpreadCausesSpawn(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_waterSpreadCausesSpawn<Base>(0);
    }

    template <class B>
    auto base_Block_playerDestroy(Level* level, boost::shared_ptr<Player> player, const BlockPos& pos, const BlockState* state, boost::shared_ptr<BlockEntity> blockEntity, not_null_ptr<ItemInstance> tool, int) -> decltype(static_cast<B*>(this)->B::playerDestroy(level, player, pos, state, blockEntity, tool))
    {
        static_cast<B*>(this)->B::playerDestroy(level, player, pos, state, blockEntity, tool);
    }
    template <class B>
    void base_Block_playerDestroy(Level* level, boost::shared_ptr<Player> player, const BlockPos& pos, const BlockState* state, boost::shared_ptr<BlockEntity> blockEntity, not_null_ptr<ItemInstance> tool, long)
    {
        static_cast<Block*>(this)->Block::playerDestroy(level, player, pos, state, blockEntity, tool);
    }

    virtual void playerDestroy(Level* level, boost::shared_ptr<Player> player, const BlockPos& pos, const BlockState* state, boost::shared_ptr<BlockEntity> blockEntity, not_null_ptr<ItemInstance> tool)
    {
        if (ModHooks::IsActive(kModHook_Block_playerDestroy))
        {
            if (!ModEvents::BlockPlayerDestroy(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, player, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), const_cast<BlockEntity*>(blockEntity.get()), const_cast<ItemInstance*>(tool.get())))
                return;
        }
        base_Block_playerDestroy<Base>(level, player, pos, state, blockEntity, tool, 0);
    }

    template <class B>
    auto base_Block_isSilkTouchable(int) -> decltype(static_cast<B*>(this)->B::isSilkTouchable())
    {
        return static_cast<B*>(this)->B::isSilkTouchable();
    }
    template <class B>
    bool base_Block_isSilkTouchable(long)
    {
        return static_cast<Block*>(this)->Block::isSilkTouchable();
    }

    virtual bool isSilkTouchable()
    {
        if (ModHooks::IsActive(kModHook_Block_isSilkTouchable))
        {
            bool result = false;
            if (!ModEvents::BlockIsSilkTouchable(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isSilkTouchable<Base>(0);
    }

    template <class B>
    auto base_Block_canSurvive(Level* level, const BlockPos& pos, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::canSurvive(level, pos, state))
    {
        return static_cast<B*>(this)->B::canSurvive(level, pos, state);
    }
    template <class B>
    bool base_Block_canSurvive(Level* level, const BlockPos& pos, const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::canSurvive(level, pos, state);
    }

    virtual bool canSurvive(Level* level, const BlockPos& pos, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_canSurvive))
        {
            bool result = false;
            if (!ModEvents::BlockCanSurvive(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_canSurvive<Base>(level, pos, state, 0);
    }

    template <class B>
    auto base_Block_getResourceCountForLootBonus(int lootBonus, Random* random, int) -> decltype(static_cast<B*>(this)->B::getResourceCountForLootBonus(lootBonus, random))
    {
        return static_cast<B*>(this)->B::getResourceCountForLootBonus(lootBonus, random);
    }
    template <class B>
    int base_Block_getResourceCountForLootBonus(int lootBonus, Random* random, long)
    {
        return static_cast<Block*>(this)->Block::getResourceCountForLootBonus(lootBonus, random);
    }

    virtual int getResourceCountForLootBonus(int lootBonus, Random* random)
    {
        if (ModHooks::IsActive(kModHook_Block_getResourceCountForLootBonus))
        {
            int result = 0;
            if (!ModEvents::BlockGetResourceCountForLootBonus(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), lootBonus, result))
                return result;
        }
        return base_Block_getResourceCountForLootBonus<Base>(lootBonus, random, 0);
    }

    template <class B>
    auto base_Block_setPlacedBy(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<LivingEntity> placer, not_null_ptr<ItemInstance> itemInstance, int) -> decltype(static_cast<B*>(this)->B::setPlacedBy(level, pos, state, placer, itemInstance))
    {
        static_cast<B*>(this)->B::setPlacedBy(level, pos, state, placer, itemInstance);
    }
    template <class B>
    void base_Block_setPlacedBy(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<LivingEntity> placer, not_null_ptr<ItemInstance> itemInstance, long)
    {
        static_cast<Block*>(this)->Block::setPlacedBy(level, pos, state, placer, itemInstance);
    }

    virtual void setPlacedBy(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<LivingEntity> placer, not_null_ptr<ItemInstance> itemInstance)
    {
        if (ModHooks::IsActive(kModHook_Block_setPlacedBy))
        {
            if (!ModEvents::BlockSetPlacedBy(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), placer, const_cast<ItemInstance*>(itemInstance.get())))
                return;
        }
        base_Block_setPlacedBy<Base>(level, pos, state, placer, itemInstance, 0);
    }

    template <class B>
    auto base_Block_setNameAndDescriptionId(int descriptionId, int useDescriptionId, int) -> decltype(static_cast<B*>(this)->B::setNameAndDescriptionId(descriptionId, useDescriptionId))
    {
        return static_cast<B*>(this)->B::setNameAndDescriptionId(descriptionId, useDescriptionId);
    }
    template <class B>
    Block* base_Block_setNameAndDescriptionId(int descriptionId, int useDescriptionId, long)
    {
        return static_cast<Block*>(this)->Block::setNameAndDescriptionId(descriptionId, useDescriptionId);
    }

    virtual Block* setNameAndDescriptionId(int descriptionId, int useDescriptionId)
    {
        if (ModHooks::IsActive(kModHook_Block_setNameAndDescriptionId))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetNameAndDescriptionId(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), descriptionId, useDescriptionId, result))
                return result;
        }
        return base_Block_setNameAndDescriptionId<Base>(descriptionId, useDescriptionId, 0);
    }

    template <class B>
    auto base_Block_isPossibleToRespawnInThis(int) -> decltype(static_cast<B*>(this)->B::isPossibleToRespawnInThis())
    {
        return static_cast<B*>(this)->B::isPossibleToRespawnInThis();
    }
    template <class B>
    bool base_Block_isPossibleToRespawnInThis(long)
    {
        return static_cast<Block*>(this)->Block::isPossibleToRespawnInThis();
    }

    virtual bool isPossibleToRespawnInThis()
    {
        if (ModHooks::IsActive(kModHook_Block_isPossibleToRespawnInThis))
        {
            bool result = false;
            if (!ModEvents::BlockIsPossibleToRespawnInThis(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isPossibleToRespawnInThis<Base>(0);
    }

    template <class B>
    auto base_Block_getName(int) -> decltype(static_cast<B*>(this)->B::getName())
    {
        return static_cast<B*>(this)->B::getName();
    }
    template <class B>
    std::wstring base_Block_getName(long)
    {
        return static_cast<Block*>(this)->Block::getName();
    }

    virtual std::wstring getName()
    {
        if (ModHooks::IsActive(kModHook_Block_getName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::BlockGetName(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getName<Base>(0);
    }

    template <class B>
    auto base_Block_getDescriptionId(int auxValue, int) -> decltype(static_cast<B*>(this)->B::getDescriptionId(auxValue))
    {
        return static_cast<B*>(this)->B::getDescriptionId(auxValue);
    }
    template <class B>
    int base_Block_getDescriptionId(int auxValue, long)
    {
        return static_cast<Block*>(this)->Block::getDescriptionId(auxValue);
    }

    virtual int getDescriptionId(int auxValue)
    {
        if (ModHooks::IsActive(kModHook_Block_getDescriptionId))
        {
            int result = 0;
            if (!ModEvents::BlockGetDescriptionId(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), auxValue, result))
                return result;
        }
        return base_Block_getDescriptionId<Base>(auxValue, 0);
    }

    template <class B>
    auto base_Block_getUseDescriptionId(int auxValue, int) -> decltype(static_cast<B*>(this)->B::getUseDescriptionId(auxValue))
    {
        return static_cast<B*>(this)->B::getUseDescriptionId(auxValue);
    }
    template <class B>
    int base_Block_getUseDescriptionId(int auxValue, long)
    {
        return static_cast<Block*>(this)->Block::getUseDescriptionId(auxValue);
    }

    virtual int getUseDescriptionId(int auxValue)
    {
        if (ModHooks::IsActive(kModHook_Block_getUseDescriptionId))
        {
            int result = 0;
            if (!ModEvents::BlockGetUseDescriptionId(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), auxValue, result))
                return result;
        }
        return base_Block_getUseDescriptionId<Base>(auxValue, 0);
    }

    template <class B>
    auto base_Block_triggerEvent(const BlockState* state, Level* level, const BlockPos& pos, int eventId, int eventParam, int) -> decltype(static_cast<B*>(this)->B::triggerEvent(state, level, pos, eventId, eventParam))
    {
        return static_cast<B*>(this)->B::triggerEvent(state, level, pos, eventId, eventParam);
    }
    template <class B>
    bool base_Block_triggerEvent(const BlockState* state, Level* level, const BlockPos& pos, int eventId, int eventParam, long)
    {
        return static_cast<Block*>(this)->Block::triggerEvent(state, level, pos, eventId, eventParam);
    }

    virtual bool triggerEvent(const BlockState* state, Level* level, const BlockPos& pos, int eventId, int eventParam)
    {
        if (ModHooks::IsActive(kModHook_Block_triggerEvent))
        {
            bool result = false;
            if (!ModEvents::BlockTriggerEvent(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), level, pos.x, pos.y, pos.z, eventId, eventParam, result))
                return result;
        }
        return base_Block_triggerEvent<Base>(state, level, pos, eventId, eventParam, 0);
    }

    template <class B>
    auto base_Block_isCollectStatistics(int) -> decltype(static_cast<B*>(this)->B::isCollectStatistics())
    {
        return static_cast<B*>(this)->B::isCollectStatistics();
    }
    template <class B>
    bool base_Block_isCollectStatistics(long)
    {
        return static_cast<Block*>(this)->Block::isCollectStatistics();
    }

    virtual bool isCollectStatistics()
    {
        if (ModHooks::IsActive(kModHook_Block_isCollectStatistics))
        {
            bool result = false;
            if (!ModEvents::BlockIsCollectStatistics(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isCollectStatistics<Base>(0);
    }

    template <class B>
    auto base_Block_shouldBlockTick(Level* level, const BlockPos& pos, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::shouldBlockTick(level, pos, state))
    {
        return static_cast<B*>(this)->B::shouldBlockTick(level, pos, state);
    }
    template <class B>
    bool base_Block_shouldBlockTick(Level* level, const BlockPos& pos, const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::shouldBlockTick(level, pos, state);
    }

    virtual bool shouldBlockTick(Level* level, const BlockPos& pos, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_shouldBlockTick))
        {
            bool result = false;
            if (!ModEvents::BlockShouldBlockTick(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_shouldBlockTick<Base>(level, pos, state, 0);
    }

    template <class B>
    auto base_Block_setNotCollectStatistics(int) -> decltype(static_cast<B*>(this)->B::setNotCollectStatistics())
    {
        return static_cast<B*>(this)->B::setNotCollectStatistics();
    }
    template <class B>
    Block* base_Block_setNotCollectStatistics(long)
    {
        return static_cast<Block*>(this)->Block::setNotCollectStatistics();
    }

    virtual Block* setNotCollectStatistics()
    {
        if (ModHooks::IsActive(kModHook_Block_setNotCollectStatistics))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetNotCollectStatistics(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_setNotCollectStatistics<Base>(0);
    }

    template <class B>
    auto base_Block_getPistonPushReaction(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getPistonPushReaction(state))
    {
        return static_cast<B*>(this)->B::getPistonPushReaction(state);
    }
    template <class B>
    EPistonPushReaction base_Block_getPistonPushReaction(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getPistonPushReaction(state);
    }

    virtual EPistonPushReaction getPistonPushReaction(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getPistonPushReaction))
        {
            int result = 0;
            if (!ModEvents::BlockGetPistonPushReaction(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return static_cast<EPistonPushReaction>(result);
        }
        return base_Block_getPistonPushReaction<Base>(state, 0);
    }

    template <class B>
    auto base_Block_getShadeBrightness(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getShadeBrightness(state))
    {
        return static_cast<B*>(this)->B::getShadeBrightness(state);
    }
    template <class B>
    float base_Block_getShadeBrightness(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getShadeBrightness(state);
    }

    virtual float getShadeBrightness(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getShadeBrightness))
        {
            float result = 0.0f;
            if (!ModEvents::BlockGetShadeBrightness(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getShadeBrightness<Base>(state, 0);
    }

    template <class B>
    auto base_Block_fallOn(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, float fallDistance, int) -> decltype(static_cast<B*>(this)->B::fallOn(level, pos, entity, fallDistance))
    {
        static_cast<B*>(this)->B::fallOn(level, pos, entity, fallDistance);
    }
    template <class B>
    void base_Block_fallOn(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, float fallDistance, long)
    {
        static_cast<Block*>(this)->Block::fallOn(level, pos, entity, fallDistance);
    }

    virtual void fallOn(Level* level, const BlockPos& pos, boost::shared_ptr<Entity> entity, float fallDistance)
    {
        if (ModHooks::IsActive(kModHook_Block_fallOn))
        {
            if (!ModEvents::BlockFallOn(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, entity, fallDistance))
                return;
        }
        base_Block_fallOn<Base>(level, pos, entity, fallDistance, 0);
    }

    template <class B>
    auto base_Block_updateEntityAfterFallOn(Level* level, boost::shared_ptr<Entity> entity, int) -> decltype(static_cast<B*>(this)->B::updateEntityAfterFallOn(level, entity))
    {
        static_cast<B*>(this)->B::updateEntityAfterFallOn(level, entity);
    }
    template <class B>
    void base_Block_updateEntityAfterFallOn(Level* level, boost::shared_ptr<Entity> entity, long)
    {
        static_cast<Block*>(this)->Block::updateEntityAfterFallOn(level, entity);
    }

    virtual void updateEntityAfterFallOn(Level* level, boost::shared_ptr<Entity> entity)
    {
        if (ModHooks::IsActive(kModHook_Block_updateEntityAfterFallOn))
        {
            if (!ModEvents::BlockUpdateEntityAfterFallOn(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, entity))
                return;
        }
        base_Block_updateEntityAfterFallOn<Base>(level, entity, 0);
    }

    template <class B>
    auto base_Block_playerWillDestroy(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player, int) -> decltype(static_cast<B*>(this)->B::playerWillDestroy(level, pos, state, player))
    {
        return static_cast<B*>(this)->B::playerWillDestroy(level, pos, state, player);
    }
    template <class B>
    bool base_Block_playerWillDestroy(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player, long)
    {
        return static_cast<Block*>(this)->Block::playerWillDestroy(level, pos, state, player);
    }

    virtual bool playerWillDestroy(Level* level, const BlockPos& pos, const BlockState* state, boost::shared_ptr<Player> player)
    {
        if (ModHooks::IsActive(kModHook_Block_playerWillDestroy))
        {
            bool result = false;
            if (!ModEvents::BlockPlayerWillDestroy(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z, (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), player, result))
                return result;
        }
        return base_Block_playerWillDestroy<Base>(level, pos, state, player, 0);
    }

    template <class B>
    auto base_Block_handleRain(Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::handleRain(level, pos))
    {
        static_cast<B*>(this)->B::handleRain(level, pos);
    }
    template <class B>
    void base_Block_handleRain(Level* level, const BlockPos& pos, long)
    {
        static_cast<Block*>(this)->Block::handleRain(level, pos);
    }

    virtual void handleRain(Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_handleRain))
        {
            if (!ModEvents::BlockHandleRain(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), level, pos.x, pos.y, pos.z))
                return;
        }
        base_Block_handleRain<Base>(level, pos, 0);
    }

    template <class B>
    auto base_Block_canInstantlyTick(int) -> decltype(static_cast<B*>(this)->B::canInstantlyTick())
    {
        return static_cast<B*>(this)->B::canInstantlyTick();
    }
    template <class B>
    bool base_Block_canInstantlyTick(long)
    {
        return static_cast<Block*>(this)->Block::canInstantlyTick();
    }

    virtual bool canInstantlyTick()
    {
        if (ModHooks::IsActive(kModHook_Block_canInstantlyTick))
        {
            bool result = false;
            if (!ModEvents::BlockCanInstantlyTick(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_canInstantlyTick<Base>(0);
    }

    template <class B>
    auto base_Block_dropFromExplosion(Explosion* explosion, int) -> decltype(static_cast<B*>(this)->B::dropFromExplosion(explosion))
    {
        return static_cast<B*>(this)->B::dropFromExplosion(explosion);
    }
    template <class B>
    bool base_Block_dropFromExplosion(Explosion* explosion, long)
    {
        return static_cast<Block*>(this)->Block::dropFromExplosion(explosion);
    }

    virtual bool dropFromExplosion(Explosion* explosion)
    {
        if (ModHooks::IsActive(kModHook_Block_dropFromExplosion))
        {
            bool result = false;
            if (!ModEvents::BlockDropFromExplosion(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), const_cast<Explosion*>(explosion), result))
                return result;
        }
        return base_Block_dropFromExplosion<Base>(explosion, 0);
    }

    template <class B>
    auto base_Block_isMatching(Block* block, int) -> decltype(static_cast<B*>(this)->B::isMatching(block))
    {
        return static_cast<B*>(this)->B::isMatching(block);
    }
    template <class B>
    bool base_Block_isMatching(Block* block, long)
    {
        return static_cast<Block*>(this)->Block::isMatching(block);
    }

    virtual bool isMatching(Block* block)
    {
        if (ModHooks::IsActive(kModHook_Block_isMatching))
        {
            bool result = false;
            if (!ModEvents::BlockIsMatching(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), block, result))
                return result;
        }
        return base_Block_isMatching<Base>(block, 0);
    }

    template <class B>
    auto base_Block_hasAnalogOutputSignal(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::hasAnalogOutputSignal(state))
    {
        return static_cast<B*>(this)->B::hasAnalogOutputSignal(state);
    }
    template <class B>
    bool base_Block_hasAnalogOutputSignal(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::hasAnalogOutputSignal(state);
    }

    virtual bool hasAnalogOutputSignal(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_hasAnalogOutputSignal))
        {
            bool result = false;
            if (!ModEvents::BlockHasAnalogOutputSignal(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_hasAnalogOutputSignal<Base>(state, 0);
    }

    template <class B>
    auto base_Block_getAnalogOutputSignal(const BlockState* state, Level* level, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getAnalogOutputSignal(state, level, pos))
    {
        return static_cast<B*>(this)->B::getAnalogOutputSignal(state, level, pos);
    }
    template <class B>
    int base_Block_getAnalogOutputSignal(const BlockState* state, Level* level, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getAnalogOutputSignal(state, level, pos);
    }

    virtual int getAnalogOutputSignal(const BlockState* state, Level* level, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getAnalogOutputSignal))
        {
            int result = 0;
            if (!ModEvents::BlockGetAnalogOutputSignal(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), level, pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getAnalogOutputSignal<Base>(state, level, pos, 0);
    }

    template <class B>
    auto base_Block_setIconName(const std::wstring& iconName, int) -> decltype(static_cast<B*>(this)->B::setIconName(iconName))
    {
        return static_cast<B*>(this)->B::setIconName(iconName);
    }
    template <class B>
    Block* base_Block_setIconName(const std::wstring& iconName, long)
    {
        return static_cast<Block*>(this)->Block::setIconName(iconName);
    }

    virtual Block* setIconName(const std::wstring& iconName)
    {
        if (ModHooks::IsActive(kModHook_Block_setIconName))
        {
            Block* result = NULL;
            if (!ModEvents::BlockSetIconName(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), iconName, result))
                return result;
        }
        return base_Block_setIconName<Base>(iconName, 0);
    }

    template <class B>
    auto base_Block_getIconName(int) -> decltype(static_cast<B*>(this)->B::getIconName())
    {
        return static_cast<B*>(this)->B::getIconName();
    }
    template <class B>
    std::wstring base_Block_getIconName(long)
    {
        return static_cast<Block*>(this)->Block::getIconName();
    }

    virtual std::wstring getIconName()
    {
        if (ModHooks::IsActive(kModHook_Block_getIconName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::BlockGetIconName(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getIconName<Base>(0);
    }

    template <class B>
    auto base_Block_registerIcons(IconRegister* iconRegister, int) -> decltype(static_cast<B*>(this)->B::registerIcons(iconRegister))
    {
        static_cast<B*>(this)->B::registerIcons(iconRegister);
    }
    template <class B>
    void base_Block_registerIcons(IconRegister* iconRegister, long)
    {
        static_cast<Block*>(this)->Block::registerIcons(iconRegister);
    }

    virtual void registerIcons(IconRegister* iconRegister)
    {
        if (ModHooks::IsActive(kModHook_Block_registerIcons))
        {
            if (!ModEvents::BlockRegisterIcons(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), const_cast<IconRegister*>(iconRegister)))
                return;
        }
        base_Block_registerIcons<Base>(iconRegister, 0);
    }

    template <class B>
    auto base_Block_getTileItemIconName(int) -> decltype(static_cast<B*>(this)->B::getTileItemIconName())
    {
        return static_cast<B*>(this)->B::getTileItemIconName();
    }
    template <class B>
    std::wstring base_Block_getTileItemIconName(long)
    {
        return static_cast<Block*>(this)->Block::getTileItemIconName();
    }

    virtual std::wstring getTileItemIconName()
    {
        if (ModHooks::IsActive(kModHook_Block_getTileItemIconName))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::BlockGetTileItemIconName(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getTileItemIconName<Base>(0);
    }

    template <class B>
    auto base_Block_getIconTexture(const Direction* direction, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getIconTexture(direction, state))
    {
        return static_cast<B*>(this)->B::getIconTexture(direction, state);
    }
    template <class B>
    TextureAtlasSprite* base_Block_getIconTexture(const Direction* direction, const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getIconTexture(direction, state);
    }

    virtual TextureAtlasSprite* getIconTexture(const Direction* direction, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getIconTexture))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::BlockGetIconTexture(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (direction != NULL ? direction->get3DDataValue() : -1), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getIconTexture<Base>(direction, state, 0);
    }

    template <class B>
    auto base_Block_getTexture(LevelSource* levelSource, const BlockPos& pos, const Direction* direction, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getTexture(levelSource, pos, direction, state))
    {
        return static_cast<B*>(this)->B::getTexture(levelSource, pos, direction, state);
    }
    template <class B>
    TextureAtlasSprite* base_Block_getTexture(LevelSource* levelSource, const BlockPos& pos, const Direction* direction, const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getTexture(levelSource, pos, direction, state);
    }

    virtual TextureAtlasSprite* getTexture(LevelSource* levelSource, const BlockPos& pos, const Direction* direction, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getTexture))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::BlockGetTexture(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, (direction != NULL ? direction->get3DDataValue() : -1), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getTexture<Base>(levelSource, pos, direction, state, 0);
    }

    template <class B>
    auto base_Block_getTexture_2(const Direction* direction, const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getTexture(direction, state))
    {
        return static_cast<B*>(this)->B::getTexture(direction, state);
    }
    template <class B>
    TextureAtlasSprite* base_Block_getTexture_2(const Direction* direction, const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getTexture(direction, state);
    }

    virtual TextureAtlasSprite* getTexture(const Direction* direction, const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getTexture_2))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::BlockGetTexture2(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (direction != NULL ? direction->get3DDataValue() : -1), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return result;
        }
        return base_Block_getTexture_2<Base>(direction, state, 0);
    }

    template <class B>
    auto base_Block_getTexture_3(const Direction* direction, int) -> decltype(static_cast<B*>(this)->B::getTexture(direction))
    {
        return static_cast<B*>(this)->B::getTexture(direction);
    }
    template <class B>
    TextureAtlasSprite* base_Block_getTexture_3(const Direction* direction, long)
    {
        return static_cast<Block*>(this)->Block::getTexture(direction);
    }

    virtual TextureAtlasSprite* getTexture(const Direction* direction)
    {
        if (ModHooks::IsActive(kModHook_Block_getTexture_3))
        {
            TextureAtlasSprite* result = NULL;
            if (!ModEvents::BlockGetTexture3(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (direction != NULL ? direction->get3DDataValue() : -1), result))
                return result;
        }
        return base_Block_getTexture_3<Base>(direction, 0);
    }

    template <class B>
    auto base_Block_getInventoryRenderState(const BlockState* state, int) -> decltype(static_cast<B*>(this)->B::getInventoryRenderState(state))
    {
        return static_cast<B*>(this)->B::getInventoryRenderState(state);
    }
    template <class B>
    const BlockState* base_Block_getInventoryRenderState(const BlockState* state, long)
    {
        return static_cast<Block*>(this)->Block::getInventoryRenderState(state);
    }

    virtual const BlockState* getInventoryRenderState(const BlockState* state)
    {
        if (ModHooks::IsActive(kModHook_Block_getInventoryRenderState))
        {
            int result = 0;
            if (!ModEvents::BlockGetInventoryRenderState(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), result))
                return this->getBlockState(result);
        }
        return base_Block_getInventoryRenderState<Base>(state, 0);
    }

    template <class B>
    auto base_Block_createBlockStateDefinition(int) -> decltype(static_cast<B*>(this)->B::createBlockStateDefinition())
    {
        return static_cast<B*>(this)->B::createBlockStateDefinition();
    }
    template <class B>
    BlockStateDefinition* base_Block_createBlockStateDefinition(long)
    {
        return static_cast<Block*>(this)->Block::createBlockStateDefinition();
    }

    virtual BlockStateDefinition* createBlockStateDefinition()
    {
        if (ModHooks::IsActive(kModHook_Block_createBlockStateDefinition))
        {
            BlockStateDefinition* result = NULL;
            if (!ModEvents::BlockCreateBlockStateDefinition(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_createBlockStateDefinition<Base>(0);
    }

    template <class B>
    auto base_Block_getBlockStateDefinition(int) -> decltype(static_cast<B*>(this)->B::getBlockStateDefinition())
    {
        return static_cast<B*>(this)->B::getBlockStateDefinition();
    }
    template <class B>
    BlockStateDefinition* base_Block_getBlockStateDefinition(long)
    {
        return static_cast<Block*>(this)->Block::getBlockStateDefinition();
    }

    virtual BlockStateDefinition* getBlockStateDefinition()
    {
        if (ModHooks::IsActive(kModHook_Block_getBlockStateDefinition))
        {
            BlockStateDefinition* result = NULL;
            if (!ModEvents::BlockGetBlockStateDefinition(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getBlockStateDefinition<Base>(0);
    }

    template <class B>
    auto base_Block_getOffsetType(int) -> decltype(static_cast<B*>(this)->B::getOffsetType())
    {
        return static_cast<B*>(this)->B::getOffsetType();
    }
    template <class B>
    EBlockOffsetType base_Block_getOffsetType(long)
    {
        return static_cast<Block*>(this)->Block::getOffsetType();
    }

    virtual EBlockOffsetType getOffsetType()
    {
        if (ModHooks::IsActive(kModHook_Block_getOffsetType))
        {
            int result = 0;
            if (!ModEvents::BlockGetOffsetType(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return static_cast<EBlockOffsetType>(result);
        }
        return base_Block_getOffsetType<Base>(0);
    }

    template <class B>
    auto base_Block_getOffset(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getOffset(state, levelSource, pos))
    {
        return static_cast<B*>(this)->B::getOffset(state, levelSource, pos);
    }
    template <class B>
    Vec3* base_Block_getOffset(const BlockState* state, LevelSource* levelSource, const BlockPos& pos, long)
    {
        return static_cast<Block*>(this)->Block::getOffset(state, levelSource, pos);
    }

    virtual Vec3* getOffset(const BlockState* state, LevelSource* levelSource, const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Block_getOffset))
        {
            Vec3* result = NULL;
            if (!ModEvents::BlockGetOffset(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), (state != NULL ? const_cast<Block*>(state->getBlock())->convertBlockStateToLegacyData(state) : 0), dynamic_cast<Level*>(levelSource), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Block_getOffset<Base>(state, levelSource, pos, 0);
    }

    template <class B>
    auto base_Block_getSoundType(int) -> decltype(static_cast<B*>(this)->B::getSoundType())
    {
        return static_cast<B*>(this)->B::getSoundType();
    }
    template <class B>
    const SoundType* base_Block_getSoundType(long)
    {
        return static_cast<Block*>(this)->Block::getSoundType();
    }

    virtual const SoundType* getSoundType()
    {
        if (ModHooks::IsActive(kModHook_Block_getSoundType))
        {
            SoundType* result = NULL;
            if (!ModEvents::BlockGetSoundType(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_getSoundType<Base>(0);
    }

    template <class B>
    auto base_Block_toString(int) -> decltype(static_cast<B*>(this)->B::toString())
    {
        return static_cast<B*>(this)->B::toString();
    }
    template <class B>
    std::wstring base_Block_toString(long)
    {
        return static_cast<Block*>(this)->Block::toString();
    }

    virtual std::wstring toString()
    {
        if (ModHooks::IsActive(kModHook_Block_toString))
        {
            std::wstring result = std::wstring();
            if (!ModEvents::BlockToString(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_toString<Base>(0);
    }

    template <class B>
    auto base_Block_isLiquidBlock(int) -> decltype(static_cast<B*>(this)->B::isLiquidBlock())
    {
        return static_cast<B*>(this)->B::isLiquidBlock();
    }
    template <class B>
    bool base_Block_isLiquidBlock(long)
    {
        return static_cast<Block*>(this)->Block::isLiquidBlock();
    }

    virtual bool isLiquidBlock()
    {
        if (ModHooks::IsActive(kModHook_Block_isLiquidBlock))
        {
            bool result = false;
            if (!ModEvents::BlockIsLiquidBlock(static_cast<Block*>(const_cast<ModBlock<Base>*>(this)), result))
                return result;
        }
        return base_Block_isLiquidBlock<Base>(0);
    }

    // not answerable yet, a parameter or result type has no conversion: GetHitTooltip, GetInteractTooltip, addCollisionAABB, addCollisionAABBs, appendHoverText, getCloneItemInstance, getOutlineAABBs, getShapes, getSilkTouchItemInstance
};

#endif
