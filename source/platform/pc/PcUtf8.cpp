#include "platform/pc/PcUtf8.hpp"

void PcAppendUtf8(std::string& text, unsigned int codepoint)
{
    if (codepoint < 0x80)
    {
        text += static_cast<char>(codepoint);
    }
    else if (codepoint < 0x800)
    {
        text += static_cast<char>(0xc0 | (codepoint >> 6));
        text += static_cast<char>(0x80 | (codepoint & 0x3f));
    }
    else if (codepoint < 0x10000)
    {
        text += static_cast<char>(0xe0 | (codepoint >> 12));
        text += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f));
        text += static_cast<char>(0x80 | (codepoint & 0x3f));
    }
    else
    {
        text += static_cast<char>(0xf0 | (codepoint >> 18));
        text += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f));
        text += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f));
        text += static_cast<char>(0x80 | (codepoint & 0x3f));
    }
}

std::string PcWideToUtf8(const std::wstring& text)
{
    std::string result;
    for (size_t index = 0; index < text.size(); ++index)
    {
        unsigned int codepoint = static_cast<unsigned int>(text[index]);
        if (sizeof(wchar_t) == 2 && codepoint >= 0xd800 && codepoint < 0xdc00 && index + 1 < text.size())
        {
            unsigned int low = static_cast<unsigned int>(text[index + 1]);
            if (low >= 0xdc00 && low < 0xe000)
            {
                codepoint = 0x10000 + ((codepoint - 0xd800) << 10) + (low - 0xdc00);
                ++index;
            }
        }
        if ((codepoint >= 0xd800 && codepoint < 0xe000) || codepoint > 0x10ffff)
        {
            codepoint = 0xfffd;
        }
        PcAppendUtf8(result, codepoint);
    }
    return result;
}

void PcAppendWide(std::wstring& wide, unsigned int codepoint)
{
    if (sizeof(wchar_t) == 2 && codepoint >= 0x10000)
    {
        codepoint -= 0x10000;
        wide.push_back(static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
        wide.push_back(static_cast<wchar_t>(0xdc00 + (codepoint & 0x3ff)));
        return;
    }
    wide.push_back(static_cast<wchar_t>(codepoint));
}

bool PcUtf8DecodeOne(const std::string& text, std::size_t* index, unsigned int* codepoint)
{
    static const unsigned int kMinimum[] = {0, 0, 0x80, 0x800, 0x10000};
    const unsigned char lead = static_cast<unsigned char>(text[*index]);
    unsigned int value = lead;
    std::size_t length = 1;
    if ((lead & 0xe0) == 0xc0)
    {
        value = lead & 0x1f;
        length = 2;
    }
    else if ((lead & 0xf0) == 0xe0)
    {
        value = lead & 0x0f;
        length = 3;
    }
    else if ((lead & 0xf8) == 0xf0)
    {
        value = lead & 0x07;
        length = 4;
    }
    ++*index;
    if (lead >= 0x80 && length == 1)
        return false;
    for (std::size_t used = 1; used < length; ++used)
    {
        if (*index >= text.size() || (static_cast<unsigned char>(text[*index]) & 0xc0) != 0x80)
            return false;
        value = (value << 6) | (static_cast<unsigned char>(text[(*index)++]) & 0x3f);
    }
    if (value < kMinimum[length] || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff))
        return false;
    *codepoint = value;
    return true;
}

static bool DecodeUtf8(const std::string& text, std::wstring* wide, bool strict)
{
    wide->clear();
    std::size_t index = 0;
    while (index < text.size())
    {
        unsigned int codepoint = 0;
        if (!PcUtf8DecodeOne(text, &index, &codepoint))
        {
            if (strict)
                return false;
            codepoint = 0xfffd;
        }
        PcAppendWide(*wide, codepoint);
    }
    return true;
}

bool PcUtf8ToWideStrict(const std::string& text, std::wstring* wide)
{
    return DecodeUtf8(text, wide, true);
}

std::wstring PcUtf8ToWide(const std::string& text)
{
    std::wstring wide;
    DecodeUtf8(text, &wide, false);
    return wide;
}
