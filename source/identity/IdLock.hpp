#ifndef IDENTITY_IDLOCK_HPP
#define IDENTITY_IDLOCK_HPP

#include <stdint.h>

#include <string>

namespace identity
{
class Lock
{
public:
    Lock();
    ~Lock();

    bool Acquire(const std::string& path);
    void Release();
    bool Held() const { return m_handle != -1; }

private:
    Lock(const Lock&);
    Lock& operator=(const Lock&);

    intptr_t m_handle;
};
}

#endif
