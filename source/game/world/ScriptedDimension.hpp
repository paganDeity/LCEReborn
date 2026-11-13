#ifndef GAME_WORLD_SCRIPTEDDIMENSION_HPP
#define GAME_WORLD_SCRIPTEDDIMENSION_HPP

#include "game/world/ModDimension_generated.hpp"
#include "game/world/NormalDimension.hpp"

// a dimension a mod registered: the overworld's behaviour under the mod's own type, every method answerable by a hook
class ScriptedDimension : public ModDimension<NormalDimension>
{
public:
    explicit ScriptedDimension(const DimensionType* type) : mType(type) {}

    virtual const DimensionType* getType() const { return mType; }

private:
    const DimensionType* mType;
};

#endif
