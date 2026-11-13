#include "game/core/ModEvents_generated.hpp"

#include "game/core/ModHookCall.hpp"
#include "mod/ModApiIds_generated.hpp"

namespace ModEvents
{
void SoundStart(const SoundEvent* sound, float x, float y, float z, float volume, float pitch, int kind)
{
    ModHookCall hook(kModHook_sound_start);
    hook.AddSound(sound);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(volume);
    hook.Add(pitch);
    hook.Add(kind);
    hook.Run();
}

void GenerateTerrain(Level* level, ChunkPrimer* primer, int chunk_x, int chunk_z)
{
    ModHookCall hook(kModHook_generate_terrain);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    hook.Add(chunk_x);
    hook.Add(chunk_z);
    hook.Run();
}

void CheckLight(Level* level, int x, int y, int z, LightLayer::variety layer)
{
    ModHookCall hook(kModHook_check_light);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddLightLayer(layer);
    hook.Run();
}

void WireUpdate(Level* level, int x, int y, int z)
{
    ModHookCall hook(kModHook_wire_update);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Run();
}

void PacketEncode(bool server, bool loopback, int id, std::string& data)
{
    ModHookCall hook(kModHook_packet_encode);
    hook.Add(server);
    hook.Add(loopback);
    hook.Add(id);
    hook.Add(data);
    hook.Run();
    data = hook.GetBytes(3, data);
}

void PacketDecode(bool server, bool loopback, std::string& data)
{
    ModHookCall hook(kModHook_packet_decode);
    hook.Add(server);
    hook.Add(loopback);
    hook.Add(data);
    hook.Run();
    data = hook.GetBytes(2, data);
}

void ChunkStore(const std::wstring& world, int dimension, int x, int z, const std::string& data)
{
    ModHookCall hook(kModHook_chunk_store);
    hook.Add(world);
    hook.Add(dimension);
    hook.Add(x);
    hook.Add(z);
    hook.Add(data);
    hook.Run();
}

void ChunkFetch(const std::wstring& world, int dimension, int x, int z, std::string& data, bool& found)
{
    ModHookCall hook(kModHook_chunk_fetch);
    hook.Add(world);
    hook.Add(dimension);
    hook.Add(x);
    hook.Add(z);
    hook.Add(data);
    hook.Add(found);
    hook.Run();
    data = hook.GetBytes(4, data);
    found = hook.GetBool(5, found);
}

bool BlockIsTopSolidBlocking(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isTopSolidBlocking);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockCanProvideSupport(Block* self, int state, int direction, int supportType, bool& result)
{
    ModHookCall hook(kModHook_Block_canProvideSupport);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(direction);
    hook.Add(supportType);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(4, result);
    return proceed;
}

bool BlockIsValidSpawn(Block* self, int state, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Block_isValidSpawn);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(3, result);
    return proceed;
}

bool BlockGetLightEmission(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getLightEmission);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockParticlesSurviveWithin(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_ParticlesSurviveWithin);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockGetMapColor(Block* self, int state, Level* levelSource, int x, int y, int z, MaterialColor*& result)
{
    ModHookCall hook(kModHook_Block_getMapColor);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        MaterialColor* object = NULL;
        result = hook.GetHandle(6, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetBlockState(Block* self, int& data, int& result)
{
    ModHookCall hook(kModHook_Block_getBlockState);
    hook.AddBlock(self);
    hook.Add(data);
    hook.Add(result);
    const bool proceed = hook.Run();
    data = hook.GetInt(1, data);
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockConvertBlockStateToLegacyData(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_convertBlockStateToLegacyData);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockSetPropertyValueFromString(Block* self, int state, Property* property, const std::wstring& value, int& result)
{
    ModHookCall hook(kModHook_Block_setPropertyValueFromString);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_Property, ModHandles::Add(property));
    hook.Add(value);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(4, result);
    return proceed;
}

bool BlockFillVirtualBlockStateProperties(Block* self, int state, Level* levelSource, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_Block_fillVirtualBlockStateProperties);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(6, result);
    return proceed;
}

bool BlockRotate(Block* self, int state, Rotation* rotation, int& result)
{
    ModHookCall hook(kModHook_Block_rotate);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_Rotation, ModHandles::Add(rotation));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(3, result);
    return proceed;
}

bool BlockMirror(Block* self, int state, Mirror* mirror, int& result)
{
    ModHookCall hook(kModHook_Block_mirror);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_Mirror, ModHandles::Add(mirror));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(3, result);
    return proceed;
}

bool BlockDerivedInit(Block* self)
{
    ModHookCall hook(kModHook_Block_DerivedInit);
    hook.AddBlock(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockSendBlockData(Block* self, int blockData, Block*& result)
{
    ModHookCall hook(kModHook_Block_sendBlockData);
    hook.AddBlock(self);
    hook.Add(blockData);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockSetSoundType(Block* self, SoundType* soundType, Block*& result)
{
    ModHookCall hook(kModHook_Block_setSoundType);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_SoundType, ModHandles::Add(soundType));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockSetLightBlock(Block* self, int& lightBlock, Block*& result)
{
    ModHookCall hook(kModHook_Block_setLightBlock);
    hook.AddBlock(self);
    hook.Add(lightBlock);
    hook.AddNil();
    const bool proceed = hook.Run();
    lightBlock = hook.GetInt(1, lightBlock);
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockSetLightEmission(Block* self, float& lightEmission, Block*& result)
{
    ModHookCall hook(kModHook_Block_setLightEmission);
    hook.AddBlock(self);
    hook.Add(lightEmission);
    hook.AddNil();
    const bool proceed = hook.Run();
    lightEmission = hook.GetFloat(1, lightEmission);
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockSetExplodeable(Block* self, float& explosionResistance, Block*& result)
{
    ModHookCall hook(kModHook_Block_setExplodeable);
    hook.AddBlock(self);
    hook.Add(explosionResistance);
    hook.AddNil();
    const bool proceed = hook.Run();
    explosionResistance = hook.GetFloat(1, explosionResistance);
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockIsSolidBlockingCube(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isSolidBlockingCube);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockIsSolidBlockingCubeAndNotSignalSource(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isSolidBlockingCubeAndNotSignalSource);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockIsViewBlocking(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isViewBlocking);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockIsCubeShaped(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isCubeShaped);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockHasCustomBreakingProgress(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_hasCustomBreakingProgress);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockIsPathfindable(Block* self, Level* levelSource, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_Block_isPathfindable);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(5, result);
    return proceed;
}

bool BlockGetRenderShape(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getRenderShape);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockHasInHandRenderOffset(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_hasInHandRenderOffset);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockMayReplaceWithPlace(Block* self, Level* levelSource, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_Block_mayReplaceWithPlace);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(5, result);
    return proceed;
}

bool BlockIsTransparent(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isTransparent);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockSetDestroyTime(Block* self, float& destroyTime, Block*& result)
{
    ModHookCall hook(kModHook_Block_setDestroyTime);
    hook.AddBlock(self);
    hook.Add(destroyTime);
    hook.AddNil();
    const bool proceed = hook.Run();
    destroyTime = hook.GetFloat(1, destroyTime);
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockSetIndestructible(Block* self, Block*& result)
{
    ModHookCall hook(kModHook_Block_setIndestructible);
    hook.AddBlock(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockIsIndestructible(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isIndestructible);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockGetDestroySpeed(Block* self, int state, Level* level, int x, int y, int z, float& result)
{
    ModHookCall hook(kModHook_Block_getDestroySpeed);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(6, result);
    return proceed;
}

bool BlockSetTicking(Block* self, bool& ticking, Block*& result)
{
    ModHookCall hook(kModHook_Block_setTicking);
    hook.AddBlock(self);
    hook.Add(ticking);
    hook.AddNil();
    const bool proceed = hook.Run();
    ticking = hook.GetBool(1, ticking);
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockDisableMipmap(Block* self, Block*& result)
{
    ModHookCall hook(kModHook_Block_disableMipmap);
    hook.AddBlock(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockSetSemiTransparent(Block* self, Block*& result)
{
    ModHookCall hook(kModHook_Block_setSemiTransparent);
    hook.AddBlock(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockIsTicking(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isTicking);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockIsSlabBlock(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isSlabBlock);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockIsWaterBlocking(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isWaterBlocking);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockCanContainLiquid(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_canContainLiquid);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockIsStrippable(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isStrippable);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockGetStrippedBlock(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getStrippedBlock);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockLiquidCanFlowIntoFromDirection(Block* self, Level* levelSource, int x, int y, int z, int direction, bool& result)
{
    ModHookCall hook(kModHook_Block_liquidCanFlowIntoFromDirection);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool BlockGetShape(Block* self, int state, Level* levelSource, int x, int y, int z, AABB*& result)
{
    ModHookCall hook(kModHook_Block_getShape);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(6, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetLightColor(Block* self, int state, Level* levelSource, int x, int y, int z, int& lightValue, int& result)
{
    ModHookCall hook(kModHook_Block_getLightColor);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(lightValue);
    hook.Add(result);
    const bool proceed = hook.Run();
    lightValue = hook.GetInt(6, lightValue);
    result = hook.GetInt(7, result);
    return proceed;
}

bool BlockShouldRenderFace(Block* self, int state, Level* levelSource, int x, int y, int z, int direction, bool& result)
{
    ModHookCall hook(kModHook_Block_shouldRenderFace);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(7, result);
    return proceed;
}

bool BlockIsSolidFace(Block* self, Level* levelSource, int x, int y, int z, int direction, bool& result)
{
    ModHookCall hook(kModHook_Block_isSolidFace);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool BlockGetBlockFaceShape(Block* self, Level* levelSource, int state, int x, int y, int z, int direction, int& result)
{
    ModHookCall hook(kModHook_Block_getBlockFaceShape);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(state);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(7, result);
    return proceed;
}

bool BlockGetOutlineAABB(Block* self, int state, Level* level, int x, int y, int z, AABB*& result)
{
    ModHookCall hook(kModHook_Block_getOutlineAABB);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(6, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetClipAABB(Block* self, int state, Level* levelSource, int x, int y, int z, AABB*& result)
{
    ModHookCall hook(kModHook_Block_getClipAABB);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(6, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockIsSolidRender(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isSolidRender);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockMayPick(Block* self, int state, bool& includeLiquid, bool& result)
{
    ModHookCall hook(kModHook_Block_mayPick);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(includeLiquid);
    hook.Add(result);
    const bool proceed = hook.Run();
    includeLiquid = hook.GetBool(2, includeLiquid);
    result = hook.GetBool(3, result);
    return proceed;
}

bool BlockMayPick2(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_mayPick_2);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockRandomTick(Block* self, Level* level, int x, int y, int z, int state)
{
    ModHookCall hook(kModHook_Block_randomTick);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockTick(Block* self, Level* level, int x, int y, int z, int state)
{
    ModHookCall hook(kModHook_Block_tick);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockAnimateTick(Block* self, int state, Level* level, int x, int y, int z)
{
    ModHookCall hook(kModHook_Block_animateTick);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockDestroy(Block* self, Level* level, int x, int y, int z, int state)
{
    ModHookCall hook(kModHook_Block_destroy);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockNeighborChanged(Block* self, int state, Level* level, int x, int y, int z, Block* block, int fromPos_x, int fromPos_y, int fromPos_z)
{
    ModHookCall hook(kModHook_Block_neighborChanged);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddBlock(block);
    hook.Add(fromPos_x);
    hook.Add(fromPos_y);
    hook.Add(fromPos_z);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockAddLights(Block* self, Level* level, int x, int y, int z)
{
    ModHookCall hook(kModHook_Block_addLights);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockGetTickDelay(Block* self, Level* level, int& result)
{
    ModHookCall hook(kModHook_Block_getTickDelay);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockOnPlace(Block* self, Level* level, int x, int y, int z, int state)
{
    ModHookCall hook(kModHook_Block_onPlace);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockOnRemove(Block* self, Level* level, int x, int y, int z, int state)
{
    ModHookCall hook(kModHook_Block_onRemove);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockGetResourceCount(Block* self, int& result)
{
    ModHookCall hook(kModHook_Block_getResourceCount);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool BlockGetResource(Block* self, int state, int& lootBonus, std::wstring& result)
{
    ModHookCall hook(kModHook_Block_getResource);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(lootBonus);
    hook.Add(result);
    const bool proceed = hook.Run();
    lootBonus = hook.GetInt(2, lootBonus);
    result = hook.GetString(3, result);
    return proceed;
}

bool BlockGetDestroyProgress(Block* self, int state, const boost::shared_ptr<Entity>& player, Level* level, int x, int y, int z, float& result)
{
    ModHookCall hook(kModHook_Block_getDestroyProgress);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(player);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(7, result);
    return proceed;
}

bool BlockSpawnResources(Block* self, Level* level, int x, int y, int z, int state, int& lootBonus)
{
    ModHookCall hook(kModHook_Block_spawnResources);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(lootBonus);
    const bool proceed = hook.Run();
    lootBonus = hook.GetInt(6, lootBonus);
    return proceed;
}

bool BlockSpawnResources2(Block* self, Level* level, int x, int y, int z, int state, float& chance, int& lootBonus)
{
    ModHookCall hook(kModHook_Block_spawnResources_2);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(chance);
    hook.Add(lootBonus);
    const bool proceed = hook.Run();
    chance = hook.GetFloat(6, chance);
    lootBonus = hook.GetInt(7, lootBonus);
    return proceed;
}

bool BlockPopExperience(Block* self, Level* level, int x, int y, int z, int& amount)
{
    ModHookCall hook(kModHook_Block_popExperience);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(amount);
    const bool proceed = hook.Run();
    amount = hook.GetInt(5, amount);
    return proceed;
}

bool BlockGetSpawnResourcesAuxValue(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getSpawnResourcesAuxValue);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockGetExplosionResistance(Block* self, const boost::shared_ptr<Entity>& entity, float& result)
{
    ModHookCall hook(kModHook_Block_getExplosionResistance);
    hook.AddBlock(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(2, result);
    return proceed;
}

bool BlockClip(Block* self, int state, Level* level, int x, int y, int z, Vec3* from, Vec3* to, HitResult*& result)
{
    ModHookCall hook(kModHook_Block_clip);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(from));
    hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(to));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        HitResult* object = NULL;
        result = hook.GetHandle(8, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockClip2(Block* self, int x, int y, int z, Vec3* from, Vec3* to, AABB* box, HitResult*& result)
{
    ModHookCall hook(kModHook_Block_clip_2);
    hook.AddBlock(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(from));
    hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(to));
    hook.AddObject(mod::api::eHandle_AABB, ModHandles::Add(box));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        HitResult* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockWasExploded(Block* self, Level* level, int x, int y, int z, Explosion* explosion)
{
    ModHookCall hook(kModHook_Block_wasExploded);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddObject(mod::api::eHandle_Explosion, ModHandles::Add(explosion));
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockGetRenderLayer(Block* self, int& result)
{
    ModHookCall hook(kModHook_Block_getRenderLayer);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool BlockMayPlace(Block* self, Level* level, int x, int y, int z, int direction, bool& result)
{
    ModHookCall hook(kModHook_Block_mayPlace);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool BlockMayPlace2(Block* self, Level* level, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_Block_mayPlace_2);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(5, result);
    return proceed;
}

bool BlockMayPlaceOn(Block* self, Level* level, int x, int y, int z, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_mayPlaceOn);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool BlockTestUse(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_TestUse);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockTestUse2(Block* self, Level* level, int x, int y, int z, int state, const boost::shared_ptr<Entity>& player, bool& result)
{
    ModHookCall hook(kModHook_Block_TestUse_2);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(player);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(7, result);
    return proceed;
}

bool BlockUse(Block* self, Level* level, int x, int y, int z, int state, const boost::shared_ptr<Entity>& player, int hand, int direction, float& hitX, float& hitY, float& hitZ, bool& clientSide, bool& result)
{
    ModHookCall hook(kModHook_Block_use);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(player);
    hook.Add(hand);
    hook.Add(direction);
    hook.Add(hitX);
    hook.Add(hitY);
    hook.Add(hitZ);
    hook.Add(clientSide);
    hook.Add(result);
    const bool proceed = hook.Run();
    hitX = hook.GetFloat(9, hitX);
    hitY = hook.GetFloat(10, hitY);
    hitZ = hook.GetFloat(11, hitZ);
    clientSide = hook.GetBool(12, clientSide);
    result = hook.GetBool(13, result);
    return proceed;
}

bool BlockStepOn(Block* self, Level* level, int x, int y, int z, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_Block_stepOn);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockGetStateForPlacement(Block* self, Level* level, int x, int y, int z, int direction, float& hitX, float& hitY, float& hitZ, int& auxValue, const boost::shared_ptr<Entity>& placer, int& result)
{
    ModHookCall hook(kModHook_Block_getStateForPlacement);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(hitX);
    hook.Add(hitY);
    hook.Add(hitZ);
    hook.Add(auxValue);
    hook.Add(placer);
    hook.Add(result);
    const bool proceed = hook.Run();
    hitX = hook.GetFloat(6, hitX);
    hitY = hook.GetFloat(7, hitY);
    hitZ = hook.GetFloat(8, hitZ);
    auxValue = hook.GetInt(9, auxValue);
    result = hook.GetInt(11, result);
    return proceed;
}

bool BlockPrepareRender(Block* self, Level* level, int x, int y, int z)
{
    ModHookCall hook(kModHook_Block_prepareRender);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockAttack(Block* self, Level* level, int x, int y, int z, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_Block_attack);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockHandleEntityInside(Block* self, Level* level, int x, int y, int z, const boost::shared_ptr<Entity>& entity, Vec3* motion, Vec3*& result)
{
    ModHookCall hook(kModHook_Block_handleEntityInside);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(entity);
    hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(motion));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetColor(Block* self, int& result)
{
    ModHookCall hook(kModHook_Block_getColor);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool BlockGetColor2(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getColor_2);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockGetColor3(Block* self, Level* levelSource, int x, int y, int z, int& tintIndex, int& result)
{
    ModHookCall hook(kModHook_Block_getColor_3);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(tintIndex);
    hook.Add(result);
    const bool proceed = hook.Run();
    tintIndex = hook.GetInt(5, tintIndex);
    result = hook.GetInt(6, result);
    return proceed;
}

bool BlockGetColor4(Block* self, Level* levelSource, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_Block_getColor_4);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(5, result);
    return proceed;
}

bool BlockGetColor5(Block* self, Level* levelSource, int x, int y, int z, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getColor_5);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(6, result);
    return proceed;
}

bool BlockGetSignal(Block* self, int state, Level* levelSource, int x, int y, int z, int direction, int& result)
{
    ModHookCall hook(kModHook_Block_getSignal);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(7, result);
    return proceed;
}

bool BlockIsSignalSource(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_isSignalSource);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockEntityInside(Block* self, Level* level, int x, int y, int z, int state, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_Block_entityInside);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockGetDirectSignal(Block* self, int state, Level* levelSource, int x, int y, int z, int direction, int& result)
{
    ModHookCall hook(kModHook_Block_getDirectSignal);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(7, result);
    return proceed;
}

bool BlockWaterSpreadCausesSpawn(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_waterSpreadCausesSpawn);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockPlayerDestroy(Block* self, Level* level, const boost::shared_ptr<Entity>& player, int x, int y, int z, int state, BlockEntity* blockEntity, ItemInstance* tool)
{
    ModHookCall hook(kModHook_Block_playerDestroy);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(player);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_BlockEntity, ModHandles::Add(blockEntity));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(tool));
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockIsSilkTouchable(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isSilkTouchable);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockCanSurvive(Block* self, Level* level, int x, int y, int z, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_canSurvive);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool BlockGetResourceCountForLootBonus(Block* self, int& lootBonus, int& result)
{
    ModHookCall hook(kModHook_Block_getResourceCountForLootBonus);
    hook.AddBlock(self);
    hook.Add(lootBonus);
    hook.Add(result);
    const bool proceed = hook.Run();
    lootBonus = hook.GetInt(1, lootBonus);
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockSetPlacedBy(Block* self, Level* level, int x, int y, int z, int state, const boost::shared_ptr<Entity>& placer, ItemInstance* itemInstance)
{
    ModHookCall hook(kModHook_Block_setPlacedBy);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(placer);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockSetNameAndDescriptionId(Block* self, int& descriptionId, int& useDescriptionId, Block*& result)
{
    ModHookCall hook(kModHook_Block_setNameAndDescriptionId);
    hook.AddBlock(self);
    hook.Add(descriptionId);
    hook.Add(useDescriptionId);
    hook.AddNil();
    const bool proceed = hook.Run();
    descriptionId = hook.GetInt(1, descriptionId);
    useDescriptionId = hook.GetInt(2, useDescriptionId);
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockIsPossibleToRespawnInThis(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isPossibleToRespawnInThis);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockGetName(Block* self, std::wstring& result)
{
    ModHookCall hook(kModHook_Block_getName);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool BlockGetDescriptionId(Block* self, int& auxValue, int& result)
{
    ModHookCall hook(kModHook_Block_getDescriptionId);
    hook.AddBlock(self);
    hook.Add(auxValue);
    hook.Add(result);
    const bool proceed = hook.Run();
    auxValue = hook.GetInt(1, auxValue);
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockGetUseDescriptionId(Block* self, int& auxValue, int& result)
{
    ModHookCall hook(kModHook_Block_getUseDescriptionId);
    hook.AddBlock(self);
    hook.Add(auxValue);
    hook.Add(result);
    const bool proceed = hook.Run();
    auxValue = hook.GetInt(1, auxValue);
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockTriggerEvent(Block* self, int state, Level* level, int x, int y, int z, int& eventId, int& eventParam, bool& result)
{
    ModHookCall hook(kModHook_Block_triggerEvent);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(eventId);
    hook.Add(eventParam);
    hook.Add(result);
    const bool proceed = hook.Run();
    eventId = hook.GetInt(6, eventId);
    eventParam = hook.GetInt(7, eventParam);
    result = hook.GetBool(8, result);
    return proceed;
}

bool BlockIsCollectStatistics(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isCollectStatistics);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockShouldBlockTick(Block* self, Level* level, int x, int y, int z, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_shouldBlockTick);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool BlockSetNotCollectStatistics(Block* self, Block*& result)
{
    ModHookCall hook(kModHook_Block_setNotCollectStatistics);
    hook.AddBlock(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetPistonPushReaction(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getPistonPushReaction);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockGetShadeBrightness(Block* self, int state, float& result)
{
    ModHookCall hook(kModHook_Block_getShadeBrightness);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(2, result);
    return proceed;
}

bool BlockFallOn(Block* self, Level* level, int x, int y, int z, const boost::shared_ptr<Entity>& entity, float& fallDistance)
{
    ModHookCall hook(kModHook_Block_fallOn);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(entity);
    hook.Add(fallDistance);
    const bool proceed = hook.Run();
    fallDistance = hook.GetFloat(6, fallDistance);
    return proceed;
}

bool BlockUpdateEntityAfterFallOn(Block* self, Level* level, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_Block_updateEntityAfterFallOn);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockPlayerWillDestroy(Block* self, Level* level, int x, int y, int z, int state, const boost::shared_ptr<Entity>& player, bool& result)
{
    ModHookCall hook(kModHook_Block_playerWillDestroy);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(state);
    hook.Add(player);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(7, result);
    return proceed;
}

bool BlockHandleRain(Block* self, Level* level, int x, int y, int z)
{
    ModHookCall hook(kModHook_Block_handleRain);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockCanInstantlyTick(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_canInstantlyTick);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool BlockDropFromExplosion(Block* self, Explosion* explosion, bool& result)
{
    ModHookCall hook(kModHook_Block_dropFromExplosion);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_Explosion, ModHandles::Add(explosion));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockIsMatching(Block* self, Block* block, bool& result)
{
    ModHookCall hook(kModHook_Block_isMatching);
    hook.AddBlock(self);
    hook.AddBlock(block);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockHasAnalogOutputSignal(Block* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Block_hasAnalogOutputSignal);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool BlockGetAnalogOutputSignal(Block* self, int state, Level* level, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_Block_getAnalogOutputSignal);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(6, result);
    return proceed;
}

bool BlockSetIconName(Block* self, const std::wstring& iconName, Block*& result)
{
    ModHookCall hook(kModHook_Block_setIconName);
    hook.AddBlock(self);
    hook.Add(iconName);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Block* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetIconName(Block* self, std::wstring& result)
{
    ModHookCall hook(kModHook_Block_getIconName);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool BlockRegisterIcons(Block* self, IconRegister* iconRegister)
{
    ModHookCall hook(kModHook_Block_registerIcons);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_IconRegister, ModHandles::Add(iconRegister));
    const bool proceed = hook.Run();
    return proceed;
}

bool BlockGetTileItemIconName(Block* self, std::wstring& result)
{
    ModHookCall hook(kModHook_Block_getTileItemIconName);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool BlockGetIconTexture(Block* self, int direction, int state, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Block_getIconTexture);
    hook.AddBlock(self);
    hook.Add(direction);
    hook.Add(state);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetTexture(Block* self, Level* levelSource, int x, int y, int z, int direction, int state, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Block_getTexture);
    hook.AddBlock(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(state);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetTexture2(Block* self, int direction, int state, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Block_getTexture_2);
    hook.AddBlock(self);
    hook.Add(direction);
    hook.Add(state);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetTexture3(Block* self, int direction, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Block_getTexture_3);
    hook.AddBlock(self);
    hook.Add(direction);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetInventoryRenderState(Block* self, int state, int& result)
{
    ModHookCall hook(kModHook_Block_getInventoryRenderState);
    hook.AddBlock(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool BlockCreateBlockStateDefinition(Block* self, BlockStateDefinition*& result)
{
    ModHookCall hook(kModHook_Block_createBlockStateDefinition);
    hook.AddBlock(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        BlockStateDefinition* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetBlockStateDefinition(Block* self, BlockStateDefinition*& result)
{
    ModHookCall hook(kModHook_Block_getBlockStateDefinition);
    hook.AddBlock(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        BlockStateDefinition* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetOffsetType(Block* self, int& result)
{
    ModHookCall hook(kModHook_Block_getOffsetType);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool BlockGetOffset(Block* self, int state, Level* levelSource, int x, int y, int z, Vec3*& result)
{
    ModHookCall hook(kModHook_Block_getOffset);
    hook.AddBlock(self);
    hook.Add(state);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(6, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockGetSoundType(Block* self, SoundType*& result)
{
    ModHookCall hook(kModHook_Block_getSoundType);
    hook.AddBlock(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        SoundType* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool BlockToString(Block* self, std::wstring& result)
{
    ModHookCall hook(kModHook_Block_toString);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool BlockIsLiquidBlock(Block* self, bool& result)
{
    ModHookCall hook(kModHook_Block_isLiquidBlock);
    hook.AddBlock(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ItemVerifyTagAfterLoad(Item* self, CompoundTag* tag, bool& result)
{
    ModHookCall hook(kModHook_Item_verifyTagAfterLoad);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_CompoundTag, ModHandles::Add(tag));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool ItemUseOn(Item* self, const boost::shared_ptr<Entity>& player, Level* level, int x, int y, int z, int hand, int direction, float& hitX, float& hitY, float& hitZ, bool& simulate, int& result)
{
    ModHookCall hook(kModHook_Item_useOn);
    hook.AddItem(self);
    hook.Add(player);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(hand);
    hook.Add(direction);
    hook.Add(hitX);
    hook.Add(hitY);
    hook.Add(hitZ);
    hook.Add(simulate);
    hook.Add(result);
    const bool proceed = hook.Run();
    hitX = hook.GetFloat(8, hitX);
    hitY = hook.GetFloat(9, hitY);
    hitZ = hook.GetFloat(10, hitZ);
    simulate = hook.GetBool(11, simulate);
    result = hook.GetInt(12, result);
    return proceed;
}

bool ItemGetDestroySpeed(Item* self, ItemInstance* itemInstance, int state, float& result)
{
    ModHookCall hook(kModHook_Item_getDestroySpeed);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(3, result);
    return proceed;
}

bool ItemTestUse(Item* self, Level* level, const boost::shared_ptr<Entity>& player, int hand, bool& result)
{
    ModHookCall hook(kModHook_Item_TestUse);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(player);
    hook.Add(hand);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(4, result);
    return proceed;
}

bool ItemGetMaxStackSize(Item* self, int& result)
{
    ModHookCall hook(kModHook_Item_getMaxStackSize);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool ItemGetLevelDataForAuxValue(Item* self, int& auxValue, int& result)
{
    ModHookCall hook(kModHook_Item_getLevelDataForAuxValue);
    hook.AddItem(self);
    hook.Add(auxValue);
    hook.Add(result);
    const bool proceed = hook.Run();
    auxValue = hook.GetInt(1, auxValue);
    result = hook.GetInt(2, result);
    return proceed;
}

bool ItemHurtEnemy(Item* self, ItemInstance* itemInstance, const boost::shared_ptr<Entity>& target, const boost::shared_ptr<Entity>& attacker, bool& result)
{
    ModHookCall hook(kModHook_Item_hurtEnemy);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(target);
    hook.Add(attacker);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(4, result);
    return proceed;
}

bool ItemMineBlock(Item* self, ItemInstance* itemInstance, Level* level, int state, int x, int y, int z, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Item_mineBlock);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(state);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(8, result);
    return proceed;
}

bool ItemGetAttackDamage(Item* self, const boost::shared_ptr<Entity>& entity, int& result)
{
    ModHookCall hook(kModHook_Item_getAttackDamage);
    hook.AddItem(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool ItemCanDestroySpecial(Item* self, int state, bool& result)
{
    ModHookCall hook(kModHook_Item_canDestroySpecial);
    hook.AddItem(self);
    hook.Add(state);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool ItemInteractEnemy(Item* self, ItemInstance* itemInstance, const boost::shared_ptr<Entity>& player, const boost::shared_ptr<Entity>& target, int hand, bool& result)
{
    ModHookCall hook(kModHook_Item_interactEnemy);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(player);
    hook.Add(target);
    hook.Add(hand);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(5, result);
    return proceed;
}

bool ItemIsHandEquipped(Item* self, bool& result)
{
    ModHookCall hook(kModHook_Item_isHandEquipped);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ItemIsMirroredArt(Item* self, bool& result)
{
    ModHookCall hook(kModHook_Item_isMirroredArt);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ItemGetDescriptionId(Item* self, int& auxValue, int& result)
{
    ModHookCall hook(kModHook_Item_getDescriptionId);
    hook.AddItem(self);
    hook.Add(auxValue);
    hook.Add(result);
    const bool proceed = hook.Run();
    auxValue = hook.GetInt(1, auxValue);
    result = hook.GetInt(2, result);
    return proceed;
}

bool ItemGetDescriptionId2(Item* self, ItemInstance* itemInstance, int& result)
{
    ModHookCall hook(kModHook_Item_getDescriptionId_2);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool ItemGetUseDescriptionId(Item* self, int& result)
{
    ModHookCall hook(kModHook_Item_getUseDescriptionId);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool ItemGetUseDescriptionId2(Item* self, ItemInstance* itemInstance, int& result)
{
    ModHookCall hook(kModHook_Item_getUseDescriptionId_2);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool ItemShouldOverrideMultiplayerNBT(Item* self, bool& result)
{
    ModHookCall hook(kModHook_Item_shouldOverrideMultiplayerNBT);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ItemGetColor(Item* self, ItemInstance* itemInstance, int& layer, int& result)
{
    ModHookCall hook(kModHook_Item_getColor);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(layer);
    hook.Add(result);
    const bool proceed = hook.Run();
    layer = hook.GetInt(2, layer);
    result = hook.GetInt(3, result);
    return proceed;
}

bool ItemInventoryTick(Item* self, ItemInstance* itemInstance, Level* level, const boost::shared_ptr<Entity>& entity, int& slot, bool& selected)
{
    ModHookCall hook(kModHook_Item_inventoryTick);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(entity);
    hook.Add(slot);
    hook.Add(selected);
    const bool proceed = hook.Run();
    slot = hook.GetInt(4, slot);
    selected = hook.GetBool(5, selected);
    return proceed;
}

bool ItemOnCraftedBy(Item* self, ItemInstance* itemInstance, Level* level, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_Item_onCraftedBy);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool ItemIsComplex(Item* self, bool& result)
{
    ModHookCall hook(kModHook_Item_isComplex);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ItemGetUseAnimation(Item* self, ItemInstance* itemInstance, int& result)
{
    ModHookCall hook(kModHook_Item_getUseAnimation);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool ItemGetUseDuration(Item* self, ItemInstance* itemInstance, int& result)
{
    ModHookCall hook(kModHook_Item_getUseDuration);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool ItemReleaseUsing(Item* self, ItemInstance* itemInstance, Level* level, const boost::shared_ptr<Entity>& entity, int& timeLeft)
{
    ModHookCall hook(kModHook_Item_releaseUsing);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(entity);
    hook.Add(timeLeft);
    const bool proceed = hook.Run();
    timeLeft = hook.GetInt(4, timeLeft);
    return proceed;
}

bool ItemGetName(Item* self, ItemInstance* itemInstance, std::wstring& result)
{
    ModHookCall hook(kModHook_Item_getName);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(2, result);
    return proceed;
}

bool ItemIsFoil(Item* self, ItemInstance* itemInstance, bool& result)
{
    ModHookCall hook(kModHook_Item_isFoil);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool ItemGetRarity(Item* self, ItemInstance* itemInstance, Rarity*& result)
{
    ModHookCall hook(kModHook_Item_getRarity);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Rarity* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool ItemIsEnchantable(Item* self, ItemInstance* itemInstance, bool& result)
{
    ModHookCall hook(kModHook_Item_isEnchantable);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool ItemGetEnchantmentValue(Item* self, int& result)
{
    ModHookCall hook(kModHook_Item_getEnchantmentValue);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool ItemMayBePlacedInAdventureMode(Item* self, bool& result)
{
    ModHookCall hook(kModHook_Item_mayBePlacedInAdventureMode);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ItemIsValidRepairItem(Item* self, ItemInstance* itemInstance, ItemInstance* repairItem, bool& result)
{
    ModHookCall hook(kModHook_Item_isValidRepairItem);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(repairItem));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(3, result);
    return proceed;
}

bool ItemRegisterIcons(Item* self, IconRegister* iconRegister)
{
    ModHookCall hook(kModHook_Item_registerIcons);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_IconRegister, ModHandles::Add(iconRegister));
    const bool proceed = hook.Run();
    return proceed;
}

bool ItemHasMultipleSpriteLayers(Item* self, bool& result)
{
    ModHookCall hook(kModHook_Item_hasMultipleSpriteLayers);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ItemGetLayerIcon(Item* self, int& auxValue, int& layer, ItemInstance* itemInstance, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Item_getLayerIcon);
    hook.AddItem(self);
    hook.Add(auxValue);
    hook.Add(layer);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddNil();
    const bool proceed = hook.Run();
    auxValue = hook.GetInt(1, auxValue);
    layer = hook.GetInt(2, layer);
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(4, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool ItemGetIconType(Item* self, int& result)
{
    ModHookCall hook(kModHook_Item_getIconType);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool ItemGetIcon(Item* self, int& auxValue, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Item_getIcon);
    hook.AddItem(self);
    hook.Add(auxValue);
    hook.AddNil();
    const bool proceed = hook.Run();
    auxValue = hook.GetInt(1, auxValue);
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool ItemGetIcon2(Item* self, ItemInstance* itemInstance, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Item_getIcon_2);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool ItemGetArmorType(Item* self, int& result)
{
    ModHookCall hook(kModHook_Item_GetArmorType);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool ItemGetOverrideCount(Item* self, int& result)
{
    ModHookCall hook(kModHook_Item_GetOverrideCount);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool ItemGetOverrideCountColour(Item* self, int& result)
{
    ModHookCall hook(kModHook_Item_GetOverrideCountColour);
    hook.AddItem(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool ItemGetOverrideCountIcon(Item* self, ItemInstance* itemInstance, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Item_GetOverrideCountIcon);
    hook.AddItem(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityFjDerivedCtorCalls(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_fjDerivedCtorCalls);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityKill(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_kill);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityResetPos(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_resetPos);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityRemove(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_remove);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntitySetDropContainerContent(const boost::shared_ptr<Entity>& self, bool& dropContainerContent)
{
    ModHookCall hook(kModHook_Entity_setDropContainerContent);
    hook.Add(self);
    hook.Add(dropContainerContent);
    const bool proceed = hook.Run();
    dropContainerContent = hook.GetBool(1, dropContainerContent);
    return proceed;
}

bool EntitySetSize(const boost::shared_ptr<Entity>& self, float& width, float& height)
{
    ModHookCall hook(kModHook_Entity_setSize);
    hook.Add(self);
    hook.Add(width);
    hook.Add(height);
    const bool proceed = hook.Run();
    width = hook.GetFloat(1, width);
    height = hook.GetFloat(2, height);
    return proceed;
}

bool EntitySetPos(const boost::shared_ptr<Entity>& self, float x, float y, float z)
{
    ModHookCall hook(kModHook_Entity_setPos);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityTick(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_tick);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityBaseTick(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_baseTick);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityProcessDimensionDelay(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_processDimensionDelay);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetPortalWaitTime(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getPortalWaitTime);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntitySetSecondsOnFire(const boost::shared_ptr<Entity>& self, int& seconds)
{
    ModHookCall hook(kModHook_Entity_setSecondsOnFire);
    hook.Add(self);
    hook.Add(seconds);
    const bool proceed = hook.Run();
    seconds = hook.GetInt(1, seconds);
    return proceed;
}

bool EntityClearFire(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_clearFire);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityOutOfWorld(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_outOfWorld);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityMove(const boost::shared_ptr<Entity>& self, int type, float x, float y, float z, bool& pushedByPiston)
{
    ModHookCall hook(kModHook_Entity_move);
    hook.Add(self);
    hook.Add(type);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(pushedByPiston);
    const bool proceed = hook.Run();
    pushedByPiston = hook.GetBool(5, pushedByPiston);
    return proceed;
}

bool EntitySetLocationFromBoundingbox(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_setLocationFromBoundingbox);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetSwimSound(const boost::shared_ptr<Entity>& self, SoundEvent*& result)
{
    ModHookCall hook(kModHook_Entity_getSwimSound);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        SoundEvent* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetSwimSplashSound(const boost::shared_ptr<Entity>& self, SoundEvent*& result)
{
    ModHookCall hook(kModHook_Entity_getSwimSplashSound);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        SoundEvent* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetHeavySplashSound(const boost::shared_ptr<Entity>& self, SoundEvent*& result)
{
    ModHookCall hook(kModHook_Entity_getHeavySplashSound);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        SoundEvent* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityCheckInsideBlocks(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_checkInsideBlocks);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityOnInsideBlock(const boost::shared_ptr<Entity>& self, int state)
{
    ModHookCall hook(kModHook_Entity_onInsideBlock);
    hook.Add(self);
    hook.Add(state);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPlayStepSound(const boost::shared_ptr<Entity>& self, int x, int y, int z, Block* block)
{
    ModHookCall hook(kModHook_Entity_playStepSound);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddBlock(block);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPlayUnderwaterSound(const boost::shared_ptr<Entity>& self, int x, int y, int z, Block* block)
{
    ModHookCall hook(kModHook_Entity_playUnderwaterSound);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddBlock(block);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPlayFlySound(const boost::shared_ptr<Entity>& self, float& moveDist, float& result)
{
    ModHookCall hook(kModHook_Entity_playFlySound);
    hook.Add(self);
    hook.Add(moveDist);
    hook.Add(result);
    const bool proceed = hook.Run();
    moveDist = hook.GetFloat(1, moveDist);
    result = hook.GetFloat(2, result);
    return proceed;
}

bool EntityMakeFlySound(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_makeFlySound);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityPlaySound(const boost::shared_ptr<Entity>& self, SoundEvent* sound, float& volume, float& pitch)
{
    ModHookCall hook(kModHook_Entity_playSound);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_SoundEvent, ModHandles::Add(sound));
    hook.Add(volume);
    hook.Add(pitch);
    const bool proceed = hook.Run();
    volume = hook.GetFloat(2, volume);
    pitch = hook.GetFloat(3, pitch);
    return proceed;
}

bool EntityIsSilent(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isSilent);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetSilent(const boost::shared_ptr<Entity>& self, bool& silent)
{
    ModHookCall hook(kModHook_Entity_setSilent);
    hook.Add(self);
    hook.Add(silent);
    const bool proceed = hook.Run();
    silent = hook.GetBool(1, silent);
    return proceed;
}

bool EntityIsNoGravity(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isNoGravity);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetNoGravity(const boost::shared_ptr<Entity>& self, bool& noGravity)
{
    ModHookCall hook(kModHook_Entity_setNoGravity);
    hook.Add(self);
    hook.Add(noGravity);
    const bool proceed = hook.Run();
    noGravity = hook.GetBool(1, noGravity);
    return proceed;
}

bool EntityIsMoving(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isMoving);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetMoving(const boost::shared_ptr<Entity>& self, bool& moving)
{
    ModHookCall hook(kModHook_Entity_setMoving);
    hook.Add(self);
    hook.Add(moving);
    const bool proceed = hook.Run();
    moving = hook.GetBool(1, moving);
    return proceed;
}

bool EntityMakeStepSound(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_makeStepSound);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetCollideBox(const boost::shared_ptr<Entity>& self, AABB*& result)
{
    ModHookCall hook(kModHook_Entity_getCollideBox);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityBurn(const boost::shared_ptr<Entity>& self, int& damage)
{
    ModHookCall hook(kModHook_Entity_burn);
    hook.Add(self);
    hook.Add(damage);
    const bool proceed = hook.Run();
    damage = hook.GetInt(1, damage);
    return proceed;
}

bool EntityCauseFallDamage(const boost::shared_ptr<Entity>& self, float& fallDistance, float& damageMultiplier)
{
    ModHookCall hook(kModHook_Entity_causeFallDamage);
    hook.Add(self);
    hook.Add(fallDistance);
    hook.Add(damageMultiplier);
    const bool proceed = hook.Run();
    fallDistance = hook.GetFloat(1, fallDistance);
    damageMultiplier = hook.GetFloat(2, damageMultiplier);
    return proceed;
}

bool EntityIsInWater(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInWater);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityHeadInWater(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_headInWater);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsOverWater(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isOverWater);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityUpdateInWaterState(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_updateInWaterState);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityDoWaterSplashEffect(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_doWaterSplashEffect);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityUpdateSprintingState(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_updateSprintingState);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityDoSprintParticleEffect(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_doSprintParticleEffect);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityIsUnderLiquid(const boost::shared_ptr<Entity>& self, Material* material, bool& result)
{
    ModHookCall hook(kModHook_Entity_isUnderLiquid);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Material, ModHandles::Add(material));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsInLiquid(const boost::shared_ptr<Entity>& self, Material* material, Vec3* pos, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInLiquid);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Material, ModHandles::Add(material));
    hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(pos));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(3, result);
    return proceed;
}

bool EntityGetLightColor(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getLightColor);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityGetBrightness(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getBrightness);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntitySetLevel(const boost::shared_ptr<Entity>& self, Level* level)
{
    ModHookCall hook(kModHook_Entity_setLevel);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityMoveTo(const boost::shared_ptr<Entity>& self, float x, float y, float z, float& yRot, float& xRot)
{
    ModHookCall hook(kModHook_Entity_moveTo);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(yRot);
    hook.Add(xRot);
    const bool proceed = hook.Run();
    yRot = hook.GetFloat(4, yRot);
    xRot = hook.GetFloat(5, xRot);
    return proceed;
}

bool EntityPlayerTouch(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_Entity_playerTouch);
    hook.Add(self);
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPush(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_Entity_push);
    hook.Add(self);
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPush2(const boost::shared_ptr<Entity>& self, float x, float y, float z)
{
    ModHookCall hook(kModHook_Entity_push_2);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityMarkHurt(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_markHurt);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityHurt(const boost::shared_ptr<Entity>& self, DamageSource* source, float& damage, bool& result)
{
    ModHookCall hook(kModHook_Entity_hurt);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(damage);
    hook.Add(result);
    const bool proceed = hook.Run();
    damage = hook.GetFloat(2, damage);
    result = hook.GetBool(3, result);
    return proceed;
}

bool EntityGetViewVector(const boost::shared_ptr<Entity>& self, float& partialTicks, Vec3*& result)
{
    ModHookCall hook(kModHook_Entity_getViewVector);
    hook.Add(self);
    hook.Add(partialTicks);
    hook.AddNil();
    const bool proceed = hook.Run();
    partialTicks = hook.GetFloat(1, partialTicks);
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityIsPickable(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isPickable);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsPushable(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Entity_isPushable);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityAwardKillScore(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& killed, int& score, DamageSource* source)
{
    ModHookCall hook(kModHook_Entity_awardKillScore);
    hook.Add(self);
    hook.Add(killed);
    hook.Add(score);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    const bool proceed = hook.Run();
    score = hook.GetInt(2, score);
    return proceed;
}

bool EntityShouldRender(const boost::shared_ptr<Entity>& self, float x, float y, float z, bool& result)
{
    ModHookCall hook(kModHook_Entity_shouldRender);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(4, result);
    return proceed;
}

bool EntityShouldRender2(const boost::shared_ptr<Entity>& self, float distanceSqr, bool& result)
{
    ModHookCall hook(kModHook_Entity_shouldRender_2);
    hook.Add(self);
    hook.Add(distanceSqr);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityShouldRenderAtSqrDistance(const boost::shared_ptr<Entity>& self, float distanceSqr, bool& result)
{
    ModHookCall hook(kModHook_Entity_shouldRenderAtSqrDistance);
    hook.Add(self);
    hook.Add(distanceSqr);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntitySave(const boost::shared_ptr<Entity>& self, CompoundTag* tag, bool& result)
{
    ModHookCall hook(kModHook_Entity_save);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_CompoundTag, ModHandles::Add(tag));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityLoad(const boost::shared_ptr<Entity>& self, CompoundTag* tag)
{
    ModHookCall hook(kModHook_Entity_load);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_CompoundTag, ModHandles::Add(tag));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityShouldTick(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_shouldTick);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityRepositionEntityAfterLoad(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_repositionEntityAfterLoad);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetAdditionContructionTag(const boost::shared_ptr<Entity>& self, CompoundTag*& result)
{
    ModHookCall hook(kModHook_Entity_getAdditionContructionTag);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        CompoundTag* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityIsAlive(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isAlive);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsInWall(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInWall);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityInteract(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, int hand, bool& result)
{
    ModHookCall hook(kModHook_Entity_interact);
    hook.Add(self);
    hook.Add(player);
    hook.Add(hand);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(3, result);
    return proceed;
}

bool EntityGetCollideAgainstBox(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, AABB*& result)
{
    ModHookCall hook(kModHook_Entity_getCollideAgainstBox);
    hook.Add(self);
    hook.Add(entity);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityRideTick(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_rideTick);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPositionRider(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& passenger)
{
    ModHookCall hook(kModHook_Entity_positionRider);
    hook.Add(self);
    hook.Add(passenger);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityOnPassengerTurned(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& passenger)
{
    ModHookCall hook(kModHook_Entity_onPassengerTurned);
    hook.Add(self);
    hook.Add(passenger);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityShowVehicleHealth(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_showVehicleHealth);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityStartRiding(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& vehicle, bool& result)
{
    ModHookCall hook(kModHook_Entity_startRiding);
    hook.Add(self);
    hook.Add(vehicle);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityStartRiding2(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, bool& force, bool& result)
{
    ModHookCall hook(kModHook_Entity_startRiding_2);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(force);
    hook.Add(result);
    const bool proceed = hook.Run();
    force = hook.GetBool(2, force);
    result = hook.GetBool(3, result);
    return proceed;
}

bool EntityHideUseTooltips(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_HideUseTooltips);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityCanRide(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& vehicle, bool& result)
{
    ModHookCall hook(kModHook_Entity_canRide);
    hook.Add(self);
    hook.Add(vehicle);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityEjectPassengers(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_ejectPassengers);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityStopRiding(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_stopRiding);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityStopRidingBeforeTeleport(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_stopRidingBeforeTeleport);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityAddPassenger(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& passenger)
{
    ModHookCall hook(kModHook_Entity_addPassenger);
    hook.Add(self);
    hook.Add(passenger);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityRemovePassenger(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& passenger)
{
    ModHookCall hook(kModHook_Entity_removePassenger);
    hook.Add(self);
    hook.Add(passenger);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCanAddPassenger(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& passenger, bool& result)
{
    ModHookCall hook(kModHook_Entity_canAddPassenger);
    hook.Add(self);
    hook.Add(passenger);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityLerpTo(const boost::shared_ptr<Entity>& self, float x, float y, float z, float& yRot, float& xRot, int& steps, bool& teleport)
{
    ModHookCall hook(kModHook_Entity_lerpTo);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(yRot);
    hook.Add(xRot);
    hook.Add(steps);
    hook.Add(teleport);
    const bool proceed = hook.Run();
    yRot = hook.GetFloat(4, yRot);
    xRot = hook.GetFloat(5, xRot);
    steps = hook.GetInt(6, steps);
    teleport = hook.GetBool(7, teleport);
    return proceed;
}

bool EntityGetPickRadius(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getPickRadius);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityGetLookAngle(const boost::shared_ptr<Entity>& self, Vec3*& result)
{
    ModHookCall hook(kModHook_Entity_getLookAngle);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetForward(const boost::shared_ptr<Entity>& self, Vec3*& result)
{
    ModHookCall hook(kModHook_Entity_getForward);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityHandleInsidePortal(const boost::shared_ptr<Entity>& self, int x, int y, int z)
{
    ModHookCall hook(kModHook_Entity_handleInsidePortal);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetDimensionChangingDelay(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getDimensionChangingDelay);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityLerpMotion(const boost::shared_ptr<Entity>& self, float xd, float yd, float zd)
{
    ModHookCall hook(kModHook_Entity_lerpMotion);
    hook.Add(self);
    hook.Add(xd);
    hook.Add(yd);
    hook.Add(zd);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityHandleEntityEvent(const boost::shared_ptr<Entity>& self, int event, int& data)
{
    ModHookCall hook(kModHook_Entity_handleEntityEvent);
    hook.Add(self);
    hook.Add(event);
    hook.Add(data);
    const bool proceed = hook.Run();
    data = hook.GetInt(2, data);
    return proceed;
}

bool EntityAnimateHurt(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_animateHurt);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntitySetItemSlot(const boost::shared_ptr<Entity>& self, EquipmentSlot* slot, ItemInstance* item)
{
    ModHookCall hook(kModHook_Entity_setItemSlot);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_EquipmentSlot, ModHandles::Add(slot));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityIsOnFire(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isOnFire);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsPassenger(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isPassenger);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsVehicle(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isVehicle);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsSneaking(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isSneaking);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetSneaking(const boost::shared_ptr<Entity>& self, bool& sneaking)
{
    ModHookCall hook(kModHook_Entity_setSneaking);
    hook.Add(self);
    hook.Add(sneaking);
    const bool proceed = hook.Run();
    sneaking = hook.GetBool(1, sneaking);
    return proceed;
}

bool EntityIsIdle(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isIdle);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetIsIdle(const boost::shared_ptr<Entity>& self, bool& idle)
{
    ModHookCall hook(kModHook_Entity_setIsIdle);
    hook.Add(self);
    hook.Add(idle);
    const bool proceed = hook.Run();
    idle = hook.GetBool(1, idle);
    return proceed;
}

bool EntityIsSprinting(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isSprinting);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetSprinting(const boost::shared_ptr<Entity>& self, bool& sprinting)
{
    ModHookCall hook(kModHook_Entity_setSprinting);
    hook.Add(self);
    hook.Add(sprinting);
    const bool proceed = hook.Run();
    sprinting = hook.GetBool(1, sprinting);
    return proceed;
}

bool EntityIsSwimming(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isSwimming);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsSwimmer(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isSwimmer);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityStartSwimming(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_startSwimming);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityStopSwimming(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_stopSwimming);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntitySetSwimmer(const boost::shared_ptr<Entity>& self, bool& swimmer)
{
    ModHookCall hook(kModHook_Entity_setSwimmer);
    hook.Add(self);
    hook.Add(swimmer);
    const bool proceed = hook.Run();
    swimmer = hook.GetBool(1, swimmer);
    return proceed;
}

bool EntitySetDamageNearbyMobs(const boost::shared_ptr<Entity>& self, bool& damageNearbyMobs)
{
    ModHookCall hook(kModHook_Entity_setDamageNearbyMobs);
    hook.Add(self);
    hook.Add(damageNearbyMobs);
    const bool proceed = hook.Run();
    damageNearbyMobs = hook.GetBool(1, damageNearbyMobs);
    return proceed;
}

bool EntityGetDamageNearbyMobs(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_getDamageNearbyMobs);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsGlowing(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isGlowing);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetGlowing(const boost::shared_ptr<Entity>& self, bool& glowing)
{
    ModHookCall hook(kModHook_Entity_setGlowing);
    hook.Add(self);
    hook.Add(glowing);
    const bool proceed = hook.Run();
    glowing = hook.GetBool(1, glowing);
    return proceed;
}

bool EntityIsInvisible(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInvisible);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsInvisibleTo(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInvisibleTo);
    hook.Add(self);
    hook.Add(player);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityGetTeam(const boost::shared_ptr<Entity>& self, Team*& result)
{
    ModHookCall hook(kModHook_Entity_getTeam);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Team* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityIsAlliedTo(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Entity_isAlliedTo);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsAlliedTo2(const boost::shared_ptr<Entity>& self, Team* team, bool& result)
{
    ModHookCall hook(kModHook_Entity_isAlliedTo_2);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Team, ModHandles::Add(team));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntitySetInvisible(const boost::shared_ptr<Entity>& self, bool& invisible)
{
    ModHookCall hook(kModHook_Entity_setInvisible);
    hook.Add(self);
    hook.Add(invisible);
    const bool proceed = hook.Run();
    invisible = hook.GetBool(1, invisible);
    return proceed;
}

bool EntityShouldRenderShadow(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_ShouldRenderShadow);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityCanFly(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_canFly);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetAttachPos(const boost::shared_ptr<Entity>& self, int location, float& partialTicks, Vec3*& result)
{
    ModHookCall hook(kModHook_Entity_getAttachPos);
    hook.Add(self);
    hook.Add(location);
    hook.Add(partialTicks);
    hook.AddNil();
    const bool proceed = hook.Run();
    partialTicks = hook.GetFloat(2, partialTicks);
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetMaxAirSupply(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getMaxAirSupply);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntitySetAirSupply(const boost::shared_ptr<Entity>& self, int& airSupply)
{
    ModHookCall hook(kModHook_Entity_setAirSupply);
    hook.Add(self);
    hook.Add(airSupply);
    const bool proceed = hook.Run();
    airSupply = hook.GetInt(1, airSupply);
    return proceed;
}

bool EntityThunderHit(const boost::shared_ptr<Entity>& self, LightningBolt* lightningBolt)
{
    ModHookCall hook(kModHook_Entity_thunderHit);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_LightningBolt, ModHandles::Add(lightningBolt));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityOnAboveBubbleCol(const boost::shared_ptr<Entity>& self, bool& down)
{
    ModHookCall hook(kModHook_Entity_onAboveBubbleCol);
    hook.Add(self);
    hook.Add(down);
    const bool proceed = hook.Run();
    down = hook.GetBool(1, down);
    return proceed;
}

bool EntityKilled(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& killed)
{
    ModHookCall hook(kModHook_Entity_killed);
    hook.Add(self);
    hook.Add(killed);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCheckInBlock(const boost::shared_ptr<Entity>& self, float x, float y, float z, bool& result)
{
    ModHookCall hook(kModHook_Entity_checkInBlock);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(4, result);
    return proceed;
}

bool EntitySpawnEatParticles(const boost::shared_ptr<Entity>& self, ItemInstance* itemInstance, int& count)
{
    ModHookCall hook(kModHook_Entity_spawnEatParticles);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(count);
    const bool proceed = hook.Run();
    count = hook.GetInt(2, count);
    return proceed;
}

bool EntityMakeStuckInWeb(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_makeStuckInWeb);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetName(const boost::shared_ptr<Entity>& self, std::wstring& result)
{
    ModHookCall hook(kModHook_Entity_getName);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool EntityIsInvulnerableTo(const boost::shared_ptr<Entity>& self, DamageSource* source, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInvulnerableTo);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsInvulnerable(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInvulnerable);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetInvulnerable(const boost::shared_ptr<Entity>& self, bool& invulnerable)
{
    ModHookCall hook(kModHook_Entity_setInvulnerable);
    hook.Add(self);
    hook.Add(invulnerable);
    const bool proceed = hook.Run();
    invulnerable = hook.GetBool(1, invulnerable);
    return proceed;
}

bool EntityIs(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Entity_is);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityGetYHeadRot(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getYHeadRot);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntitySetYHeadRot(const boost::shared_ptr<Entity>& self, float& yHeadRot)
{
    ModHookCall hook(kModHook_Entity_setYHeadRot);
    hook.Add(self);
    hook.Add(yHeadRot);
    const bool proceed = hook.Run();
    yHeadRot = hook.GetFloat(1, yHeadRot);
    return proceed;
}

bool EntitySetYBodyRot(const boost::shared_ptr<Entity>& self, float& yBodyRot)
{
    ModHookCall hook(kModHook_Entity_setYBodyRot);
    hook.Add(self);
    hook.Add(yBodyRot);
    const bool proceed = hook.Run();
    yBodyRot = hook.GetFloat(1, yBodyRot);
    return proceed;
}

bool EntityIsAttackable(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isAttackable);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySkipAttackInteraction(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Entity_skipAttackInteraction);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityCopyPosition(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_Entity_copyPosition);
    hook.Add(self);
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCanChangeDimensions(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_canChangeDimensions);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetBlockExplosionResistance(const boost::shared_ptr<Entity>& self, Explosion* explosion, Level* level, int x, int y, int z, float& result)
{
    ModHookCall hook(kModHook_Entity_getBlockExplosionResistance);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Explosion, ModHandles::Add(explosion));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(6, result);
    return proceed;
}

bool EntityShouldBlockExplode(const boost::shared_ptr<Entity>& self, Explosion* explosion, Level* level, int x, int y, int z, float& power, bool& result)
{
    ModHookCall hook(kModHook_Entity_shouldBlockExplode);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Explosion, ModHandles::Add(explosion));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(power);
    hook.Add(result);
    const bool proceed = hook.Run();
    power = hook.GetFloat(6, power);
    result = hook.GetBool(7, result);
    return proceed;
}

bool EntityGetMaxFallDistance(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getMaxFallDistance);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityGetPortalEntranceOffset(const boost::shared_ptr<Entity>& self, Vec3*& result)
{
    ModHookCall hook(kModHook_Entity_getPortalEntranceOffset);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetPortalEntranceForwards(const boost::shared_ptr<Entity>& self, Direction*& result)
{
    ModHookCall hook(kModHook_Entity_getPortalEntranceForwards);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Direction* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityIsIgnoringBlockTriggers(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isIgnoringBlockTriggers);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityDisplayFireAnimation(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_displayFireAnimation);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetUUID(const boost::shared_ptr<Entity>& self, const std::wstring& uuid)
{
    ModHookCall hook(kModHook_Entity_setUUID);
    hook.Add(self);
    hook.Add(uuid);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetUUID(const boost::shared_ptr<Entity>& self, std::wstring& result)
{
    ModHookCall hook(kModHook_Entity_getUUID);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool EntityGetStringUUID(const boost::shared_ptr<Entity>& self, std::wstring& result)
{
    ModHookCall hook(kModHook_Entity_getStringUUID);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool EntityIsPushedByWater(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isPushedByWater);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetViewScale(const boost::shared_ptr<Entity>& self, float viewScale)
{
    ModHookCall hook(kModHook_Entity_setViewScale);
    hook.Add(self);
    hook.Add(viewScale);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetDisplayName(const boost::shared_ptr<Entity>& self, std::wstring& result)
{
    ModHookCall hook(kModHook_Entity_getDisplayName);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool EntityGetNetworkName(const boost::shared_ptr<Entity>& self, std::wstring& result)
{
    ModHookCall hook(kModHook_Entity_getNetworkName);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool EntitySetCustomName(const boost::shared_ptr<Entity>& self, const std::wstring& customName, const std::wstring& ownerUUID, bool& namedByRestrictedPlayer)
{
    ModHookCall hook(kModHook_Entity_setCustomName);
    hook.Add(self);
    hook.Add(customName);
    hook.Add(ownerUUID);
    hook.Add(namedByRestrictedPlayer);
    const bool proceed = hook.Run();
    namedByRestrictedPlayer = hook.GetBool(3, namedByRestrictedPlayer);
    return proceed;
}

bool EntityGetCustomName(const boost::shared_ptr<Entity>& self, std::wstring& result)
{
    ModHookCall hook(kModHook_Entity_getCustomName);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool EntityHasCustomName(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasCustomName);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetCustomNameVisible(const boost::shared_ptr<Entity>& self, bool& customNameVisible)
{
    ModHookCall hook(kModHook_Entity_setCustomNameVisible);
    hook.Add(self);
    hook.Add(customNameVisible);
    const bool proceed = hook.Run();
    customNameVisible = hook.GetBool(1, customNameVisible);
    return proceed;
}

bool EntityIsCustomNameVisible(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isCustomNameVisible);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityTeleportTo(const boost::shared_ptr<Entity>& self, float x, float y, float z)
{
    ModHookCall hook(kModHook_Entity_teleportTo);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityShouldShowName(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_shouldShowName);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityOnSyncedDataUpdated(const boost::shared_ptr<Entity>& self, EntityDataAccessor_Base* accessor)
{
    ModHookCall hook(kModHook_Entity_onSyncedDataUpdated);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_EntityDataAccessor_Base, ModHandles::Add(accessor));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetDirection(const boost::shared_ptr<Entity>& self, Direction*& result)
{
    ModHookCall hook(kModHook_Entity_getDirection);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Direction* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetMotionDirection(const boost::shared_ptr<Entity>& self, Direction*& result)
{
    ModHookCall hook(kModHook_Entity_getMotionDirection);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Direction* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityBroadcastToPlayer(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, bool& result)
{
    ModHookCall hook(kModHook_Entity_broadcastToPlayer);
    hook.Add(self);
    hook.Add(player);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityGetBoundingBoxForCulling(const boost::shared_ptr<Entity>& self, AABB*& result)
{
    ModHookCall hook(kModHook_Entity_getBoundingBoxForCulling);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntitySetBoundingBox(const boost::shared_ptr<Entity>& self, AABB* boundingBox)
{
    ModHookCall hook(kModHook_Entity_setBoundingBox);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_AABB, ModHandles::Add(boundingBox));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetEyeHeight(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getEyeHeight);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntitySetSlot(const boost::shared_ptr<Entity>& self, int& slot, ItemInstance* itemInstance, bool& result)
{
    ModHookCall hook(kModHook_Entity_setSlot);
    hook.Add(self);
    hook.Add(slot);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    slot = hook.GetInt(1, slot);
    result = hook.GetBool(3, result);
    return proceed;
}

bool EntityHasPermission(const boost::shared_ptr<Entity>& self, int command, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasPermission);
    hook.Add(self);
    hook.Add(command);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityGetCommandSenderWorldPosition(const boost::shared_ptr<Entity>& self, Vec3*& result)
{
    ModHookCall hook(kModHook_Entity_getCommandSenderWorldPosition);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetCommandSenderWorld(const boost::shared_ptr<Entity>& self, Level*& result)
{
    ModHookCall hook(kModHook_Entity_getCommandSenderWorld);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Level* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityIsSendMessageEnabled(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isSendMessageEnabled);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetCommandStats(const boost::shared_ptr<Entity>& self, int statType, int& value)
{
    ModHookCall hook(kModHook_Entity_setCommandStats);
    hook.Add(self);
    hook.Add(statType);
    hook.Add(value);
    const bool proceed = hook.Run();
    value = hook.GetInt(2, value);
    return proceed;
}

bool EntityGetServer(const boost::shared_ptr<Entity>& self, MinecraftServer*& result)
{
    ModHookCall hook(kModHook_Entity_getServer);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        MinecraftServer* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityInteractAt(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, Vec3* pos, int hand, int& result)
{
    ModHookCall hook(kModHook_Entity_interactAt);
    hook.Add(self);
    hook.Add(player);
    hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(pos));
    hook.Add(hand);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(4, result);
    return proceed;
}

bool EntityIgnoreExplosion(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_ignoreExplosion);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityStartSeenByPlayer(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_Entity_startSeenByPlayer);
    hook.Add(self);
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityStopSeenByPlayer(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_Entity_stopSeenByPlayer);
    hook.Add(self);
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityRotate(const boost::shared_ptr<Entity>& self, Rotation* rotation, float& result)
{
    ModHookCall hook(kModHook_Entity_rotate);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Rotation, ModHandles::Add(rotation));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(2, result);
    return proceed;
}

bool EntityMirror(const boost::shared_ptr<Entity>& self, Mirror* mirror, float& result)
{
    ModHookCall hook(kModHook_Entity_mirror);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Mirror, ModHandles::Add(mirror));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(2, result);
    return proceed;
}

bool EntityOnlyOpCanSetNbt(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_onlyOpCanSetNbt);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityCheckAndResetTeleportedFlag(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_checkAndResetTeleportedFlag);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityHasPassenger(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& passenger, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasPassenger);
    hook.Add(self);
    hook.Add(passenger);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityHasPassenger2(const boost::shared_ptr<Entity>& self, int type, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasPassenger_2);
    hook.Add(self);
    hook.Add(type);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsPassengerOfSameVehicle(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Entity_isPassengerOfSameVehicle);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityHasIndirectPassenger(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& passenger, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasIndirectPassenger);
    hook.Add(self);
    hook.Add(passenger);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityHasIndirectPassenger2(const boost::shared_ptr<Entity>& self, int type, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasIndirectPassenger_2);
    hook.Add(self);
    hook.Add(type);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsControlledByLocalInstance(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isControlledByLocalInstance);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetPistonPushReaction(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getPistonPushReaction);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityGetSoundSource(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getSoundSource);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityIsCreative(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isCreative);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsDespawnProtected(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isDespawnProtected);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetDespawnProtected(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_setDespawnProtected);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCouldWander(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_couldWander);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityCanCreateParticles(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_canCreateParticles);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityStopCurrentLerp(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_stopCurrentLerp);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityDoAutoAttackOnTouch(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_Entity_DoAutoAttackOnTouch);
    hook.Add(self);
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityTickLeash(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_tickLeash);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityDropLeash(const boost::shared_ptr<Entity>& self, bool& broadcast, bool& dropItem)
{
    ModHookCall hook(kModHook_Entity_dropLeash);
    hook.Add(self);
    hook.Add(broadcast);
    hook.Add(dropItem);
    const bool proceed = hook.Run();
    broadcast = hook.GetBool(1, broadcast);
    dropItem = hook.GetBool(2, dropItem);
    return proceed;
}

bool EntityCanBeLeashed(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, bool& result)
{
    ModHookCall hook(kModHook_Entity_canBeLeashed);
    hook.Add(self);
    hook.Add(player);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsLeashed(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isLeashed);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsLeashedToPlayer(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, bool& result)
{
    ModHookCall hook(kModHook_Entity_isLeashedToPlayer);
    hook.Add(self);
    hook.Add(player);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntitySetLeashedTo(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& leashHolder, bool& broadcast)
{
    ModHookCall hook(kModHook_Entity_setLeashedTo);
    hook.Add(self);
    hook.Add(leashHolder);
    hook.Add(broadcast);
    const bool proceed = hook.Run();
    broadcast = hook.GetBool(2, broadcast);
    return proceed;
}

bool EntityRestoreLeashFromSave(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_restoreLeashFromSave);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityDefineSynchedData(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_defineSynchedData);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityReadAdditionalSaveData(const boost::shared_ptr<Entity>& self, CompoundTag* tag)
{
    ModHookCall hook(kModHook_Entity_readAdditionalSaveData);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_CompoundTag, ModHandles::Add(tag));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityAddAdditonalSaveData(const boost::shared_ptr<Entity>& self, CompoundTag* tag)
{
    ModHookCall hook(kModHook_Entity_addAdditonalSaveData);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_CompoundTag, ModHandles::Add(tag));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityRegisterAttributes(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_registerAttributes);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityIsWaterMob(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isWaterMob);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityBreathesWater(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_breathesWater);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityBreathesAir(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_breathesAir);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityBreathesLava(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_breathesLava);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityBreathesSolids(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_breathesSolids);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetSuffocateTime(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_GetSuffocateTime);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityGetInhaleTime(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_GetInhaleTime);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityGeneratesBubbles(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_generatesBubbles);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityOnChangedBlock(const boost::shared_ptr<Entity>& self, int x, int y, int z)
{
    ModHookCall hook(kModHook_Entity_onChangedBlock);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityIsBaby(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isBaby);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityTickDeath(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_tickDeath);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityShouldDropExperience(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_shouldDropExperience);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityDecreaseAirSupply(const boost::shared_ptr<Entity>& self, int& airSupply, int& result)
{
    ModHookCall hook(kModHook_Entity_decreaseAirSupply);
    hook.Add(self);
    hook.Add(airSupply);
    hook.Add(result);
    const bool proceed = hook.Run();
    airSupply = hook.GetInt(1, airSupply);
    result = hook.GetInt(2, result);
    return proceed;
}

bool EntityGetExperienceReward(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, int& result)
{
    ModHookCall hook(kModHook_Entity_getExperienceReward);
    hook.Add(self);
    hook.Add(player);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool EntityIsAlwaysExperienceDropper(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isAlwaysExperienceDropper);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetRandom(const boost::shared_ptr<Entity>& self, Random*& result)
{
    ModHookCall hook(kModHook_Entity_getRandom);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Random* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetLastHurtByMobTimestamp(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getLastHurtByMobTimestamp);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntitySetLastHurtByMob(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& attacker)
{
    ModHookCall hook(kModHook_Entity_setLastHurtByMob);
    hook.Add(self);
    hook.Add(attacker);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetLastHurtMobTimestamp(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getLastHurtMobTimestamp);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntitySetLastHurtMob(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& target)
{
    ModHookCall hook(kModHook_Entity_setLastHurtMob);
    hook.Add(self);
    hook.Add(target);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetNoActionTime(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getNoActionTime);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityTickEffects(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_tickEffects);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityUpdateInvisibilityStatus(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_updateInvisibilityStatus);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityRemoveEffectParticles(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_removeEffectParticles);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityRemoveAllEffects(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_removeAllEffects);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityHasEffect(const boost::shared_ptr<Entity>& self, MobEffect* effect, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasEffect);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffect, ModHandles::Add(effect));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityGetEffect(const boost::shared_ptr<Entity>& self, MobEffect* effect, MobEffectInstance*& result)
{
    ModHookCall hook(kModHook_Entity_getEffect);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffect, ModHandles::Add(effect));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        MobEffectInstance* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityAddEffect(const boost::shared_ptr<Entity>& self, MobEffectInstance* effectInstance, const boost::shared_ptr<Entity>& source)
{
    ModHookCall hook(kModHook_Entity_addEffect);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffectInstance, ModHandles::Add(effectInstance));
    hook.Add(source);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityAddEffectNoUpdate(const boost::shared_ptr<Entity>& self, MobEffectInstance* effectInstance)
{
    ModHookCall hook(kModHook_Entity_addEffectNoUpdate);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffectInstance, ModHandles::Add(effectInstance));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCanBeAffected(const boost::shared_ptr<Entity>& self, MobEffectInstance* effectInstance, bool& result)
{
    ModHookCall hook(kModHook_Entity_canBeAffected);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffectInstance, ModHandles::Add(effectInstance));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsInvertedHealAndHarm(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isInvertedHealAndHarm);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityRemoveEffectNoUpdate(const boost::shared_ptr<Entity>& self, MobEffect* effect, MobEffectInstance*& result)
{
    ModHookCall hook(kModHook_Entity_removeEffectNoUpdate);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffect, ModHandles::Add(effect));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        MobEffectInstance* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityRemoveEffect(const boost::shared_ptr<Entity>& self, MobEffect* effect)
{
    ModHookCall hook(kModHook_Entity_removeEffect);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffect, ModHandles::Add(effect));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityOnEffectAdded(const boost::shared_ptr<Entity>& self, MobEffectInstance* effectInstance)
{
    ModHookCall hook(kModHook_Entity_onEffectAdded);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffectInstance, ModHandles::Add(effectInstance));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityOnEffectUpdated(const boost::shared_ptr<Entity>& self, MobEffectInstance* effectInstance, bool& reapply)
{
    ModHookCall hook(kModHook_Entity_onEffectUpdated);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffectInstance, ModHandles::Add(effectInstance));
    hook.Add(reapply);
    const bool proceed = hook.Run();
    reapply = hook.GetBool(2, reapply);
    return proceed;
}

bool EntityOnEffectRemoved(const boost::shared_ptr<Entity>& self, MobEffectInstance* effectInstance)
{
    ModHookCall hook(kModHook_Entity_onEffectRemoved);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_MobEffectInstance, ModHandles::Add(effectInstance));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityHeal(const boost::shared_ptr<Entity>& self, float& amount)
{
    ModHookCall hook(kModHook_Entity_heal);
    hook.Add(self);
    hook.Add(amount);
    const bool proceed = hook.Run();
    amount = hook.GetFloat(1, amount);
    return proceed;
}

bool EntityGetHealth(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getHealth);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntitySetHealth(const boost::shared_ptr<Entity>& self, float& health)
{
    ModHookCall hook(kModHook_Entity_setHealth);
    hook.Add(self);
    hook.Add(health);
    const bool proceed = hook.Run();
    health = hook.GetFloat(1, health);
    return proceed;
}

bool EntityGetLastDamageSource(const boost::shared_ptr<Entity>& self, DamageSource*& result)
{
    ModHookCall hook(kModHook_Entity_getLastDamageSource);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        DamageSource* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityBlockUsingShield(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& attacker)
{
    ModHookCall hook(kModHook_Entity_blockUsingShield);
    hook.Add(self);
    hook.Add(attacker);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPlayHurtSound(const boost::shared_ptr<Entity>& self, DamageSource* source)
{
    ModHookCall hook(kModHook_Entity_playHurtSound);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCheckTotemDeathProtection(const boost::shared_ptr<Entity>& self, DamageSource* source, bool& result)
{
    ModHookCall hook(kModHook_Entity_checkTotemDeathProtection);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsDamageSourceBlocked(const boost::shared_ptr<Entity>& self, DamageSource* source, bool& result)
{
    ModHookCall hook(kModHook_Entity_isDamageSourceBlocked);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityBreakItem(const boost::shared_ptr<Entity>& self, ItemInstance* itemInstance)
{
    ModHookCall hook(kModHook_Entity_breakItem);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityDie(const boost::shared_ptr<Entity>& self, DamageSource* source)
{
    ModHookCall hook(kModHook_Entity_die);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityDropEquipment(const boost::shared_ptr<Entity>& self, bool& hitByPlayer, int& lootingLevel)
{
    ModHookCall hook(kModHook_Entity_dropEquipment);
    hook.Add(self);
    hook.Add(hitByPlayer);
    hook.Add(lootingLevel);
    const bool proceed = hook.Run();
    hitByPlayer = hook.GetBool(1, hitByPlayer);
    lootingLevel = hook.GetInt(2, lootingLevel);
    return proceed;
}

bool EntityKnockback(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& attacker, float& strength, float x, float z)
{
    ModHookCall hook(kModHook_Entity_knockback);
    hook.Add(self);
    hook.Add(attacker);
    hook.Add(strength);
    hook.Add(x);
    hook.Add(z);
    const bool proceed = hook.Run();
    strength = hook.GetFloat(2, strength);
    return proceed;
}

bool EntityGetHurtSound(const boost::shared_ptr<Entity>& self, DamageSource* source, SoundEvent*& result)
{
    ModHookCall hook(kModHook_Entity_getHurtSound);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        SoundEvent* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetDeathSound(const boost::shared_ptr<Entity>& self, SoundEvent*& result)
{
    ModHookCall hook(kModHook_Entity_getDeathSound);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        SoundEvent* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetFallDamageSound(const boost::shared_ptr<Entity>& self, int& distance, SoundEvent*& result)
{
    ModHookCall hook(kModHook_Entity_getFallDamageSound);
    hook.Add(self);
    hook.Add(distance);
    hook.AddNil();
    const bool proceed = hook.Run();
    distance = hook.GetInt(1, distance);
    {
        uint32_t id = 0;
        SoundEvent* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityDropAllDeathLoot(const boost::shared_ptr<Entity>& self, bool& hitByPlayer, int& lootingLevel, DamageSource* source)
{
    ModHookCall hook(kModHook_Entity_dropAllDeathLoot);
    hook.Add(self);
    hook.Add(hitByPlayer);
    hook.Add(lootingLevel);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    const bool proceed = hook.Run();
    hitByPlayer = hook.GetBool(1, hitByPlayer);
    lootingLevel = hook.GetInt(2, lootingLevel);
    return proceed;
}

bool EntityDropDeathLoot(const boost::shared_ptr<Entity>& self, bool& hitByPlayer, int& lootingLevel)
{
    ModHookCall hook(kModHook_Entity_dropDeathLoot);
    hook.Add(self);
    hook.Add(hitByPlayer);
    hook.Add(lootingLevel);
    const bool proceed = hook.Run();
    hitByPlayer = hook.GetBool(1, hitByPlayer);
    lootingLevel = hook.GetInt(2, lootingLevel);
    return proceed;
}

bool EntityOnLadder(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_onLadder);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsShootable(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isShootable);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetArmorValue(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getArmorValue);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityHurtArmor(const boost::shared_ptr<Entity>& self, float& damage)
{
    ModHookCall hook(kModHook_Entity_hurtArmor);
    hook.Add(self);
    hook.Add(damage);
    const bool proceed = hook.Run();
    damage = hook.GetFloat(1, damage);
    return proceed;
}

bool EntityHurtCurrentlyUsedShield(const boost::shared_ptr<Entity>& self, float& damage)
{
    ModHookCall hook(kModHook_Entity_hurtCurrentlyUsedShield);
    hook.Add(self);
    hook.Add(damage);
    const bool proceed = hook.Run();
    damage = hook.GetFloat(1, damage);
    return proceed;
}

bool EntityGetDamageAfterArmorAbsorb(const boost::shared_ptr<Entity>& self, DamageSource* source, float& damage, float& result)
{
    ModHookCall hook(kModHook_Entity_getDamageAfterArmorAbsorb);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(damage);
    hook.Add(result);
    const bool proceed = hook.Run();
    damage = hook.GetFloat(2, damage);
    result = hook.GetFloat(3, result);
    return proceed;
}

bool EntityGetDamageAfterMagicAbsorb(const boost::shared_ptr<Entity>& self, DamageSource* source, float& damage, float& result)
{
    ModHookCall hook(kModHook_Entity_getDamageAfterMagicAbsorb);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(damage);
    hook.Add(result);
    const bool proceed = hook.Run();
    damage = hook.GetFloat(2, damage);
    result = hook.GetFloat(3, result);
    return proceed;
}

bool EntityActuallyHurt(const boost::shared_ptr<Entity>& self, DamageSource* source, float& damage)
{
    ModHookCall hook(kModHook_Entity_actuallyHurt);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(damage);
    const bool proceed = hook.Run();
    damage = hook.GetFloat(2, damage);
    return proceed;
}

bool EntityGetCombatTracker(const boost::shared_ptr<Entity>& self, CombatTracker*& result)
{
    ModHookCall hook(kModHook_Entity_getCombatTracker);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        CombatTracker* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetMaxHealth(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getMaxHealth);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityGetArrowCount(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getArrowCount);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntitySetArrowCount(const boost::shared_ptr<Entity>& self, int& arrowCount)
{
    ModHookCall hook(kModHook_Entity_setArrowCount);
    hook.Add(self);
    hook.Add(arrowCount);
    const bool proceed = hook.Run();
    arrowCount = hook.GetInt(1, arrowCount);
    return proceed;
}

bool EntitySwing(const boost::shared_ptr<Entity>& self, int hand, bool& result)
{
    ModHookCall hook(kModHook_Entity_swing);
    hook.Add(self);
    hook.Add(hand);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityUpdateSwingTime(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_updateSwingTime);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetAttribute(const boost::shared_ptr<Entity>& self, Attribute* attribute, AttributeInstance*& result)
{
    ModHookCall hook(kModHook_Entity_getAttribute);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_Attribute, ModHandles::Add(attribute));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AttributeInstance* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetAttributes(const boost::shared_ptr<Entity>& self, BaseAttributeMap*& result)
{
    ModHookCall hook(kModHook_Entity_getAttributes);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        BaseAttributeMap* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetMobType(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getMobType);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntitySetItemInHand(const boost::shared_ptr<Entity>& self, int hand, ItemInstance* itemInstance)
{
    ModHookCall hook(kModHook_Entity_setItemInHand);
    hook.Add(self);
    hook.Add(hand);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityHasItemInSlot(const boost::shared_ptr<Entity>& self, EquipmentSlot* slot, bool& result)
{
    ModHookCall hook(kModHook_Entity_hasItemInSlot);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_EquipmentSlot, ModHandles::Add(slot));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityGetItemInHandIcon(const boost::shared_ptr<Entity>& self, ItemInstance* itemInstance, int& frame, TextureAtlasSprite*& result)
{
    ModHookCall hook(kModHook_Entity_getItemInHandIcon);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(frame);
    hook.AddNil();
    const bool proceed = hook.Run();
    frame = hook.GetInt(2, frame);
    {
        uint32_t id = 0;
        TextureAtlasSprite* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetSoundVolume(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getSoundVolume);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityGetVoicePitch(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getVoicePitch);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityIsImmobile(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isImmobile);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityFindStandUpPosition(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& vehicle)
{
    ModHookCall hook(kModHook_Entity_findStandUpPosition);
    hook.Add(self);
    hook.Add(vehicle);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetJumpPower(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getJumpPower);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityJumpFromGround(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_jumpFromGround);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityJumpInWater(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_jumpInWater);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGoDownInWater(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_goDownInWater);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityJumpInLava(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_jumpInLava);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetWaterSlowDown(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getWaterSlowDown);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityTravel(const boost::shared_ptr<Entity>& self, float& xxa, float& yya, float& zza)
{
    ModHookCall hook(kModHook_Entity_travel);
    hook.Add(self);
    hook.Add(xxa);
    hook.Add(yya);
    hook.Add(zza);
    const bool proceed = hook.Run();
    xxa = hook.GetFloat(1, xxa);
    yya = hook.GetFloat(2, yya);
    zza = hook.GetFloat(3, zza);
    return proceed;
}

bool EntityUpdateModelAnimation(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_updateModelAnimation);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetSpeed(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getSpeed);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntitySetSpeed(const boost::shared_ptr<Entity>& self, float& speed)
{
    ModHookCall hook(kModHook_Entity_setSpeed);
    hook.Add(self);
    hook.Add(speed);
    const bool proceed = hook.Run();
    speed = hook.GetFloat(1, speed);
    return proceed;
}

bool EntityDoHurtTarget(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& target, bool& result)
{
    ModHookCall hook(kModHook_Entity_doHurtTarget);
    hook.Add(self);
    hook.Add(target);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityIsSleeping(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isSleeping);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityTickHeadTurn(const boost::shared_ptr<Entity>& self, float& yBodyRot, float& headTurnDelta, float& result)
{
    ModHookCall hook(kModHook_Entity_tickHeadTurn);
    hook.Add(self);
    hook.Add(yBodyRot);
    hook.Add(headTurnDelta);
    hook.Add(result);
    const bool proceed = hook.Run();
    yBodyRot = hook.GetFloat(1, yBodyRot);
    headTurnDelta = hook.GetFloat(2, headTurnDelta);
    result = hook.GetFloat(3, result);
    return proceed;
}

bool EntityAiStep(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_aiStep);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityServerAiStep(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_serverAiStep);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPushEntities(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_pushEntities);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityDoPush(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_Entity_doPush);
    hook.Add(self);
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntitySetJumping(const boost::shared_ptr<Entity>& self, bool& jumping)
{
    ModHookCall hook(kModHook_Entity_setJumping);
    hook.Add(self);
    hook.Add(jumping);
    const bool proceed = hook.Run();
    jumping = hook.GetBool(1, jumping);
    return proceed;
}

bool EntityTake(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, int& count)
{
    ModHookCall hook(kModHook_Entity_take);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(count);
    const bool proceed = hook.Run();
    count = hook.GetInt(2, count);
    return proceed;
}

bool EntityCanSee(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Entity_canSee);
    hook.Add(self);
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityGetAttackAnim(const boost::shared_ptr<Entity>& self, float& partialTicks, float& result)
{
    ModHookCall hook(kModHook_Entity_getAttackAnim);
    hook.Add(self);
    hook.Add(partialTicks);
    hook.Add(result);
    const bool proceed = hook.Run();
    partialTicks = hook.GetFloat(1, partialTicks);
    result = hook.GetFloat(2, result);
    return proceed;
}

bool EntityGetSweptVolume(const boost::shared_ptr<Entity>& self, AABB*& result)
{
    ModHookCall hook(kModHook_Entity_getSweptVolume);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityIsEffectiveAi(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isEffectiveAi);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetAbsorptionAmount(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getAbsorptionAmount);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntitySetAbsorptionAmount(const boost::shared_ptr<Entity>& self, float& absorptionAmount)
{
    ModHookCall hook(kModHook_Entity_setAbsorptionAmount);
    hook.Add(self);
    hook.Add(absorptionAmount);
    const bool proceed = hook.Run();
    absorptionAmount = hook.GetFloat(1, absorptionAmount);
    return proceed;
}

bool EntityOnEnterCombat(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_onEnterCombat);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityOnLeaveCombat(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_onLeaveCombat);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityUpdateEffectVisibility(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_updateEffectVisibility);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityIsUsingItem(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isUsingItem);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetUsedItemHand(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getUsedItemHand);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityUpdatingUsingItem(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_updatingUsingItem);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityStartUsingItem(const boost::shared_ptr<Entity>& self, int hand)
{
    ModHookCall hook(kModHook_Entity_startUsingItem);
    hook.Add(self);
    hook.Add(hand);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntitySpawnItemUseParticles(const boost::shared_ptr<Entity>& self, ItemInstance* itemInstance, int& count)
{
    ModHookCall hook(kModHook_Entity_spawnItemUseParticles);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(itemInstance));
    hook.Add(count);
    const bool proceed = hook.Run();
    count = hook.GetInt(2, count);
    return proceed;
}

bool EntityCompleteUsingItem(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_completeUsingItem);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetTicksUsingItem(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getTicksUsingItem);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityReleaseUsingItem(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_releaseUsingItem);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityReleaseUsingItem2(const boost::shared_ptr<Entity>& self, int& useDuration)
{
    ModHookCall hook(kModHook_Entity_releaseUsingItem_2);
    hook.Add(self);
    hook.Add(useDuration);
    const bool proceed = hook.Run();
    useDuration = hook.GetInt(1, useDuration);
    return proceed;
}

bool EntityStopUsingItem(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_stopUsingItem);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityIsBlocking(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isBlocking);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityIsFallFlying(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isFallFlying);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetFallFlyingTicks(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getFallFlyingTicks);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityIsAffectedByPotions(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isAffectedByPotions);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityAttackable(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_attackable);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetRecordPlayingNearby(const boost::shared_ptr<Entity>& self, int x, int y, int z, bool& playing)
{
    ModHookCall hook(kModHook_Entity_setRecordPlayingNearby);
    hook.Add(self);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(playing);
    const bool proceed = hook.Run();
    playing = hook.GetBool(4, playing);
    return proceed;
}

bool EntityIsCreativeFlying(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_IsCreativeFlying);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetMainArm(const boost::shared_ptr<Entity>& self, HumanoidArm*& result)
{
    ModHookCall hook(kModHook_Entity_getMainArm);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        HumanoidArm* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityRegisterGoals(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_registerGoals);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCreateNavigation(const boost::shared_ptr<Entity>& self, Level* level, PathNavigation*& result)
{
    ModHookCall hook(kModHook_Entity_createNavigation);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        PathNavigation* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetPathfindingMalus(const boost::shared_ptr<Entity>& self, BlockPathTypes* type, float& result)
{
    ModHookCall hook(kModHook_Entity_getPathfindingMalus);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_BlockPathTypes, ModHandles::Add(type));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(2, result);
    return proceed;
}

bool EntitySetPathfindingMalus(const boost::shared_ptr<Entity>& self, BlockPathTypes* type, float& malus)
{
    ModHookCall hook(kModHook_Entity_setPathfindingMalus);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_BlockPathTypes, ModHandles::Add(type));
    hook.Add(malus);
    const bool proceed = hook.Run();
    malus = hook.GetFloat(2, malus);
    return proceed;
}

bool EntityCreateBodyControl(const boost::shared_ptr<Entity>& self, BodyControl*& result)
{
    ModHookCall hook(kModHook_Entity_createBodyControl);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        BodyControl* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetLookControl(const boost::shared_ptr<Entity>& self, LookControl*& result)
{
    ModHookCall hook(kModHook_Entity_getLookControl);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        LookControl* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetMoveControl(const boost::shared_ptr<Entity>& self, MoveControl*& result)
{
    ModHookCall hook(kModHook_Entity_getMoveControl);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        MoveControl* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetJumpControl(const boost::shared_ptr<Entity>& self, JumpControl*& result)
{
    ModHookCall hook(kModHook_Entity_getJumpControl);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        JumpControl* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetNavigation(const boost::shared_ptr<Entity>& self, PathNavigation*& result)
{
    ModHookCall hook(kModHook_Entity_getNavigation);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        PathNavigation* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetSensing(const boost::shared_ptr<Entity>& self, Sensing*& result)
{
    ModHookCall hook(kModHook_Entity_getSensing);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Sensing* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntitySetTarget(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& target)
{
    ModHookCall hook(kModHook_Entity_setTarget);
    hook.Add(self);
    hook.Add(target);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCanAttackType(const boost::shared_ptr<Entity>& self, int type, bool& result)
{
    ModHookCall hook(kModHook_Entity_canAttackType);
    hook.Add(self);
    hook.Add(type);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityAte(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_ate);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetAmbientSoundInterval(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getAmbientSoundInterval);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityPlayAmbientSound(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_playAmbientSound);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityResetAmbientSoundTime(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_resetAmbientSoundTime);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntitySpawnAnim(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_spawnAnim);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetAmbientSound(const boost::shared_ptr<Entity>& self, SoundEvent*& result)
{
    ModHookCall hook(kModHook_Entity_getAmbientSound);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        SoundEvent* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetDeathLoot(const boost::shared_ptr<Entity>& self, std::wstring& result)
{
    ModHookCall hook(kModHook_Entity_getDeathLoot);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool EntityGetDefaultLootTable(const boost::shared_ptr<Entity>& self, ResourceLocation*& result)
{
    ModHookCall hook(kModHook_Entity_getDefaultLootTable);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        ResourceLocation* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityGetDefaultGearTable(const boost::shared_ptr<Entity>& self, ResourceLocation*& result)
{
    ModHookCall hook(kModHook_Entity_getDefaultGearTable);
    hook.Add(self);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        ResourceLocation* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntitySetZza(const boost::shared_ptr<Entity>& self, float& zza)
{
    ModHookCall hook(kModHook_Entity_setZza);
    hook.Add(self);
    hook.Add(zza);
    const bool proceed = hook.Run();
    zza = hook.GetFloat(1, zza);
    return proceed;
}

bool EntitySetYya(const boost::shared_ptr<Entity>& self, float& yya)
{
    ModHookCall hook(kModHook_Entity_setYya);
    hook.Add(self);
    hook.Add(yya);
    const bool proceed = hook.Run();
    yya = hook.GetFloat(1, yya);
    return proceed;
}

bool EntitySetXxa(const boost::shared_ptr<Entity>& self, float& xxa)
{
    ModHookCall hook(kModHook_Entity_setXxa);
    hook.Add(self);
    hook.Add(xxa);
    const bool proceed = hook.Run();
    xxa = hook.GetFloat(1, xxa);
    return proceed;
}

bool EntityPickUpItem(const boost::shared_ptr<Entity>& self, ItemEntity* item)
{
    ModHookCall hook(kModHook_Entity_pickUpItem);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_ItemEntity, ModHandles::Add(item));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCanHoldItem(const boost::shared_ptr<Entity>& self, ItemInstance* item, bool& result)
{
    ModHookCall hook(kModHook_Entity_canHoldItem);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool EntityRemoveWhenFarAway(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_removeWhenFarAway);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityCheckDespawn(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_checkDespawn);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityCustomServerAiStep(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_customServerAiStep);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityGetMaxHeadXRot(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getMaxHeadXRot);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityGetMaxHeadYRot(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getMaxHeadYRot);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityCheckSpawnRules(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_checkSpawnRules);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityCheckSpawnObstruction(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_checkSpawnObstruction);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityGetSizeScale(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getSizeScale);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityGetHeadSizeScale(const boost::shared_ptr<Entity>& self, float& result)
{
    ModHookCall hook(kModHook_Entity_getHeadSizeScale);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool EntityGetMaxSpawnClusterSize(const boost::shared_ptr<Entity>& self, int& result)
{
    ModHookCall hook(kModHook_Entity_getMaxSpawnClusterSize);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool EntityPopulateDefaultEquipmentSlots(const boost::shared_ptr<Entity>& self, DifficultyInstance* difficulty)
{
    ModHookCall hook(kModHook_Entity_populateDefaultEquipmentSlots);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DifficultyInstance, ModHandles::Add(difficulty));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityPopulateDefaultEquipmentEnchantments(const boost::shared_ptr<Entity>& self, DifficultyInstance* difficulty)
{
    ModHookCall hook(kModHook_Entity_populateDefaultEquipmentEnchantments);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DifficultyInstance, ModHandles::Add(difficulty));
    const bool proceed = hook.Run();
    return proceed;
}

bool EntityFinalizeMobSpawn(const boost::shared_ptr<Entity>& self, DifficultyInstance* difficulty, MobGroupData* data, const std::wstring& dataString, MobGroupData*& result)
{
    ModHookCall hook(kModHook_Entity_finalizeMobSpawn);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DifficultyInstance, ModHandles::Add(difficulty));
    hook.AddObject(mod::api::eHandle_MobGroupData, ModHandles::Add(data));
    hook.Add(dataString);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        MobGroupData* object = NULL;
        result = hook.GetHandle(4, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool EntityFinalizeSpawnEggSpawn(const boost::shared_ptr<Entity>& self, int& type)
{
    ModHookCall hook(kModHook_Entity_finalizeSpawnEggSpawn);
    hook.Add(self);
    hook.Add(type);
    const bool proceed = hook.Run();
    type = hook.GetInt(1, type);
    return proceed;
}

bool EntityCanBeControlledByRider(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_canBeControlledByRider);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetDropChance(const boost::shared_ptr<Entity>& self, EquipmentSlot* slot, float& chance)
{
    ModHookCall hook(kModHook_Entity_setDropChance);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_EquipmentSlot, ModHandles::Add(slot));
    hook.Add(chance);
    const bool proceed = hook.Run();
    chance = hook.GetFloat(2, chance);
    return proceed;
}

bool EntityCanPickUpLoot(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_canPickUpLoot);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntitySetCanPickUpLoot(const boost::shared_ptr<Entity>& self, bool& canPickUp)
{
    ModHookCall hook(kModHook_Entity_setCanPickUpLoot);
    hook.Add(self);
    hook.Add(canPickUp);
    const bool proceed = hook.Run();
    canPickUp = hook.GetBool(1, canPickUp);
    return proceed;
}

bool EntityIsPersistenceRequired(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isPersistenceRequired);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool EntityMobInteract(const boost::shared_ptr<Entity>& self, const boost::shared_ptr<Entity>& player, int hand, bool& result)
{
    ModHookCall hook(kModHook_Entity_mobInteract);
    hook.Add(self);
    hook.Add(player);
    hook.Add(hand);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(3, result);
    return proceed;
}

bool EntityUnderwaterEjectRider(const boost::shared_ptr<Entity>& self)
{
    ModHookCall hook(kModHook_Entity_underwaterEjectRider);
    hook.Add(self);
    const bool proceed = hook.Run();
    return proceed;
}

bool EntitySetNoAi(const boost::shared_ptr<Entity>& self, bool& noAi)
{
    ModHookCall hook(kModHook_Entity_setNoAi);
    hook.Add(self);
    hook.Add(noAi);
    const bool proceed = hook.Run();
    noAi = hook.GetBool(1, noAi);
    return proceed;
}

bool EntitySetLeftHanded(const boost::shared_ptr<Entity>& self, bool& leftHanded)
{
    ModHookCall hook(kModHook_Entity_setLeftHanded);
    hook.Add(self);
    hook.Add(leftHanded);
    const bool proceed = hook.Run();
    leftHanded = hook.GetBool(1, leftHanded);
    return proceed;
}

bool EntityIsNoAi(const boost::shared_ptr<Entity>& self, bool& result)
{
    ModHookCall hook(kModHook_Entity_isNoAi);
    hook.Add(self);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool DimensionInit(Dimension* self, Level* level)
{
    ModHookCall hook(kModHook_Dimension_init);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    const bool proceed = hook.Run();
    return proceed;
}

bool DimensionInit2(Dimension* self)
{
    ModHookCall hook(kModHook_Dimension_init_2);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool DimensionUpdateLightRamp(Dimension* self)
{
    ModHookCall hook(kModHook_Dimension_updateLightRamp);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool DimensionCreateRandomLevelGenerator(Dimension* self, ChunkGenerator*& result)
{
    ModHookCall hook(kModHook_Dimension_createRandomLevelGenerator);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        ChunkGenerator* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DimensionCreateFlatLevelSource(Dimension* self, ChunkGenerator*& result)
{
    ModHookCall hook(kModHook_Dimension_createFlatLevelSource);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        ChunkGenerator* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DimensionIsValidSpawn(Dimension* self, int& x, int& z, bool& result)
{
    ModHookCall hook(kModHook_Dimension_isValidSpawn);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    result = hook.GetBool(3, result);
    return proceed;
}

bool DimensionGetTimeOfDay(Dimension* self, int time, float& partialTick, float& result)
{
    ModHookCall hook(kModHook_Dimension_getTimeOfDay);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(time);
    hook.Add(partialTick);
    hook.Add(result);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(2, partialTick);
    result = hook.GetFloat(3, result);
    return proceed;
}

bool DimensionGetMoonPhase(Dimension* self, int time, int& result)
{
    ModHookCall hook(kModHook_Dimension_getMoonPhase);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(time);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(2, result);
    return proceed;
}

bool DimensionIsNaturalDimension(Dimension* self, bool& result)
{
    ModHookCall hook(kModHook_Dimension_isNaturalDimension);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool DimensionGetFogColor(Dimension* self, float& celestialAngle, float& timeOfDay, Vec3*& result)
{
    ModHookCall hook(kModHook_Dimension_getFogColor);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(celestialAngle);
    hook.Add(timeOfDay);
    hook.AddNil();
    const bool proceed = hook.Run();
    celestialAngle = hook.GetFloat(1, celestialAngle);
    timeOfDay = hook.GetFloat(2, timeOfDay);
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DimensionMayRespawn(Dimension* self, bool& result)
{
    ModHookCall hook(kModHook_Dimension_mayRespawn);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool DimensionGetCloudHeight(Dimension* self, float& result)
{
    ModHookCall hook(kModHook_Dimension_getCloudHeight);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(1, result);
    return proceed;
}

bool DimensionHasGround(Dimension* self, bool& result)
{
    ModHookCall hook(kModHook_Dimension_hasGround);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool DimensionGetSpawnPos(Dimension* self, BlockPos*& result)
{
    ModHookCall hook(kModHook_Dimension_getSpawnPos);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        BlockPos* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DimensionGetSpawnYPosition(Dimension* self, int& result)
{
    ModHookCall hook(kModHook_Dimension_getSpawnYPosition);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool DimensionIsFoggyAt(Dimension* self, int& x, int& z, bool& result)
{
    ModHookCall hook(kModHook_Dimension_isFoggyAt);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    result = hook.GetBool(3, result);
    return proceed;
}

bool DimensionCreateWorldBorder(Dimension* self, Level* level, WorldBorder*& result)
{
    ModHookCall hook(kModHook_Dimension_createWorldBorder);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        WorldBorder* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DimensionSaveData(Dimension* self)
{
    ModHookCall hook(kModHook_Dimension_saveData);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool DimensionTick(Dimension* self)
{
    ModHookCall hook(kModHook_Dimension_tick);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool DimensionCanUnloadChunk(Dimension* self, int& x, int& z, bool& result)
{
    ModHookCall hook(kModHook_Dimension_canUnloadChunk);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    result = hook.GetBool(3, result);
    return proceed;
}

bool DimensionGetXZSize(Dimension* self, int& result)
{
    ModHookCall hook(kModHook_Dimension_getXZSize);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool DimensionGetPlayerConstraints(Dimension* self, int x, int y, int z, AABB*& result)
{
    ModHookCall hook(kModHook_Dimension_getPlayerConstraints);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        AABB* object = NULL;
        result = hook.GetHandle(4, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DimensionInPlayerConstraints(Dimension* self, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_Dimension_inPlayerConstraints);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(4, result);
    return proceed;
}

bool DimensionContainsChunk(Dimension* self, int& x, int& z, bool& result)
{
    ModHookCall hook(kModHook_Dimension_containsChunk);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    result = hook.GetBool(3, result);
    return proceed;
}

bool DimensionGetBlockLightColour(Dimension* self, int& result)
{
    ModHookCall hook(kModHook_Dimension_getBlockLightColour);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

bool DimensionGetType(Dimension* self, DimensionType*& result)
{
    ModHookCall hook(kModHook_Dimension_getType);
    hook.AddObject(mod::api::eHandle_Dimension, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        DimensionType* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool UISceneReloadMovie(UIScene* self, bool& force)
{
    ModHookCall hook(kModHook_UIScene_reloadMovie);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(force);
    const bool proceed = hook.Run();
    force = hook.GetBool(1, force);
    return proceed;
}

bool UISceneNeedsReloaded(UIScene* self, bool& result)
{
    ModHookCall hook(kModHook_UIScene_needsReloaded);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool UISceneHasMovie(UIScene* self, bool& result)
{
    ModHookCall hook(kModHook_UIScene_hasMovie);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool UISceneUpdateSafeZone(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_updateSafeZone);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneUpdateViewportTouchOffset(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_updateViewportTouchOffset);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneTick(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_tick);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneSetFocusToElement(UIScene* self, int& focus)
{
    ModHookCall hook(kModHook_UIScene_SetFocusToElement);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(focus);
    const bool proceed = hook.Run();
    focus = hook.GetInt(1, focus);
    return proceed;
}

bool UISceneHandleTimerComplete(UIScene* self, int& timerId)
{
    ModHookCall hook(kModHook_UIScene_handleTimerComplete);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(timerId);
    const bool proceed = hook.Run();
    timerId = hook.GetInt(1, timerId);
    return proceed;
}

bool UISceneStealsFocus(UIScene* self, bool& result)
{
    ModHookCall hook(kModHook_UIScene_stealsFocus);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool UISceneHasFocus(UIScene* self, int& playerId, bool& result)
{
    ModHookCall hook(kModHook_UIScene_hasFocus);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(playerId);
    hook.Add(result);
    const bool proceed = hook.Run();
    playerId = hook.GetInt(1, playerId);
    result = hook.GetBool(2, result);
    return proceed;
}

bool UISceneUpdateTooltips(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_updateTooltips);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneUpdateComponents(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_updateComponents);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleGainFocus(UIScene* self, bool& hadFocus)
{
    ModHookCall hook(kModHook_UIScene_handleGainFocus);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(hadFocus);
    const bool proceed = hook.Run();
    hadFocus = hook.GetBool(1, hadFocus);
    return proceed;
}

bool UISceneHandleLoseFocus(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_handleLoseFocus);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHidesLowerScenes(UIScene* self, bool& result)
{
    ModHookCall hook(kModHook_UIScene_hidesLowerScenes);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool UISceneBlocksInput(UIScene* self, bool& result)
{
    ModHookCall hook(kModHook_UIScene_blocksInput);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool UISceneHandlesPointer(UIScene* self, bool& result)
{
    ModHookCall hook(kModHook_UIScene_handlesPointer);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool UISceneGetMainPanel(UIScene* self, UIControl*& result)
{
    ModHookCall hook(kModHook_UIScene_GetMainPanel);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        UIControl* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool UISceneRender(UIScene* self, int& width, int& height, int viewport)
{
    ModHookCall hook(kModHook_UIScene_render);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(width);
    hook.Add(height);
    hook.Add(viewport);
    const bool proceed = hook.Run();
    width = hook.GetInt(1, width);
    height = hook.GetInt(2, height);
    return proceed;
}

bool UISceneAllowRepeat(UIScene* self, int& action, bool& result)
{
    ModHookCall hook(kModHook_UIScene_allowRepeat);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(action);
    hook.Add(result);
    const bool proceed = hook.Run();
    action = hook.GetInt(1, action);
    result = hook.GetBool(2, result);
    return proceed;
}

bool UISceneHandleInput(UIScene* self, int& playerId, int& action, bool& repeat, bool& pressed, bool& released, bool& handled)
{
    ModHookCall hook(kModHook_UIScene_handleInput);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(playerId);
    hook.Add(action);
    hook.Add(repeat);
    hook.Add(pressed);
    hook.Add(released);
    hook.Add(handled);
    const bool proceed = hook.Run();
    playerId = hook.GetInt(1, playerId);
    action = hook.GetInt(2, action);
    repeat = hook.GetBool(3, repeat);
    pressed = hook.GetBool(4, pressed);
    released = hook.GetBool(5, released);
    handled = hook.GetBool(6, handled);
    return proceed;
}

bool UISceneHandleDestroy(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_handleDestroy);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandlePreUnloadForReload(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_handlePreUnloadForReload);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandlePreReload(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_handlePreReload);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleReload(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_handleReload);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandlePress(UIScene* self, int& controlId, int& childId)
{
    ModHookCall hook(kModHook_UIScene_handlePress);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(controlId);
    hook.Add(childId);
    const bool proceed = hook.Run();
    controlId = hook.GetInt(1, controlId);
    childId = hook.GetInt(2, childId);
    return proceed;
}

bool UISceneHandleFocusChange(UIScene* self, int& focus, int& childFocus)
{
    ModHookCall hook(kModHook_UIScene_handleFocusChange);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(focus);
    hook.Add(childFocus);
    const bool proceed = hook.Run();
    focus = hook.GetInt(1, focus);
    childFocus = hook.GetInt(2, childFocus);
    return proceed;
}

bool UISceneHandleInitFocus(UIScene* self, int& focus, int& childFocus)
{
    ModHookCall hook(kModHook_UIScene_handleInitFocus);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(focus);
    hook.Add(childFocus);
    const bool proceed = hook.Run();
    focus = hook.GetInt(1, focus);
    childFocus = hook.GetInt(2, childFocus);
    return proceed;
}

bool UISceneHandleCheckboxToggled(UIScene* self, int& id, bool& checked)
{
    ModHookCall hook(kModHook_UIScene_handleCheckboxToggled);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(id);
    hook.Add(checked);
    const bool proceed = hook.Run();
    id = hook.GetInt(1, id);
    checked = hook.GetBool(2, checked);
    return proceed;
}

bool UISceneHandleCheckboxElementToggled(UIScene* self, int& first, int& second, bool& checked)
{
    ModHookCall hook(kModHook_UIScene_handleCheckboxElementToggled);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(first);
    hook.Add(second);
    hook.Add(checked);
    const bool proceed = hook.Run();
    first = hook.GetInt(1, first);
    second = hook.GetInt(2, second);
    checked = hook.GetBool(3, checked);
    return proceed;
}

bool UISceneHandleSliderMove(UIScene* self, int& id, int& value)
{
    ModHookCall hook(kModHook_UIScene_handleSliderMove);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(id);
    hook.Add(value);
    const bool proceed = hook.Run();
    id = hook.GetInt(1, id);
    value = hook.GetInt(2, value);
    return proceed;
}

bool UISceneHandleSliderElementMove(UIScene* self, int& first, int& second, int& value)
{
    ModHookCall hook(kModHook_UIScene_handleSliderElementMove);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(first);
    hook.Add(second);
    hook.Add(value);
    const bool proceed = hook.Run();
    first = hook.GetInt(1, first);
    second = hook.GetInt(2, second);
    value = hook.GetInt(3, value);
    return proceed;
}

bool UISceneHandleAnimationEnd(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_handleAnimationEnd);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleSelectionChanged(UIScene* self, int& selection)
{
    ModHookCall hook(kModHook_UIScene_handleSelectionChanged);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(selection);
    const bool proceed = hook.Run();
    selection = hook.GetInt(1, selection);
    return proceed;
}

bool UISceneHandleRequestMoreData(UIScene* self, int& id, bool& forward)
{
    ModHookCall hook(kModHook_UIScene_handleRequestMoreData);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(id);
    hook.Add(forward);
    const bool proceed = hook.Run();
    id = hook.GetInt(1, id);
    forward = hook.GetBool(2, forward);
    return proceed;
}

bool UISceneHandleTouchBoxRebuild(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_handleTouchBoxRebuild);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleDLCMountingComplete(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_HandleDLCMountingComplete);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleDLCInstalled(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_HandleDLCInstalled);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleTMSBanFileRetrieved(UIScene* self)
{
    ModHookCall hook(kModHook_UIScene_HandleTMSBanFileRetrieved);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleMessage(UIScene* self, int message)
{
    ModHookCall hook(kModHook_UIScene_HandleMessage);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(message);
    const bool proceed = hook.Run();
    return proceed;
}

bool UISceneHandleSaveDeviceRemoved(UIScene* self, int& deviceIndex)
{
    ModHookCall hook(kModHook_UIScene_HandleSaveDeviceRemoved);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(deviceIndex);
    const bool proceed = hook.Run();
    deviceIndex = hook.GetInt(1, deviceIndex);
    return proceed;
}

bool UISceneHandleTouchInput(UIScene* self, int playerId, int& touchX, int& touchY, int& touchBox, bool& pressed, bool& repeat, bool& released)
{
    ModHookCall hook(kModHook_UIScene_handleTouchInput);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(playerId);
    hook.Add(touchX);
    hook.Add(touchY);
    hook.Add(touchBox);
    hook.Add(pressed);
    hook.Add(repeat);
    hook.Add(released);
    const bool proceed = hook.Run();
    touchX = hook.GetInt(2, touchX);
    touchY = hook.GetInt(3, touchY);
    touchBox = hook.GetInt(4, touchBox);
    pressed = hook.GetBool(5, pressed);
    repeat = hook.GetBool(6, repeat);
    released = hook.GetBool(7, released);
    return proceed;
}

bool UISceneIsReadyToDelete(UIScene* self, bool& result)
{
    ModHookCall hook(kModHook_UIScene_isReadyToDelete);
    hook.AddObject(mod::api::eHandle_UIScene, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ClientPacketListenerHandleCustomPayload(ClientPacketListener* self, ClientboundCustomPayloadPacket* packet)
{
    ModHookCall hook(kModHook_ClientPacketListener_handleCustomPayload);
    hook.AddObject(mod::api::eHandle_ClientPacketListener, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ClientboundCustomPayloadPacket, ModHandles::Add(packet));
    const bool proceed = hook.Run();
    return proceed;
}

void ClientPacketListenerHandleCustomPayloadAfter(ClientPacketListener* self, ClientboundCustomPayloadPacket* packet)
{
    ModHookCall hook(kModHook_ClientPacketListener_handleCustomPayload_after);
    hook.AddObject(mod::api::eHandle_ClientPacketListener, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ClientboundCustomPayloadPacket, ModHandles::Add(packet));
    hook.Run();
}

bool MoveControlSetWantedPosition(MoveControl* self, float x, float y, float z, float speedModifier)
{
    ModHookCall hook(kModHook_MoveControl_setWantedPosition);
    hook.AddObject(mod::api::eHandle_MoveControl, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(speedModifier);
    const bool proceed = hook.Run();
    return proceed;
}

bool MoveControlTick(MoveControl* self)
{
    ModHookCall hook(kModHook_MoveControl_tick);
    hook.AddObject(mod::api::eHandle_MoveControl, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

void MoveControlSetWantedPositionAfter(MoveControl* self, float x, float y, float z, float speedModifier)
{
    ModHookCall hook(kModHook_MoveControl_setWantedPosition_after);
    hook.AddObject(mod::api::eHandle_MoveControl, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(speedModifier);
    hook.Run();
}

void MoveControlTickAfter(MoveControl* self)
{
    ModHookCall hook(kModHook_MoveControl_tick_after);
    hook.AddObject(mod::api::eHandle_MoveControl, ModHandles::Add(self));
    hook.Run();
}

bool PathFinderFindPath(PathFinder* self, float& maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.Add(maxDistance);
    hook.AddNil();
    const bool proceed = hook.Run();
    maxDistance = hook.GetFloat(1, maxDistance);
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool PathFinderFindPath2(PathFinder* self, Level* levelSource, const boost::shared_ptr<Entity>& mob, float x, float y, float z, float& maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath_2);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(mob);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(maxDistance);
    hook.AddNil();
    const bool proceed = hook.Run();
    maxDistance = hook.GetFloat(6, maxDistance);
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool PathFinderFindPath3(PathFinder* self, Level* levelSource, const boost::shared_ptr<Entity>& mob, int x, int y, int z, float& maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath_3);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(mob);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(maxDistance);
    hook.AddNil();
    const bool proceed = hook.Run();
    maxDistance = hook.GetFloat(6, maxDistance);
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool PathFinderFindPath4(PathFinder* self, Level* levelSource, const boost::shared_ptr<Entity>& mob, const boost::shared_ptr<Entity>& entity, float& maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath_4);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(mob);
    hook.Add(entity);
    hook.Add(maxDistance);
    hook.AddNil();
    const bool proceed = hook.Run();
    maxDistance = hook.GetFloat(4, maxDistance);
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(5, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

void PathFinderFindPathAfter(PathFinder* self, float maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath_after);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.Add(maxDistance);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_Path, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void PathFinderFindPath2After(PathFinder* self, Level* levelSource, const boost::shared_ptr<Entity>& mob, float x, float y, float z, float maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath_2_after);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(mob);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(maxDistance);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_Path, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void PathFinderFindPath3After(PathFinder* self, Level* levelSource, const boost::shared_ptr<Entity>& mob, int x, int y, int z, float maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath_3_after);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(mob);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(maxDistance);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_Path, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void PathFinderFindPath4After(PathFinder* self, Level* levelSource, const boost::shared_ptr<Entity>& mob, const boost::shared_ptr<Entity>& entity, float maxDistance, Path*& result)
{
    ModHookCall hook(kModHook_PathFinder_findPath_4_after);
    hook.AddObject(mod::api::eHandle_PathFinder, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(levelSource));
    hook.Add(mob);
    hook.Add(entity);
    hook.Add(maxDistance);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_Path, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        Path* object = NULL;
        result = hook.GetHandle(5, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

bool DirectoryLevelStorageSave(DirectoryLevelStorage* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_save);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool DirectoryLevelStorageLoadPlayerDataTag(DirectoryLevelStorage* self, CompoundTag*& result)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_loadPlayerDataTag);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        CompoundTag* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DirectoryLevelStorageLoad(DirectoryLevelStorage* self, const boost::shared_ptr<Entity>& player, CompoundTag*& result)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_load);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.Add(player);
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        CompoundTag* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool DirectoryLevelStorageSaveLevelData(DirectoryLevelStorage* self, LevelData* levelData)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_saveLevelData);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_LevelData, ModHandles::Add(levelData));
    const bool proceed = hook.Run();
    return proceed;
}

bool DirectoryLevelStorageSaveLevelData2(DirectoryLevelStorage* self, LevelData* levelData)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_saveLevelData_2);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_LevelData, ModHandles::Add(levelData));
    const bool proceed = hook.Run();
    return proceed;
}

void DirectoryLevelStorageSaveAfter(DirectoryLevelStorage* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_save_after);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.Add(player);
    hook.Run();
}

void DirectoryLevelStorageLoadPlayerDataTagAfter(DirectoryLevelStorage* self, CompoundTag*& result)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_loadPlayerDataTag_after);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_CompoundTag, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        CompoundTag* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void DirectoryLevelStorageLoadAfter(DirectoryLevelStorage* self, const boost::shared_ptr<Entity>& player, CompoundTag*& result)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_load_after);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.Add(player);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_CompoundTag, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        CompoundTag* object = NULL;
        result = hook.GetHandle(2, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void DirectoryLevelStorageSaveLevelDataAfter(DirectoryLevelStorage* self, LevelData* levelData)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_saveLevelData_after);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_LevelData, ModHandles::Add(levelData));
    hook.Run();
}

void DirectoryLevelStorageSaveLevelData2After(DirectoryLevelStorage* self, LevelData* levelData)
{
    ModHookCall hook(kModHook_DirectoryLevelStorage_saveLevelData_2_after);
    hook.AddObject(mod::api::eHandle_DirectoryLevelStorage, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_LevelData, ModHandles::Add(levelData));
    hook.Run();
}

bool OverworldLevelSourceGetHeightFalloff(OverworldLevelSource* self, int& x, int& z, int distance, float& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_getHeightFalloff);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(distance);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    result = hook.GetFloat(4, result);
    return proceed;
}

bool OverworldLevelSourceGetHeights(OverworldLevelSource* self, int& x, int& y, int& z)
{
    ModHookCall hook(kModHook_OverworldLevelSource_getHeights);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    y = hook.GetInt(2, y);
    z = hook.GetInt(3, z);
    return proceed;
}

bool OverworldLevelSourcePrepareHeights(OverworldLevelSource* self, int& chunkX, int& chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_OverworldLevelSource_prepareHeights);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool OverworldLevelSourceBuildSurfaces(OverworldLevelSource* self, int& chunkX, int& chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_OverworldLevelSource_buildSurfaces);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool OverworldLevelSourceCreateChunk(OverworldLevelSource* self, int& chunkX, int& chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_createChunk);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddNil();
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool OverworldLevelSourceLightChunk(OverworldLevelSource* self, LevelChunk* chunk)
{
    ModHookCall hook(kModHook_OverworldLevelSource_lightChunk);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    const bool proceed = hook.Run();
    return proceed;
}

bool OverworldLevelSourceIsWater(OverworldLevelSource* self, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_isWater);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(4, result);
    return proceed;
}

bool OverworldLevelSourceCalcWaterDepths(OverworldLevelSource* self, int& chunkX, int& chunkZ)
{
    ModHookCall hook(kModHook_OverworldLevelSource_calcWaterDepths);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool OverworldLevelSourcePostProcess(OverworldLevelSource* self, int& chunkX, int& chunkZ)
{
    ModHookCall hook(kModHook_OverworldLevelSource_postProcess);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool OverworldLevelSourcePostProcessLoadedChunk(OverworldLevelSource* self, LevelChunk* lceArg0, int& lceArg1, int& lceArg2, bool& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_postProcessLoadedChunk);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(lceArg1);
    hook.Add(lceArg2);
    hook.Add(result);
    const bool proceed = hook.Run();
    lceArg1 = hook.GetInt(2, lceArg1);
    lceArg2 = hook.GetInt(3, lceArg2);
    result = hook.GetBool(4, result);
    return proceed;
}

bool OverworldLevelSourceIsPosInFeature(OverworldLevelSource* self, Level* lceArg0, const std::wstring& feature, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_isPosInFeature);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(lceArg0));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool OverworldLevelSourceFindNearestMapFeature(OverworldLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool& takeBlock, BlockPos*& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_findNearestMapFeature);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(takeBlock);
    hook.AddNil();
    const bool proceed = hook.Run();
    takeBlock = hook.GetBool(6, takeBlock);
    {
        uint32_t id = 0;
        BlockPos* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool OverworldLevelSourceRecreateLogicStructuresForChunk(OverworldLevelSource* self, LevelChunk* lceArg0, int& chunkX, int& chunkZ)
{
    ModHookCall hook(kModHook_OverworldLevelSource_recreateLogicStructuresForChunk);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(2, chunkX);
    chunkZ = hook.GetInt(3, chunkZ);
    return proceed;
}

void OverworldLevelSourceGetHeightFalloffAfter(OverworldLevelSource* self, int x, int z, int distance, float& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_getHeightFalloff_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(distance);
    hook.Add(result);
    hook.Run();
    result = hook.GetFloat(4, result);
}

void OverworldLevelSourceGetHeightsAfter(OverworldLevelSource* self, int x, int y, int z)
{
    ModHookCall hook(kModHook_OverworldLevelSource_getHeights_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Run();
}

void OverworldLevelSourcePrepareHeightsAfter(OverworldLevelSource* self, int chunkX, int chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_OverworldLevelSource_prepareHeights_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    hook.Run();
}

void OverworldLevelSourceBuildSurfacesAfter(OverworldLevelSource* self, int chunkX, int chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_OverworldLevelSource_buildSurfaces_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    hook.Run();
}

void OverworldLevelSourceCreateChunkAfter(OverworldLevelSource* self, int chunkX, int chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_createChunk_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void OverworldLevelSourceLightChunkAfter(OverworldLevelSource* self, LevelChunk* chunk)
{
    ModHookCall hook(kModHook_OverworldLevelSource_lightChunk_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    hook.Run();
}

void OverworldLevelSourceIsWaterAfter(OverworldLevelSource* self, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_isWater_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(4, result);
}

void OverworldLevelSourceCalcWaterDepthsAfter(OverworldLevelSource* self, int chunkX, int chunkZ)
{
    ModHookCall hook(kModHook_OverworldLevelSource_calcWaterDepths_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Run();
}

void OverworldLevelSourcePostProcessAfter(OverworldLevelSource* self, int chunkX, int chunkZ)
{
    ModHookCall hook(kModHook_OverworldLevelSource_postProcess_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Run();
}

void OverworldLevelSourcePostProcessLoadedChunkAfter(OverworldLevelSource* self, LevelChunk* lceArg0, int lceArg1, int lceArg2, bool& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_postProcessLoadedChunk_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(lceArg1);
    hook.Add(lceArg2);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(4, result);
}

void OverworldLevelSourceIsPosInFeatureAfter(OverworldLevelSource* self, Level* lceArg0, const std::wstring& feature, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_isPosInFeature_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(lceArg0));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(6, result);
}

void OverworldLevelSourceFindNearestMapFeatureAfter(OverworldLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool takeBlock, BlockPos*& result)
{
    ModHookCall hook(kModHook_OverworldLevelSource_findNearestMapFeature_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(takeBlock);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_BlockPos, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        BlockPos* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void OverworldLevelSourceRecreateLogicStructuresForChunkAfter(OverworldLevelSource* self, LevelChunk* lceArg0, int chunkX, int chunkZ)
{
    ModHookCall hook(kModHook_OverworldLevelSource_recreateLogicStructuresForChunk_after);
    hook.AddObject(mod::api::eHandle_OverworldLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Run();
}

bool NetherLevelSourcePrepareHeights(NetherLevelSource* self, int& chunkX, int& chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_NetherLevelSource_prepareHeights);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool NetherLevelSourceBuildSurfaces(NetherLevelSource* self, int& chunkX, int& chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_NetherLevelSource_buildSurfaces);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool NetherLevelSourceLightChunk(NetherLevelSource* self, LevelChunk* chunk)
{
    ModHookCall hook(kModHook_NetherLevelSource_lightChunk);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    const bool proceed = hook.Run();
    return proceed;
}

bool NetherLevelSourceGetHeights(NetherLevelSource* self, const std::string& buffer, int& x, int& y, int& z, int& xSize, int& ySize, int& zSize, std::string& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_getHeights);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(buffer);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(xSize);
    hook.Add(ySize);
    hook.Add(zSize);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(2, x);
    y = hook.GetInt(3, y);
    z = hook.GetInt(4, z);
    xSize = hook.GetInt(5, xSize);
    ySize = hook.GetInt(6, ySize);
    zSize = hook.GetInt(7, zSize);
    result = hook.GetBytes(8, result);
    return proceed;
}

bool NetherLevelSourceCreateChunk(NetherLevelSource* self, int& chunkX, int& chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_createChunk);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddNil();
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool NetherLevelSourcePostProcess(NetherLevelSource* self, int& chunkX, int& chunkZ)
{
    ModHookCall hook(kModHook_NetherLevelSource_postProcess);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool NetherLevelSourcePostProcessLoadedChunk(NetherLevelSource* self, LevelChunk* chunk, int& chunkX, int& chunkZ, bool& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_postProcessLoadedChunk);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Add(result);
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(2, chunkX);
    chunkZ = hook.GetInt(3, chunkZ);
    result = hook.GetBool(4, result);
    return proceed;
}

bool NetherLevelSourceFindNearestMapFeature(NetherLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool& findUnexplored, BlockPos*& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_findNearestMapFeature);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(findUnexplored);
    hook.AddNil();
    const bool proceed = hook.Run();
    findUnexplored = hook.GetBool(6, findUnexplored);
    {
        uint32_t id = 0;
        BlockPos* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool NetherLevelSourceRecreateLogicStructuresForChunk(NetherLevelSource* self, LevelChunk* chunk, int& chunkX, int& chunkZ)
{
    ModHookCall hook(kModHook_NetherLevelSource_recreateLogicStructuresForChunk);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(2, chunkX);
    chunkZ = hook.GetInt(3, chunkZ);
    return proceed;
}

bool NetherLevelSourceIsPosInFeature(NetherLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_isPosInFeature);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

void NetherLevelSourcePrepareHeightsAfter(NetherLevelSource* self, int chunkX, int chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_NetherLevelSource_prepareHeights_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    hook.Run();
}

void NetherLevelSourceBuildSurfacesAfter(NetherLevelSource* self, int chunkX, int chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_NetherLevelSource_buildSurfaces_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    hook.Run();
}

void NetherLevelSourceLightChunkAfter(NetherLevelSource* self, LevelChunk* chunk)
{
    ModHookCall hook(kModHook_NetherLevelSource_lightChunk_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    hook.Run();
}

void NetherLevelSourceGetHeightsAfter(NetherLevelSource* self, const std::string& buffer, int x, int y, int z, int xSize, int ySize, int zSize, std::string& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_getHeights_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(buffer);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(xSize);
    hook.Add(ySize);
    hook.Add(zSize);
    hook.Add(result);
    hook.Run();
    result = hook.GetBytes(8, result);
}

void NetherLevelSourceCreateChunkAfter(NetherLevelSource* self, int chunkX, int chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_createChunk_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void NetherLevelSourcePostProcessAfter(NetherLevelSource* self, int chunkX, int chunkZ)
{
    ModHookCall hook(kModHook_NetherLevelSource_postProcess_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Run();
}

void NetherLevelSourcePostProcessLoadedChunkAfter(NetherLevelSource* self, LevelChunk* chunk, int chunkX, int chunkZ, bool& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_postProcessLoadedChunk_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(4, result);
}

void NetherLevelSourceFindNearestMapFeatureAfter(NetherLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool findUnexplored, BlockPos*& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_findNearestMapFeature_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(findUnexplored);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_BlockPos, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        BlockPos* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void NetherLevelSourceRecreateLogicStructuresForChunkAfter(NetherLevelSource* self, LevelChunk* chunk, int chunkX, int chunkZ)
{
    ModHookCall hook(kModHook_NetherLevelSource_recreateLogicStructuresForChunk_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(chunk));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Run();
}

void NetherLevelSourceIsPosInFeatureAfter(NetherLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_NetherLevelSource_isPosInFeature_after);
    hook.AddObject(mod::api::eHandle_NetherLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(6, result);
}

bool TheEndLevelSourceFjIslandCheck(TheEndLevelSource* self, int& x, int& z, bool& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_fjIslandCheck);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    result = hook.GetBool(3, result);
    return proceed;
}

bool TheEndLevelSourceGetIslandHeightValue(TheEndLevelSource* self, int& x, int& z, int& xOffset, int& zOffset, float& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_getIslandHeightValue);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(xOffset);
    hook.Add(zOffset);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    xOffset = hook.GetInt(3, xOffset);
    zOffset = hook.GetInt(4, zOffset);
    result = hook.GetFloat(5, result);
    return proceed;
}

bool TheEndLevelSourceGetHeights(TheEndLevelSource* self, const std::string& values, int& x, int& y, int& z, int& xSize, int& ySize, int& zSize, std::string& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_getHeights);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(values);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(xSize);
    hook.Add(ySize);
    hook.Add(zSize);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(2, x);
    y = hook.GetInt(3, y);
    z = hook.GetInt(4, z);
    xSize = hook.GetInt(5, xSize);
    ySize = hook.GetInt(6, ySize);
    zSize = hook.GetInt(7, zSize);
    result = hook.GetBytes(8, result);
    return proceed;
}

bool TheEndLevelSourcePrepareHeights(TheEndLevelSource* self, int& chunkX, int& chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_TheEndLevelSource_prepareHeights);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool TheEndLevelSourceBuildSurfaces(TheEndLevelSource* self, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_TheEndLevelSource_buildSurfaces);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    const bool proceed = hook.Run();
    return proceed;
}

bool TheEndLevelSourceCreateChunk(TheEndLevelSource* self, int& chunkX, int& chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_createChunk);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddNil();
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool TheEndLevelSourcePostProcess(TheEndLevelSource* self, int& chunkX, int& chunkZ)
{
    ModHookCall hook(kModHook_TheEndLevelSource_postProcess);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    return proceed;
}

bool TheEndLevelSourcePostProcessLoadedChunk(TheEndLevelSource* self, LevelChunk* lceArg0, int& lceArg1, int& lceArg2, bool& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_postProcessLoadedChunk);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(lceArg1);
    hook.Add(lceArg2);
    hook.Add(result);
    const bool proceed = hook.Run();
    lceArg1 = hook.GetInt(2, lceArg1);
    lceArg2 = hook.GetInt(3, lceArg2);
    result = hook.GetBool(4, result);
    return proceed;
}

bool TheEndLevelSourceFindNearestMapFeature(TheEndLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool& findUnexplored, BlockPos*& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_findNearestMapFeature);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(findUnexplored);
    hook.AddNil();
    const bool proceed = hook.Run();
    findUnexplored = hook.GetBool(6, findUnexplored);
    {
        uint32_t id = 0;
        BlockPos* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool TheEndLevelSourceRecreateLogicStructuresForChunk(TheEndLevelSource* self, LevelChunk* lceArg0, int& lceArg1, int& lceArg2)
{
    ModHookCall hook(kModHook_TheEndLevelSource_recreateLogicStructuresForChunk);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(lceArg1);
    hook.Add(lceArg2);
    const bool proceed = hook.Run();
    lceArg1 = hook.GetInt(2, lceArg1);
    lceArg2 = hook.GetInt(3, lceArg2);
    return proceed;
}

bool TheEndLevelSourceIsPosInFeature(TheEndLevelSource* self, Level* lceArg0, const std::wstring& feature, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_isPosInFeature);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(lceArg0));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

void TheEndLevelSourceFjIslandCheckAfter(TheEndLevelSource* self, int x, int z, bool& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_fjIslandCheck_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(3, result);
}

void TheEndLevelSourceGetIslandHeightValueAfter(TheEndLevelSource* self, int x, int z, int xOffset, int zOffset, float& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_getIslandHeightValue_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(xOffset);
    hook.Add(zOffset);
    hook.Add(result);
    hook.Run();
    result = hook.GetFloat(5, result);
}

void TheEndLevelSourceGetHeightsAfter(TheEndLevelSource* self, const std::string& values, int x, int y, int z, int xSize, int ySize, int zSize, std::string& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_getHeights_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(values);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(xSize);
    hook.Add(ySize);
    hook.Add(zSize);
    hook.Add(result);
    hook.Run();
    result = hook.GetBytes(8, result);
}

void TheEndLevelSourcePrepareHeightsAfter(TheEndLevelSource* self, int chunkX, int chunkZ, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_TheEndLevelSource_prepareHeights_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    hook.Run();
}

void TheEndLevelSourceBuildSurfacesAfter(TheEndLevelSource* self, ChunkPrimer* primer)
{
    ModHookCall hook(kModHook_TheEndLevelSource_buildSurfaces_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_primer, ModHandles::Add(primer));
    hook.Run();
}

void TheEndLevelSourceCreateChunkAfter(TheEndLevelSource* self, int chunkX, int chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_createChunk_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void TheEndLevelSourcePostProcessAfter(TheEndLevelSource* self, int chunkX, int chunkZ)
{
    ModHookCall hook(kModHook_TheEndLevelSource_postProcess_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.Run();
}

void TheEndLevelSourcePostProcessLoadedChunkAfter(TheEndLevelSource* self, LevelChunk* lceArg0, int lceArg1, int lceArg2, bool& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_postProcessLoadedChunk_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(lceArg1);
    hook.Add(lceArg2);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(4, result);
}

void TheEndLevelSourceFindNearestMapFeatureAfter(TheEndLevelSource* self, Level* level, const std::wstring& feature, int x, int y, int z, bool findUnexplored, BlockPos*& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_findNearestMapFeature_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(findUnexplored);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_BlockPos, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        BlockPos* object = NULL;
        result = hook.GetHandle(7, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void TheEndLevelSourceRecreateLogicStructuresForChunkAfter(TheEndLevelSource* self, LevelChunk* lceArg0, int lceArg1, int lceArg2)
{
    ModHookCall hook(kModHook_TheEndLevelSource_recreateLogicStructuresForChunk_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(lceArg0));
    hook.Add(lceArg1);
    hook.Add(lceArg2);
    hook.Run();
}

void TheEndLevelSourceIsPosInFeatureAfter(TheEndLevelSource* self, Level* lceArg0, const std::wstring& feature, int x, int y, int z, bool& result)
{
    ModHookCall hook(kModHook_TheEndLevelSource_isPosInFeature_after);
    hook.AddObject(mod::api::eHandle_TheEndLevelSource, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(lceArg0));
    hook.Add(feature);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(6, result);
}

bool ServerPlayerGameModeSetLevel(ServerPlayerGameMode* self, ServerLevel* level)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_setLevel);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerLevel, ModHandles::Add(level));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeUpdateGameMode(ServerPlayerGameMode* self, GameType* gameType)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_updateGameMode);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_GameType, ModHandles::Add(gameType));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeSetGameRules(ServerPlayerGameMode* self, GameRulesInstance* gameRules)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_setGameRules);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_GameRulesInstance, ModHandles::Add(gameRules));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeNullifyGameRules(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_nullifyGameRules);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeSetLastCheckpoint(ServerPlayerGameMode* self, int& checkpoint)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_setLastCheckpoint);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(checkpoint);
    const bool proceed = hook.Run();
    checkpoint = hook.GetInt(1, checkpoint);
    return proceed;
}

bool ServerPlayerGameModeRestoreFromOld(ServerPlayerGameMode* self, ServerPlayerGameMode* oldMode)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RestoreFromOld);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(oldMode));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeSuperDestroyBlock(ServerPlayerGameMode* self, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_superDestroyBlock);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(4, result);
    return proceed;
}

bool ServerPlayerGameModeDestroyBlock(ServerPlayerGameMode* self, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_destroyBlock);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(4, result);
    return proceed;
}

bool ServerPlayerGameModeStopDestroyBlock(ServerPlayerGameMode* self, int x, int y, int z)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_stopDestroyBlock);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeAbortDestroyBlock(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_abortDestroyBlock);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeUseItemOn(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& player, Level* level, ItemInstance* item, int hand, int x, int y, int z, int direction, float& hitX, float& hitY, float& hitZ, bool& simulate, bool result_arg, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_useItemOn);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(player);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    hook.Add(hand);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(hitX);
    hook.Add(hitY);
    hook.Add(hitZ);
    hook.Add(simulate);
    hook.Add(result_arg);
    hook.Add(result);
    const bool proceed = hook.Run();
    hitX = hook.GetFloat(9, hitX);
    hitY = hook.GetFloat(10, hitY);
    hitZ = hook.GetFloat(11, hitZ);
    simulate = hook.GetBool(12, simulate);
    result = hook.GetInt(14, result);
    return proceed;
}

bool ServerPlayerGameModeStartDestroyBlock(ServerPlayerGameMode* self, int x, int y, int z, int direction, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_startDestroyBlock);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(5, result);
    return proceed;
}

bool ServerPlayerGameModeUseItem(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& player, Level* level, int hand, bool& simulate, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_useItem);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(player);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(hand);
    hook.Add(simulate);
    hook.Add(result);
    const bool proceed = hook.Run();
    simulate = hook.GetBool(4, simulate);
    result = hook.GetInt(5, result);
    return proceed;
}

bool ServerPlayerGameModeSetGameType(ServerPlayerGameMode* self, GameType* gameType)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_SetGameType);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_GameType, ModHandles::Add(gameType));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeGetGameType(ServerPlayerGameMode* self, GameType*& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_GetGameType);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        GameType* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool ServerPlayerGameModeIsNearCentreOf(ServerPlayerGameMode* self, AABB* box, const boost::shared_ptr<Entity>& player, float x, float y, float z, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_IsNearCentreOf);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_AABB, ModHandles::Add(box));
    hook.Add(player);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(6, result);
    return proceed;
}

bool ServerPlayerGameModeTick(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_tick);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeOnDeath(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_OnDeath);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeSetDeaths(ServerPlayerGameMode* self, int& deaths)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_SetDeaths);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(deaths);
    const bool proceed = hook.Run();
    deaths = hook.GetInt(1, deaths);
    return proceed;
}

bool ServerPlayerGameModeSetKills(ServerPlayerGameMode* self, int& lceArg0)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_SetKills);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(lceArg0);
    const bool proceed = hook.Run();
    lceArg0 = hook.GetInt(1, lceArg0);
    return proceed;
}

bool ServerPlayerGameModeRecordDamage(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& entity, CombatEntry* entry, ClientboundChatPacket* packet)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordDamage);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(entity);
    hook.AddObject(mod::api::eHandle_CombatEntry, ModHandles::Add(entry));
    hook.AddObject(mod::api::eHandle_ClientboundChatPacket, ModHandles::Add(packet));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordDeath(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordDeath);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(entity);
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordItem(ServerPlayerGameMode* self, ItemInstance* item)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordItem);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordContainer(ServerPlayerGameMode* self, int& type, int& slot, int& count, int& data)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordContainer);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(type);
    hook.Add(slot);
    hook.Add(count);
    hook.Add(data);
    const bool proceed = hook.Run();
    type = hook.GetInt(1, type);
    slot = hook.GetInt(2, slot);
    count = hook.GetInt(3, count);
    data = hook.GetInt(4, data);
    return proceed;
}

bool ServerPlayerGameModeRecordWalk(ServerPlayerGameMode* self, int& distance, bool& sprinting, bool& sneaking)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordWalk);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(distance);
    hook.Add(sprinting);
    hook.Add(sneaking);
    const bool proceed = hook.Run();
    distance = hook.GetInt(1, distance);
    sprinting = hook.GetBool(2, sprinting);
    sneaking = hook.GetBool(3, sneaking);
    return proceed;
}

bool ServerPlayerGameModeRecordSwim(ServerPlayerGameMode* self, int& distance, bool& sprinting)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSwim);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(distance);
    hook.Add(sprinting);
    const bool proceed = hook.Run();
    distance = hook.GetInt(1, distance);
    sprinting = hook.GetBool(2, sprinting);
    return proceed;
}

bool ServerPlayerGameModeRecordClimb(ServerPlayerGameMode* self, int& distance)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordClimb);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(distance);
    const bool proceed = hook.Run();
    distance = hook.GetInt(1, distance);
    return proceed;
}

bool ServerPlayerGameModeRecordUseItem(ServerPlayerGameMode* self, ItemInstance* item)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordUseItem);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordJump(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordJump);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordGainPoints(ServerPlayerGameMode* self, int& points)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordGainPoints);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(points);
    const bool proceed = hook.Run();
    points = hook.GetInt(1, points);
    return proceed;
}

bool ServerPlayerGameModeResetTempPoints(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_ResetTempPoints);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordSecondaryTargetReached(ServerPlayerGameMode* self, const std::wstring& name, int& target, int size, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSecondaryTargetReached);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(name);
    hook.Add(target);
    hook.Add(size);
    hook.Add(result);
    const bool proceed = hook.Run();
    target = hook.GetInt(2, target);
    result = hook.GetBool(4, result);
    return proceed;
}

bool ServerPlayerGameModeRecordLastCheckpoint(ServerPlayerGameMode* self, int& checkpoint, int& lap, bool& first, bool& second, bool& third, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLastCheckpoint);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(checkpoint);
    hook.Add(lap);
    hook.Add(first);
    hook.Add(second);
    hook.Add(third);
    hook.Add(result);
    const bool proceed = hook.Run();
    checkpoint = hook.GetInt(1, checkpoint);
    lap = hook.GetInt(2, lap);
    first = hook.GetBool(3, first);
    second = hook.GetBool(4, second);
    third = hook.GetBool(5, third);
    result = hook.GetBool(6, result);
    return proceed;
}

bool ServerPlayerGameModeRecordLapCompleted(ServerPlayerGameMode* self, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLapCompleted);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ServerPlayerGameModeRecordThermalEntered(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordThermalEntered);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordProgress(ServerPlayerGameMode* self, float first, float second, float third, float fourth, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordProgress);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(first);
    hook.Add(second);
    hook.Add(third);
    hook.Add(fourth);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(5, result);
    return proceed;
}

bool ServerPlayerGameModeRecordSpawnedAtCheckpoint(ServerPlayerGameMode* self, int& checkpoint)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSpawnedAtCheckpoint);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(checkpoint);
    const bool proceed = hook.Run();
    checkpoint = hook.GetInt(1, checkpoint);
    return proceed;
}

bool ServerPlayerGameModeRecordBlockPlaced(ServerPlayerGameMode* self, int x, int y, int z, int& blockId)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordBlockPlaced);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(blockId);
    const bool proceed = hook.Run();
    blockId = hook.GetInt(4, blockId);
    return proceed;
}

bool ServerPlayerGameModeRecordBlockDestroyed(ServerPlayerGameMode* self, int x, int y, int z, int& blockId)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordBlockDestroyed);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(blockId);
    const bool proceed = hook.Run();
    blockId = hook.GetInt(4, blockId);
    return proceed;
}

bool ServerPlayerGameModeRecordBlockPresent(ServerPlayerGameMode* self, int x, int y, int z, int& blockId)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordBlockPresent);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(blockId);
    const bool proceed = hook.Run();
    blockId = hook.GetInt(4, blockId);
    return proceed;
}

bool ServerPlayerGameModeRecordLayerTime(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLayerTime);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordSnowballHit(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSnowballHit);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordHitBySnowball(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordHitBySnowball);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeRecordLanding(ServerPlayerGameMode* self, int x, int y, int z)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLanding);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerPlayerGameModeGetMiniGame(ServerPlayerGameMode* self, MiniGameDef*& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_GetMiniGame);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        MiniGameDef* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

bool ServerPlayerGameModeGetLevel(ServerPlayerGameMode* self, Level*& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_GetLevel);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddNil();
    const bool proceed = hook.Run();
    {
        uint32_t id = 0;
        Level* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

void ServerPlayerGameModeSetLevelAfter(ServerPlayerGameMode* self, ServerLevel* level)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_setLevel_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerLevel, ModHandles::Add(level));
    hook.Run();
}

void ServerPlayerGameModeUpdateGameModeAfter(ServerPlayerGameMode* self, GameType* gameType)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_updateGameMode_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_GameType, ModHandles::Add(gameType));
    hook.Run();
}

void ServerPlayerGameModeSetGameRulesAfter(ServerPlayerGameMode* self, GameRulesInstance* gameRules)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_setGameRules_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_GameRulesInstance, ModHandles::Add(gameRules));
    hook.Run();
}

void ServerPlayerGameModeNullifyGameRulesAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_nullifyGameRules_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeSetLastCheckpointAfter(ServerPlayerGameMode* self, int checkpoint)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_setLastCheckpoint_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(checkpoint);
    hook.Run();
}

void ServerPlayerGameModeRestoreFromOldAfter(ServerPlayerGameMode* self, ServerPlayerGameMode* oldMode)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RestoreFromOld_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(oldMode));
    hook.Run();
}

void ServerPlayerGameModeSuperDestroyBlockAfter(ServerPlayerGameMode* self, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_superDestroyBlock_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetInt(4, result);
}

void ServerPlayerGameModeDestroyBlockAfter(ServerPlayerGameMode* self, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_destroyBlock_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetInt(4, result);
}

void ServerPlayerGameModeStopDestroyBlockAfter(ServerPlayerGameMode* self, int x, int y, int z)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_stopDestroyBlock_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Run();
}

void ServerPlayerGameModeAbortDestroyBlockAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_abortDestroyBlock_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeUseItemOnAfter(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& player, Level* level, ItemInstance* item, int hand, int x, int y, int z, int direction, float hitX, float hitY, float hitZ, bool simulate, bool result_arg, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_useItemOn_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(player);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    hook.Add(hand);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(hitX);
    hook.Add(hitY);
    hook.Add(hitZ);
    hook.Add(simulate);
    hook.Add(result_arg);
    hook.Add(result);
    hook.Run();
    result = hook.GetInt(14, result);
}

void ServerPlayerGameModeStartDestroyBlockAfter(ServerPlayerGameMode* self, int x, int y, int z, int direction, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_startDestroyBlock_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(direction);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(5, result);
}

void ServerPlayerGameModeUseItemAfter(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& player, Level* level, int hand, bool simulate, int& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_useItem_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(player);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(hand);
    hook.Add(simulate);
    hook.Add(result);
    hook.Run();
    result = hook.GetInt(5, result);
}

void ServerPlayerGameModeSetGameTypeAfter(ServerPlayerGameMode* self, GameType* gameType)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_SetGameType_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_GameType, ModHandles::Add(gameType));
    hook.Run();
}

void ServerPlayerGameModeGetGameTypeAfter(ServerPlayerGameMode* self, GameType*& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_GetGameType_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_GameType, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        GameType* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void ServerPlayerGameModeIsNearCentreOfAfter(ServerPlayerGameMode* self, AABB* box, const boost::shared_ptr<Entity>& player, float x, float y, float z, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_IsNearCentreOf_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_AABB, ModHandles::Add(box));
    hook.Add(player);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(6, result);
}

void ServerPlayerGameModeTickAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_tick_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeOnDeathAfter(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_OnDeath_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(player);
    hook.Run();
}

void ServerPlayerGameModeSetDeathsAfter(ServerPlayerGameMode* self, int deaths)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_SetDeaths_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(deaths);
    hook.Run();
}

void ServerPlayerGameModeSetKillsAfter(ServerPlayerGameMode* self, int lceArg0)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_SetKills_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(lceArg0);
    hook.Run();
}

void ServerPlayerGameModeRecordDamageAfter(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& entity, CombatEntry* entry, ClientboundChatPacket* packet)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordDamage_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(entity);
    hook.AddObject(mod::api::eHandle_CombatEntry, ModHandles::Add(entry));
    hook.AddObject(mod::api::eHandle_ClientboundChatPacket, ModHandles::Add(packet));
    hook.Run();
}

void ServerPlayerGameModeRecordDeathAfter(ServerPlayerGameMode* self, const boost::shared_ptr<Entity>& entity)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordDeath_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(entity);
    hook.Run();
}

void ServerPlayerGameModeRecordItemAfter(ServerPlayerGameMode* self, ItemInstance* item)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordItem_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    hook.Run();
}

void ServerPlayerGameModeRecordContainerAfter(ServerPlayerGameMode* self, int type, int slot, int count, int data)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordContainer_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(type);
    hook.Add(slot);
    hook.Add(count);
    hook.Add(data);
    hook.Run();
}

void ServerPlayerGameModeRecordWalkAfter(ServerPlayerGameMode* self, int distance, bool sprinting, bool sneaking)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordWalk_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(distance);
    hook.Add(sprinting);
    hook.Add(sneaking);
    hook.Run();
}

void ServerPlayerGameModeRecordSwimAfter(ServerPlayerGameMode* self, int distance, bool sprinting)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSwim_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(distance);
    hook.Add(sprinting);
    hook.Run();
}

void ServerPlayerGameModeRecordClimbAfter(ServerPlayerGameMode* self, int distance)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordClimb_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(distance);
    hook.Run();
}

void ServerPlayerGameModeRecordUseItemAfter(ServerPlayerGameMode* self, ItemInstance* item)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordUseItem_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ItemInstance, ModHandles::Add(item));
    hook.Run();
}

void ServerPlayerGameModeRecordJumpAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordJump_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeRecordGainPointsAfter(ServerPlayerGameMode* self, int points)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordGainPoints_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(points);
    hook.Run();
}

void ServerPlayerGameModeResetTempPointsAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_ResetTempPoints_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeRecordSecondaryTargetReachedAfter(ServerPlayerGameMode* self, const std::wstring& name, int target, int size, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSecondaryTargetReached_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(name);
    hook.Add(target);
    hook.Add(size);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(4, result);
}

void ServerPlayerGameModeRecordLastCheckpointAfter(ServerPlayerGameMode* self, int checkpoint, int lap, bool first, bool second, bool third, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLastCheckpoint_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(checkpoint);
    hook.Add(lap);
    hook.Add(first);
    hook.Add(second);
    hook.Add(third);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(6, result);
}

void ServerPlayerGameModeRecordLapCompletedAfter(ServerPlayerGameMode* self, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLapCompleted_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(1, result);
}

void ServerPlayerGameModeRecordThermalEnteredAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordThermalEntered_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeRecordProgressAfter(ServerPlayerGameMode* self, float first, float second, float third, float fourth, bool& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordProgress_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(first);
    hook.Add(second);
    hook.Add(third);
    hook.Add(fourth);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(5, result);
}

void ServerPlayerGameModeRecordSpawnedAtCheckpointAfter(ServerPlayerGameMode* self, int checkpoint)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSpawnedAtCheckpoint_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(checkpoint);
    hook.Run();
}

void ServerPlayerGameModeRecordBlockPlacedAfter(ServerPlayerGameMode* self, int x, int y, int z, int blockId)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordBlockPlaced_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(blockId);
    hook.Run();
}

void ServerPlayerGameModeRecordBlockDestroyedAfter(ServerPlayerGameMode* self, int x, int y, int z, int blockId)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordBlockDestroyed_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(blockId);
    hook.Run();
}

void ServerPlayerGameModeRecordBlockPresentAfter(ServerPlayerGameMode* self, int x, int y, int z, int blockId)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordBlockPresent_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(blockId);
    hook.Run();
}

void ServerPlayerGameModeRecordLayerTimeAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLayerTime_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeRecordSnowballHitAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordSnowballHit_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeRecordHitBySnowballAfter(ServerPlayerGameMode* self)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordHitBySnowball_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Run();
}

void ServerPlayerGameModeRecordLandingAfter(ServerPlayerGameMode* self, int x, int y, int z)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_RecordLanding_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Run();
}

void ServerPlayerGameModeGetMiniGameAfter(ServerPlayerGameMode* self, MiniGameDef*& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_GetMiniGame_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_MiniGameDef, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        MiniGameDef* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

void ServerPlayerGameModeGetLevelAfter(ServerPlayerGameMode* self, Level*& result)
{
    ModHookCall hook(kModHook_ServerPlayerGameMode_GetLevel_after);
    hook.AddObject(mod::api::eHandle_ServerPlayerGameMode, ModHandles::Add(self));
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_level, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        Level* object = NULL;
        result = hook.GetHandle(1, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

bool PlayerListRemove(PlayerList* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_PlayerList_remove);
    hook.AddObject(mod::api::eHandle_PlayerList, ModHandles::Add(self));
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

bool PlayerListAdd(PlayerList* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_PlayerList_add);
    hook.AddObject(mod::api::eHandle_PlayerList, ModHandles::Add(self));
    hook.Add(player);
    const bool proceed = hook.Run();
    return proceed;
}

void PlayerListRemoveAfter(PlayerList* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_PlayerList_remove_after);
    hook.AddObject(mod::api::eHandle_PlayerList, ModHandles::Add(self));
    hook.Add(player);
    hook.Run();
}

void PlayerListAddAfter(PlayerList* self, const boost::shared_ptr<Entity>& player)
{
    ModHookCall hook(kModHook_PlayerList_add_after);
    hook.AddObject(mod::api::eHandle_PlayerList, ModHandles::Add(self));
    hook.Add(player);
    hook.Run();
}

bool MinecraftServerTick(MinecraftServer* self)
{
    ModHookCall hook(kModHook_MinecraftServer_tick);
    hook.AddObject(mod::api::eHandle_MinecraftServer, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

void MinecraftServerTickAfter(MinecraftServer* self)
{
    ModHookCall hook(kModHook_MinecraftServer_tick_after);
    hook.AddObject(mod::api::eHandle_MinecraftServer, ModHandles::Add(self));
    hook.Run();
}

bool LevelGetBrightness(Level* self, int layer, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_Level_getBrightness);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(layer);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(5, result);
    return proceed;
}

bool LevelGetBrightness2(Level* self, int x, int y, int z, int& lightValue, float& result)
{
    ModHookCall hook(kModHook_Level_getBrightness_2);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(lightValue);
    hook.Add(result);
    const bool proceed = hook.Run();
    lightValue = hook.GetInt(4, lightValue);
    result = hook.GetFloat(5, result);
    return proceed;
}

bool LevelGetBrightness3(Level* self, int x, int y, int z, float& result)
{
    ModHookCall hook(kModHook_Level_getBrightness_3);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetFloat(4, result);
    return proceed;
}

bool LevelPlaySound(Level* self, const boost::shared_ptr<Entity>& player, int x, int y, int z, SoundEvent* sound, int source, float& volume, float& pitch, float& range)
{
    ModHookCall hook(kModHook_Level_playSound);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(player);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddObject(mod::api::eHandle_SoundEvent, ModHandles::Add(sound));
    hook.Add(source);
    hook.Add(volume);
    hook.Add(pitch);
    hook.Add(range);
    const bool proceed = hook.Run();
    volume = hook.GetFloat(7, volume);
    pitch = hook.GetFloat(8, pitch);
    range = hook.GetFloat(9, range);
    return proceed;
}

bool LevelPlaySound2(Level* self, const boost::shared_ptr<Entity>& player, float x, float y, float z, SoundEvent* sound, int source, float& volume, float& pitch, float& range)
{
    ModHookCall hook(kModHook_Level_playSound_2);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(player);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddObject(mod::api::eHandle_SoundEvent, ModHandles::Add(sound));
    hook.Add(source);
    hook.Add(volume);
    hook.Add(pitch);
    hook.Add(range);
    const bool proceed = hook.Run();
    volume = hook.GetFloat(7, volume);
    pitch = hook.GetFloat(8, pitch);
    range = hook.GetFloat(9, range);
    return proceed;
}

bool LevelAddEntity(Level* self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Level_addEntity);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

bool LevelGetSkyColor(Level* self, const boost::shared_ptr<Entity>& entity, float& partialTick, Vec3*& result)
{
    ModHookCall hook(kModHook_Level_getSkyColor);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(partialTick);
    hook.AddNil();
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(2, partialTick);
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

void LevelGetBrightnessAfter(Level* self, int layer, int x, int y, int z, int& result)
{
    ModHookCall hook(kModHook_Level_getBrightness_after);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(layer);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetInt(5, result);
}

void LevelGetBrightness2After(Level* self, int x, int y, int z, int lightValue, float& result)
{
    ModHookCall hook(kModHook_Level_getBrightness_2_after);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(lightValue);
    hook.Add(result);
    hook.Run();
    result = hook.GetFloat(5, result);
}

void LevelGetBrightness3After(Level* self, int x, int y, int z, float& result)
{
    ModHookCall hook(kModHook_Level_getBrightness_3_after);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Add(result);
    hook.Run();
    result = hook.GetFloat(4, result);
}

void LevelPlaySoundAfter(Level* self, const boost::shared_ptr<Entity>& player, int x, int y, int z, SoundEvent* sound, int source, float volume, float pitch, float range)
{
    ModHookCall hook(kModHook_Level_playSound_after);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(player);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddObject(mod::api::eHandle_SoundEvent, ModHandles::Add(sound));
    hook.Add(source);
    hook.Add(volume);
    hook.Add(pitch);
    hook.Add(range);
    hook.Run();
}

void LevelPlaySound2After(Level* self, const boost::shared_ptr<Entity>& player, float x, float y, float z, SoundEvent* sound, int source, float volume, float pitch, float range)
{
    ModHookCall hook(kModHook_Level_playSound_2_after);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(player);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.AddObject(mod::api::eHandle_SoundEvent, ModHandles::Add(sound));
    hook.Add(source);
    hook.Add(volume);
    hook.Add(pitch);
    hook.Add(range);
    hook.Run();
}

void LevelAddEntityAfter(Level* self, const boost::shared_ptr<Entity>& entity, bool& result)
{
    ModHookCall hook(kModHook_Level_addEntity_after);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(2, result);
}

void LevelGetSkyColorAfter(Level* self, const boost::shared_ptr<Entity>& entity, float partialTick, Vec3*& result)
{
    ModHookCall hook(kModHook_Level_getSkyColor_after);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(partialTick);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_Vec3, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        Vec3* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

bool FlatLevelSourceCreateChunk(FlatLevelSource* self, int& chunkX, int& chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_FlatLevelSource_createChunk);
    hook.AddObject(mod::api::eHandle_FlatLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    hook.AddNil();
    const bool proceed = hook.Run();
    chunkX = hook.GetInt(1, chunkX);
    chunkZ = hook.GetInt(2, chunkZ);
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
    return proceed;
}

void FlatLevelSourceCreateChunkAfter(FlatLevelSource* self, int chunkX, int chunkZ, LevelChunk*& result)
{
    ModHookCall hook(kModHook_FlatLevelSource_createChunk_after);
    hook.AddObject(mod::api::eHandle_FlatLevelSource, ModHandles::Add(self));
    hook.Add(chunkX);
    hook.Add(chunkZ);
    if (result != NULL)
        hook.AddObject(mod::api::eHandle_chunk, ModHandles::Add(result));
    else
        hook.AddNil();
    hook.Run();
    {
        uint32_t id = 0;
        LevelChunk* object = NULL;
        result = hook.GetHandle(3, &id) && ModHandles::Get(id, &object) ? object : NULL;
    }
}

bool ServerGamePacketListenerImplHandleChat(ServerGamePacketListenerImpl* self, ClientboundChatPacket* packet)
{
    ModHookCall hook(kModHook_ServerGamePacketListenerImpl_handleChat);
    hook.AddObject(mod::api::eHandle_ServerGamePacketListenerImpl, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ClientboundChatPacket, ModHandles::Add(packet));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerGamePacketListenerImplHandleChat2(ServerGamePacketListenerImpl* self, ServerboundChatPacket* packet)
{
    ModHookCall hook(kModHook_ServerGamePacketListenerImpl_handleChat_2);
    hook.AddObject(mod::api::eHandle_ServerGamePacketListenerImpl, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerboundChatPacket, ModHandles::Add(packet));
    const bool proceed = hook.Run();
    return proceed;
}

bool ServerGamePacketListenerImplHandleCustomPayload(ServerGamePacketListenerImpl* self, ServerboundCustomPayloadPacket* packet)
{
    ModHookCall hook(kModHook_ServerGamePacketListenerImpl_handleCustomPayload);
    hook.AddObject(mod::api::eHandle_ServerGamePacketListenerImpl, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerboundCustomPayloadPacket, ModHandles::Add(packet));
    const bool proceed = hook.Run();
    return proceed;
}

void ServerGamePacketListenerImplHandleChatAfter(ServerGamePacketListenerImpl* self, ClientboundChatPacket* packet)
{
    ModHookCall hook(kModHook_ServerGamePacketListenerImpl_handleChat_after);
    hook.AddObject(mod::api::eHandle_ServerGamePacketListenerImpl, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ClientboundChatPacket, ModHandles::Add(packet));
    hook.Run();
}

void ServerGamePacketListenerImplHandleChat2After(ServerGamePacketListenerImpl* self, ServerboundChatPacket* packet)
{
    ModHookCall hook(kModHook_ServerGamePacketListenerImpl_handleChat_2_after);
    hook.AddObject(mod::api::eHandle_ServerGamePacketListenerImpl, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerboundChatPacket, ModHandles::Add(packet));
    hook.Run();
}

void ServerGamePacketListenerImplHandleCustomPayloadAfter(ServerGamePacketListenerImpl* self, ServerboundCustomPayloadPacket* packet)
{
    ModHookCall hook(kModHook_ServerGamePacketListenerImpl_handleCustomPayload_after);
    hook.AddObject(mod::api::eHandle_ServerGamePacketListenerImpl, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_ServerboundCustomPayloadPacket, ModHandles::Add(packet));
    hook.Run();
}

bool ConsoleUIControllerNavigateToScene(ConsoleUIController* self, int& playerId, int scene, int layer, int groupId, bool& result)
{
    ModHookCall hook(kModHook_ConsoleUIController_NavigateToScene);
    hook.AddObject(mod::api::eHandle_ConsoleUIController, ModHandles::Add(self));
    hook.Add(playerId);
    hook.Add(scene);
    hook.Add(layer);
    hook.Add(groupId);
    hook.Add(result);
    const bool proceed = hook.Run();
    playerId = hook.GetInt(1, playerId);
    result = hook.GetBool(5, result);
    return proceed;
}

void ConsoleUIControllerNavigateToSceneAfter(ConsoleUIController* self, int playerId, int scene, int layer, int groupId, bool& result)
{
    ModHookCall hook(kModHook_ConsoleUIController_NavigateToScene_after);
    hook.AddObject(mod::api::eHandle_ConsoleUIController, ModHandles::Add(self));
    hook.Add(playerId);
    hook.Add(scene);
    hook.Add(layer);
    hook.Add(groupId);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(5, result);
}

bool LivingEntityHurt(const boost::shared_ptr<Entity>& self, DamageSource* source, float& damage, bool& result)
{
    ModHookCall hook(kModHook_LivingEntity_hurt);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(damage);
    hook.Add(result);
    const bool proceed = hook.Run();
    damage = hook.GetFloat(2, damage);
    result = hook.GetBool(3, result);
    return proceed;
}

bool LivingEntityDie(const boost::shared_ptr<Entity>& self, DamageSource* source)
{
    ModHookCall hook(kModHook_LivingEntity_die);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    const bool proceed = hook.Run();
    return proceed;
}

void LivingEntityHurtAfter(const boost::shared_ptr<Entity>& self, DamageSource* source, float damage, bool& result)
{
    ModHookCall hook(kModHook_LivingEntity_hurt_after);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Add(damage);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(3, result);
}

void LivingEntityDieAfter(const boost::shared_ptr<Entity>& self, DamageSource* source)
{
    ModHookCall hook(kModHook_LivingEntity_die_after);
    hook.Add(self);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Run();
}

bool GameRendererPrepareAndRenderClouds(GameRenderer* self, LevelRenderer* levelRenderer, float& partialTick, int& eyeIndex, float x, float y, float z)
{
    ModHookCall hook(kModHook_GameRenderer_prepareAndRenderClouds);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(levelRenderer));
    hook.Add(partialTick);
    hook.Add(eyeIndex);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(2, partialTick);
    eyeIndex = hook.GetInt(3, eyeIndex);
    return proceed;
}

bool GameRendererRenderSnowAndRain(GameRenderer* self, float& partialTick)
{
    ModHookCall hook(kModHook_GameRenderer_renderSnowAndRain);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(1, partialTick);
    return proceed;
}

bool GameRendererRenderWeather(GameRenderer* self, ModHandles::WorldInfo world, Level* level, float& partialTick)
{
    ModHookCall hook(kModHook_GameRenderer_renderWeather);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.Add(world);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(partialTick);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(3, partialTick);
    return proceed;
}

bool GameRendererRender(GameRenderer* self, float& partialTick, int frameNanoTime, bool& refreshLightTexture)
{
    ModHookCall hook(kModHook_GameRenderer_render);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    hook.Add(frameNanoTime);
    hook.Add(refreshLightTexture);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(1, partialTick);
    refreshLightTexture = hook.GetBool(3, refreshLightTexture);
    return proceed;
}

void GameRendererPrepareAndRenderCloudsAfter(GameRenderer* self, LevelRenderer* levelRenderer, float partialTick, int eyeIndex, float x, float y, float z)
{
    ModHookCall hook(kModHook_GameRenderer_prepareAndRenderClouds_after);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(levelRenderer));
    hook.Add(partialTick);
    hook.Add(eyeIndex);
    hook.Add(x);
    hook.Add(y);
    hook.Add(z);
    hook.Run();
}

void GameRendererRenderSnowAndRainAfter(GameRenderer* self, float partialTick)
{
    ModHookCall hook(kModHook_GameRenderer_renderSnowAndRain_after);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    hook.Run();
}

void GameRendererRenderWeatherAfter(GameRenderer* self, ModHandles::WorldInfo world, Level* level, float partialTick)
{
    ModHookCall hook(kModHook_GameRenderer_renderWeather_after);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.Add(world);
    hook.AddObject(mod::api::eHandle_level, ModHandles::Add(level));
    hook.Add(partialTick);
    hook.Run();
}

void GameRendererRenderAfter(GameRenderer* self, float partialTick, int frameNanoTime, bool refreshLightTexture)
{
    ModHookCall hook(kModHook_GameRenderer_render_after);
    hook.AddObject(mod::api::eHandle_GameRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    hook.Add(frameNanoTime);
    hook.Add(refreshLightTexture);
    hook.Run();
}

bool LevelRendererRenderEntities(LevelRenderer* self, const boost::shared_ptr<Entity>& camera, Culler* culler, float& partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderEntities);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(camera);
    hook.AddObject(mod::api::eHandle_Culler, ModHandles::Add(culler));
    hook.Add(partialTick);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(3, partialTick);
    return proceed;
}

bool LevelRendererRender(LevelRenderer* self, const boost::shared_ptr<Entity>& camera, int& layer, float partialTick, bool& sort)
{
    ModHookCall hook(kModHook_LevelRenderer_render);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(camera);
    hook.Add(layer);
    hook.Add(partialTick);
    hook.Add(sort);
    const bool proceed = hook.Run();
    layer = hook.GetInt(2, layer);
    sort = hook.GetBool(4, sort);
    return proceed;
}

bool LevelRendererRenderSky(LevelRenderer* self, float& partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderSky);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(1, partialTick);
    return proceed;
}

bool LevelRendererRenderHaloRing(LevelRenderer* self, float& partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderHaloRing);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(1, partialTick);
    return proceed;
}

bool LevelRendererRenderDestroyAnimation(LevelRenderer* self, Tesselator* tesselator, const boost::shared_ptr<Entity>& player, float& partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderDestroyAnimation);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_Tesselator, ModHandles::Add(tesselator));
    hook.Add(player);
    hook.Add(partialTick);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(3, partialTick);
    return proceed;
}

void LevelRendererRenderEntitiesAfter(LevelRenderer* self, const boost::shared_ptr<Entity>& camera, Culler* culler, float partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderEntities_after);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(camera);
    hook.AddObject(mod::api::eHandle_Culler, ModHandles::Add(culler));
    hook.Add(partialTick);
    hook.Run();
}

void LevelRendererRenderAfter(LevelRenderer* self, const boost::shared_ptr<Entity>& camera, int layer, float partialTick, bool sort)
{
    ModHookCall hook(kModHook_LevelRenderer_render_after);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(camera);
    hook.Add(layer);
    hook.Add(partialTick);
    hook.Add(sort);
    hook.Run();
}

void LevelRendererRenderSkyAfter(LevelRenderer* self, float partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderSky_after);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    hook.Run();
}

void LevelRendererRenderHaloRingAfter(LevelRenderer* self, float partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderHaloRing_after);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.Add(partialTick);
    hook.Run();
}

void LevelRendererRenderDestroyAnimationAfter(LevelRenderer* self, Tesselator* tesselator, const boost::shared_ptr<Entity>& player, float partialTick)
{
    ModHookCall hook(kModHook_LevelRenderer_renderDestroyAnimation_after);
    hook.AddObject(mod::api::eHandle_LevelRenderer, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_Tesselator, ModHandles::Add(tesselator));
    hook.Add(player);
    hook.Add(partialTick);
    hook.Run();
}

bool ParticleEngineRender(ParticleEngine* self, const boost::shared_ptr<Entity>& entity, float& partialTick, int& layer)
{
    ModHookCall hook(kModHook_ParticleEngine_render);
    hook.AddObject(mod::api::eHandle_ParticleEngine, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(partialTick);
    hook.Add(layer);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(2, partialTick);
    layer = hook.GetInt(3, layer);
    return proceed;
}

bool ParticleEngineRenderLit(ParticleEngine* self, const boost::shared_ptr<Entity>& entity, float& partialTick, int& layer)
{
    ModHookCall hook(kModHook_ParticleEngine_renderLit);
    hook.AddObject(mod::api::eHandle_ParticleEngine, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(partialTick);
    hook.Add(layer);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(2, partialTick);
    layer = hook.GetInt(3, layer);
    return proceed;
}

void ParticleEngineRenderAfter(ParticleEngine* self, const boost::shared_ptr<Entity>& entity, float partialTick, int layer)
{
    ModHookCall hook(kModHook_ParticleEngine_render_after);
    hook.AddObject(mod::api::eHandle_ParticleEngine, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(partialTick);
    hook.Add(layer);
    hook.Run();
}

void ParticleEngineRenderLitAfter(ParticleEngine* self, const boost::shared_ptr<Entity>& entity, float partialTick, int layer)
{
    ModHookCall hook(kModHook_ParticleEngine_renderLit_after);
    hook.AddObject(mod::api::eHandle_ParticleEngine, ModHandles::Add(self));
    hook.Add(entity);
    hook.Add(partialTick);
    hook.Add(layer);
    hook.Run();
}

bool LootTableFill(LootTable* self, Container* container, LootTableContext* context)
{
    ModHookCall hook(kModHook_LootTable_fill);
    hook.AddObject(mod::api::eHandle_LootTable, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_Container, ModHandles::Add(container));
    hook.AddObject(mod::api::eHandle_LootTableContext, ModHandles::Add(context));
    const bool proceed = hook.Run();
    return proceed;
}

void LootTableFillAfter(LootTable* self, Container* container, LootTableContext* context)
{
    ModHookCall hook(kModHook_LootTable_fill_after);
    hook.AddObject(mod::api::eHandle_LootTable, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_Container, ModHandles::Add(container));
    hook.AddObject(mod::api::eHandle_LootTableContext, ModHandles::Add(context));
    hook.Run();
}

bool MobDropDeathLoot(const boost::shared_ptr<Entity>& self, bool& lceArg0, int& lootingLevel)
{
    ModHookCall hook(kModHook_Mob_dropDeathLoot);
    hook.Add(self);
    hook.Add(lceArg0);
    hook.Add(lootingLevel);
    const bool proceed = hook.Run();
    lceArg0 = hook.GetBool(1, lceArg0);
    lootingLevel = hook.GetInt(2, lootingLevel);
    return proceed;
}

bool MobDropAllDeathLoot(const boost::shared_ptr<Entity>& self, bool& hitByPlayer, int& lootingLevel, DamageSource* source)
{
    ModHookCall hook(kModHook_Mob_dropAllDeathLoot);
    hook.Add(self);
    hook.Add(hitByPlayer);
    hook.Add(lootingLevel);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    const bool proceed = hook.Run();
    hitByPlayer = hook.GetBool(1, hitByPlayer);
    lootingLevel = hook.GetInt(2, lootingLevel);
    return proceed;
}

void MobDropDeathLootAfter(const boost::shared_ptr<Entity>& self, bool lceArg0, int lootingLevel)
{
    ModHookCall hook(kModHook_Mob_dropDeathLoot_after);
    hook.Add(self);
    hook.Add(lceArg0);
    hook.Add(lootingLevel);
    hook.Run();
}

void MobDropAllDeathLootAfter(const boost::shared_ptr<Entity>& self, bool hitByPlayer, int lootingLevel, DamageSource* source)
{
    ModHookCall hook(kModHook_Mob_dropAllDeathLoot_after);
    hook.Add(self);
    hook.Add(hitByPlayer);
    hook.Add(lootingLevel);
    hook.AddObject(mod::api::eHandle_DamageSource, ModHandles::Add(source));
    hook.Run();
}

bool FishingHookRetrieve(FishingHook* self, int& result)
{
    ModHookCall hook(kModHook_FishingHook_retrieve);
    hook.AddObject(mod::api::eHandle_FishingHook, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetInt(1, result);
    return proceed;
}

void FishingHookRetrieveAfter(FishingHook* self, int& result)
{
    ModHookCall hook(kModHook_FishingHook_retrieve_after);
    hook.AddObject(mod::api::eHandle_FishingHook, ModHandles::Add(self));
    hook.Add(result);
    hook.Run();
    result = hook.GetInt(1, result);
}

bool GuiRender(Gui* self, float& partialTick, bool& hideGui, int& width, int& height)
{
    ModHookCall hook(kModHook_Gui_render);
    hook.AddObject(mod::api::eHandle_Gui, ModHandles::Add(self));
    hook.Add(partialTick);
    hook.Add(hideGui);
    hook.Add(width);
    hook.Add(height);
    const bool proceed = hook.Run();
    partialTick = hook.GetFloat(1, partialTick);
    hideGui = hook.GetBool(2, hideGui);
    width = hook.GetInt(3, width);
    height = hook.GetInt(4, height);
    return proceed;
}

bool GuiRenderOverlay(Gui* self, const ModHandles::GuiInfo& hud, int& playerIndex)
{
    ModHookCall hook(kModHook_Gui_renderOverlay);
    hook.AddObject(mod::api::eHandle_Gui, ModHandles::Add(self));
    hook.Add(hud);
    hook.Add(playerIndex);
    const bool proceed = hook.Run();
    playerIndex = hook.GetInt(2, playerIndex);
    return proceed;
}

bool GuiRenderChatLine(Gui* self, const ModHandles::GuiInfo& display, const std::wstring& text, int& left, int& y, int& width, int& height, int& message, int& first, int& indent, int colour)
{
    ModHookCall hook(kModHook_Gui_renderChatLine);
    hook.AddObject(mod::api::eHandle_Gui, ModHandles::Add(self));
    hook.Add(display);
    hook.Add(text);
    hook.Add(left);
    hook.Add(y);
    hook.Add(width);
    hook.Add(height);
    hook.Add(message);
    hook.Add(first);
    hook.Add(indent);
    hook.Add(colour);
    const bool proceed = hook.Run();
    left = hook.GetInt(3, left);
    y = hook.GetInt(4, y);
    width = hook.GetInt(5, width);
    height = hook.GetInt(6, height);
    message = hook.GetInt(7, message);
    first = hook.GetInt(8, first);
    indent = hook.GetInt(9, indent);
    return proceed;
}

void GuiRenderAfter(Gui* self, float partialTick, bool hideGui, int width, int height)
{
    ModHookCall hook(kModHook_Gui_render_after);
    hook.AddObject(mod::api::eHandle_Gui, ModHandles::Add(self));
    hook.Add(partialTick);
    hook.Add(hideGui);
    hook.Add(width);
    hook.Add(height);
    hook.Run();
}

void GuiRenderOverlayAfter(Gui* self, const ModHandles::GuiInfo& hud, int playerIndex)
{
    ModHookCall hook(kModHook_Gui_renderOverlay_after);
    hook.AddObject(mod::api::eHandle_Gui, ModHandles::Add(self));
    hook.Add(hud);
    hook.Add(playerIndex);
    hook.Run();
}

void GuiRenderChatLineAfter(Gui* self, const ModHandles::GuiInfo& display, const std::wstring& text, int left, int y, int width, int height, int message, int first, int indent, int colour)
{
    ModHookCall hook(kModHook_Gui_renderChatLine_after);
    hook.AddObject(mod::api::eHandle_Gui, ModHandles::Add(self));
    hook.Add(display);
    hook.Add(text);
    hook.Add(left);
    hook.Add(y);
    hook.Add(width);
    hook.Add(height);
    hook.Add(message);
    hook.Add(first);
    hook.Add(indent);
    hook.Add(colour);
    hook.Run();
}

bool MinecraftTick(Minecraft* self, bool& handleInput, bool& lastTickOfFrame)
{
    ModHookCall hook(kModHook_Minecraft_tick);
    hook.AddObject(mod::api::eHandle_Minecraft, ModHandles::Add(self));
    hook.Add(handleInput);
    hook.Add(lastTickOfFrame);
    const bool proceed = hook.Run();
    handleInput = hook.GetBool(1, handleInput);
    lastTickOfFrame = hook.GetBool(2, lastTickOfFrame);
    return proceed;
}

void MinecraftTickAfter(Minecraft* self, bool handleInput, bool lastTickOfFrame)
{
    ModHookCall hook(kModHook_Minecraft_tick_after);
    hook.AddObject(mod::api::eHandle_Minecraft, ModHandles::Add(self));
    hook.Add(handleInput);
    hook.Add(lastTickOfFrame);
    hook.Run();
}

bool ModScreenDraw(int& pad, const ModHandles::GuiInfo& gui, int& width, int& height)
{
    ModHookCall hook(kModHook_ModScreen_Draw);
    hook.Add(pad);
    hook.Add(gui);
    hook.Add(width);
    hook.Add(height);
    const bool proceed = hook.Run();
    pad = hook.GetInt(0, pad);
    width = hook.GetInt(2, width);
    height = hook.GetInt(3, height);
    return proceed;
}

bool ModScreenInput(int& pad, const std::wstring& kind, int& x, int& y, int& button, const std::wstring& text, bool& result)
{
    ModHookCall hook(kModHook_ModScreen_Input);
    hook.Add(pad);
    hook.Add(kind);
    hook.Add(x);
    hook.Add(y);
    hook.Add(button);
    hook.Add(text);
    hook.Add(result);
    const bool proceed = hook.Run();
    pad = hook.GetInt(0, pad);
    x = hook.GetInt(2, x);
    y = hook.GetInt(3, y);
    button = hook.GetInt(4, button);
    result = hook.GetBool(6, result);
    return proceed;
}

void ModScreenDrawAfter(int pad, const ModHandles::GuiInfo& gui, int width, int height)
{
    ModHookCall hook(kModHook_ModScreen_Draw_after);
    hook.Add(pad);
    hook.Add(gui);
    hook.Add(width);
    hook.Add(height);
    hook.Run();
}

void ModScreenInputAfter(int pad, const std::wstring& kind, int x, int y, int button, const std::wstring& text, bool& result)
{
    ModHookCall hook(kModHook_ModScreen_Input_after);
    hook.Add(pad);
    hook.Add(kind);
    hook.Add(x);
    hook.Add(y);
    hook.Add(button);
    hook.Add(text);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(6, result);
}

bool ChatScreenPointer(int& pad, int& message, const std::wstring& kind, int& x, int& y, int& button)
{
    ModHookCall hook(kModHook_ChatScreen_Pointer);
    hook.Add(pad);
    hook.Add(message);
    hook.Add(kind);
    hook.Add(x);
    hook.Add(y);
    hook.Add(button);
    const bool proceed = hook.Run();
    pad = hook.GetInt(0, pad);
    message = hook.GetInt(1, message);
    x = hook.GetInt(3, x);
    y = hook.GetInt(4, y);
    button = hook.GetInt(5, button);
    return proceed;
}

void ChatScreenPointerAfter(int pad, int message, const std::wstring& kind, int x, int y, int button)
{
    ModHookCall hook(kModHook_ChatScreen_Pointer_after);
    hook.Add(pad);
    hook.Add(message);
    hook.Add(kind);
    hook.Add(x);
    hook.Add(y);
    hook.Add(button);
    hook.Run();
}

bool ItemStaticCtor()
{
    ModHookCall hook(kModHook_Item_staticCtor);
    const bool proceed = hook.Run();
    return proceed;
}

void ItemStaticCtorAfter()
{
    ModHookCall hook(kModHook_Item_staticCtor_after);
    hook.Run();
}

bool UISceneCustomMenuHandlePress(UIScene_CustomMenu* self, int& lceArg0, int& childId)
{
    ModHookCall hook(kModHook_UIScene_CustomMenu_handlePress);
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(self));
    hook.Add(lceArg0);
    hook.Add(childId);
    const bool proceed = hook.Run();
    lceArg0 = hook.GetInt(1, lceArg0);
    childId = hook.GetInt(2, childId);
    return proceed;
}

bool UISceneCustomMenuHandleCheckboxElementToggled(UIScene_CustomMenu* self, int& lceArg0, int& childId, bool& checked)
{
    ModHookCall hook(kModHook_UIScene_CustomMenu_handleCheckboxElementToggled);
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(self));
    hook.Add(lceArg0);
    hook.Add(childId);
    hook.Add(checked);
    const bool proceed = hook.Run();
    lceArg0 = hook.GetInt(1, lceArg0);
    childId = hook.GetInt(2, childId);
    checked = hook.GetBool(3, checked);
    return proceed;
}

bool UISceneCustomMenuHandleSliderElementMove(UIScene_CustomMenu* self, int& lceArg0, int& childId, int& value)
{
    ModHookCall hook(kModHook_UIScene_CustomMenu_handleSliderElementMove);
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(self));
    hook.Add(lceArg0);
    hook.Add(childId);
    hook.Add(value);
    const bool proceed = hook.Run();
    lceArg0 = hook.GetInt(1, lceArg0);
    childId = hook.GetInt(2, childId);
    value = hook.GetInt(3, value);
    return proceed;
}

void UISceneCustomMenuHandlePressAfter(UIScene_CustomMenu* self, int lceArg0, int childId)
{
    ModHookCall hook(kModHook_UIScene_CustomMenu_handlePress_after);
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(self));
    hook.Add(lceArg0);
    hook.Add(childId);
    hook.Run();
}

void UISceneCustomMenuHandleCheckboxElementToggledAfter(UIScene_CustomMenu* self, int lceArg0, int childId, bool checked)
{
    ModHookCall hook(kModHook_UIScene_CustomMenu_handleCheckboxElementToggled_after);
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(self));
    hook.Add(lceArg0);
    hook.Add(childId);
    hook.Add(checked);
    hook.Run();
}

void UISceneCustomMenuHandleSliderElementMoveAfter(UIScene_CustomMenu* self, int lceArg0, int childId, int value)
{
    ModHookCall hook(kModHook_UIScene_CustomMenu_handleSliderElementMove_after);
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(self));
    hook.Add(lceArg0);
    hook.Add(childId);
    hook.Add(value);
    hook.Run();
}

bool UIMoreOptionsPageBuild(UIMoreOptionsPage* self, UIScene_CustomMenu* menu)
{
    ModHookCall hook(kModHook_UIMoreOptionsPage_Build);
    hook.AddObject(mod::api::eHandle_UIMoreOptionsPage, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(menu));
    const bool proceed = hook.Run();
    return proceed;
}

void UIMoreOptionsPageBuildAfter(UIMoreOptionsPage* self, UIScene_CustomMenu* menu)
{
    ModHookCall hook(kModHook_UIMoreOptionsPage_Build_after);
    hook.AddObject(mod::api::eHandle_UIMoreOptionsPage, ModHandles::Add(self));
    hook.AddObject(mod::api::eHandle_UIScene_CustomMenu, ModHandles::Add(menu));
    hook.Run();
}

bool ScriptedGoalCanUse(ScriptedGoal* self, bool& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_canUse);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ScriptedGoalCanContinueToUse(ScriptedGoal* self, bool& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_canContinueToUse);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(1, result);
    return proceed;
}

bool ScriptedGoalStart(ScriptedGoal* self)
{
    ModHookCall hook(kModHook_ScriptedGoal_start);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ScriptedGoalStop(ScriptedGoal* self)
{
    ModHookCall hook(kModHook_ScriptedGoal_stop);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ScriptedGoalTick(ScriptedGoal* self)
{
    ModHookCall hook(kModHook_ScriptedGoal_tick);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

bool ScriptedGoalGetName(ScriptedGoal* self, std::wstring& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_getName);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetString(1, result);
    return proceed;
}

bool ScriptedGoalRemove(GoalSelector* selector, const std::wstring& name, bool& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_Remove);
    hook.AddObject(mod::api::eHandle_GoalSelector, ModHandles::Add(selector));
    hook.Add(name);
    hook.Add(result);
    const bool proceed = hook.Run();
    result = hook.GetBool(2, result);
    return proceed;
}

void ScriptedGoalCanUseAfter(ScriptedGoal* self, bool& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_canUse_after);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(1, result);
}

void ScriptedGoalCanContinueToUseAfter(ScriptedGoal* self, bool& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_canContinueToUse_after);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(1, result);
}

void ScriptedGoalStartAfter(ScriptedGoal* self)
{
    ModHookCall hook(kModHook_ScriptedGoal_start_after);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Run();
}

void ScriptedGoalStopAfter(ScriptedGoal* self)
{
    ModHookCall hook(kModHook_ScriptedGoal_stop_after);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Run();
}

void ScriptedGoalTickAfter(ScriptedGoal* self)
{
    ModHookCall hook(kModHook_ScriptedGoal_tick_after);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Run();
}

void ScriptedGoalGetNameAfter(ScriptedGoal* self, std::wstring& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_getName_after);
    hook.AddObject(mod::api::eHandle_ScriptedGoal, ModHandles::Add(self));
    hook.Add(result);
    hook.Run();
    result = hook.GetString(1, result);
}

void ScriptedGoalRemoveAfter(GoalSelector* selector, const std::wstring& name, bool& result)
{
    ModHookCall hook(kModHook_ScriptedGoal_Remove_after);
    hook.AddObject(mod::api::eHandle_GoalSelector, ModHandles::Add(selector));
    hook.Add(name);
    hook.Add(result);
    hook.Run();
    result = hook.GetBool(2, result);
}

bool ScriptedBlockEntityTick(ScriptedBlockEntity* self)
{
    ModHookCall hook(kModHook_ScriptedBlockEntity_tick);
    hook.AddObject(mod::api::eHandle_ScriptedBlockEntity, ModHandles::Add(self));
    const bool proceed = hook.Run();
    return proceed;
}

void ScriptedBlockEntityTickAfter(ScriptedBlockEntity* self)
{
    ModHookCall hook(kModHook_ScriptedBlockEntity_tick_after);
    hook.AddObject(mod::api::eHandle_ScriptedBlockEntity, ModHandles::Add(self));
    hook.Run();
}

bool BiomeInitLayerGetArea(BiomeInitLayer* self, int& x, int& z, int& width, int& height, std::string& result)
{
    ModHookCall hook(kModHook_BiomeInitLayer_getArea);
    hook.AddObject(mod::api::eHandle_BiomeInitLayer, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(width);
    hook.Add(height);
    hook.Add(result);
    const bool proceed = hook.Run();
    x = hook.GetInt(1, x);
    z = hook.GetInt(2, z);
    width = hook.GetInt(3, width);
    height = hook.GetInt(4, height);
    result = hook.GetBytes(5, result);
    return proceed;
}

void BiomeInitLayerGetAreaAfter(BiomeInitLayer* self, int x, int z, int width, int height, std::string& result)
{
    ModHookCall hook(kModHook_BiomeInitLayer_getArea_after);
    hook.AddObject(mod::api::eHandle_BiomeInitLayer, ModHandles::Add(self));
    hook.Add(x);
    hook.Add(z);
    hook.Add(width);
    hook.Add(height);
    hook.Add(result);
    hook.Run();
    result = hook.GetBytes(5, result);
}

bool ScriptedSoundEngineTick(ScriptedSoundEngine* self, float& lceArg1)
{
    ModHookCall hook(kModHook_ScriptedSoundEngine_tick);
    hook.AddObject(mod::api::eHandle_ScriptedSoundEngine, ModHandles::Add(self));
    hook.Add(lceArg1);
    const bool proceed = hook.Run();
    lceArg1 = hook.GetFloat(1, lceArg1);
    return proceed;
}

void ScriptedSoundEngineTickAfter(ScriptedSoundEngine* self, float lceArg1)
{
    ModHookCall hook(kModHook_ScriptedSoundEngine_tick_after);
    hook.AddObject(mod::api::eHandle_ScriptedSoundEngine, ModHandles::Add(self));
    hook.Add(lceArg1);
    hook.Run();
}

}
