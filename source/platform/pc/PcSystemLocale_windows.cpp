#include "platform/pc/PcSystemLocale.hpp"

#include <windows.h>

#include "platform/pc/PcUtf8.hpp"

std::string PcOperatingSystemLocaleName()
{
    wchar_t name[LOCALE_NAME_MAX_LENGTH];
    if (GetUserDefaultLocaleName(name, LOCALE_NAME_MAX_LENGTH) <= 0)
        return std::string();
    return PcWideToUtf8(name);
}
