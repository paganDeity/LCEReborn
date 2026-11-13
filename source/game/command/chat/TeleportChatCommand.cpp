#include "game/command/chat/TeleportChatCommand.hpp"

#include <math.h>

#include "game/command/CommandArguments.hpp"
#include "game/command/TargetSelector.hpp"
#include "game/command/TeleportCommand.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/network/ServerGamePacketListenerImpl.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/world/Level.hpp"

TeleportChatCommand::TeleportChatCommand()
    : ChatCommand(L"tp", L"[targets] <destination|x y z> [yaw pitch|facing <x y z>|facing entity <target> [eyes|feet]]", L"targets targets coord coord", ePermission_Operator)
{
    addAlias(L"teleport");
}

namespace
{
const double kRadians = 3.14159265358979323846 / 180.0;

bool ParseAngle(const std::wstring& word, float current, float* result)
{
    if (!word.empty() && word[0] == L'~')
    {
        double offset = 0.0;
        if (word.size() > 1 && !CommandArguments::ParseNumber(word.substr(1), &offset))
            return false;
        *result = current + static_cast<float>(offset);
        return true;
    }
    double value = 0.0;
    if (!CommandArguments::ParseNumber(word, &value))
        return false;
    *result = static_cast<float>(value);
    return true;
}

void Facing(double fromX, double fromY, double fromZ, double toX, double toY, double toZ, float* yaw, float* pitch)
{
    const double dx = toX - fromX;
    const double dy = toY - fromY;
    const double dz = toZ - fromZ;
    const double flat = sqrt(dx * dx + dz * dz);
    *yaw = static_cast<float>(atan2(dz, dx) / kRadians - 90.0);
    *pitch = static_cast<float>(-(atan2(dy, flat) / kRadians));
}
}

bool TeleportChatCommand::run(const CommandSource& source, const Words& words)
{
    const size_t arguments = words.size() - 1;
    if (arguments < 1 || arguments > 9)
        return false;

    EntityList moved;
    size_t next = 1;
    if (arguments == 2 || arguments >= 4)
    {
        if (!ResolveEntities(source, words[1], moved))
            return true;
        next = 2;
    }
    else if (source.entity)
        moved.push_back(source.entity);
    else
    {
        Fail(source, L"No entity was found.");
        return true;
    }

    EntityPtr destination;
    if (words.size() - next == 1)
    {
        EntityList found;
        if (!ResolveEntities(source, words[next], found))
            return true;
        if (found.size() != 1)
        {
            Fail(source, L"The destination matched more than one entity.");
            return true;
        }
        destination = found[0];
    }

    double target[3] = {0.0, 0.0, 0.0};
    if (!destination && !ParsePosition(source, words, next, true, target))
        return true;

    enum { eTurn_None, eTurn_Angles, eTurn_Point, eTurn_Entity } turn = eTurn_None;
    double look[3] = {0.0, 0.0, 0.0};
    EntityPtr lookAt;
    bool lookAtEyes = false;
    const size_t after = next + 3;
    if (!destination && words.size() > after)
    {
        const std::wstring keyword = CommandArguments::Lower(words[after]);
        if (keyword == L"facing")
        {
            if (words.size() > after + 1 && CommandArguments::Lower(words[after + 1]) == L"entity")
            {
                if (words.size() < after + 3 || words.size() > after + 4)
                    return false;
                EntityList found;
                if (!ResolveEntities(source, words[after + 2], found))
                    return true;
                if (found.size() != 1)
                {
                    Fail(source, L"The facing target matched more than one entity.");
                    return true;
                }
                lookAt = found[0];
                if (words.size() == after + 4)
                {
                    const std::wstring anchor = CommandArguments::Lower(words[after + 3]);
                    if (anchor != L"eyes" && anchor != L"feet")
                        return false;
                    lookAtEyes = anchor == L"eyes";
                }
                turn = eTurn_Entity;
            }
            else
            {
                if (words.size() != after + 4)
                    return false;
                if (!ParsePosition(source, words, after + 1, false, look))
                    return true;
                turn = eTurn_Point;
            }
        }
        else if (words.size() == after + 2)
            turn = eTurn_Angles;
        else
            return false;
    }
    else if (!destination && words.size() != after && words.size() != next + 1)
        return false;

    Level* level = destination ? destination->mLevel : source.level;
    const std::wstring where = destination ? TargetSelector::DisplayName(destination)
                                           : Position(target[0], target[1], target[2]);

    int teleported = 0;
    for (size_t index = 0; index < moved.size(); ++index)
    {
        const EntityPtr& entity = moved[index];
        if (entity->mLevel != level)
        {
            Fail(source, TargetSelector::DisplayName(entity) + L" is in another dimension.");
            continue;
        }
        const double x = destination ? destination->mX : target[0];
        const double y = destination ? destination->mY : target[1];
        const double z = destination ? destination->mZ : target[2];
        float yaw = entity->mYRot;
        float pitch = entity->mXRot;
        if (turn == eTurn_Angles)
        {
            if (!ParseAngle(words[after], entity->mYRot, &yaw) || !ParseAngle(words[after + 1], entity->mXRot, &pitch))
            {
                Fail(source, L"A rotation is a yaw and a pitch in degrees, or ~ for relative.");
                return true;
            }
        }
        else if (turn == eTurn_Point)
            Facing(x, y + (Entity::instanceof(entity, EntityType::PLAYER) ? 1.62 : 0.0), z, look[0], look[1], look[2], &yaw, &pitch);
        else if (turn == eTurn_Entity)
            Facing(x, y + (Entity::instanceof(entity, EntityType::PLAYER) ? 1.62 : 0.0), z, lookAt->mX,
                   lookAt->mY + (lookAtEyes ? lookAt->getEyeHeight() : 0.0), lookAt->mZ, &yaw, &pitch);
        PlayerPtr movedPlayer = boost::dynamic_pointer_cast<ServerPlayer>(entity);
        PlayerPtr destinationPlayer = boost::dynamic_pointer_cast<ServerPlayer>(destination);
        if (movedPlayer && turn != eTurn_None)
            movedPlayer->mConnection->teleport(x, y, z, yaw, pitch, true, true);
        else if (movedPlayer && destinationPlayer)
            Perform(source, TeleportCommand::preparePacket(movedPlayer->getXuid(), destinationPlayer->getXuid()));
        else if (movedPlayer)
            Perform(source, TeleportCommand::preparePacket(movedPlayer->getXuid(), (float)x, (float)y, (float)z));
        else
            entity->moveTo(x, y, z, yaw, pitch);
        ++teleported;
    }
    if (teleported == 1 && moved.size() == 1)
        Tell(source, L"Teleported " + TargetSelector::DisplayName(moved[0]) + L" to " + where + L".");
    else if (teleported > 0)
        Tell(source, L"Teleported " + Count(teleported, L"entity", L"entities") + L" to " + where + L".");
    return true;
}
