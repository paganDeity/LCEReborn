#ifndef GAME_DATA_NBTPATH_HPP
#define GAME_DATA_NBTPATH_HPP

#include <string>
#include <vector>

class NbtSelection;
class Tag;

class NbtPath
{
public:
    enum EModify
    {
        EModify_Set,
        EModify_Merge,
        EModify_Append,
        EModify_Prepend,
        EModify_Insert
    };

    static bool select(Tag* root, const std::wstring& path, NbtSelection* selected, std::wstring* error);
    static bool remove(Tag* root, const std::wstring& path, int* removed, std::wstring* error);
    static bool modify(Tag* root, const std::wstring& path, int operation, int index, Tag* value, int* changed,
                       std::wstring* error);
    static bool modify(Tag* root, const std::wstring& path, int operation, int index, const std::vector<Tag*>& values,
                       int* changed, std::wstring* error);
};

#endif
