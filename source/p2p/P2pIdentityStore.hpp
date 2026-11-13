#ifndef P2P_P2PIDENTITYSTORE_HPP
#define P2P_P2PIDENTITYSTORE_HPP

#include <string>

#include "p2p/P2pIdentity.hpp"

namespace p2p
{
class IdentityStore
{
public:
    static bool LoadOrCreate(const std::string& path, Identity& identity);
};
}

#endif
