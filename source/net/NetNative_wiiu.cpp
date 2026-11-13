#include "net/NetNative.hpp"

#include <stdio.h>
#include <string.h>

#include <nn/ac/ac_cpp.h>

#include "platform/wiiu/C4JThreadImpl.hpp"

namespace lcenet
{
namespace native
{
namespace
{
struct NsnSockaddr
{
    uint16_t family;
    uint16_t port;
    uint32_t ip;
    unsigned char zero[8];
};

struct NsnTimeval
{
    long seconds;
    long microseconds;
};

struct NsnHostent
{
    char* name;
    char** aliases;
    int type;
    int length;
    char** addresses;
};

extern "C" {
int NsnSocket(int domain, int type, int protocol) __asm__("socket");
int NsnConnect(int socket, const NsnSockaddr* address, int length) __asm__("connect");
int NsnBind(int socket, const NsnSockaddr* address, int length) __asm__("bind");
int NsnListen(int socket, int backlog) __asm__("listen");
int NsnAccept(int socket, NsnSockaddr* address, int* length) __asm__("accept");
int NsnSend(int socket, const void* data, int size, int flags) __asm__("send");
int NsnRecv(int socket, void* data, int size, int flags) __asm__("recv");
int NsnSendTo(int socket, const void* data, int size, int flags, const NsnSockaddr* address, int length)
    __asm__("sendto");
int NsnRecvFrom(int socket, void* data, int size, int flags, NsnSockaddr* address, int* length)
    __asm__("recvfrom");
int NsnSetSockOpt(int socket, int level, int name, const void* value, int length) __asm__("setsockopt");
int NsnGetPeerName(int socket, NsnSockaddr* address, int* length) __asm__("getpeername");
int NsnShutdown(int socket, int how) __asm__("shutdown");
int NsnSocketClose(int socket) __asm__("socketclose");
int NsnSelect(int count, uint32_t* read, uint32_t* write, uint32_t* error, NsnTimeval* timeout) __asm__("select");
int NsnLastError() __asm__("socketlasterr");
void NsnLibInit() __asm__("socket_lib_init");
int NsnSetMulticastState(int enabled) __asm__("set_multicast_state");
NsnHostent* NsnGetHostByName(const char* name) __asm__("gethostbyname");
}

const int kAfInet = 2;
const int kSockStream = 1;
const int kSockDgram = 2;
const int kProtoTcp = 6;
const int kProtoUdp = 17;
const int kSolSocket = -1;
const int kSoReuseAddr = 0x0004;
const int kSoBroadcast = 0x0020;
const int kSoNonBlocking = 0x1014;
const int kSoBlocking = 0x1015;
const int kTcpNoDelay = 0x2004;
const int kShutBoth = 2;
const int kTimedOut = 2;
const int kWouldBlock = 6;
const int kRefused = 7;
const int kAddressInUse = 20;
const int kInProgress = 22;
const int kNetUnreachable = 30;
const int kHostUnreachable = 30;


NsnSockaddr ToNative(const Address& address)
{
    NsnSockaddr result;
    memset(&result, 0, sizeof(result));
    result.family = kAfInet;
    result.port = address.port;
    result.ip = address.ip;
    return result;
}

Address FromNative(const NsnSockaddr& address)
{
    Address result;
    result.ip = address.ip;
    result.port = address.port;
    return result;
}

void SetOption(intptr_t socket, int level, int name, int value)
{
    NsnSetSockOpt(static_cast<int>(socket), level, name, &value, sizeof(value));
}
}

bool Start(bool* connected)
{
    NsnLibInit();
    NsnSetMulticastState(1);
    nn::ac::ConnectAsync();
    *connected = false;
    return true;
}

bool CheckConnected()
{
    bool connected = false;
    nn::ac::IsApplicationConnected(&connected);
    if (!connected)
        connected = nn::ac::Connect().IsSuccess();
    return connected;
}

int LastError()
{
    return NsnLastError();
}

EError Classify(int code)
{
    if (code == kWouldBlock)
        return eError_WouldBlock;
    if (code == kInProgress)
        return eError_InProgress;
    if (code == kTimedOut)
        return eError_TimedOut;
    if (code == kRefused)
        return eError_Refused;
    if (code == kAddressInUse)
        return eError_AddressInUse;
    if (code == kNetUnreachable || code == kHostUnreachable)
        return eError_Unreachable;
    return eError_Other;
}

int TimedOutCode()
{
    return kTimedOut;
}

intptr_t Open(bool stream)
{
    const int socket = NsnSocket(kAfInet, stream ? kSockStream : kSockDgram, stream ? kProtoTcp : kProtoUdp);
    return socket < 0 ? kInvalid : socket;
}

void Close(intptr_t socket)
{
    NsnSocketClose(static_cast<int>(socket));
}

void SetBlocking(intptr_t socket, bool blocking)
{
    SetOption(socket, kSolSocket, blocking ? kSoBlocking : kSoNonBlocking, 1);
}

void SetNoDelay(intptr_t socket)
{
    SetOption(socket, kProtoTcp, kTcpNoDelay, 1);
}

void SetBroadcast(intptr_t socket)
{
    SetOption(socket, kSolSocket, kSoBroadcast, 1);
}

void PrepareListener(intptr_t socket)
{
    SetOption(socket, kSolSocket, kSoReuseAddr, 1);
}

void PrepareSharedPort(intptr_t socket)
{
    SetOption(socket, kSolSocket, kSoReuseAddr, 1);
}

int Connect(intptr_t socket, const Address& address)
{
    const NsnSockaddr target = ToNative(address);
    return NsnConnect(static_cast<int>(socket), &target, sizeof(target));
}

int PendingError(intptr_t socket)
{
    NsnSockaddr peer;
    int length = sizeof(peer);
    return NsnGetPeerName(static_cast<int>(socket), &peer, &length) != 0 ? kRefused : 0;
}

int Bind(intptr_t socket, const Address& address)
{
    const NsnSockaddr target = ToNative(address);
    return NsnBind(static_cast<int>(socket), &target, sizeof(target));
}

int Listen(intptr_t socket)
{
    const int result = NsnListen(static_cast<int>(socket), 8);
    if (result == 0)
        SetBlocking(socket, false);
    return result;
}

intptr_t AcceptWithin(intptr_t listener, int timeoutMs, Address& peer)
{
    NsnSockaddr from;
    memset(&from, 0, sizeof(from));
    int length = sizeof(from);
    const int socket = NsnAccept(static_cast<int>(listener), &from, &length);
    if (socket < 0)
    {
        ::Sleep(timeoutMs);
        return kInvalid;
    }
    SetBlocking(socket, true);
    peer = FromNative(from);
    return socket;
}

bool WaitFor(intptr_t socket, bool write, int timeoutMs)
{
    const int native = static_cast<int>(socket);
    uint32_t set = 1u << native;
    uint32_t errors = 1u << native;
    NsnTimeval timeout;
    timeout.seconds = timeoutMs / 1000;
    timeout.microseconds = (timeoutMs % 1000) * 1000;
    return NsnSelect(native + 1, write ? 0 : &set, write ? &set : 0, &errors, &timeout) > 0;
}

int Send(intptr_t socket, const void* data, int size)
{
    return NsnSend(static_cast<int>(socket), data, size, 0);
}

int Receive(intptr_t socket, void* data, int size)
{
    return NsnRecv(static_cast<int>(socket), data, size, 0);
}

int SendTo(intptr_t socket, const void* data, int size, const Address& address)
{
    const NsnSockaddr target = ToNative(address);
    return NsnSendTo(static_cast<int>(socket), data, size, 0, &target, sizeof(target));
}

int ReceiveFrom(intptr_t socket, void* data, int size, Address& address)
{
    NsnSockaddr source;
    memset(&source, 0, sizeof(source));
    int length = sizeof(source);
    const int received = NsnRecvFrom(static_cast<int>(socket), data, size, 0, &source, &length);
    if (received > 0)
        address = FromNative(source);
    return received;
}

void Shutdown(intptr_t socket)
{
    NsnShutdown(static_cast<int>(socket), kShutBoth);
}

bool ResolveHost(const std::string& host, uint32_t& ip)
{
    unsigned int parts[4];
    char extra;
    if (sscanf(host.c_str(), "%u.%u.%u.%u%c", &parts[0], &parts[1], &parts[2], &parts[3], &extra) == 4
        && parts[0] < 256 && parts[1] < 256 && parts[2] < 256 && parts[3] < 256)
    {
        ip = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
        return true;
    }
    NsnHostent* entry = NsnGetHostByName(host.c_str());
    if (entry == 0 || entry->addresses == 0 || entry->addresses[0] == 0 || entry->length != 4)
        return false;
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(entry->addresses[0]);
    ip = ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) | ((uint32_t)bytes[2] << 8) | bytes[3];
    return true;
}

void AddBroadcastAddresses(std::vector<uint32_t>& addresses)
{
    uint32_t ip = 0;
    uint32_t mask = 0;
    if (nn::ac::GetAssignedAddress(&ip).IsSuccess() && nn::ac::GetAssignedSubnet(&mask).IsSuccess() && ip != 0)
        addresses.push_back(ip | ~mask);
}

uint32_t LocalAddress()
{
    uint32_t assigned = 0;
    if (nn::ac::GetAssignedAddress(&assigned).IsSuccess())
        return assigned;
    return 0;
}
}
}
