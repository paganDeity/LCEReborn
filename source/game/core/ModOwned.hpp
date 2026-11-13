#ifndef GAME_CORE_MODOWNED_HPP
#define GAME_CORE_MODOWNED_HPP

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "game/core/CriticalSection.hpp"

class ModOwnedRegistryBase
{
public:
    static void ReleaseOwnerEverywhere(const std::string& owner);

protected:
    ModOwnedRegistryBase();
    virtual ~ModOwnedRegistryBase() {}
    virtual void ReleaseOwner(const std::string& owner) = 0;
};

#endif
