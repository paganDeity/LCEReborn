#include "net/NetNative.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <string.h>

namespace lcenet
{
namespace native
{
namespace
{
typedef SOCKET NativeSocket;
const NativeSocket kInvalidNative = INVALID_SOCKET;
const int kSendFlags = 0;
const int kShutdownBoth = SD_BOTH;
}
}
}

#include "net/NetBsdSockets.inl"

namespace lcenet
{
namespace native
{
namespace
{
template <class Visit>
void ForEachInterface(Visit visit)
{
    const intptr_t probe = Open(false);
    if (probe == kInvalid)
        return;
    INTERFACE_INFO interfaces[32];
    DWORD bytes = 0;
    if (WSAIoctl(static_cast<NativeSocket>(probe), SIO_GET_INTERFACE_LIST, 0, 0, interfaces, sizeof(interfaces),
                 &bytes, 0, 0)
        == 0)
    {
        for (DWORD index = 0; index < bytes / sizeof(INTERFACE_INFO); ++index)
        {
            if (!visit(interfaces[index]))
                break;
        }
    }
    Close(probe);
}
}

bool Start(bool* connected)
{
    WSADATA data;
    const bool started = WSAStartup(MAKEWORD(2, 2), &data) == 0;
    *connected = started;
    return started;
}

bool CheckConnected()
{
    return true;
}

int LastError()
{
    return WSAGetLastError();
}

EError Classify(int code)
{
    if (code == WSAEWOULDBLOCK)
        return eError_WouldBlock;
    if (code == WSAEINPROGRESS)
        return eError_InProgress;
    if (code == WSAETIMEDOUT)
        return eError_TimedOut;
    if (code == WSAECONNREFUSED)
        return eError_Refused;
    if (code == WSAEADDRINUSE)
        return eError_AddressInUse;
    if (code == WSAENETUNREACH || code == WSAEHOSTUNREACH)
        return eError_Unreachable;
    return eError_Other;
}

int TimedOutCode()
{
    return WSAETIMEDOUT;
}

void Close(intptr_t socket)
{
    closesocket(static_cast<NativeSocket>(socket));
}

void SetBlocking(intptr_t socket, bool blocking)
{
    u_long nonBlocking = blocking ? 0 : 1;
    ioctlsocket(static_cast<NativeSocket>(socket), FIONBIO, &nonBlocking);
}

void PrepareListener(intptr_t socket)
{
    SetOption(socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, 1);
}

void PrepareSharedPort(intptr_t socket)
{
    SetOption(socket, SOL_SOCKET, SO_REUSEADDR, 1);
}

void AddBroadcastAddresses(std::vector<uint32_t>& addresses)
{
    ForEachInterface([&addresses](const INTERFACE_INFO& entry) {
        if ((entry.iiFlags & IFF_UP) != 0 && (entry.iiFlags & IFF_BROADCAST) != 0
            && (entry.iiFlags & IFF_LOOPBACK) == 0)
        {
            const uint32_t ip = ntohl(entry.iiAddress.AddressIn.sin_addr.s_addr);
            const uint32_t mask = ntohl(entry.iiNetmask.AddressIn.sin_addr.s_addr);
            addresses.push_back(ip | ~mask);
        }
        return true;
    });
}

uint32_t FirstPrivateInterface()
{
    uint32_t found = 0;
    ForEachInterface([&found](const INTERFACE_INFO& entry) {
        const uint32_t ip = ntohl(entry.iiAddress.AddressIn.sin_addr.s_addr);
        if ((entry.iiFlags & IFF_UP) != 0 && (entry.iiFlags & IFF_LOOPBACK) == 0 && IsPrivateAddress(ip))
            found = ip;
        return found == 0;
    });
    return found;
}
}
}
