#include "net/NetSocket.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game/core/CriticalSection.hpp"
#include "net/NetNative.hpp"

namespace lcenet
{
namespace
{
std::string ConnectError(int code)
{
    const native::EError kind = native::Classify(code);
    if (kind == native::eError_Refused)
        return "Nothing is accepting games at that address.";
    if (kind == native::eError_TimedOut)
        return "The server didn't answer.";
    if (kind == native::eError_Unreachable)
        return "That address can't be reached from this network.";
    char text[64];
    snprintf(text, sizeof(text), "Couldn't connect (error %d).", code);
    return text;
}

std::string Trim(const std::string& text)
{
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return std::string();
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

C4JCriticalSection gStartupLock;
bool gStarted = false;
bool gConnected = false;
}

bool native::IsPrivateAddress(uint32_t ip)
{
    return (ip >> 24) == 10 || (ip >> 20) == 0xAC1 || (ip >> 16) == 0xC0A8;
}

std::string Address::Text() const
{
    char text[32];
    snprintf(text, sizeof(text), "%u.%u.%u.%u:%u", (unsigned int)((ip >> 24) & 0xff),
             (unsigned int)((ip >> 16) & 0xff), (unsigned int)((ip >> 8) & 0xff), (unsigned int)(ip & 0xff),
             (unsigned int)port);
    return text;
}

void Startup()
{
    if (gStarted)
        return;
    InitializeCriticalSection(&gStartupLock);
    gStarted = native::Start(&gConnected);
}

bool EnsureConnected()
{
    if (!gStarted)
        return false;
    EnterCriticalSection(&gStartupLock);
    if (!gConnected)
        gConnected = native::CheckConnected();
    bool connected = gConnected;
    LeaveCriticalSection(&gStartupLock);
    return connected;
}

bool Resolve(const std::string& input, Address& address, std::string& error)
{
    std::string text = Trim(input);
    if (text.empty())
    {
        error = "Enter a server address.";
        return false;
    }
    if (text[0] == '[' || text.find(':') != text.rfind(':'))
    {
        error = "IPv6 addresses aren't supported.";
        return false;
    }

    std::string host = text;
    unsigned long port = kDefaultPort;
    size_t colon = text.find(':');
    if (colon != std::string::npos)
    {
        host = text.substr(0, colon);
        std::string portText = text.substr(colon + 1);
        char* end = 0;
        port = strtoul(portText.c_str(), &end, 10);
        if (portText.empty() || *end != 0 || port == 0 || port > 65535)
        {
            error = "That port isn't valid.";
            return false;
        }
    }
    if (host.empty())
    {
        error = "Enter a server address.";
        return false;
    }
    if (!EnsureConnected())
    {
        error = "This system isn't connected to a network.";
        return false;
    }
    uint32_t ip = 0;
    if (!native::ResolveHost(host, ip))
    {
        error = "Couldn't find " + host + ".";
        return false;
    }
    address.ip = ip;
    address.port = (unsigned short)port;
    return true;
}

std::vector<uint32_t> BroadcastAddresses()
{
    std::vector<uint32_t> addresses;
    addresses.push_back(0xffffffffu);
    native::AddBroadcastAddresses(addresses);
    std::vector<uint32_t> unique;
    for (size_t index = 0; index < addresses.size(); ++index)
    {
        bool seen = false;
        for (size_t other = 0; other < unique.size(); ++other)
            seen = seen || unique[other] == addresses[index];
        if (!seen)
            unique.push_back(addresses[index]);
    }
    return unique;
}

bool LocalAddress(uint32_t& ip)
{
    ip = 0;
    if (!gStarted)
        return false;
    ip = native::LocalAddress();
    return ip != 0;
}

TcpSocket::TcpSocket() : m_handle(kInvalid)
{
}

TcpSocket::~TcpSocket()
{
    Close();
}

void TcpSocket::Swap(TcpSocket& other)
{
    intptr_t handle = m_handle;
    m_handle = other.m_handle;
    other.m_handle = handle;
}

bool TcpSocket::Connect(const Address& address, int timeoutMs, const CancelFlag& cancel, std::string& error)
{
    Close();
    if (!EnsureConnected())
    {
        error = "This system isn't connected to a network.";
        return false;
    }
    intptr_t socket = native::Open(true);
    if (socket == native::kInvalid)
    {
        error = ConnectError(native::LastError());
        return false;
    }
    native::SetBlocking(socket, false);
    if (native::Connect(socket, address) != 0)
    {
        int code = native::LastError();
        native::EError kind = native::Classify(code);
        if (kind != native::eError_WouldBlock && kind != native::eError_InProgress)
        {
            native::Close(socket);
            error = ConnectError(code);
            return false;
        }
        int waited = 0;
        while (!native::WaitFor(socket, true, 100))
        {
            waited += 100;
            if (cancel.Cancelled())
            {
                native::Close(socket);
                error = "Cancelled.";
                return false;
            }
            if (waited >= timeoutMs)
            {
                native::Close(socket);
                error = ConnectError(native::TimedOutCode());
                return false;
            }
        }
        int result = native::PendingError(socket);
        if (result != 0)
        {
            native::Close(socket);
            error = ConnectError(result);
            return false;
        }
    }
    native::SetBlocking(socket, true);
    native::SetNoDelay(socket);
    m_handle = socket;
    return true;
}

bool TcpSocket::Listen(unsigned short port, std::string& error)
{
    Address any;
    any.port = port;
    return Listen(any, error);
}

bool TcpSocket::Listen(const Address& address, std::string& error)
{
    Close();
    if (!EnsureConnected())
    {
        error = "This system isn't connected to a network.";
        return false;
    }
    intptr_t socket = native::Open(true);
    if (socket == native::kInvalid)
    {
        error = "Couldn't open a socket.";
        return false;
    }
    native::PrepareListener(socket);
    if (native::Bind(socket, address) != 0)
    {
        int code = native::LastError();
        native::Close(socket);
        char text[96];
        if (native::Classify(code) == native::eError_AddressInUse)
            snprintf(text, sizeof(text), "Port %u is already in use.", (unsigned int)address.port);
        else
            snprintf(text, sizeof(text), "Couldn't use port %u (error %d).", (unsigned int)address.port, code);
        error = text;
        return false;
    }
    if (native::Listen(socket) != 0)
    {
        native::Close(socket);
        error = "Couldn't listen for connections.";
        return false;
    }
    m_handle = socket;
    return true;
}

bool TcpSocket::Accept(int timeoutMs, TcpSocket& accepted, Address& peer)
{
    if (!IsOpen())
        return false;
    intptr_t socket = native::AcceptWithin(m_handle, timeoutMs, peer);
    if (socket == native::kInvalid)
        return false;
    native::SetNoDelay(socket);
    accepted.Close();
    accepted.m_handle = socket;
    return true;
}

bool TcpSocket::SendAll(const void* data, size_t size)
{
    const char* bytes = static_cast<const char*>(data);
    while (size > 0)
    {
        if (!IsOpen())
            return false;
        int chunk = size > 0x10000 ? 0x10000 : (int)size;
        int sent = native::Send(m_handle, bytes, chunk);
        if (sent <= 0)
            return false;
        bytes += sent;
        size -= (size_t)sent;
    }
    return true;
}

int TcpSocket::Receive(void* data, size_t size, int timeoutMs)
{
    if (!IsOpen())
        return -1;
    if (!native::WaitFor(m_handle, false, timeoutMs))
        return 0;
    int chunk = size > 0x10000 ? 0x10000 : (int)size;
    int received = native::Receive(m_handle, data, chunk);
    return received > 0 ? received : -1;
}

void TcpSocket::Shutdown()
{
    if (IsOpen())
        native::Shutdown(m_handle);
}

void TcpSocket::Close()
{
    if (!IsOpen())
        return;
    native::Close(m_handle);
    m_handle = kInvalid;
}

UdpSocket::UdpSocket() : m_handle(kInvalid)
{
}

UdpSocket::~UdpSocket()
{
    Close();
}

bool UdpSocket::Open(unsigned short bindPort, std::string& error)
{
    Close();
    if (!EnsureConnected())
    {
        error = "This system isn't connected to a network.";
        return false;
    }
    intptr_t socket = native::Open(false);
    if (socket == native::kInvalid)
    {
        error = "Couldn't open a socket.";
        return false;
    }
    native::SetBroadcast(socket);
    if (bindPort != 0)
    {
        native::PrepareSharedPort(socket);
        Address any;
        any.port = bindPort;
        if (native::Bind(socket, any) != 0)
        {
            char text[96];
            snprintf(text, sizeof(text), "Couldn't use port %u (error %d).", (unsigned int)bindPort,
                     native::LastError());
            error = text;
            native::Close(socket);
            return false;
        }
    }
    m_handle = socket;
    return true;
}

bool UdpSocket::SendTo(const Address& address, const void* data, size_t size)
{
    if (!IsOpen())
        return false;
    return native::SendTo(m_handle, data, (int)size, address) == (int)size;
}

int UdpSocket::ReceiveFrom(void* data, size_t size, Address& from, int timeoutMs)
{
    if (!IsOpen())
        return -1;
    if (!native::WaitFor(m_handle, false, timeoutMs))
        return 0;
    int received = native::ReceiveFrom(m_handle, data, (int)size, from);
    if (received <= 0)
        return received < 0 ? -1 : 0;
    return received;
}

void UdpSocket::Close()
{
    if (!IsOpen())
        return;
    native::Close(m_handle);
    m_handle = kInvalid;
}
}
