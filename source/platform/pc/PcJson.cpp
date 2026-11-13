#include "platform/pc/PcJson.hpp"

#include "platform/pc/PcUtf8.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>

namespace
{
class JsonReader
{
public:
    explicit JsonReader(const std::string& source) : mSource(source), mPosition(0) {}

    bool Read(JsonValue& value)
    {
        if (!ReadValue(value, 0))
            return false;
        SkipSpace();
        return mPosition == mSource.size();
    }

private:
    static const int kMaxDepth = 32;

    void SkipSpace()
    {
        while (mPosition < mSource.size() && std::isspace(static_cast<unsigned char>(mSource[mPosition])))
            ++mPosition;
    }

    bool Match(const char* word)
    {
        const size_t length = std::char_traits<char>::length(word);
        if (mSource.compare(mPosition, length, word) != 0)
            return false;
        mPosition += length;
        return true;
    }

    bool ReadValue(JsonValue& value, int depth)
    {
        if (depth > kMaxDepth)
            return false;
        SkipSpace();
        if (mPosition >= mSource.size())
            return false;
        const char lead = mSource[mPosition];
        if (lead == '{')
            return ReadObject(value, depth);
        if (lead == '[')
            return ReadArray(value, depth);
        if (lead == '"')
        {
            value.type = JsonValue::String;
            return ReadString(value.text);
        }
        if (Match("true") || Match("false"))
        {
            value.type = JsonValue::Boolean;
            return true;
        }
        if (Match("null"))
        {
            value.type = JsonValue::Null;
            return true;
        }
        const char* start = mSource.c_str() + mPosition;
        char* end = NULL;
        std::strtod(start, &end);
        if (end == start)
            return false;
        mPosition += static_cast<size_t>(end - start);
        value.type = JsonValue::Number;
        value.number = std::strtod(start, NULL);
        return true;
    }

    bool ReadObject(JsonValue& value, int depth)
    {
        value.type = JsonValue::Object;
        ++mPosition;
        SkipSpace();
        if (mPosition < mSource.size() && mSource[mPosition] == '}')
        {
            ++mPosition;
            return true;
        }
        for (;;)
        {
            SkipSpace();
            std::wstring key;
            if (mPosition >= mSource.size() || mSource[mPosition] != '"' || !ReadString(key))
                return false;
            SkipSpace();
            if (mPosition >= mSource.size() || mSource[mPosition] != ':')
                return false;
            ++mPosition;
            if (!ReadValue(value.members[key], depth + 1))
                return false;
            SkipSpace();
            if (mPosition >= mSource.size())
                return false;
            if (mSource[mPosition] == '}')
            {
                ++mPosition;
                return true;
            }
            if (mSource[mPosition] != ',')
                return false;
            ++mPosition;
        }
    }

    bool ReadArray(JsonValue& value, int depth)
    {
        value.type = JsonValue::Array;
        ++mPosition;
        SkipSpace();
        if (mPosition < mSource.size() && mSource[mPosition] == ']')
        {
            ++mPosition;
            return true;
        }
        for (;;)
        {
            value.items.push_back(JsonValue());
            if (!ReadValue(value.items.back(), depth + 1))
                return false;
            SkipSpace();
            if (mPosition >= mSource.size())
                return false;
            if (mSource[mPosition] == ']')
            {
                ++mPosition;
                return true;
            }
            if (mSource[mPosition] != ',')
                return false;
            ++mPosition;
        }
    }

    bool ReadHex(unsigned int& code)
    {
        if (mPosition + 4 > mSource.size())
            return false;
        code = 0;
        for (int digit = 0; digit < 4; ++digit)
        {
            const char character = mSource[mPosition++];
            code <<= 4;
            if (character >= '0' && character <= '9')
                code |= static_cast<unsigned int>(character - '0');
            else if (character >= 'a' && character <= 'f')
                code |= static_cast<unsigned int>(character - 'a' + 10);
            else if (character >= 'A' && character <= 'F')
                code |= static_cast<unsigned int>(character - 'A' + 10);
            else
                return false;
        }
        return true;
    }

    bool ReadString(std::wstring& text)
    {
        ++mPosition;
        while (mPosition < mSource.size())
        {
            unsigned char character = static_cast<unsigned char>(mSource[mPosition++]);
            if (character == '"')
                return true;
            if (character < 0x20)
                return false;
            if (character == '\\')
            {
                if (mPosition >= mSource.size())
                    return false;
                const char escape = mSource[mPosition++];
                unsigned int code = 0;
                switch (escape)
                {
                case '"':
                case '\\':
                case '/':
                    text.push_back(static_cast<wchar_t>(escape));
                    break;
                case 'b':
                    text.push_back(L'\b');
                    break;
                case 'f':
                    text.push_back(L'\f');
                    break;
                case 'n':
                    text.push_back(L'\n');
                    break;
                case 'r':
                    text.push_back(L'\r');
                    break;
                case 't':
                    text.push_back(L'\t');
                    break;
                case 'u':
                    if (!ReadHex(code))
                        return false;
                    if (code >= 0xd800 && code < 0xdc00 && mSource.compare(mPosition, 2, "\\u") == 0)
                    {
                        mPosition += 2;
                        unsigned int low = 0;
                        if (!ReadHex(low) || low < 0xdc00 || low >= 0xe000)
                            return false;
                        code = 0x10000 + ((code - 0xd800) << 10) + (low - 0xdc00);
                    }
                    PcAppendWide(text, code);
                    break;
                default:
                    return false;
                }
                continue;
            }
            --mPosition;
            unsigned int code = 0;
            if (!PcUtf8DecodeOne(mSource, &mPosition, &code))
                return false;
            PcAppendWide(text, code);
        }
        return false;
    }

    const std::string& mSource;
    size_t mPosition;
};
}

bool PcJsonParse(const std::string& source, JsonValue& value)
{
    JsonReader reader(source);
    return reader.Read(value);
}

std::string PcJsonQuote(const std::wstring& text)
{
    std::string out = "\"";
    for (size_t index = 0; index < text.size(); ++index)
    {
        unsigned int code = static_cast<unsigned int>(text[index]);
        if (sizeof(wchar_t) == 2 && code >= 0xd800 && code < 0xdc00 && index + 1 < text.size())
        {
            const unsigned int low = static_cast<unsigned int>(text[index + 1]);
            if (low >= 0xdc00 && low < 0xe000)
            {
                code = 0x10000 + ((code - 0xd800) << 10) + (low - 0xdc00);
                ++index;
            }
        }
        if (code == '"' || code == '\\')
        {
            out += '\\';
            out += static_cast<char>(code);
        }
        else if (code < 0x20)
        {
            char escaped[8];
            snprintf(escaped, sizeof(escaped), "\\u%04x", code);
            out += escaped;
        }
        else
        {
            PcAppendUtf8(out, code);
        }
    }
    return out + "\"";
}
