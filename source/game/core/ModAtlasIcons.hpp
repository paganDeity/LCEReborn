#ifndef GAME_CORE_MODATLASICONS_HPP
#define GAME_CORE_MODATLASICONS_HPP

#include <string>
#include <vector>

class BufferedImage;

namespace ModAtlasIcons
{
struct Placement
{
    std::wstring name;
    int x;
    int y;
    int size;
};

enum EAtlas
{
    eAtlas_Terrain = 0,
    eAtlas_Items = 1
};

bool Declare(int atlas, const std::wstring& name, int width, int height, const std::vector<int>& argb);
std::vector<std::wstring> Names(int atlas);
BufferedImage* Extend(int atlas, BufferedImage* original, std::vector<Placement>* placements);
}

#endif
