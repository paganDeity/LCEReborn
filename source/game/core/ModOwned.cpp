#include "game/core/ModOwned.hpp"

namespace
{
std::vector<ModOwnedRegistryBase*>& All()
{
    static std::vector<ModOwnedRegistryBase*> registries;
    return registries;
}
}

ModOwnedRegistryBase::ModOwnedRegistryBase()
{
    All().push_back(this);
}

void ModOwnedRegistryBase::ReleaseOwnerEverywhere(const std::string& owner)
{
    for (size_t index = 0; index < All().size(); ++index)
        All()[index]->ReleaseOwner(owner);
}
