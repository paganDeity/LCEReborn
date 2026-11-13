#include "game/world/redstone/ScriptedRedstoneEngine.hpp"

#include <algorithm>

#include "game/core/ModEvents_generated.hpp"
#include "game/world/Level.hpp"
#include "platform/pc/Log.hpp"

namespace
{
const Logger kRedstoneLog("Redstone");
// a mod that keeps changing the same wires would hold the server thread forever
const int kMaxNestedUpdates = 4096;
}

const BlockState* ScriptedRedstoneEngine::UpdateWire(const BlockPos& pos, const BlockState*)
{
    if (mRunning)
    {
        if (std::find(mPending.begin(), mPending.end(), pos) == mPending.end())
            mPending.push_back(pos);
        return mLevel.getBlockState(pos);
    }

    mRunning = true;
    ModEvents::WireUpdate(&mLevel, pos.x, pos.y, pos.z);
    int rounds = 0;
    while (!mPending.empty())
    {
        if (++rounds > kMaxNestedUpdates)
        {
            LOG_WARN(kRedstoneLog, "wire_update did not settle after %d nested updates; dropping the rest", rounds);
            mPending.clear();
            break;
        }
        const BlockPos next = mPending.back();
        mPending.pop_back();
        ModEvents::WireUpdate(&mLevel, next.x, next.y, next.z);
    }
    mRunning = false;
    return mLevel.getBlockState(pos);
}
