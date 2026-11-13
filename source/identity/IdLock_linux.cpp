#include "identity/IdLock.hpp"

#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

namespace identity
{
Lock::Lock() : m_handle(-1)
{
}

Lock::~Lock()
{
    Release();
}

bool Lock::Acquire(const std::string& path)
{
    if (Held())
        return true;
    const int file = open(path.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0600);
    if (file < 0)
        return false;
    if (flock(file, LOCK_EX | LOCK_NB) != 0)
    {
        close(file);
        return false;
    }
    m_handle = file;
    return true;
}

void Lock::Release()
{
    if (!Held())
        return;
    close(static_cast<int>(m_handle));
    m_handle = -1;
}
}
