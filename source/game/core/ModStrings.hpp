#ifndef GAME_CORE_MODSTRINGS_HPP
#define GAME_CORE_MODSTRINGS_HPP

#include <string>

// the names mods give their content: a registered title is a key, and the text the player sees is the translation for the game's language, or the key itself
namespace ModStrings
{
int Add(const std::wstring& text);
const wchar_t* Lookup(int id);
// Say what a title is called in a language, such as es-ES (the game's own codes: en-US, de-DE, fr-FR, ja-JP ...): a title given to registry.item or
// registry.block shows this text for players of that language, and the title itself for every other. Any time, from any mod; false for an empty key.
bool Translate(const std::wstring& locale, const std::wstring& key, const std::wstring& text);
}

#endif
