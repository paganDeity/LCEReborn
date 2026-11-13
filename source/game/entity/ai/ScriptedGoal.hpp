#ifndef GAME_ENTITY_AI_SCRIPTEDGOAL_HPP
#define GAME_ENTITY_AI_SCRIPTEDGOAL_HPP

#include <string>

#include "game/entity/ai/Goal.hpp"
#include "game/entity/mob/Mob.hpp"

class GoalSelector;
class Mob;

class ScriptedGoal : public Goal
{
public:
    ScriptedGoal(Mob* mob, const std::wstring& name, int flags);

    virtual bool canUse();
    virtual bool canContinueToUse();
    virtual void start();
    virtual void stop();
    virtual void tick();
    virtual std::wstring getName();

    // the mob the goal belongs to, for a mod that answers for it
    boost::shared_ptr<Entity> GetEntity() { return mMob->shared_from_this(); }

    static bool Remove(GoalSelector& selector, const std::wstring& name);

private:
    Mob* mMob;
    std::wstring mName;
};

#endif
