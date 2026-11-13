#include "game/core/Language.hpp"

static Language* sLanguage = 0;

#if LCE_PLATFORM_PC
Language::Language()
{
}
#endif

Language* Language::getInstance()
{
#if LCE_PLATFORM_PC
    // Parity Not Checked
    // nothing ever made the instance, so the screens that ask for it called members on a null pointer; the members
    // keep no state, so one instance does for all
    if (sLanguage == 0)
        sLanguage = new Language();
#endif
    return sLanguage;
}

std::wstring Language::getElement(const std::wstring& key, ...)
{
    return L"";
}

std::wstring Language::getElement(const std::wstring& key, va_list args)
{
    return L"";
}
