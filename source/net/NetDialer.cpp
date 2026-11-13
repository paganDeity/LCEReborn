#include "net/NetDialer.hpp"

#include <map>

#include "game/core/CriticalSection.hpp"

namespace lcenet
{
namespace
{
std::map<std::string, Dialer*>& Dialers()
{
    static std::map<std::string, Dialer*> dialers;
    return dialers;
}
}

void Dialer::Register(const std::string& scheme, Dialer* dialer)
{
    Dialers()[scheme] = dialer;
}

Dialer* Dialer::For(const std::string& target)
{
    const size_t colon = target.find(':');
    if (colon == std::string::npos)
        return 0;
    std::map<std::string, Dialer*>::const_iterator found = Dialers().find(target.substr(0, colon));
    return found == Dialers().end() ? 0 : found->second;
}
}
