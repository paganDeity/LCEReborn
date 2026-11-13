#include "game/data/LongArrayTag.hpp"
#include "game/data/NbtBudget.hpp"

#include <string.h>
#include <wchar.h>

#include "game/core/DataInput.hpp"
#include "game/core/DataOutput.hpp"

LongArrayTag::LongArrayTag()
{
}

LongArrayTag::LongArrayTag(arrayWithLength<long long> data)
{
    mData = data;
}

LongArrayTag::~LongArrayTag()
{
    delete[] mData.data;
}

void LongArrayTag::write(DataOutput* output)
{
    output->writeInt(mData.length);
    for (unsigned int index = 0; index < mData.length; ++index)
        output->writeLong(mData.data[index]);
}

void LongArrayTag::load(DataInput* input, int depth)
{
    int length = input->readInt();
#if LCE_PLATFORM_PC
    if (!NbtBudget::Charge(static_cast<long long>(length) * 8))
        length = 0;
#endif
    delete[] mData.data;
    mData = arrayWithLength<long long>(length, true);
    for (int index = 0; index < length; ++index)
        mData.data[index] = input->readLong();
}

std::wstring LongArrayTag::toString()
{
    wchar_t description[32];
    swprintf(description, 32, L"[%d longs]", mData.length);
    return description;
}

unsigned char LongArrayTag::getId()
{
    return TAG_LONG_ARRAY;
}

bool LongArrayTag::equals(Tag* other)
{
    if (!Tag::equals(other))
        return false;
    LongArrayTag* otherArray = static_cast<LongArrayTag*>(other);
    return mData.length == otherArray->mData.length
           && (mData.length == 0 || memcmp(mData.data, otherArray->mData.data, mData.length * sizeof(long long)) == 0);
}

LongArrayTag* LongArrayTag::copy()
{
    arrayWithLength<long long> copied(mData.length, true);
    for (unsigned int index = 0; index < mData.length; ++index)
        copied.data[index] = mData.data[index];
    return new LongArrayTag(copied);
}

arrayWithLength<long long> LongArrayTag::getAsLongArray()
{
    return mData;
}

void LongArrayTag::setData(arrayWithLength<long long> data)
{
    delete[] mData.data;
    mData = data;
}
