#include "platform/pc/PcSystemLocale.hpp"

#include <cstdlib>

std::string PcOperatingSystemLocaleName()
{
    const char* variables[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
    for (const char* variable : variables)
    {
        const char* value = std::getenv(variable);
        if (value != nullptr && value[0] != '\0')
            return value;
    }
    return std::string();
}
