#include "game/data/StringNbtReader.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include <vector>

#include "game/data/ByteArrayTag.hpp"
#include "game/data/ByteTag.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/DoubleTag.hpp"
#include "game/data/FloatTag.hpp"
#include "game/data/IntArrayTag.hpp"
#include "game/data/IntTag.hpp"
#include "game/data/ListTag.hpp"
#include "game/data/LongArrayTag.hpp"
#include "game/data/LongTag.hpp"
#include "game/data/NumericTag.hpp"
#include "game/data/ShortTag.hpp"
#include "game/data/StringTag.hpp"
#include "game/data/Tag.hpp"
#include "game/world/biome/arrayWithLength.hpp"

namespace
{
const int kMaxDepth = 64;

bool IsSpace(wchar_t character)
{
    return character == L' ' || character == L'\t' || character == L'\n' || character == L'\r';
}

bool IsValueTerminator(wchar_t character)
{
    return character == L',' || character == L']' || character == L'}' || IsSpace(character);
}

bool IsKeyTerminator(wchar_t character)
{
    return character == L':' || character == L',' || character == L'}' || IsSpace(character);
}

bool ParseInteger(const std::wstring& text, long long* value)
{
    size_t index = 0;
    bool negative = false;
    if (index < text.size() && text[index] == L'-')
    {
        negative = true;
        ++index;
    }
    if (index == text.size())
        return false;
    unsigned long long magnitude = 0;
    for (; index < text.size(); ++index)
    {
        const wchar_t character = text[index];
        if (character < L'0' || character > L'9')
            return false;
        magnitude = magnitude * 10ULL + (unsigned long long)(character - L'0');
        if (magnitude > 9223372036854775808ULL)
            return false;
    }
    if (negative)
    {
        if (magnitude > 9223372036854775808ULL)
            return false;
        *value = magnitude == 9223372036854775808ULL ? (long long)(-9223372036854775807LL - 1)
                                                     : -(long long)magnitude;
    }
    else
    {
        if (magnitude > 9223372036854775807ULL)
            return false;
        *value = (long long)magnitude;
    }
    return true;
}

bool ParseDouble(const std::wstring& text, double* value)
{
    if (text.empty())
        return false;
    bool hasDigit = false;
    for (size_t index = 0; index < text.size(); ++index)
    {
        const wchar_t character = text[index];
        if (character >= L'0' && character <= L'9')
        {
            hasDigit = true;
            continue;
        }
        if (character == L'-' || character == L'+' || character == L'.' || character == L'e' || character == L'E')
            continue;
        return false;
    }
    if (!hasDigit)
        return false;
    wchar_t* end = NULL;
    const double number = wcstod(text.c_str(), &end);
    if (end == text.c_str() || *end != L'\0')
        return false;
    *value = number;
    return true;
}

void AppendArray(wchar_t type, const std::vector<long long>& values, Tag** tag)
{
    if (type == L'B')
    {
        arrayWithLength<unsigned char> data((unsigned int)values.size(), false);
        for (size_t index = 0; index < values.size(); ++index)
            data.data[index] = (unsigned char)values[index];
        *tag = new ByteArrayTag(data, true);
    }
    else if (type == L'I')
    {
        arrayWithLength<int> data((unsigned int)values.size(), false);
        for (size_t index = 0; index < values.size(); ++index)
            data.data[index] = (int)values[index];
        *tag = new IntArrayTag(data);
    }
    else
    {
        arrayWithLength<long long> data((unsigned int)values.size(), false);
        for (size_t index = 0; index < values.size(); ++index)
            data.data[index] = values[index];
        *tag = new LongArrayTag(data);
    }
}
}

StringNbtReader::StringNbtReader(const std::wstring& text) : mText(text)
{
    mPosition = 0;
}

void StringNbtReader::skipSpace()
{
    while (mPosition < mText.size() && IsSpace(mText[mPosition]))
        ++mPosition;
}

bool StringNbtReader::atEnd() const
{
    return mPosition >= mText.size();
}

wchar_t StringNbtReader::peek() const
{
    return mText[mPosition];
}

bool StringNbtReader::take(wchar_t character)
{
    if (atEnd() || mText[mPosition] != character)
        return false;
    ++mPosition;
    return true;
}

Tag* StringNbtReader::fail(const std::wstring& message)
{
    if (mError.empty())
    {
        wchar_t position[32];
        swprintf(position, sizeof(position) / sizeof(position[0]), L" at character %d", (int)mPosition);
        mError = message + position;
    }
    return NULL;
}

bool StringNbtReader::failBool(const std::wstring& message)
{
    fail(message);
    return false;
}

Tag* StringNbtReader::readTag(int depth)
{
    if (depth > kMaxDepth)
        return fail(L"the tag is nested too deeply");
    skipSpace();
    if (atEnd())
        return fail(L"a tag value is missing");
    const wchar_t character = peek();
    if (character == L'{')
        return readCompound(depth);
    if (character == L'[')
        return readList(depth);
    if (character == L'"' || character == L'\'')
    {
        std::wstring text;
        if (!readQuotedText(text))
            return NULL;
        return new StringTag(text);
    }
    return readBare();
}

Tag* StringNbtReader::readCompound(int depth)
{
    ++mPosition;
    CompoundTag* compound = new CompoundTag();
    skipSpace();
    if (take(L'}'))
        return compound;
    for (;;)
    {
        std::wstring key;
        if (!readKey(key))
        {
            delete compound;
            return NULL;
        }
        skipSpace();
        if (!take(L':'))
        {
            delete compound;
            return fail(L"a tag name must be followed by a colon");
        }
        Tag* value = readTag(depth + 1);
        if (value == NULL)
        {
            delete compound;
            return NULL;
        }
        compound->put(key, value);
        skipSpace();
        if (take(L'}'))
            return compound;
        if (!take(L','))
        {
            delete compound;
            return fail(L"a compound needs a comma between its tags");
        }
        skipSpace();
        if (take(L'}'))
            return compound;
    }
}

Tag* StringNbtReader::readList(int depth)
{
    ++mPosition;
    skipSpace();
    if (!atEnd() && (peek() == L'B' || peek() == L'I' || peek() == L'L') && mPosition + 1 < mText.size()
        && mText[mPosition + 1] == L';')
    {
        const wchar_t type = peek();
        mPosition += 2;
        return readArray(type, depth);
    }
    ListTag* list = new ListTag();
    skipSpace();
    if (take(L']'))
        return list;
    for (;;)
    {
        Tag* value = readTag(depth + 1);
        if (value == NULL)
        {
            delete list;
            return NULL;
        }
        if (list->size() > 0 && list->getType() != value->getId())
        {
            delete value;
            delete list;
            return fail(L"every element of a list must have the same type");
        }
        list->add(value);
        skipSpace();
        if (take(L']'))
            return list;
        if (!take(L','))
        {
            delete list;
            return fail(L"a list needs a comma between its elements");
        }
        skipSpace();
        if (take(L']'))
            return list;
    }
}

Tag* StringNbtReader::readArray(wchar_t type, int depth)
{
    std::vector<long long> values;
    skipSpace();
    if (take(L']'))
    {
        Tag* tag = NULL;
        AppendArray(type, values, &tag);
        return tag;
    }
    for (;;)
    {
        Tag* value = readTag(depth + 1);
        if (value == NULL)
            return NULL;
        const unsigned char id = value->getId();
        if (id < Tag::TAG_BYTE || id > Tag::TAG_DOUBLE)
        {
            delete value;
            return fail(L"an array holds numbers only");
        }
        values.push_back(static_cast<NumericTag*>(value)->getAsLong());
        delete value;
        skipSpace();
        if (take(L']'))
        {
            Tag* tag = NULL;
            AppendArray(type, values, &tag);
            return tag;
        }
        if (!take(L','))
            return fail(L"an array needs a comma between its values");
        skipSpace();
        if (take(L']'))
        {
            Tag* tag = NULL;
            AppendArray(type, values, &tag);
            return tag;
        }
    }
}

Tag* StringNbtReader::readBare()
{
    const size_t start = mPosition;
    while (!atEnd() && !IsValueTerminator(peek()))
        ++mPosition;
    const std::wstring token = mText.substr(start, mPosition - start);
    if (token.empty())
        return fail(L"a tag value is missing");
    const wchar_t suffix = token[token.size() - 1];
    const std::wstring body = token.substr(0, token.size() - 1);
    long long integer = 0;
    double number = 0.0;
    if (suffix == L'b' || suffix == L'B')
    {
        if (!ParseInteger(body, &integer) || integer < -128 || integer > 127)
            return fail(L"a byte must be a whole number from -128 to 127");
        return new ByteTag((unsigned char)integer);
    }
    if (suffix == L's' || suffix == L'S')
    {
        if (!ParseInteger(body, &integer) || integer < -32768 || integer > 32767)
            return fail(L"a short must be a whole number from -32768 to 32767");
        return new ShortTag((int)integer);
    }
    if (suffix == L'l' || suffix == L'L')
    {
        if (!ParseInteger(body, &integer))
            return fail(L"a long must be a whole number");
        return new LongTag(integer);
    }
    if (suffix == L'f' || suffix == L'F')
    {
        if (!ParseDouble(body, &number))
            return fail(L"a float must be a number");
        return new FloatTag((float)number);
    }
    if (suffix == L'd' || suffix == L'D')
    {
        if (!ParseDouble(body, &number))
            return fail(L"a double must be a number");
        return new DoubleTag(number);
    }
    if (ParseInteger(token, &integer))
    {
        if (integer < -2147483647LL - 1 || integer > 2147483647LL)
            return fail(L"an int must be a whole number from -2147483648 to 2147483647");
        return new IntTag((int)integer);
    }
    if (ParseDouble(token, &number))
        return new DoubleTag(number);
    return new StringTag(token);
}

bool StringNbtReader::readKey(std::wstring& key)
{
    skipSpace();
    if (atEnd())
        return failBool(L"a tag name is missing");
    if (peek() == L'"' || peek() == L'\'')
        return readQuotedText(key);
    const size_t start = mPosition;
    while (!atEnd() && !IsKeyTerminator(peek()))
        ++mPosition;
    key = mText.substr(start, mPosition - start);
    if (key.empty())
        return failBool(L"a tag name is missing");
    return true;
}

bool StringNbtReader::readQuotedText(std::wstring& text)
{
    const wchar_t quote = mText[mPosition++];
    while (!atEnd())
    {
        const wchar_t character = mText[mPosition++];
        if (character == quote)
            return true;
        if (character != L'\\')
        {
            text.push_back(character);
            continue;
        }
        if (atEnd())
            break;
        const wchar_t escape = mText[mPosition++];
        switch (escape)
        {
        case L'"':
        case L'\\':
        case L'/':
            text.push_back(escape);
            break;
        case L'n':
            text.push_back(L'\n');
            break;
        case L'r':
            text.push_back(L'\r');
            break;
        case L't':
            text.push_back(L'\t');
            break;
        case L'b':
            text.push_back(L'\b');
            break;
        case L'f':
            text.push_back(L'\f');
            break;
        default:
            text.push_back(escape);
            break;
        }
    }
    return failBool(L"a quoted string is not closed");
}

Tag* StringNbtReader::parse(const std::wstring& text, std::wstring* error)
{
    StringNbtReader reader(text);
    Tag* tag = reader.readTag(0);
    reader.skipSpace();
    if (tag != NULL && !reader.atEnd())
    {
        reader.fail(L"unexpected text after the tag");
        delete tag;
        tag = NULL;
    }
    if (error != NULL)
        *error = reader.mError;
    return tag;
}

Tag* StringNbtReader::parseCompound(const std::wstring& text, size_t* position, std::wstring* error)
{
    StringNbtReader reader(text);
    reader.mPosition = *position;
    Tag* tag = NULL;
    if (reader.atEnd() || reader.peek() != L'{')
        reader.fail(L"a compound is needed");
    else
        tag = reader.readCompound(0);
    if (tag != NULL)
        *position = reader.mPosition;
    if (error != NULL)
        *error = reader.mError;
    return tag;
}
