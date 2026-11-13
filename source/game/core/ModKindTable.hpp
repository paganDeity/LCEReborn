#ifndef GAME_CORE_MODKINDTABLE_HPP
#define GAME_CORE_MODKINDTABLE_HPP

#include <string>

// a name a mod gives for one of the game's own classes, and the function that makes one
template <class Factory> struct ModKind
{
    const wchar_t* name;
    Factory make;
};

template <class Kind, size_t Count> const Kind* ModKindFind(const Kind (&kinds)[Count], const std::wstring& name)
{
    for (size_t index = 0; index < Count; ++index)
    {
        if (name == kinds[index].name)
            return &kinds[index];
    }
    return NULL;
}

#endif
