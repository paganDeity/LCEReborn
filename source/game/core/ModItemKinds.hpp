#ifndef GAME_CORE_MODITEMKINDS_HPP
#define GAME_CORE_MODITEMKINDS_HPP

#include <string>

class Item;

// the game's own item classes a mod can make more of, by name
namespace ModItemKinds
{
// four numbers whose meaning depends on the kind
struct Numbers
{
    float a;
    float b;
    float c;
    float d;
};

bool Known(const std::wstring& kind);
Item* Make(const std::wstring& kind, const Numbers& numbers);
}

#endif
