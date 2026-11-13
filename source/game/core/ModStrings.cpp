#include "game/core/ModStrings.hpp"

#include <deque>
#include <map>
#include <vector>

#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/CMinecraftApp.hpp"

extern CConsoleMinecraftApp gApp;

namespace
{
const int kBase = 0x4D530000;
const size_t kMax = 65535;

std::deque<std::wstring>& Texts()
{
    static std::deque<std::wstring> texts;
    return texts;
}

// language code to key to text
std::map<std::wstring, std::map<std::wstring, std::wstring> >& Translations()
{
    static std::map<std::wstring, std::map<std::wstring, std::wstring> > translations;
    return translations;
}
}

namespace ModStrings
{
int Add(const std::wstring& text)
{
    if (Texts().size() >= kMax)
        return 0;
    Texts().push_back(text);
    return kBase + static_cast<int>(Texts().size());
}

const wchar_t* Lookup(int id)
{
    const int index = id - kBase;
    if (index < 1 || static_cast<size_t>(index) > Texts().size())
        return NULL;
    const std::wstring& key = Texts()[index - 1];
    if (Translations().empty())
        return key.c_str();
    std::vector<std::wstring> locales;
    gApp.getLocale(locales);
    for (size_t at = 0; at < locales.size(); ++at)
    {
        std::map<std::wstring, std::map<std::wstring, std::wstring> >::const_iterator language = Translations().find(locales[at]);
        if (language == Translations().end())
            continue;
        std::map<std::wstring, std::wstring>::const_iterator found = language->second.find(key);
        if (found != language->second.end())
            return found->second.c_str();
    }
    return key.c_str();
}

bool Translate(const std::wstring& locale, const std::wstring& key, const std::wstring& text)
{
    if (locale.empty() || key.empty() || key.size() > 256 || text.size() > 1024)
        return false;
    Translations()[locale][key] = text;
    return true;
}
}
