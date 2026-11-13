#include "net/NetNative.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

namespace lcenet
{
namespace native
{
namespace
{
typedef int NativeSocket;
const NativeSocket kInvalidNative = -1;
const int kSendFlags = MSG_NOSIGNAL;
const int kShutdownBoth = SHUT_RDWR;
}
}
}

#include "net/NetBsdSockets.inl"

namespace lcenet
{
namespace native
{
bool Start(bool* connected)
{
    *connected = true;
    return true;
}

bool CheckConnected()
{
    return true;
}

int LastError()
{
    return errno;
}

EError Classify(int code)
{
    if (code == EWOULDBLOCK || code == EAGAIN)
        return eError_WouldBlock;
    if (code == EINPROGRESS)
        return eError_InProgress;
    if (code == ETIMEDOUT)
        return eError_TimedOut;
    if (code == ECONNREFUSED)
        return eError_Refused;
    if (code == EADDRINUSE)
        return eError_AddressInUse;
    if (code == ENETUNREACH || code == EHOSTUNREACH)
        return eError_Unreachable;
    return eError_Other;
}

int TimedOutCode()
{
    return ETIMEDOUT;
}

void Close(intptr_t socket)
{
    close(static_cast<NativeSocket>(socket));
}

void SetBlocking(intptr_t socket, bool blocking)
{
    const int flags = fcntl(static_cast<NativeSocket>(socket), F_GETFL, 0);
    fcntl(static_cast<NativeSocket>(socket), F_SETFL, blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK));
}

void PrepareListener(intptr_t socket)
{
    SetOption(socket, SOL_SOCKET, SO_REUSEADDR, 1);
}

void PrepareSharedPort(intptr_t socket)
{
    SetOption(socket, SOL_SOCKET, SO_REUSEADDR, 1);
    SetOption(socket, SOL_SOCKET, SO_REUSEPORT, 1);
}

void AddBroadcastAddresses(std::vector<uint32_t>& addresses)
{
    ifaddrs* list = 0;
    if (getifaddrs(&list) != 0)
        return;
    for (ifaddrs* entry = list; entry != 0; entry = entry->ifa_next)
    {
        if (entry->ifa_addr == 0 || entry->ifa_addr->sa_family != AF_INET || (entry->ifa_flags & IFF_UP) == 0
            || (entry->ifa_flags & IFF_BROADCAST) == 0 || (entry->ifa_flags & IFF_LOOPBACK) != 0
            || entry->ifa_broadaddr == 0)
            continue;
        addresses.push_back(ntohl(reinterpret_cast<sockaddr_in*>(entry->ifa_broadaddr)->sin_addr.s_addr));
    }
    freeifaddrs(list);
}

uint32_t FirstPrivateInterface()
{
    uint32_t found = 0;
    ifaddrs* list = 0;
    if (getifaddrs(&list) != 0)
        return 0;
    for (ifaddrs* entry = list; entry != 0 && found == 0; entry = entry->ifa_next)
    {
        if (entry->ifa_addr == 0 || entry->ifa_addr->sa_family != AF_INET || (entry->ifa_flags & IFF_UP) == 0
            || (entry->ifa_flags & IFF_LOOPBACK) != 0)
            continue;
        const uint32_t ip = ntohl(reinterpret_cast<sockaddr_in*>(entry->ifa_addr)->sin_addr.s_addr);
        if (IsPrivateAddress(ip))
            found = ip;
    }
    freeifaddrs(list);
    return found;
}
}
}
