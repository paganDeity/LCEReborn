#include "game/data/StringNbtWriter.hpp"

#include <stdio.h>

#include <vector>

#include "game/data/ByteArrayTag.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/IntArrayTag.hpp"
#include "game/data/ListTag.hpp"
#include "game/data/LongArrayTag.hpp"
#include "game/data/NumericTag.hpp"
#include "game/data/StringTag.hpp"
#include "game/data/Tag.hpp"
#include "game/world/biome/arrayWithLength.hpp"

namespace
{
bool NeedsQuote(const std::wstring& text)
{
    if (text.empty())
        return true;
    for (size_t index = 0; index < text.size(); ++index)
    {
        const wchar_t character = text[index];
        const bool plain = (character >= L'a' && character <= L'z') || (character >= L'A' && character <= L'Z')
                           || (character >= L'0' && character <= L'9') || character == L'_' || character == L'-'
                           || character == L'.' || character == L'+';
        if (!plain)
            return true;
    }
    return false;
}

void AppendQuoted(const std::wstring& text, std::wstring& out)
{
    out += L'"';
    for (size_t index = 0; index < text.size(); ++index)
    {
        const wchar_t character = text[index];
        switch (character)
        {
        case L'"':
            out += L"\\\"";
            break;
        case L'\\':
            out += L"\\\\";
            break;
        case L'\n':
            out += L"\\n";
            break;
        case L'\r':
            out += L"\\r";
            break;
        case L'\t':
            out += L"\\t";
            break;
        default:
            out += character;
            break;
        }
    }
    out += L'"';
}

void AppendKey(const std::wstring& key, std::wstring& out)
{
    if (NeedsQuote(key))
        AppendQuoted(key, out);
    else
        out += key;
}

void AppendFloat(float value, std::wstring& out)
{
    wchar_t text[64];
    swprintf(text, sizeof(text) / sizeof(text[0]), L"%.9g", (double)value);
    out += text;
    out += L'f';
}

void AppendDouble(double value, std::wstring& out)
{
    wchar_t text[64];
    swprintf(text, sizeof(text) / sizeof(text[0]), L"%.17g", value);
    out += text;
    out += L'd';
}

void AppendLongArray(LongArrayTag* array, std::wstring& out)
{
    out += L"[L;";
    const arrayWithLength<long long> data = array->getAsLongArray();
    for (unsigned int index = 0; index < data.length; ++index)
    {
        if (index > 0)
            out += L',';
        wchar_t text[32];
        swprintf(text, sizeof(text) / sizeof(text[0]), L"%lldL", data.data[index]);
        out += text;
    }
    out += L']';
}

void AppendTag(Tag* tag, std::wstring& out);

void AppendCompound(CompoundTag* compound, std::wstring& out)
{
    out += L'{';
    std::vector<std::wstring>* keys = compound->getAllKeys();
    for (size_t index = 0; index < keys->size(); ++index)
    {
        if (index > 0)
            out += L',';
        const std::wstring& key = (*keys)[index];
        AppendKey(key, out);
        out += L':';
        AppendTag(compound->get(key), out);
    }
    delete keys;
    out += L'}';
}

void AppendList(ListTag* list, std::wstring& out)
{
    out += L'[';
    for (int index = 0; index < list->size(); ++index)
    {
        if (index > 0)
            out += L',';
        AppendTag(list->get(index), out);
    }
    out += L']';
}

void AppendByteArray(ByteArrayTag* array, std::wstring& out)
{
    out += L"[B;";
    const arrayWithLength<unsigned char> data = array->getAsByteArray();
    for (unsigned int index = 0; index < data.length; ++index)
    {
        if (index > 0)
            out += L',';
        wchar_t text[16];
        swprintf(text, sizeof(text) / sizeof(text[0]), L"%db", (int)(signed char)data.data[index]);
        out += text;
    }
    out += L']';
}

void AppendIntArray(IntArrayTag* array, std::wstring& out)
{
    out += L"[I;";
    const arrayWithLength<int> data = array->getAsIntArray();
    for (unsigned int index = 0; index < data.length; ++index)
    {
        if (index > 0)
            out += L',';
        wchar_t text[16];
        swprintf(text, sizeof(text) / sizeof(text[0]), L"%d", data.data[index]);
        out += text;
    }
    out += L']';
}

void AppendTag(Tag* tag, std::wstring& out)
{
    if (tag == NULL)
    {
        out += L"null";
        return;
    }
    NumericTag* numeric = NULL;
    switch (tag->getId())
    {
    case Tag::TAG_BYTE:
        numeric = static_cast<NumericTag*>(tag);
        {
            wchar_t text[16];
            swprintf(text, sizeof(text) / sizeof(text[0]), L"%db", (int)(signed char)numeric->getAsByte());
            out += text;
        }
        break;
    case Tag::TAG_SHORT:
        numeric = static_cast<NumericTag*>(tag);
        {
            wchar_t text[16];
            swprintf(text, sizeof(text) / sizeof(text[0]), L"%d", (int)numeric->getAsShort());
            out += text;
        }
        out += L's';
        break;
    case Tag::TAG_INT:
        numeric = static_cast<NumericTag*>(tag);
        {
            wchar_t text[16];
            swprintf(text, sizeof(text) / sizeof(text[0]), L"%d", numeric->getAsInt());
            out += text;
        }
        break;
    case Tag::TAG_LONG:
        numeric = static_cast<NumericTag*>(tag);
        {
            wchar_t text[32];
            swprintf(text, sizeof(text) / sizeof(text[0]), L"%lld", numeric->getAsLong());
            out += text;
        }
        out += L'L';
        break;
    case Tag::TAG_FLOAT:
        numeric = static_cast<NumericTag*>(tag);
        AppendFloat(numeric->getAsFloat(), out);
        break;
    case Tag::TAG_DOUBLE:
        numeric = static_cast<NumericTag*>(tag);
        AppendDouble(numeric->getAsDouble(), out);
        break;
    case Tag::TAG_BYTE_ARRAY:
        AppendByteArray(static_cast<ByteArrayTag*>(tag), out);
        break;
    case Tag::TAG_STRING:
        AppendQuoted(static_cast<StringTag*>(tag)->getAsString(), out);
        break;
    case Tag::TAG_LIST:
        AppendList(static_cast<ListTag*>(tag), out);
        break;
    case Tag::TAG_COMPOUND:
        AppendCompound(static_cast<CompoundTag*>(tag), out);
        break;
    case Tag::TAG_INT_ARRAY:
        AppendIntArray(static_cast<IntArrayTag*>(tag), out);
        break;
    case Tag::TAG_LONG_ARRAY:
        AppendLongArray(static_cast<LongArrayTag*>(tag), out);
        break;
    default:
        out += L"null";
        break;
    }
}
}

std::wstring StringNbtWriter::write(Tag* tag)
{
    std::wstring out;
    AppendTag(tag, out);
    return out;
}
