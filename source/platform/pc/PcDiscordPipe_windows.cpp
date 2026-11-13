#include "platform/pc/PcDiscordPipe.hpp"

#include <windows.h>

#include <string>

static HANDLE Handle(std::intptr_t handle)
{
    return reinterpret_cast<HANDLE>(handle);
}

bool PcDiscordPipe::Open()
{
    for (int index = 0; index < 10; ++index)
    {
        const std::string name = "\\\\?\\pipe\\discord-ipc-" + std::to_string(index);
        const HANDLE handle = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (handle != INVALID_HANDLE_VALUE)
        {
            mHandle = reinterpret_cast<std::intptr_t>(handle);
            return true;
        }
    }
    return false;
}

void PcDiscordPipe::Close()
{
    if (Handle(mHandle) != INVALID_HANDLE_VALUE)
        CloseHandle(Handle(mHandle));
    mHandle = -1;
}

bool PcDiscordPipe::Write(const std::vector<unsigned char>& message)
{
    DWORD written = 0;
    return WriteFile(Handle(mHandle), message.data(), static_cast<DWORD>(message.size()), &written, NULL)
           && written == message.size();
}

bool PcDiscordPipe::Drain()
{
    unsigned char buffer[4096];
    DWORD available = 0;
    while (PeekNamedPipe(Handle(mHandle), NULL, 0, NULL, &available, NULL))
    {
        if (available == 0)
            return true;
        DWORD read = 0;
        if (!ReadFile(Handle(mHandle), buffer, available < sizeof(buffer) ? available : sizeof(buffer), &read, NULL)
            || read == 0)
            return false;
    }
    return false;
}
