#include "game/command/CommandLimits.hpp"

#include <stdlib.h>

namespace
{
int sExecuteForks = 4096;
int sCompareBlocks = 32768;
int sCommandChain = 65536;
}

namespace CommandLimits
{
int ExecuteForks()
{
    return sExecuteForks;
}

int CompareBlocks()
{
    return sCompareBlocks;
}

int CommandChain()
{
    return sCommandChain;
}

bool Apply(const std::string& text, std::string* error)
{
    size_t at = 0;
    while (at <= text.size())
    {
        size_t end = text.find(',', at);
        if (end == std::string::npos)
            end = text.size();
        const std::string pair = text.substr(at, end - at);
        at = end + 1;
        if (pair.empty())
            continue;
        const size_t equals = pair.find('=');
        const int value = equals == std::string::npos ? 0 : atoi(pair.c_str() + equals + 1);
        const std::string key = pair.substr(0, equals);
        if (value < 1)
        {
            *error = "a limit is name=number, the number at least 1: " + pair;
            return false;
        }
        if (key == "execute-forks")
            sExecuteForks = value;
        else if (key == "compare-blocks")
            sCompareBlocks = value;
        else if (key == "command-chain")
            sCommandChain = value;
        else
        {
            *error = "unknown command limit " + key;
            return false;
        }
    }
    return true;
}
}
