#include "platform/pc/PcCommandLine.hpp"

#include <windows.h>
#include <shellapi.h>

#include "platform/pc/PcUtf8.hpp"

std::vector<std::string> PcCommandLineArguments(int, char**)
{
    std::vector<std::string> arguments;
    int count = 0;
    LPWSTR* wide = CommandLineToArgvW(GetCommandLineW(), &count);
    if (wide == NULL)
        return arguments;
    for (int index = 1; index < count; ++index)
        arguments.push_back(PcWideToUtf8(wide[index]));
    LocalFree(wide);
    return arguments;
}
