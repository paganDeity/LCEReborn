#include "game/entity/ai/ScriptedGoal.hpp"

#include <vector>

#include "game/entity/ai/GoalSelector.hpp"
#include "game/entity/mob/Mob.hpp"

ScriptedGoal::ScriptedGoal(Mob* mob, const std::wstring& name, int flags) : mMob(mob), mName(name)
{
    setRequiredControlFlags(flags);
}

// what a goal does is the mod's: it hooks these methods (ScriptedGoal_canUse and the others) and answers for the goal it names
bool ScriptedGoal::canUse()
{
    return false;
}

bool ScriptedGoal::canContinueToUse()
{
    return canUse();
}

void ScriptedGoal::start()
{
}

void ScriptedGoal::stop()
{
}

void ScriptedGoal::tick()
{
}

std::wstring ScriptedGoal::getName()
{
    return L"Scripted:" + mName;
}

bool ScriptedGoal::Remove(GoalSelector& selector, const std::wstring& name)
{
    std::vector<Goal*> found;
    for (size_t index = 0; index < selector.mAvailableGoals.size(); ++index)
    {
        ScriptedGoal* goal = dynamic_cast<ScriptedGoal*>(selector.mAvailableGoals[index]->mGoal);
        if (goal != NULL && goal->mName == name)
            found.push_back(goal);
    }
    for (size_t index = 0; index < found.size(); ++index)
        selector.removeGoal(found[index]);
    return !found.empty();
}
