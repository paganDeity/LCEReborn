#ifndef GAME_CORE_MODTEXT_HPP
#define GAME_CORE_MODTEXT_HPP

#include <string>

namespace ModText
{
std::string ToUtf8(const std::wstring& text);
std::wstring FromUtf8(const std::string& text);
// a name a mod gives to what it registers: 1 to 32 characters of a-z, 0-9 and _ (and . for a sound), so it is safe in a key, a file and a command
bool IsName(const std::wstring& name, bool allowDots = false);
}

#endif
