#include "game/core/ModHooks.hpp"

#include "game/entity/mob/Entity.hpp"
#include "game/world/Level.hpp"

namespace ModHooks
{
volatile int sActive[kModHook_Count];

void SetActive(int hook, bool active)
{
    sActive[hook] = active ? 1 : 0;
}

boost::shared_ptr<Entity> SharedEntity(Entity* entity)
{
    if (entity == NULL || entity->mLevel == NULL)
        return boost::shared_ptr<Entity>();
    return entity->mLevel->getEntity(entity->getId());
}
}
