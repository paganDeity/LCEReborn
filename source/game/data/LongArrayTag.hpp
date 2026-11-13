#ifndef GAME_DATA_LONGARRAYTAG_HPP
#define GAME_DATA_LONGARRAYTAG_HPP

#include <string>

#include "game/data/Tag.hpp"
#include "game/world/biome/arrayWithLength.hpp"

class DataInput;
class DataOutput;

class LongArrayTag : public Tag
{
public:
    LongArrayTag();
    LongArrayTag(arrayWithLength<long long> data);

    virtual void write(DataOutput* output);
    virtual void load(DataInput* input, int depth);
    virtual std::wstring toString();
    virtual unsigned char getId();
    virtual ~LongArrayTag();
    virtual bool equals(Tag* other);
    virtual LongArrayTag* copy();

    arrayWithLength<long long> getAsLongArray();
    void setData(arrayWithLength<long long> data);

private:
    arrayWithLength<long long> mData;
};

#endif
