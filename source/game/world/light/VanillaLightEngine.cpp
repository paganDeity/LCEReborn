#include "game/world/light/VanillaLightEngine.hpp"

#include "game/core/Mth.hpp"
#include "game/core/PIXNamedEvents.hpp"
#include "game/core/XMem.hpp"
#include "game/world/Dimension.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/block/Blocks.hpp"
#include "game/world/block/MutableBlockPos.hpp"
#include "game/world/chunk/LevelChunk.hpp"
#include "platform/TLSStorage.hpp"

static unsigned int sLightingCacheTlsIndex = TlsAlloc();

VanillaLightEngine::VanillaLightEngine(Level& level) : LightEngine(level), mUpdateQueue(new unsigned int[0x8000])
{
    InitializeCriticalSectionAndSpinCount(&mMutex, 0x1400);
}

VanillaLightEngine::~VanillaLightEngine()
{
    delete[] mUpdateQueue;
    DeleteCriticalSection(&mMutex);
}

void VanillaLightEngine::EnableThreadCache()
{
    TlsSetValue(sLightingCacheTlsIndex, XPhysicalAlloc(0x40000, 0xFFFFFFFF, 0x20000, 0x20000004));
}

void VanillaLightEngine::DestroyThreadCache()
{
    XPhysicalFree(TlsGetValue(sLightingCacheTlsIndex));
}

static int CachedLightBlock(unsigned int chunkLowBits, int localBits, int referenceBlock)
{
    const int referenceChunk = referenceBlock >> 4;
    const int delta = (int)(((chunkLowBits - (unsigned int)referenceChunk) & 0x3F) ^ 0x20) - 0x20;
    return ((referenceChunk + delta) << 4) | (localBits & 0xF);
}

int VanillaLightEngine::getBrightnessCached(unsigned int* cache, LightLayer::variety layer, int x, int y, int z)
{
    if (cache == NULL || (y & 0xFFFFFF00) != 0)
    {
        return mLevel.getBrightness(layer, BlockPos(x, y, z));
    }

    int index = ((x & 0xF) << 8) | ((y & 0xF) << 4) | (z & 0xF);
    unsigned int entry = cache[index];
    unsigned int key = ((unsigned int)(y & 0xF0) << 2) | ((unsigned int)(x & 0x3F0) << 6)
                       | ((unsigned int)(z >> 4) & 0x3F);

    if ((entry & 0xFFFF) == key)
    {
        if ((entry & 0x10000000) != 0)
        {
            return (entry >> 24) & 0xF;
        }

        int brightness = mLevel.getBrightness(layer, BlockPos(x, y, z));
        entry = entry | ((unsigned int)brightness << 24) | 0x10000000;
    }
    else
    {
        if ((entry & 0x80000000) != 0)
        {
            mLevel.setBrightness(layer,
                          BlockPos(CachedLightBlock((entry >> 10) & 0x3F, x, x), ((entry >> 2) & 0xF0) | (y & 0xF),
                                   CachedLightBlock(entry & 0x3F, z, z)),
                          (entry >> 24) & 0xF, true);
        }

        int brightness = mLevel.getBrightness(layer, BlockPos(x, y, z));
        entry = key | 0x10000000 | ((unsigned int)brightness << 24);
    }

    cache[index] = entry;
    return (entry >> 24) & 0xF;
}

int VanillaLightEngine::getEmissionCached(unsigned int* cache, Block* block, const BlockPos& pos)
{
    if (cache == NULL)
    {
        return block->defaultBlockState()->getLightEmission();
    }

    int index = ((pos.x & 0xF) << 8) | ((pos.y & 0xF) << 4) | (pos.z & 0xF);
    unsigned int entry = cache[index];
    unsigned int key = ((unsigned int)(pos.y & 0xF0) << 2) | ((unsigned int)(pos.x & 0x3F0) << 6)
                       | ((unsigned int)(pos.z >> 4) & 0x3F);

    if ((entry & 0xFFFF) == key && (entry & 0x40000000) != 0)
    {
        return (entry >> 16) & 0xF;
    }

    if ((entry & 0xFFFF) != key && (entry & 0x80000000) != 0)
    {
        mLevel.setBrightness(LightLayer::BLOCK,
                      BlockPos(CachedLightBlock((entry >> 10) & 0x3F, pos.x, pos.x), ((entry >> 2) & 0xF0) | (pos.y & 0xF),
                               CachedLightBlock(entry & 0x3F, pos.z, pos.z)),
                      (entry >> 24) & 0xF, true);
    }

    Block* blockAt = mLevel.getBlock(pos);
    Block* extraBlockAt = mLevel.getExtraBlock(pos);
    int emission = blockAt->defaultBlockState()->getLightEmission();
    int blocking = blockAt->defaultBlockState()->getLightBlock();
    int extraBlocking = extraBlockAt->defaultBlockState()->getLightBlock();

    if (blocking < extraBlocking)
    {
        blocking = extraBlocking;
    }

    if ((entry & 0xFFFF) == key)
    {
        entry = entry | 0x60000000 | ((unsigned int)(emission & 0xF) << 16) | ((unsigned int)(blocking & 0xF) << 20);
    }
    else
    {
        entry = key | 0x60000000 | ((unsigned int)(emission & 0xF) << 16) | ((unsigned int)(blocking & 0xF) << 20);
    }

    cache[index] = entry;
    return (entry >> 16) & 0xF;
}

int VanillaLightEngine::getExpectedLight(unsigned int* cache, const BlockPos& pos, LightLayer::variety layer, bool ignoreEmission)
{
    if (layer == LightLayer::SKY && mLevel.canSeeSky(pos))
    {
        return 15;
    }

    const BlockState* state = mLevel.getBlockState(pos);
    const BlockState* extraState = mLevel.getExtraBlockState(pos);

    int light = (layer == LightLayer::SKY) ? 0 : state->getLightEmission();
    int blocking = state->getLightBlock();
    int extraBlocking = extraState->getLightBlock();

    if (blocking < extraBlocking)
    {
        blocking = extraBlocking;
    }

    if (blocking >= 15)
    {
        if (light < 1)
        {
            if (!ignoreEmission && layer != LightLayer::SKY)
            {
                return getEmissionCached(cache, Blocks::air, pos);
            }

            return 0;
        }

        blocking = 1;
    }
    else if (blocking < 1)
    {
        blocking = 1;
    }

    if (light < 14)
    {
        int limit = 15 - blocking;
        int neighbour = getBrightnessCached(cache, layer, pos.x, pos.y, pos.z + 1) - blocking;

        if (light < neighbour)
        {
            light = neighbour;
        }

        if (light < limit)
        {
            neighbour = getBrightnessCached(cache, layer, pos.x, pos.y, pos.z - 1) - blocking;

            if (light < neighbour)
            {
                light = neighbour;
            }
        }

        if (light < limit)
        {
            neighbour = getBrightnessCached(cache, layer, pos.x, pos.y + 1, pos.z) - blocking;

            if (light < neighbour)
            {
                light = neighbour;
            }
        }

        if (light < limit)
        {
            neighbour = getBrightnessCached(cache, layer, pos.x + 1, pos.y, pos.z) - blocking;

            if (light < neighbour)
            {
                light = neighbour;
            }
        }

        if (light < limit)
        {
            neighbour = getBrightnessCached(cache, layer, pos.x - 1, pos.y, pos.z) - blocking;

            if (light < neighbour)
            {
                light = neighbour;
            }
        }

        if (light < limit)
        {
            neighbour = getBrightnessCached(cache, layer, pos.x, pos.y - 1, pos.z) - blocking;

            if (light < neighbour)
            {
                light = neighbour;
            }
        }
    }

    return light;
}

int VanillaLightEngine::getBlockingCached(unsigned int* cache, LightLayer::variety layer, Block* block, Block* extraBlock,
                             const BlockPos& pos)
{
    if (cache == NULL)
    {
        if (block == NULL)
        {
            block = mLevel.getBlock(pos);
        }

        if (extraBlock == NULL)
        {
            extraBlock = mLevel.getExtraBlock(pos);
        }

        int blocking = block->defaultBlockState()->getLightBlock();
        int extraBlocking = extraBlock->defaultBlockState()->getLightBlock();
        return (extraBlocking < blocking) ? blocking : extraBlocking;
    }

    int index = ((pos.x & 0xF) << 8) | ((pos.y & 0xF) << 4) | (pos.z & 0xF);
    unsigned int entry = cache[index];
    unsigned int key = ((unsigned int)(pos.y & 0xF0) << 2) | ((unsigned int)(pos.x & 0x3F0) << 6)
                       | ((unsigned int)(pos.z >> 4) & 0x3F);

    if ((entry & 0xFFFF) == key && (entry & 0x40000000) != 0)
    {
        return (entry >> 20) & 0xF;
    }

    if ((entry & 0xFFFF) != key && (entry & 0x80000000) != 0)
    {
        mLevel.setBrightness(layer,
                      BlockPos(CachedLightBlock((entry >> 10) & 0x3F, pos.x, pos.x), ((entry >> 2) & 0xF0) | (pos.y & 0xF),
                               CachedLightBlock(entry & 0x3F, pos.z, pos.z)),
                      (entry >> 24) & 0xF, true);
    }

    Block* blockAt = mLevel.getBlock(pos);
    Block* extraBlockAt = mLevel.getExtraBlock(pos);
    int emission = blockAt->defaultBlockState()->getLightEmission();
    int blocking = blockAt->defaultBlockState()->getLightBlock();
    int extraBlocking = extraBlockAt->defaultBlockState()->getLightBlock();

    if (blocking < extraBlocking)
    {
        blocking = extraBlocking;
    }

    if ((entry & 0xFFFF) == key)
    {
        entry = entry | 0x60000000 | ((unsigned int)(emission & 0xF) << 16) | ((unsigned int)(blocking & 0xF) << 20);
    }
    else
    {
        entry = key | 0x60000000 | ((unsigned int)(emission & 0xF) << 16) | ((unsigned int)(blocking & 0xF) << 20);
    }

    cache[index] = entry;
    return (entry >> 20) & 0xF;
}

void VanillaLightEngine::initCachePartial(unsigned int* cache, int x, int y, int z)
{
    mLevel.resetLightDirtyRegion();

    if (cache == NULL)
    {
        return;
    }

    if ((y & 0xFFFFFF00) == 0)
    {
        cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | (z & 0xF)] = 0;
        cache[(((x - 1) << 8) & 0xF00) | ((y & 0xF) << 4) | (z & 0xF)] = 0;
        cache[(((x + 1) << 8) & 0xF00) | ((y & 0xF) << 4) | (z & 0xF)] = 0;
        cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | ((z - 1) & 0xF)] = 0;
        cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | ((z + 1) & 0xF)] = 0;
    }

    if (((y - 1) & 0xFFFFFF00) == 0)
    {
        cache[((x & 0xF) << 8) | (((y - 1) << 4) & 0xF0) | (z & 0xF)] = 0;
    }

    if (((y + 1) & 0xFFFFFF00) == 0)
    {
        cache[((x & 0xF) << 8) | (((y + 1) << 4) & 0xF0) | (z & 0xF)] = 0;
    }
}

void VanillaLightEngine::initCacheComplete(unsigned int* cache, int x, int y, int z)
{
    unsigned int centre = 0;
    unsigned int west = 0;
    unsigned int east = 0;
    unsigned int below = 0;
    unsigned int above = 0;
    unsigned int north = 0;
    unsigned int south = 0;

    if ((y & 0xFFFFFF00) == 0)
    {
        centre = cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | (z & 0xF)];
        west = cache[(((x - 1) << 8) & 0xF00) | ((y & 0xF) << 4) | (z & 0xF)];
        east = cache[(((x + 1) << 8) & 0xF00) | ((y & 0xF) << 4) | (z & 0xF)];
        north = cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | ((z - 1) & 0xF)];
        south = cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | ((z + 1) & 0xF)];
    }

    if (((y - 1) & 0xFFFFFF00) == 0)
    {
        below = cache[((x & 0xF) << 8) | (((y - 1) << 4) & 0xF0) | (z & 0xF)];
    }

    if (((y + 1) & 0xFFFFFF00) == 0)
    {
        above = cache[((x & 0xF) << 8) | (((y + 1) << 4) & 0xF0) | (z & 0xF)];
    }

    XMemSet128(cache, 0, 0x4000);

    if ((y & 0xFFFFFF00) == 0)
    {
        cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | (z & 0xF)] = centre;
        cache[(((x - 1) << 8) & 0xF00) | ((y & 0xF) << 4) | (z & 0xF)] = west;
        cache[(((x + 1) << 8) & 0xF00) | ((y & 0xF) << 4) | (z & 0xF)] = east;
        cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | ((z - 1) & 0xF)] = north;
        cache[((x & 0xF) << 8) | ((y & 0xF) << 4) | ((z + 1) & 0xF)] = south;
    }

    if (((y - 1) & 0xFFFFFF00) == 0)
    {
        cache[((x & 0xF) << 8) | (((y - 1) << 4) & 0xF0) | (z & 0xF)] = below;
    }

    if (((y + 1) & 0xFFFFFF00) == 0)
    {
        cache[((x & 0xF) << 8) | (((y + 1) << 4) & 0xF0) | (z & 0xF)] = above;
    }
}

void VanillaLightEngine::setBrightnessCached(unsigned int* cache, unsigned long long* dirty, LightLayer::variety layer, int x, int y,
                                int z, int brightness)
{
    if (cache == NULL)
    {
        mLevel.setBrightness(layer, BlockPos(x, y, z), brightness, true);
        return;
    }

    if ((y & 0xFFFFFF00) != 0)
    {
        return;
    }

    int index = ((x & 0xF) << 8) | ((y & 0xF) << 4) | (z & 0xF);
    unsigned int entry = cache[index];
    unsigned int key = ((unsigned int)(y & 0xF0) << 2) | ((unsigned int)(x & 0x3F0) << 6)
                       | ((unsigned int)(z >> 4) & 0x3F);

    if ((entry & 0xFFFF) != key)
    {
        if ((entry & 0x80000000) != 0)
        {
            mLevel.setBrightness(layer,
                          BlockPos(CachedLightBlock((entry >> 10) & 0x3F, x, x), ((entry >> 2) & 0xF0) | (y & 0xF),
                                   CachedLightBlock(entry & 0x3F, z, z)),
                          (entry >> 24) & 0xF, true);
        }

        entry = key;
    }

    *dirty |= (1ULL << (x & 0xF)) | (0x10000ULL << (y & 0xF)) | (1ULL << (32 + (z & 0xF)));
    cache[index] = (entry & 0xF0FFFFFF) | ((unsigned int)brightness << 24) | 0x90000000;
}

void VanillaLightEngine::flushCache(unsigned int* cache, unsigned long long dirty, LightLayer::variety layer, int centreX,
                       int centreZ)
{
    if (dirty == 0)
    {
        return;
    }

    if (cache != NULL)
    {
        unsigned int* entryPtr = cache;

        for (int x = 0; x < 16; ++x)
        {
            if ((dirty & (1ULL << x)) == 0)
            {
                entryPtr += 256;
                continue;
            }

            for (int y = 0; y < 16; ++y)
            {
                if ((dirty & (0x10000ULL << y)) == 0)
                {
                    entryPtr += 16;
                    continue;
                }

                for (int z = 0; z < 16; ++z, ++entryPtr)
                {
                    if ((dirty & (1ULL << (32 + z))) == 0)
                    {
                        continue;
                    }

                    unsigned int entry = *entryPtr;

                    if ((entry & 0x80000000) != 0)
                    {
                        mLevel.setBrightness(layer,
                                      BlockPos(CachedLightBlock((entry >> 10) & 0x3F, x, centreX),
                                               ((entry >> 2) & 0xF0) | (y & 0xF),
                                               CachedLightBlock(entry & 0x3F, z, centreZ)),
                                      (entry >> 24) & 0xF, true);
                    }
                }
            }
        }
    }

    mLevel.flushLightDirtyRegion();
}

bool VanillaLightEngine::Check(LightLayer::variety layer, const BlockPos& pos, bool onlyLoadedChunks, bool skipCentre)
{
    unsigned int* cache = (unsigned int*)TlsGetValue(sLightingCacheTlsIndex);
    unsigned long long dirty = 0;

    if (!mLevel.hasChunksAt(pos, onlyLoadedChunks ? 0 : 17))
    {
        return false;
    }

    EnterCriticalSection(&mMutex);

    int centreX = pos.x;
    int centreY = pos.y;
    int centreZ = pos.z;

    PIXBeginNamedEvent(0.0f, "initCachePartial");
    initCachePartial(cache, centreX, centreY, centreZ);
    PIXEndNamedEvent();

    unsigned int* queue = (cache == NULL) ? mUpdateQueue : cache + 0x1000;
    int checked = 0;
    int toCheck = 0;
    int minBound = -(mLevel.mDimension->getXZSize() * 16) / 2;
    int maxBound = mLevel.mDimension->getXZSize() * 16 / 2 - 1;

    if (centreX > maxBound || centreX < minBound || centreZ > maxBound || centreZ < minBound)
    {
        LeaveCriticalSection(&mMutex);
        return false;
    }

    PIXBeginNamedEvent(0.0f, "First block");

    int currentLight = getBrightnessCached(cache, layer, pos.x, pos.y, pos.z);
    int expectedLight = getExpectedLight(cache, pos, layer, false);

    if (expectedLight != currentLight && cache != NULL)
    {
        initCacheComplete(cache, centreX, centreY, centreZ);
    }

    if (expectedLight > currentLight)
    {
        queue[0] = 0x20820;
        toCheck = 1;
    }
    else if (expectedLight < currentLight)
    {
        queue[0] = ((unsigned int)currentLight << 18) | 0x20820;

        int count = 1;
        MutableBlockPos current(0, 0, 0);
        unsigned int* keep = queue;

        do
        {
            unsigned int entry = queue[checked];
            ++checked;

            int x = (int)(entry & 0x3F) - 32 + centreX;
            int light = (entry >> 18) & 0xF;
            int y = (int)((entry >> 6) & 0x3F) - 32 + centreY;
            int z = (int)((entry >> 12) & 0x3F) - 32 + centreZ;
            current.set(x, y, z);

            if (getBrightnessCached(cache, layer, x, y, z) != light)
            {
                continue;
            }

            setBrightnessCached(cache, &dirty, layer, x, y, z, 0);

            if (light == 0)
            {
                continue;
            }

            if (Mth::abs(x - centreX) + Mth::abs(y - centreY) + Mth::abs(z - centreZ) >= 17)
            {
                continue;
            }

            bool stillLit = false;

            for (std::vector<const Direction*>::iterator it = Direction::VALUES.begin(); it != Direction::VALUES.end();
                 ++it)
            {
                const Direction* direction = *it;
                int nx = x + direction->getStepX();
                int ny = y + direction->getStepY();
                int nz = z + direction->getStepZ();

                if (nx > maxBound || nx < minBound || nz > maxBound || nz < minBound || (unsigned int)ny >= 256)
                {
                    continue;
                }

                current.set(nx, ny, nz);

                int emission;

                if (layer == LightLayer::SKY)
                {
                    emission = 0;
                }
                else
                {
                    emission = mLevel.getBlock(current)->defaultBlockState()->getLightEmission();
                }

                int blocking = std::max(
                    getBlockingCached(cache, layer, mLevel.getBlock(current), mLevel.getExtraBlock(current), current), 1);

                if (blocking >= 15 && emission > 0)
                {
                    blocking = 1;
                }

                int neighbourLight = getBrightnessCached(cache, layer, nx, ny, nz);
                int fallOff = light - blocking;

                if (neighbourLight == fallOff && count < 0x8000)
                {
                    queue[count] = (nx - centreX + 32) | ((ny - centreY + 32) << 6) | ((nz - centreZ + 32) << 12)
                                   | (fallOff << 18);
                    ++count;
                }
                else if (neighbourLight > fallOff)
                {
                    stillLit = true;
                }
            }

            if (stillLit)
            {
                *keep = entry;
                ++keep;
                ++toCheck;
            }
        } while (checked < count);
        checked = 0;
    }

    PIXEndNamedEvent();
    PIXBeginNamedEvent(0.0f, "Second block: checked=%i toCheck=%i", 0, toCheck);

    MutableBlockPos current(0, 0, 0);

    while (checked < toCheck)
    {
        unsigned int entry = queue[checked];
        ++checked;

        int x = (int)(entry & 0x3F) - 32 + centreX;
        int y = (int)((entry >> 6) & 0x3F) - 32 + centreY;
        int z = (int)((entry >> 12) & 0x3F) - 32 + centreZ;
        current.set(x, y, z);

        if (onlyLoadedChunks && !mLevel.hasChunkAt(current))
        {
            continue;
        }

        int light = getBrightnessCached(cache, layer, x, y, z);
        bool ignoreEmission = false;
        if (layer == LightLayer::BLOCK && skipCentre)
        {
            ignoreEmission = x != centreX || y != centreY || z != centreZ;
        }
        int expected = getExpectedLight(cache, current, layer, ignoreEmission);

        if (expected == light)
        {
            continue;
        }

        setBrightnessCached(cache, &dirty, layer, x, y, z, expected);

        if (expected <= light)
        {
            continue;
        }

        bool hasSpace = toCheck < 0x7FFA;
        if (abs(x - centreX) + abs(y - centreY) + abs(z - centreZ) < 17 && hasSpace)
        {
            if (x - 1 >= minBound && getBrightnessCached(cache, layer, x - 1, y, z) < expected)
            {
                queue[toCheck] = (x - 1 - centreX + 32) + ((y - centreY + 32) << 6) + ((z - centreZ + 32) << 12);
                ++toCheck;
            }

            if (x + 1 <= maxBound && getBrightnessCached(cache, layer, x + 1, y, z) < expected)
            {
                queue[toCheck] = (x + 1 - centreX + 32) + ((y - centreY + 32) << 6) + ((z - centreZ + 32) << 12);
                ++toCheck;
            }

            if (y - 1 >= 0 && getBrightnessCached(cache, layer, x, y - 1, z) < expected)
            {
                queue[toCheck] = (x - centreX + 32) + ((y - 1 - centreY + 32) << 6) + ((z - centreZ + 32) << 12);
                ++toCheck;
            }

            if (y + 1 < 256 && getBrightnessCached(cache, layer, x, y + 1, z) < expected)
            {
                queue[toCheck] = (x - centreX + 32) + ((y + 1 - centreY + 32) << 6) + ((z - centreZ + 32) << 12);
                ++toCheck;
            }

            if (z - 1 >= minBound && getBrightnessCached(cache, layer, x, y, z - 1) < expected)
            {
                queue[toCheck] = (x - centreX + 32) + ((y - centreY + 32) << 6) + ((z - 1 - centreZ + 32) << 12);
                ++toCheck;
            }

            if (z + 1 <= maxBound && getBrightnessCached(cache, layer, x, y, z + 1) < expected)
            {
                queue[toCheck] = (x - centreX + 32) + ((y - centreY + 32) << 6) + ((z + 1 - centreZ + 32) << 12);
                ++toCheck;
            }
        }
    }

    PIXEndNamedEvent();
    PIXBeginNamedEvent(0.0f, "flushCache");
    flushCache(cache, dirty, layer, centreX, centreZ);
    PIXEndNamedEvent();

    LeaveCriticalSection(&mMutex);
    return true;
}
