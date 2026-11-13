// Generated from game/world/Dimension.hpp, game/world/NormalDimension.hpp by tools/hooks/hookgen.py. Do not edit.
#ifndef GAME_MODCLASS_DIMENSION_GENERATED_HPP
#define GAME_MODCLASS_DIMENSION_GENERATED_HPP

#include <stddef.h>

#include "game/core/ModEvents_generated.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModItemNames.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/generation/Player.hpp"
#include "game/entity/mob/LivingEntity.hpp"
#include "game/world/Dimension.hpp"
#include "game/world/NormalDimension.hpp"

// every virtual method of Dimension a mod can answer, on top of any game class derived from it
template <class Base>
class ModDimension : public Base
{
public:
    using Base::Base;

    template <class B>
    auto base_Dimension_init(Level* level, int) -> decltype(static_cast<B*>(this)->B::init(level))
    {
        static_cast<B*>(this)->B::init(level);
    }
    template <class B>
    void base_Dimension_init(Level* level, long)
    {
        static_cast<Dimension*>(this)->Dimension::init(level);
    }

    virtual void init(Level* level)
    {
        if (ModHooks::IsActive(kModHook_Dimension_init))
        {
            if (!ModEvents::DimensionInit(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), level))
                return;
        }
        base_Dimension_init<Base>(level, 0);
    }

    template <class B>
    auto base_Dimension_init_2(int) -> decltype(static_cast<B*>(this)->B::init())
    {
        static_cast<B*>(this)->B::init();
    }
    template <class B>
    void base_Dimension_init_2(long)
    {
        static_cast<Dimension*>(this)->Dimension::init();
    }

    virtual void init()
    {
        if (ModHooks::IsActive(kModHook_Dimension_init_2))
        {
            if (!ModEvents::DimensionInit2(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this))))
                return;
        }
        base_Dimension_init_2<Base>(0);
    }

    template <class B>
    auto base_Dimension_updateLightRamp(int) -> decltype(static_cast<B*>(this)->B::updateLightRamp())
    {
        static_cast<B*>(this)->B::updateLightRamp();
    }
    template <class B>
    void base_Dimension_updateLightRamp(long)
    {
        static_cast<Dimension*>(this)->Dimension::updateLightRamp();
    }

    virtual void updateLightRamp()
    {
        if (ModHooks::IsActive(kModHook_Dimension_updateLightRamp))
        {
            if (!ModEvents::DimensionUpdateLightRamp(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this))))
                return;
        }
        base_Dimension_updateLightRamp<Base>(0);
    }

    template <class B>
    auto base_Dimension_createRandomLevelGenerator(int) const -> decltype(static_cast<const B*>(this)->B::createRandomLevelGenerator())
    {
        return static_cast<const B*>(this)->B::createRandomLevelGenerator();
    }
    template <class B>
    ChunkGenerator* base_Dimension_createRandomLevelGenerator(long) const
    {
        return static_cast<const Dimension*>(this)->Dimension::createRandomLevelGenerator();
    }

    virtual ChunkGenerator* createRandomLevelGenerator() const
    {
        if (ModHooks::IsActive(kModHook_Dimension_createRandomLevelGenerator))
        {
            ChunkGenerator* result = NULL;
            if (!ModEvents::DimensionCreateRandomLevelGenerator(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_createRandomLevelGenerator<Base>(0);
    }

    template <class B>
    auto base_Dimension_createFlatLevelSource(int) const -> decltype(static_cast<const B*>(this)->B::createFlatLevelSource())
    {
        return static_cast<const B*>(this)->B::createFlatLevelSource();
    }
    template <class B>
    ChunkGenerator* base_Dimension_createFlatLevelSource(long) const
    {
        return static_cast<const Dimension*>(this)->Dimension::createFlatLevelSource();
    }

    virtual ChunkGenerator* createFlatLevelSource() const
    {
        if (ModHooks::IsActive(kModHook_Dimension_createFlatLevelSource))
        {
            ChunkGenerator* result = NULL;
            if (!ModEvents::DimensionCreateFlatLevelSource(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_createFlatLevelSource<Base>(0);
    }

    template <class B>
    auto base_Dimension_isValidSpawn(int x, int z, int) const -> decltype(static_cast<const B*>(this)->B::isValidSpawn(x, z))
    {
        return static_cast<const B*>(this)->B::isValidSpawn(x, z);
    }
    template <class B>
    bool base_Dimension_isValidSpawn(int x, int z, long) const
    {
        return static_cast<const Dimension*>(this)->Dimension::isValidSpawn(x, z);
    }

    virtual bool isValidSpawn(int x, int z) const
    {
        if (ModHooks::IsActive(kModHook_Dimension_isValidSpawn))
        {
            bool result = false;
            if (!ModEvents::DimensionIsValidSpawn(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), x, z, result))
                return result;
        }
        return base_Dimension_isValidSpawn<Base>(x, z, 0);
    }

    template <class B>
    auto base_Dimension_getTimeOfDay(long long time, float partialTick, int) const -> decltype(static_cast<const B*>(this)->B::getTimeOfDay(time, partialTick))
    {
        return static_cast<const B*>(this)->B::getTimeOfDay(time, partialTick);
    }
    template <class B>
    float base_Dimension_getTimeOfDay(long long time, float partialTick, long) const
    {
        return static_cast<const Dimension*>(this)->Dimension::getTimeOfDay(time, partialTick);
    }

    virtual float getTimeOfDay(long long time, float partialTick) const
    {
        if (ModHooks::IsActive(kModHook_Dimension_getTimeOfDay))
        {
            float result = 0.0f;
            if (!ModEvents::DimensionGetTimeOfDay(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), static_cast<int>(time), partialTick, result))
                return result;
        }
        return base_Dimension_getTimeOfDay<Base>(time, partialTick, 0);
    }

    template <class B>
    auto base_Dimension_getMoonPhase(long long time, int) const -> decltype(static_cast<const B*>(this)->B::getMoonPhase(time))
    {
        return static_cast<const B*>(this)->B::getMoonPhase(time);
    }
    template <class B>
    int base_Dimension_getMoonPhase(long long time, long) const
    {
        return static_cast<const Dimension*>(this)->Dimension::getMoonPhase(time);
    }

    virtual int getMoonPhase(long long time) const
    {
        if (ModHooks::IsActive(kModHook_Dimension_getMoonPhase))
        {
            int result = 0;
            if (!ModEvents::DimensionGetMoonPhase(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), static_cast<int>(time), result))
                return result;
        }
        return base_Dimension_getMoonPhase<Base>(time, 0);
    }

    template <class B>
    auto base_Dimension_isNaturalDimension(int) -> decltype(static_cast<B*>(this)->B::isNaturalDimension())
    {
        return static_cast<B*>(this)->B::isNaturalDimension();
    }
    template <class B>
    bool base_Dimension_isNaturalDimension(long)
    {
        return static_cast<Dimension*>(this)->Dimension::isNaturalDimension();
    }

    virtual bool isNaturalDimension()
    {
        if (ModHooks::IsActive(kModHook_Dimension_isNaturalDimension))
        {
            bool result = false;
            if (!ModEvents::DimensionIsNaturalDimension(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_isNaturalDimension<Base>(0);
    }

    template <class B>
    auto base_Dimension_getFogColor(float celestialAngle, float timeOfDay, int) -> decltype(static_cast<B*>(this)->B::getFogColor(celestialAngle, timeOfDay))
    {
        return static_cast<B*>(this)->B::getFogColor(celestialAngle, timeOfDay);
    }
    template <class B>
    Vec3* base_Dimension_getFogColor(float celestialAngle, float timeOfDay, long)
    {
        return static_cast<Dimension*>(this)->Dimension::getFogColor(celestialAngle, timeOfDay);
    }

    virtual Vec3* getFogColor(float celestialAngle, float timeOfDay)
    {
        if (ModHooks::IsActive(kModHook_Dimension_getFogColor))
        {
            Vec3* result = NULL;
            if (!ModEvents::DimensionGetFogColor(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), celestialAngle, timeOfDay, result))
                return result;
        }
        return base_Dimension_getFogColor<Base>(celestialAngle, timeOfDay, 0);
    }

    template <class B>
    auto base_Dimension_mayRespawn(int) const -> decltype(static_cast<const B*>(this)->B::mayRespawn())
    {
        return static_cast<const B*>(this)->B::mayRespawn();
    }
    template <class B>
    bool base_Dimension_mayRespawn(long) const
    {
        return static_cast<const Dimension*>(this)->Dimension::mayRespawn();
    }

    virtual bool mayRespawn() const
    {
        if (ModHooks::IsActive(kModHook_Dimension_mayRespawn))
        {
            bool result = false;
            if (!ModEvents::DimensionMayRespawn(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_mayRespawn<Base>(0);
    }

    template <class B>
    auto base_Dimension_getCloudHeight(int) -> decltype(static_cast<B*>(this)->B::getCloudHeight())
    {
        return static_cast<B*>(this)->B::getCloudHeight();
    }
    template <class B>
    float base_Dimension_getCloudHeight(long)
    {
        return static_cast<Dimension*>(this)->Dimension::getCloudHeight();
    }

    virtual float getCloudHeight()
    {
        if (ModHooks::IsActive(kModHook_Dimension_getCloudHeight))
        {
            float result = 0.0f;
            if (!ModEvents::DimensionGetCloudHeight(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_getCloudHeight<Base>(0);
    }

    template <class B>
    auto base_Dimension_hasGround(int) -> decltype(static_cast<B*>(this)->B::hasGround())
    {
        return static_cast<B*>(this)->B::hasGround();
    }
    template <class B>
    bool base_Dimension_hasGround(long)
    {
        return static_cast<Dimension*>(this)->Dimension::hasGround();
    }

    virtual bool hasGround()
    {
        if (ModHooks::IsActive(kModHook_Dimension_hasGround))
        {
            bool result = false;
            if (!ModEvents::DimensionHasGround(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_hasGround<Base>(0);
    }

    template <class B>
    auto base_Dimension_getSpawnPos(int) const -> decltype(static_cast<const B*>(this)->B::getSpawnPos())
    {
        return static_cast<const B*>(this)->B::getSpawnPos();
    }
    template <class B>
    BlockPos* base_Dimension_getSpawnPos(long) const
    {
        return static_cast<const Dimension*>(this)->Dimension::getSpawnPos();
    }

    virtual BlockPos* getSpawnPos() const
    {
        if (ModHooks::IsActive(kModHook_Dimension_getSpawnPos))
        {
            BlockPos* result = NULL;
            if (!ModEvents::DimensionGetSpawnPos(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_getSpawnPos<Base>(0);
    }

    template <class B>
    auto base_Dimension_getSpawnYPosition(int) -> decltype(static_cast<B*>(this)->B::getSpawnYPosition())
    {
        return static_cast<B*>(this)->B::getSpawnYPosition();
    }
    template <class B>
    int base_Dimension_getSpawnYPosition(long)
    {
        return static_cast<Dimension*>(this)->Dimension::getSpawnYPosition();
    }

    virtual int getSpawnYPosition()
    {
        if (ModHooks::IsActive(kModHook_Dimension_getSpawnYPosition))
        {
            int result = 0;
            if (!ModEvents::DimensionGetSpawnYPosition(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_getSpawnYPosition<Base>(0);
    }

    template <class B>
    auto base_Dimension_isFoggyAt(int x, int z, int) -> decltype(static_cast<B*>(this)->B::isFoggyAt(x, z))
    {
        return static_cast<B*>(this)->B::isFoggyAt(x, z);
    }
    template <class B>
    bool base_Dimension_isFoggyAt(int x, int z, long)
    {
        return static_cast<Dimension*>(this)->Dimension::isFoggyAt(x, z);
    }

    virtual bool isFoggyAt(int x, int z)
    {
        if (ModHooks::IsActive(kModHook_Dimension_isFoggyAt))
        {
            bool result = false;
            if (!ModEvents::DimensionIsFoggyAt(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), x, z, result))
                return result;
        }
        return base_Dimension_isFoggyAt<Base>(x, z, 0);
    }

    template <class B>
    auto base_Dimension_createWorldBorder(Level* level, int) -> decltype(static_cast<B*>(this)->B::createWorldBorder(level))
    {
        return static_cast<B*>(this)->B::createWorldBorder(level);
    }
    template <class B>
    WorldBorder* base_Dimension_createWorldBorder(Level* level, long)
    {
        return static_cast<Dimension*>(this)->Dimension::createWorldBorder(level);
    }

    virtual WorldBorder* createWorldBorder(Level* level)
    {
        if (ModHooks::IsActive(kModHook_Dimension_createWorldBorder))
        {
            WorldBorder* result = NULL;
            if (!ModEvents::DimensionCreateWorldBorder(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), level, result))
                return result;
        }
        return base_Dimension_createWorldBorder<Base>(level, 0);
    }

    template <class B>
    auto base_Dimension_saveData(int) -> decltype(static_cast<B*>(this)->B::saveData())
    {
        static_cast<B*>(this)->B::saveData();
    }
    template <class B>
    void base_Dimension_saveData(long)
    {
        static_cast<Dimension*>(this)->Dimension::saveData();
    }

    virtual void saveData()
    {
        if (ModHooks::IsActive(kModHook_Dimension_saveData))
        {
            if (!ModEvents::DimensionSaveData(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this))))
                return;
        }
        base_Dimension_saveData<Base>(0);
    }

    template <class B>
    auto base_Dimension_tick(int) -> decltype(static_cast<B*>(this)->B::tick())
    {
        static_cast<B*>(this)->B::tick();
    }
    template <class B>
    void base_Dimension_tick(long)
    {
        static_cast<Dimension*>(this)->Dimension::tick();
    }

    virtual void tick()
    {
        if (ModHooks::IsActive(kModHook_Dimension_tick))
        {
            if (!ModEvents::DimensionTick(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this))))
                return;
        }
        base_Dimension_tick<Base>(0);
    }

    template <class B>
    auto base_Dimension_canUnloadChunk(int x, int z, int) -> decltype(static_cast<B*>(this)->B::canUnloadChunk(x, z))
    {
        return static_cast<B*>(this)->B::canUnloadChunk(x, z);
    }
    template <class B>
    bool base_Dimension_canUnloadChunk(int x, int z, long)
    {
        return static_cast<NormalDimension*>(this)->NormalDimension::canUnloadChunk(x, z);
    }

    virtual bool canUnloadChunk(int x, int z)
    {
        if (ModHooks::IsActive(kModHook_Dimension_canUnloadChunk))
        {
            bool result = false;
            if (!ModEvents::DimensionCanUnloadChunk(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), x, z, result))
                return result;
        }
        return base_Dimension_canUnloadChunk<Base>(x, z, 0);
    }

    template <class B>
    auto base_Dimension_getXZSize(int) -> decltype(static_cast<B*>(this)->B::getXZSize())
    {
        return static_cast<B*>(this)->B::getXZSize();
    }
    template <class B>
    int base_Dimension_getXZSize(long)
    {
        return static_cast<Dimension*>(this)->Dimension::getXZSize();
    }

    virtual int getXZSize()
    {
        if (ModHooks::IsActive(kModHook_Dimension_getXZSize))
        {
            int result = 0;
            if (!ModEvents::DimensionGetXZSize(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_getXZSize<Base>(0);
    }

    template <class B>
    auto base_Dimension_getPlayerConstraints(const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::getPlayerConstraints(pos))
    {
        return static_cast<B*>(this)->B::getPlayerConstraints(pos);
    }
    template <class B>
    AABB* base_Dimension_getPlayerConstraints(const BlockPos& pos, long)
    {
        return static_cast<Dimension*>(this)->Dimension::getPlayerConstraints(pos);
    }

    virtual AABB* getPlayerConstraints(const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Dimension_getPlayerConstraints))
        {
            AABB* result = NULL;
            if (!ModEvents::DimensionGetPlayerConstraints(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Dimension_getPlayerConstraints<Base>(pos, 0);
    }

    template <class B>
    auto base_Dimension_inPlayerConstraints(const BlockPos& pos, int) -> decltype(static_cast<B*>(this)->B::inPlayerConstraints(pos))
    {
        return static_cast<B*>(this)->B::inPlayerConstraints(pos);
    }
    template <class B>
    bool base_Dimension_inPlayerConstraints(const BlockPos& pos, long)
    {
        return static_cast<Dimension*>(this)->Dimension::inPlayerConstraints(pos);
    }

    virtual bool inPlayerConstraints(const BlockPos& pos)
    {
        if (ModHooks::IsActive(kModHook_Dimension_inPlayerConstraints))
        {
            bool result = false;
            if (!ModEvents::DimensionInPlayerConstraints(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), pos.x, pos.y, pos.z, result))
                return result;
        }
        return base_Dimension_inPlayerConstraints<Base>(pos, 0);
    }

    template <class B>
    auto base_Dimension_containsChunk(int x, int z, int) -> decltype(static_cast<B*>(this)->B::containsChunk(x, z))
    {
        return static_cast<B*>(this)->B::containsChunk(x, z);
    }
    template <class B>
    bool base_Dimension_containsChunk(int x, int z, long)
    {
        return static_cast<Dimension*>(this)->Dimension::containsChunk(x, z);
    }

    virtual bool containsChunk(int x, int z)
    {
        if (ModHooks::IsActive(kModHook_Dimension_containsChunk))
        {
            bool result = false;
            if (!ModEvents::DimensionContainsChunk(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), x, z, result))
                return result;
        }
        return base_Dimension_containsChunk<Base>(x, z, 0);
    }

    template <class B>
    auto base_Dimension_getBlockLightColour(int) -> decltype(static_cast<B*>(this)->B::getBlockLightColour())
    {
        return static_cast<B*>(this)->B::getBlockLightColour();
    }
    template <class B>
    int base_Dimension_getBlockLightColour(long)
    {
        return static_cast<Dimension*>(this)->Dimension::getBlockLightColour();
    }

    virtual int getBlockLightColour()
    {
        if (ModHooks::IsActive(kModHook_Dimension_getBlockLightColour))
        {
            int result = 0;
            if (!ModEvents::DimensionGetBlockLightColour(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_getBlockLightColour<Base>(0);
    }

    template <class B>
    auto base_Dimension_getType(int) const -> decltype(static_cast<const B*>(this)->B::getType())
    {
        return static_cast<const B*>(this)->B::getType();
    }
    template <class B>
    const DimensionType* base_Dimension_getType(long) const
    {
        return static_cast<const NormalDimension*>(this)->NormalDimension::getType();
    }

    virtual const DimensionType* getType() const
    {
        if (ModHooks::IsActive(kModHook_Dimension_getType))
        {
            DimensionType* result = NULL;
            if (!ModEvents::DimensionGetType(static_cast<Dimension*>(const_cast<ModDimension<Base>*>(this)), result))
                return result;
        }
        return base_Dimension_getType<Base>(0);
    }

    // not answerable yet, a parameter or result type has no conversion: createStorage, getSunriseColor
};

#endif
