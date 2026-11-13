#ifndef GAME_DATA_NBTSELECTION_HPP
#define GAME_DATA_NBTSELECTION_HPP

#include <stddef.h>

#include <vector>

class Tag;

class NbtSelection
{
public:
    NbtSelection();
    ~NbtSelection();

    void clear();
    void add(Tag* tag);
    void addOwned(Tag* tag);
    void own(Tag* tag);
    bool empty() const;
    size_t size() const;
    Tag* get(size_t index) const;
    const std::vector<Tag*>& tags() const;

private:
    NbtSelection(const NbtSelection&);
    NbtSelection& operator=(const NbtSelection&);

    std::vector<Tag*> mTags;
    std::vector<Tag*> mOwned;
};

#endif
