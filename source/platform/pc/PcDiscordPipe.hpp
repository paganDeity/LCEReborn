#ifndef PLATFORM_PC_PCDISCORDPIPE_HPP
#define PLATFORM_PC_PCDISCORDPIPE_HPP

#include <cstdint>
#include <vector>

class PcDiscordPipe
{
public:
    PcDiscordPipe();
    ~PcDiscordPipe();

    bool Open();
    void Close();
    bool Write(const std::vector<unsigned char>& message);
    bool Drain();

private:
    PcDiscordPipe(const PcDiscordPipe&);
    PcDiscordPipe& operator=(const PcDiscordPipe&);

    std::intptr_t mHandle;
};

#endif
