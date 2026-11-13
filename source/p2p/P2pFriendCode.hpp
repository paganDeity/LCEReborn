#ifndef P2P_P2PFRIENDCODE_HPP
#define P2P_P2PFRIENDCODE_HPP

#include <string>

#include "p2p/P2pPublicKey.hpp"

namespace p2p
{
class FriendCode
{
public:
    static std::string Of(const PublicKey& key);
    static std::string Normalize(const std::string& code);
    static bool Matches(const std::string& code, const PublicKey& key);
};
}

#endif
