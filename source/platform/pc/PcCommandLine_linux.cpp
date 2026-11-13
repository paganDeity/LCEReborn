#include "platform/pc/PcCommandLine.hpp"

std::vector<std::string> PcCommandLineArguments(int argc, char** argv)
{
    std::vector<std::string> arguments;
    for (int index = 1; index < argc; ++index)
        arguments.push_back(argv[index]);
    return arguments;
}
