#include "game/command/chat/SpreadPlayersChatCommand.hpp"

#include <math.h>

#include <vector>

#include "game/command/CommandArguments.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/network/ServerGamePacketListenerImpl.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/util/Random.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/block/Material.hpp"

using CommandArguments::Lower;
using CommandArguments::Number;
using CommandArguments::ParseCoordinate;
using CommandArguments::ParseNumber;

namespace
{
const int kAttempts = 1000;

// the block a spread player stands on: the top solid one of the column, refused when it is liquid or the column is not loaded
bool Ground(Level* level, double x, double z, double* y)
{
    const BlockPos column(static_cast<int>(floor(x)), 0, static_cast<int>(floor(z)));
    if (!level->hasChunkAt(column))
        return false;
    const BlockPos top = level->getTopSolidBlockPos(column);
    if (top.y <= 0 || level->getBlockState(top.below())->getMaterial()->isLiquid() || level->getBlockState(top)->getMaterial()->isLiquid())
        return false;
    *y = top.y;
    return true;
}
}

SpreadPlayersChatCommand::SpreadPlayersChatCommand()
    : ChatCommand(L"spreadplayers", L"<x> <z> <spreadDistance> <maxRange> <respectTeams> <targets>",
                  L"coord coord number number false targets", ePermission_Operator)
{
}

bool SpreadPlayersChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() != 7)
        return false;
    double centreX = 0.0;
    double centreZ = 0.0;
    double spread = 0.0;
    double range = 0.0;
    if (!ParseCoordinate(words[1], source.x, true, &centreX) || !ParseCoordinate(words[2], source.z, true, &centreZ))
        return false;
    if (!ParseNumber(words[3], &spread) || spread < 0.0 || !ParseNumber(words[4], &range) || range < spread + 1.0)
    {
        Fail(source, L"The spread distance must be 0 or more and the range at least one more than it");
        return true;
    }
    // the game has no teams, so there is nothing to keep together
    if (Lower(words[5]) != L"false")
    {
        Fail(source, L"There are no teams in this game, so respectTeams must be false");
        return true;
    }
    EntityList targets;
    if (!ResolveEntities(source, words[6], targets))
        return true;
    Level* level = source.level;
    Random* random = level->mRandom;
    std::vector<double> placed;
    for (size_t index = 0; index < targets.size(); ++index)
    {
        bool found = false;
        double x = 0.0, y = 0.0, z = 0.0;
        for (int attempt = 0; attempt < kAttempts && !found; ++attempt)
        {
            x = floor(centreX + (random->nextDouble() * 2.0 - 1.0) * range) + 0.5;
            z = floor(centreZ + (random->nextDouble() * 2.0 - 1.0) * range) + 0.5;
            bool apart = true;
            for (size_t other = 0; other + 1 < placed.size() && apart; other += 2)
                apart = fabs(placed[other] - x) >= spread || fabs(placed[other + 1] - z) >= spread;
            found = apart && Ground(level, x, z, &y);
        }
        if (!found)
        {
            Fail(source, L"Could not spread " + Number(static_cast<int>(targets.size())) + L" entities around " + words[1] + L", "
                             + words[2] + L" (too many entities for the space, or the ground there is not loaded or is liquid)");
            return true;
        }
        placed.push_back(x);
        placed.push_back(z);
    }
    for (size_t index = 0; index < targets.size(); ++index)
    {
        EntityPtr entity = targets[index];
        const double x = placed[index * 2];
        const double z = placed[index * 2 + 1];
        double y = 0.0;
        Ground(level, x, z, &y);
        PlayerPtr player = boost::dynamic_pointer_cast<ServerPlayer>(entity);
        if (player)
            player->mConnection->teleport(x, y, z, player->mYRot, player->mXRot, true, true);
        else
            entity->moveTo(x, y, z, entity->mYRot, entity->mXRot);
    }
    Tell(source, L"Spread " + Count(static_cast<int>(targets.size()), L"entity", L"entities") + L" around " + words[1] + L", " + words[2]);
    return true;
}
