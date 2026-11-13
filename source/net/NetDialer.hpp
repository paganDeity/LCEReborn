#ifndef NET_NETDIALER_HPP
#define NET_NETDIALER_HPP

#include <string>

#include <boost/shared_ptr.hpp>

#include "net/NetSocket.hpp"
#include "net/NetStream.hpp"

namespace lcenet
{
class Dialer
{
public:
    virtual ~Dialer() {}
    virtual boost::shared_ptr<Stream> Dial(const std::string& target, const CancelFlag& cancel, std::string& error) = 0;

    static void Register(const std::string& scheme, Dialer* dialer);
    static Dialer* For(const std::string& target);
};
}

#endif
