#include "game/command/chat/SummonChatCommand.hpp"

#include "game/command/CommandArguments.hpp"
#include "game/core/LightningBolt.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/ListTag.hpp"
#include "game/data/Tag.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/entity/mob/EntityIO.hpp"
#include "game/item/SpawnEggItem.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/chunk/OldChunkStorage.hpp"

using CommandArguments::StripNamespace;

SummonChatCommand::SummonChatCommand()
    : ChatCommand(L"summon", L"<entity> [x y z] [data]", L"entity coord coord coord", ePermission_Operator)
{
}

bool SummonChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() != 2 && words.size() != 3 && words.size() != 5 && words.size() != 6)
        return false;
    double x = source.x;
    double y = source.y;
    double z = source.z;
    if (words.size() >= 5)
    {
        double result[3];
        if (!ParsePosition(source, words, 2, false, result))
            return true;
        x = result[0];
        y = result[1];
        z = result[2];
    }
    const size_t tagIndex = words.size() == 3 ? 2 : (words.size() == 6 ? 5 : 0);
    CompoundTag* tag = NULL;
    if (tagIndex != 0)
    {
        if (!ReadCompound(source, words[tagIndex], L"The entity data is not valid: ", &tag))
            return true;
    }
    Level* level = source.level;
    const BlockPos pos(x, y, z);
    if (!level->isInWorldBounds(pos) || !level->hasChunkAt(pos))
    {
        delete tag;
        Fail(source, L"Cannot summon the object out of the world");
        return true;
    }

    const std::wstring type = StripNamespace(words[1]);
    if (type == L"lightning_bolt" || type == L"lightningbolt")
    {
        if (tag != NULL)
        {
            delete tag;
            Fail(source, L"A lightning bolt does not take entity data.");
            return true;
        }
        level->addGlobalEntity(boost::shared_ptr<Entity>(new LightningBolt(level, x, y, z, false)));
        Tell(source, L"Object successfully summoned");
        return true;
    }
    const ResourceLocation location(type);
    if (type == L"player" || !EntityIO::isValidEntityType(location))
    {
        delete tag;
        Fail(source, L"Unknown entity type " + words[1] + L".");
        return true;
    }

    EntityPtr entity;
    if (EntityIO::sSpawnInfo.find(location) != EntityIO::sSpawnInfo.end())
    {
        int error = 0;
        entity = SpawnEggItem::spawnMobAt(level, location, x, y, z, &error, L"");
        if (!entity && error != 0)
        {
            delete tag;
            if (source.output)
                SpawnEggItem::DisplaySpawnError(source.output, error);
            else
                Fail(source, L"Unable to summon the object here.");
            return true;
        }
    }
    else
    {
        entity = EntityIO::newEntity(location, level);
        if (entity)
        {
            entity->moveTo(x, y, z, source.yRot, 0.0f);
            if (!level->addEntity(entity))
                entity.reset();
        }
    }
    if (!entity)
    {
        delete tag;
        Fail(source, L"Unable to summon object");
        return true;
    }
    if (tag != NULL)
    {
        entity->load(tag);
        entity->moveTo(x, y, z, entity->mYRot, entity->mXRot);
        ListTag* passengers = Entity::GetPassengerTags(tag);
        for (int i = 0; passengers != NULL && i < passengers->size(); ++i)
        {
            EntityPtr passenger = OldChunkStorage::loadEntity(passengers->getCompound(i), level, x, y, z, true);
            if (passenger)
                passenger->startRiding(entity, true);
        }
    }
    delete tag;
    Tell(source, L"Object successfully summoned");
    return true;
}
