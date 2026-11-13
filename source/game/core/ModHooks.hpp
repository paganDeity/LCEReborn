#ifndef GAME_CORE_MODHOOKS_HPP
#define GAME_CORE_MODHOOKS_HPP

#include "game/core/ModHookIds_generated.hpp"

#include <boost/shared_ptr.hpp>

class Entity;

namespace ModHooks
{
extern volatile int sActive[kModHook_Count];

inline bool IsActive(int hook)
{
    return sActive[hook] != 0;
}

void SetActive(int hook, bool active);

// the level's own owning pointer to an entity a hook got as a plain pointer, or none when it is not in a level yet; taken
// from the level, not from the entity, because this build has no exceptions and a pointer made from an expired owner
// would count from zero
boost::shared_ptr<Entity> SharedEntity(Entity* entity);
}
#endif
