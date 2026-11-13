#include "platform/pc/PcDiscordPipe.hpp"

PcDiscordPipe::PcDiscordPipe() : mHandle(-1)
{
}

PcDiscordPipe::~PcDiscordPipe()
{
    Close();
}
