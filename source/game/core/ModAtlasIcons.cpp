#include "game/core/ModAtlasIcons.hpp"

#include <map>

#include "game/core/BufferedImage.hpp"

namespace
{
const int kColumns = 16;
const size_t kMaxIcons = 512;

struct Icon
{
    int width;
    int height;
    std::vector<int> argb;
};

std::map<std::wstring, Icon>& Icons(int atlas)
{
    static std::map<std::wstring, Icon> icons[2];
    return icons[atlas == 0 ? 0 : 1];
}
}

namespace ModAtlasIcons
{
bool Declare(int atlas, const std::wstring& name, int width, int height, const std::vector<int>& argb)
{
    if (width < 1 || height < 1 || argb.size() != static_cast<size_t>(width) * height || Icons(atlas).size() >= kMaxIcons)
        return false;
    Icon icon = {width, height, argb};
    Icons(atlas)[name] = icon;
    return true;
}

std::vector<std::wstring> Names(int atlas)
{
    std::vector<std::wstring> names;
    for (std::map<std::wstring, Icon>::const_iterator it = Icons(atlas).begin(); it != Icons(atlas).end(); ++it)
        names.push_back(it->first);
    return names;
}

BufferedImage* Extend(int atlas, BufferedImage* original, std::vector<Placement>* placements)
{
    placements->clear();
    if (Icons(atlas).empty())
        return original;
    const int width = static_cast<int>(original->getWidth());
    const int height = static_cast<int>(original->getHeight());
    const int size = width / kColumns;
    const int rows = (static_cast<int>(Icons(atlas).size()) + kColumns - 1) / kColumns;
    BufferedImage* extended = new BufferedImage(width, height + rows * size, 0);
    int* target = extended->getData();
    const int* source = original->getData();
    for (int index = 0; index < width * height; ++index)
        target[index] = source[index];
    for (int index = width * height; index < width * (height + rows * size); ++index)
        target[index] = 0;
    int slot = 0;
    for (std::map<std::wstring, Icon>::const_iterator it = Icons(atlas).begin(); it != Icons(atlas).end(); ++it, ++slot)
    {
        const Icon& icon = it->second;
        Placement placement = {it->first, (slot % kColumns) * size, height + (slot / kColumns) * size, size};
        for (int row = 0; row < size; ++row)
            for (int column = 0; column < size; ++column)
                target[(placement.y + row) * width + placement.x + column] =
                    icon.argb[static_cast<size_t>(row * icon.height / size) * icon.width + column * icon.width / size];
        placements->push_back(placement);
    }
    delete original;
    return extended;
}
}
