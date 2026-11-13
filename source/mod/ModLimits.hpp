#ifndef MOD_MODLIMITS_HPP
#define MOD_MODLIMITS_HPP

#include <stddef.h>
#include <stdint.h>

#include <string>

namespace mod
{
class Limits
{
public:
    Limits()
        : memoryBytes(16u * 1024 * 1024), maxMemoryBytes(256u * 1024 * 1024), instructionBudget(5000000),
          maxInstructionBudget(500000000), failureLimit(10), maxMods(256), wasmStackBytes(128u * 1024),
          pollMilliseconds(1000), maxFileBytes(16u * 1024 * 1024)
    {
    }

    bool Apply(const std::string& text, std::string* error);

    size_t memoryBytes;
    size_t maxMemoryBytes;
    int instructionBudget;
    int maxInstructionBudget;
    int failureLimit;
    size_t maxMods;
    unsigned int wasmStackBytes;
    int64_t pollMilliseconds;
    size_t maxFileBytes;
};
}

#endif
