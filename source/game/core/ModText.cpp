#include "game/core/ModText.hpp"

namespace ModText
{
std::string ToUtf8(const std::wstring& text)
{
    std::string out;
    for (size_t index = 0; index < text.size(); ++index)
    {
        unsigned long code = static_cast<unsigned long>(text[index]);
        if (sizeof(wchar_t) == 2 && code >= 0xD800 && code < 0xDC00 && index + 1 < text.size())
            code = 0x10000 + ((code - 0xD800) << 10) + (static_cast<unsigned long>(text[++index]) - 0xDC00);
        if (code < 0x80)
            out += static_cast<char>(code);
        else if (code < 0x800)
        {
            out += static_cast<char>(0xC0 | (code >> 6));
            out += static_cast<char>(0x80 | (code & 0x3F));
        }
        else if (code < 0x10000)
        {
            out += static_cast<char>(0xE0 | (code >> 12));
            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code & 0x3F));
        }
        else
        {
            out += static_cast<char>(0xF0 | (code >> 18));
            out += static_cast<char>(0x80 | ((code >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code & 0x3F));
        }
    }
    return out;
}

std::wstring FromUtf8(const std::string& text)
{
    std::wstring out;
    for (size_t index = 0; index < text.size();)
    {
        const unsigned char lead = static_cast<unsigned char>(text[index]);
        unsigned long code = lead;
        size_t extra = 0;
        if (lead >= 0xF0)
        {
            code = lead & 0x07;
            extra = 3;
        }
        else if (lead >= 0xE0)
        {
            code = lead & 0x0F;
            extra = 2;
        }
        else if (lead >= 0xC0)
        {
            code = lead & 0x1F;
            extra = 1;
        }
        ++index;
        for (size_t step = 0; step < extra && index < text.size(); ++step, ++index)
            code = (code << 6) | (static_cast<unsigned char>(text[index]) & 0x3F);
        if (sizeof(wchar_t) == 2 && code >= 0x10000)
        {
            code -= 0x10000;
            out += static_cast<wchar_t>(0xD800 + (code >> 10));
            out += static_cast<wchar_t>(0xDC00 + (code & 0x3FF));
        }
        else
            out += static_cast<wchar_t>(code);
    }
    return out;
}

bool IsName(const std::wstring& name, bool allowDots)
{
    if (name.empty() || name.size() > 32)
        return false;
    for (size_t index = 0; index < name.size(); ++index)
    {
        const wchar_t c = name[index];
        if (!((c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') || c == L'_' || (allowDots && c == L'.')))
            return false;
    }
    return true;
}
}
