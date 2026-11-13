#ifndef PLATFORM_PC_PCPLAYERIDENTITIES_HPP
#define PLATFORM_PC_PCPLAYERIDENTITIES_HPP

#include "p2p/P2pIdentity.hpp"

bool PcActiveIdentityKey(p2p::Identity& key);
void PcStartIdentities(const char* program);

#endif
