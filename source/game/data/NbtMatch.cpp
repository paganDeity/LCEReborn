#include "game/data/NbtMatch.hpp"

#include <string>
#include <vector>

#include "game/data/CompoundTag.hpp"
#include "game/data/ListTag.hpp"
#include "game/data/Tag.hpp"

namespace NbtMatch
{
bool matches(Tag* pattern, Tag* actual)
{
    if (pattern == NULL)
        return true;
    if (actual == NULL)
        return false;
    if (pattern->getId() != actual->getId())
        return false;
    if (pattern->getId() == Tag::TAG_COMPOUND)
    {
        CompoundTag* expected = static_cast<CompoundTag*>(pattern);
        CompoundTag* found = static_cast<CompoundTag*>(actual);
        std::vector<std::wstring>* keys = expected->getAllKeys();
        bool equal = true;
        for (size_t index = 0; index < keys->size() && equal; ++index)
        {
            const std::wstring& key = (*keys)[index];
            if (!found->contains(key) || !matches(expected->get(key), found->get(key)))
                equal = false;
        }
        delete keys;
        return equal;
    }
    if (pattern->getId() == Tag::TAG_LIST)
    {
        ListTag* expected = static_cast<ListTag*>(pattern);
        ListTag* found = static_cast<ListTag*>(actual);
        if (expected->size() == 0)
            return found->size() == 0;
        for (int index = 0; index < expected->size(); ++index)
        {
            bool present = false;
            for (int candidate = 0; candidate < found->size() && !present; ++candidate)
                present = matches(expected->get(index), found->get(candidate));
            if (!present)
                return false;
        }
        return true;
    }
    return pattern->equals(actual);
}
}
