#include "mod/ModValue.hpp"

#include <string.h>

namespace mod
{
Value Value::FromBool(bool value)
{
    Value result;
    result.m_type = eType_Bool;
    result.m_int = value ? 1 : 0;
    return result;
}

Value Value::FromInt(int64_t value)
{
    Value result;
    result.m_type = eType_Int;
    result.m_int = value;
    return result;
}

Value Value::FromFloat(double value)
{
    Value result;
    result.m_type = eType_Float;
    result.m_float = value;
    return result;
}

Value Value::FromString(const std::string& value)
{
    Value result;
    result.m_type = eType_String;
    result.m_text = value;
    return result;
}

Value Value::FromHandle(unsigned int kind, uint32_t id)
{
    Value result;
    result.m_type = eType_Handle;
    result.m_kind = kind;
    result.m_int = id;
    return result;
}

namespace
{
void Put64(std::vector<uint8_t>& out, uint64_t value)
{
    for (int byte = 0; byte < 8; ++byte)
        out.push_back(static_cast<uint8_t>(value >> (8 * byte)));
}

uint64_t Get64(const uint8_t* at)
{
    uint64_t value = 0;
    for (int byte = 7; byte >= 0; --byte)
        value = (value << 8) | at[byte];
    return value;
}

void Put32(std::vector<uint8_t>& out, size_t value)
{
    for (int shift = 0; shift < 32; shift += 8)
        out.push_back(static_cast<uint8_t>((value >> shift) & 0xFF));
}

bool Get32(const uint8_t* data, size_t size, size_t* at, size_t* value)
{
    if (*at + 4 > size)
        return false;
    *value = static_cast<size_t>(data[*at]) | (static_cast<size_t>(data[*at + 1]) << 8) | (static_cast<size_t>(data[*at + 2]) << 16)
             | (static_cast<size_t>(data[*at + 3]) << 24);
    *at += 4;
    return true;
}

// a list keeps its items encoded: a count of four bytes, then each item as one value of the argument list
void EncodeItems(const std::vector<Value>& items, std::vector<uint8_t>& out)
{
    Put32(out, items.size());
    for (size_t index = 0; index < items.size(); ++index)
        Value::EncodeOne(items[index], out);
}

bool DecodeItems(const uint8_t* data, size_t size, std::vector<Value>& items)
{
    items.clear();
    size_t at = 0;
    size_t count = 0;
    if (!Get32(data, size, &at, &count) || count > size)
        return false;
    for (size_t index = 0; index < count; ++index)
    {
        Value item;
        if (!Value::DecodeOne(data, size, &at, &item))
            return false;
        items.push_back(item);
    }
    return at == size;
}
}

Value Value::FromList(const std::vector<Value>& items)
{
    Value result;
    result.m_type = eType_List;
    std::vector<uint8_t> bytes;
    EncodeItems(items, bytes);
    result.m_text.assign(bytes.begin(), bytes.end());
    return result;
}

std::vector<Value> Value::Items() const
{
    std::vector<Value> items;
    if (m_type == eType_List && !m_text.empty())
        DecodeItems(reinterpret_cast<const uint8_t*>(m_text.data()), m_text.size(), items);
    return items;
}

void Value::EncodeOne(const Value& value, std::vector<uint8_t>& out)
{
    out.push_back(static_cast<uint8_t>(value.m_type));
    switch (value.m_type)
    {
    case eType_Bool:
        out.push_back(value.m_int != 0 ? 1 : 0);
        break;
    case eType_Int:
        Put64(out, static_cast<uint64_t>(value.m_int));
        break;
    case eType_Float:
    {
        uint64_t bits;
        memcpy(&bits, &value.m_float, 8);
        Put64(out, bits);
        break;
    }
    case eType_String:
    case eType_List:
        Put32(out, value.m_text.size());
        out.insert(out.end(), value.m_text.begin(), value.m_text.end());
        break;
    case eType_Handle:
        out.push_back(static_cast<uint8_t>(value.m_kind));
        Put64(out, static_cast<uint64_t>(value.m_int));
        break;
    default:
        break;
    }
}

bool Value::DecodeOne(const uint8_t* data, size_t size, size_t* at, Value* value)
{
    if (*at >= size)
        return false;
    const uint8_t type = data[(*at)++];
    switch (type)
    {
    case eType_Nil:
        *value = Value();
        return true;
    case eType_Bool:
        if (*at + 1 > size)
            return false;
        *value = FromBool(data[(*at)++] != 0);
        return true;
    case eType_Int:
        if (*at + 8 > size)
            return false;
        *value = FromInt(static_cast<int64_t>(Get64(data + *at)));
        *at += 8;
        return true;
    case eType_Float:
    {
        if (*at + 8 > size)
            return false;
        const uint64_t bits = Get64(data + *at);
        double number;
        memcpy(&number, &bits, 8);
        *value = FromFloat(number);
        *at += 8;
        return true;
    }
    case eType_String:
    case eType_List:
    {
        size_t length = 0;
        if (!Get32(data, size, at, &length) || *at + length > size)
            return false;
        const std::string text(reinterpret_cast<const char*>(data) + *at, length);
        std::vector<Value> items;
        if (type == eType_List && !DecodeItems(data + *at, length, items))
            return false;
        *at += length;
        if (type == eType_String)
        {
            *value = FromString(text);
        }
        else
        {
            *value = Value();
            value->m_type = eType_List;
            value->m_text = text;
        }
        return true;
    }
    case eType_Handle:
        if (*at + 9 > size)
            return false;
        *value = FromHandle(data[*at], static_cast<uint32_t>(Get64(data + *at + 1)));
        *at += 9;
        return true;
    default:
        return false;
    }
}

void Value::Encode(const std::vector<Value>& values, std::vector<uint8_t>& out)
{
    out.push_back(static_cast<uint8_t>(values.size()));
    for (size_t index = 0; index < values.size(); ++index)
        EncodeOne(values[index], out);
}

bool Value::Decode(const uint8_t* data, size_t size, std::vector<Value>& values)
{
    values.clear();
    if (size < 1)
        return false;
    const size_t count = data[0];
    size_t at = 1;
    for (size_t index = 0; index < count; ++index)
    {
        Value value;
        if (!DecodeOne(data, size, &at, &value))
            return false;
        values.push_back(value);
    }
    return at == size;
}
}
