#include "platform/pc/PcDiscordPipe.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <string>

bool PcDiscordPipe::Open()
{
    static const char* const kVariables[] = {"XDG_RUNTIME_DIR", "TMPDIR", "TMP", "TEMP"};
    std::vector<std::string> folders;
    for (const char* variable : kVariables)
    {
        const char* value = std::getenv(variable);
        if (value != NULL && value[0] != '\0')
            folders.push_back(value);
    }
    folders.push_back("/tmp");
    for (int index = 0; index < 10; ++index)
    {
        for (const std::string& folder : folders)
        {
            const std::string path = folder + "/discord-ipc-" + std::to_string(index);
            sockaddr_un address;
            std::memset(&address, 0, sizeof(address));
            if (path.size() >= sizeof(address.sun_path))
                continue;
            address.sun_family = AF_UNIX;
            std::memcpy(address.sun_path, path.c_str(), path.size());
            const int socketHandle = socket(AF_UNIX, SOCK_STREAM, 0);
            if (socketHandle < 0)
                return false;
            if (connect(socketHandle, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0)
            {
                mHandle = socketHandle;
                return true;
            }
            close(socketHandle);
        }
    }
    return false;
}

void PcDiscordPipe::Close()
{
    if (mHandle >= 0)
        close(static_cast<int>(mHandle));
    mHandle = -1;
}

bool PcDiscordPipe::Write(const std::vector<unsigned char>& message)
{
    size_t sent = 0;
    while (sent < message.size())
    {
        const ssize_t result =
            send(static_cast<int>(mHandle), message.data() + sent, message.size() - sent, MSG_NOSIGNAL);
        if (result <= 0)
            return false;
        sent += static_cast<size_t>(result);
    }
    return true;
}

bool PcDiscordPipe::Drain()
{
    unsigned char buffer[4096];
    for (;;)
    {
        const ssize_t result = recv(static_cast<int>(mHandle), buffer, sizeof(buffer), MSG_DONTWAIT);
        if (result > 0)
            continue;
        if (result == 0)
            return false;
        return errno == EAGAIN || errno == EWOULDBLOCK;
    }
}
