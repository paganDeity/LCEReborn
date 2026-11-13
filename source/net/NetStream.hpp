#ifndef NET_NETSTREAM_HPP
#define NET_NETSTREAM_HPP

#include <stddef.h>

namespace lcenet
{
class Stream
{
public:
    virtual ~Stream() {}
    virtual bool SendAll(const void* data, size_t size) = 0;
    virtual int Receive(void* data, size_t size, int timeoutMs) = 0;
    virtual void Shutdown() = 0;
    virtual void Close() = 0;
};
}

#endif
