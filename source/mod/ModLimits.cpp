#include "mod/ModLimits.hpp"

#include <stdlib.h>

namespace mod
{
bool Limits::Apply(const std::string& text, std::string* error)
{
    size_t at = 0;
    while (at <= text.size())
    {
        size_t end = text.find('\n', at);
        if (end == std::string::npos)
            end = text.size();
        std::string line = text.substr(at, end - at);
        at = end + 1;
        while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == ' '))
            line.erase(line.size() - 1);
        if (line.empty() || line[0] == '#')
            continue;
        const size_t equals = line.find('=');
        if (equals == std::string::npos)
        {
            if (error != NULL)
                *error = "a line without = in limits.txt";
            return false;
        }
        const std::string key = line.substr(0, equals);
        const long long value = atoll(line.c_str() + equals + 1);
        if (value <= 0)
        {
            if (error != NULL)
                *error = key + " must be a positive number";
            return false;
        }
        if (key == "memory-mb")
            memoryBytes = static_cast<size_t>(value) * 1024 * 1024;
        else if (key == "max-memory-mb")
            maxMemoryBytes = static_cast<size_t>(value) * 1024 * 1024;
        else if (key == "budget-millions")
            instructionBudget = static_cast<int>(value * 1000000 > 2000000000LL ? 2000000000LL : value * 1000000);
        else if (key == "max-budget-millions")
            maxInstructionBudget = static_cast<int>(value * 1000000 > 2000000000LL ? 2000000000LL : value * 1000000);
        else if (key == "failure-limit")
            failureLimit = static_cast<int>(value);
        else if (key == "max-mods")
            maxMods = static_cast<size_t>(value);
        else if (key == "wasm-stack-kb")
            wasmStackBytes = static_cast<unsigned int>(value) * 1024;
        else if (key == "max-file-mb")
            maxFileBytes = static_cast<size_t>(value) * 1024 * 1024;
        else if (key == "poll-ms")
            pollMilliseconds = value;
        else
        {
            if (error != NULL)
                *error = "unknown limit " + key;
            return false;
        }
    }
    return true;
}
}
