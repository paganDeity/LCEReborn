namespace lcenet
{
namespace native
{
uint32_t FirstPrivateInterface();

namespace
{
sockaddr_in ToNative(const Address& address)
{
    sockaddr_in result;
    memset(&result, 0, sizeof(result));
    result.sin_family = AF_INET;
    result.sin_addr.s_addr = htonl(address.ip);
    result.sin_port = htons(address.port);
    return result;
}

Address FromNative(const sockaddr_in& address)
{
    Address result;
    result.ip = ntohl(address.sin_addr.s_addr);
    result.port = ntohs(address.sin_port);
    return result;
}

void SetOption(intptr_t socket, int level, int name, int value)
{
    setsockopt(static_cast<NativeSocket>(socket), level, name, reinterpret_cast<const char*>(&value), sizeof(value));
}

uint32_t RoutedAddress()
{
    const intptr_t probe = Open(false);
    if (probe == kInvalid)
        return 0;
    uint32_t ip = 0;
    sockaddr_in target;
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(53);
    target.sin_addr.s_addr = htonl(0x01010101u);
    if (connect(static_cast<NativeSocket>(probe), reinterpret_cast<sockaddr*>(&target), sizeof(target)) == 0)
    {
        sockaddr_in local;
        memset(&local, 0, sizeof(local));
        socklen_t length = sizeof(local);
        if (getsockname(static_cast<NativeSocket>(probe), reinterpret_cast<sockaddr*>(&local), &length) == 0)
            ip = ntohl(local.sin_addr.s_addr);
    }
    Close(probe);
    return ip;
}
}

intptr_t Open(bool stream)
{
    const NativeSocket socket =
        ::socket(AF_INET, stream ? SOCK_STREAM : SOCK_DGRAM, stream ? IPPROTO_TCP : IPPROTO_UDP);
    return socket == kInvalidNative ? kInvalid : static_cast<intptr_t>(socket);
}

void SetNoDelay(intptr_t socket)
{
    SetOption(socket, IPPROTO_TCP, TCP_NODELAY, 1);
}

void SetBroadcast(intptr_t socket)
{
    SetOption(socket, SOL_SOCKET, SO_BROADCAST, 1);
}

int Connect(intptr_t socket, const Address& address)
{
    const sockaddr_in target = ToNative(address);
    return connect(static_cast<NativeSocket>(socket), reinterpret_cast<const sockaddr*>(&target), sizeof(target));
}

int PendingError(intptr_t socket)
{
    int result = 0;
    socklen_t length = sizeof(result);
    getsockopt(static_cast<NativeSocket>(socket), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&result), &length);
    return result;
}

int Bind(intptr_t socket, const Address& address)
{
    const sockaddr_in target = ToNative(address);
    return bind(static_cast<NativeSocket>(socket), reinterpret_cast<const sockaddr*>(&target), sizeof(target));
}

int Listen(intptr_t socket)
{
    return listen(static_cast<NativeSocket>(socket), 8);
}

intptr_t AcceptWithin(intptr_t listener, int timeoutMs, Address& peer)
{
    if (!WaitFor(listener, false, timeoutMs))
        return kInvalid;
    sockaddr_in from;
    memset(&from, 0, sizeof(from));
    socklen_t length = sizeof(from);
    const NativeSocket socket =
        accept(static_cast<NativeSocket>(listener), reinterpret_cast<sockaddr*>(&from), &length);
    if (socket == kInvalidNative)
        return kInvalid;
    peer = FromNative(from);
    return static_cast<intptr_t>(socket);
}

bool WaitFor(intptr_t socket, bool write, int timeoutMs)
{
    const NativeSocket native = static_cast<NativeSocket>(socket);
    fd_set set;
    FD_ZERO(&set);
    FD_SET(native, &set);
    fd_set errors;
    FD_ZERO(&errors);
    FD_SET(native, &errors);
    timeval timeout;
    timeout.tv_sec = timeoutMs / 1000;
    timeout.tv_usec = (timeoutMs % 1000) * 1000;
    return select(static_cast<int>(native) + 1, write ? 0 : &set, write ? &set : 0, &errors, &timeout) > 0;
}

int Send(intptr_t socket, const void* data, int size)
{
    return send(static_cast<NativeSocket>(socket), static_cast<const char*>(data), size, kSendFlags);
}

int Receive(intptr_t socket, void* data, int size)
{
    return recv(static_cast<NativeSocket>(socket), static_cast<char*>(data), size, 0);
}

int SendTo(intptr_t socket, const void* data, int size, const Address& address)
{
    const sockaddr_in target = ToNative(address);
    return sendto(static_cast<NativeSocket>(socket), static_cast<const char*>(data), size, 0,
                  reinterpret_cast<const sockaddr*>(&target), sizeof(target));
}

int ReceiveFrom(intptr_t socket, void* data, int size, Address& address)
{
    sockaddr_in source;
    memset(&source, 0, sizeof(source));
    socklen_t length = sizeof(source);
    const int received = recvfrom(static_cast<NativeSocket>(socket), static_cast<char*>(data), size, 0,
                                  reinterpret_cast<sockaddr*>(&source), &length);
    if (received > 0)
        address = FromNative(source);
    return received;
}

void Shutdown(intptr_t socket)
{
    shutdown(static_cast<NativeSocket>(socket), kShutdownBoth);
}

bool ResolveHost(const std::string& host, uint32_t& ip)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* results = 0;
    if (getaddrinfo(host.c_str(), 0, &hints, &results) != 0 || results == 0)
        return false;
    ip = ntohl(reinterpret_cast<const sockaddr_in*>(results->ai_addr)->sin_addr.s_addr);
    freeaddrinfo(results);
    return true;
}

uint32_t LocalAddress()
{
    const uint32_t routed = RoutedAddress();
    uint32_t ip = IsPrivateAddress(routed) ? routed : FirstPrivateInterface();
    if (ip == 0 && routed != 0 && (routed >> 24) != 127)
        ip = routed;
    return ip;
}
}
}
