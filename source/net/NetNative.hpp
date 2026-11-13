#ifndef NET_NETNATIVE_HPP
#define NET_NETNATIVE_HPP

#include <stdint.h>

#include <string>
#include <vector>

#include "net/NetSocket.hpp"

namespace lcenet
{
namespace native
{
enum EError
{
    eError_Other,
    eError_WouldBlock,
    eError_InProgress,
    eError_TimedOut,
    eError_Refused,
    eError_AddressInUse,
    eError_Unreachable
};

const intptr_t kInvalid = -1;

bool Start(bool* connected);
bool CheckConnected();
int LastError();
EError Classify(int code);
int TimedOutCode();

intptr_t Open(bool stream);
void Close(intptr_t socket);
void SetBlocking(intptr_t socket, bool blocking);
void SetNoDelay(intptr_t socket);
void SetBroadcast(intptr_t socket);
void PrepareListener(intptr_t socket);
void PrepareSharedPort(intptr_t socket);

int Connect(intptr_t socket, const Address& address);
int PendingError(intptr_t socket);
int Bind(intptr_t socket, const Address& address);
int Listen(intptr_t socket);
intptr_t AcceptWithin(intptr_t listener, int timeoutMs, Address& peer);
bool WaitFor(intptr_t socket, bool write, int timeoutMs);
int Send(intptr_t socket, const void* data, int size);
int Receive(intptr_t socket, void* data, int size);
int SendTo(intptr_t socket, const void* data, int size, const Address& address);
int ReceiveFrom(intptr_t socket, void* data, int size, Address& address);
void Shutdown(intptr_t socket);

bool ResolveHost(const std::string& host, uint32_t& ip);
void AddBroadcastAddresses(std::vector<uint32_t>& addresses);
uint32_t LocalAddress();
bool IsPrivateAddress(uint32_t ip);
}
}

#endif
