#include "game/data/NbtCapabilities.hpp"

#include <string>
#include <vector>

#include "game/data/CompoundTag.hpp"
#include "game/data/ListTag.hpp"
#include "game/data/Tag.hpp"

namespace
{
unsigned int RequiredByType(unsigned char id)
{
    return id == Tag::TAG_LONG_ARRAY ? NbtCapabilities::eCapability_LongArrays : 0;
}

void Strip(Tag* tag, unsigned int allowed)
{
    if (tag->getId() == Tag::TAG_COMPOUND)
    {
        CompoundTag* compound = static_cast<CompoundTag*>(tag);
        std::vector<std::wstring>* keys = compound->getAllKeys();
        for (size_t index = 0; index < keys->size(); ++index)
        {
            Tag* child = compound->get((*keys)[index]);
            if ((RequiredByType(child->getId()) & ~allowed) != 0)
            {
                compound->remove((*keys)[index]);
                delete child;
                continue;
            }
            Strip(child, allowed);
        }
        delete keys;
    }
    else if (tag->getId() == Tag::TAG_LIST)
    {
        ListTag* list = static_cast<ListTag*>(tag);
        for (int index = list->size() - 1; index >= 0; --index)
        {
            if ((RequiredByType(list->get(index)->getId()) & ~allowed) != 0)
                delete list->remove(index);
            else
                Strip(list->get(index), allowed);
        }
    }
}
}

namespace NbtCapabilities
{
unsigned int Required(Tag* tag)
{
    if (tag == NULL)
        return 0;
    unsigned int required = RequiredByType(tag->getId());
    if (tag->getId() == Tag::TAG_COMPOUND)
    {
        CompoundTag* compound = static_cast<CompoundTag*>(tag);
        std::vector<std::wstring>* keys = compound->getAllKeys();
        for (size_t index = 0; index < keys->size(); ++index)
            required |= Required(compound->get((*keys)[index]));
        delete keys;
    }
    else if (tag->getId() == Tag::TAG_LIST)
    {
        ListTag* list = static_cast<ListTag*>(tag);
        for (int index = 0; index < list->size(); ++index)
            required |= Required(list->get(index));
    }
    return required;
}

CompoundTag* Restricted(CompoundTag* tag, unsigned int allowed)
{
    if (tag == NULL || (Required(tag) & ~allowed) == 0)
        return tag;
    CompoundTag* copy = tag->copy();
    Strip(copy, allowed);
    return copy;
}
}
