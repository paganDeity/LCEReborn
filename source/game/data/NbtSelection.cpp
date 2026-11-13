#include "game/data/NbtSelection.hpp"

#include "game/data/Tag.hpp"

NbtSelection::NbtSelection()
{
}

NbtSelection::~NbtSelection()
{
    clear();
}

void NbtSelection::clear()
{
    for (size_t index = 0; index < mOwned.size(); ++index)
        delete mOwned[index];
    mOwned.clear();
    mTags.clear();
}

void NbtSelection::add(Tag* tag)
{
    mTags.push_back(tag);
}

void NbtSelection::addOwned(Tag* tag)
{
    own(tag);
    mTags.push_back(tag);
}

void NbtSelection::own(Tag* tag)
{
    mOwned.push_back(tag);
}

bool NbtSelection::empty() const
{
    return mTags.empty();
}

size_t NbtSelection::size() const
{
    return mTags.size();
}

Tag* NbtSelection::get(size_t index) const
{
    return mTags[index];
}

const std::vector<Tag*>& NbtSelection::tags() const
{
    return mTags;
}
