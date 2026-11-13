#include "game/command/EntityData.hpp"

#include "game/data/CompoundTag.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/entity/mob/EntityIO.hpp"
#include "game/util/ResourceLocation.hpp"

namespace EntityData
{
CompoundTag* Read(const boost::shared_ptr<Entity>& entity)
{
    CompoundTag* tag = entity->saveWithoutId(new CompoundTag());
    if (!Entity::instanceof(entity, EntityType::PLAYER))
    {
        const ResourceLocation key = EntityIO::getKey(entity);
        if (!key.getPath().empty())
            tag->putString(L"id", key.getNamespace() + L":" + key.getPath());
    }
    return tag;
}
}
