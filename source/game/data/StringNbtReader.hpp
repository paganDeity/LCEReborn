#ifndef GAME_DATA_STRINGNBTREADER_HPP
#define GAME_DATA_STRINGNBTREADER_HPP

#include <stddef.h>

#include <string>

class Tag;

class StringNbtReader
{
public:
    static Tag* parse(const std::wstring& text, std::wstring* error);
    static Tag* parseCompound(const std::wstring& text, size_t* position, std::wstring* error);

private:
    StringNbtReader(const std::wstring& text);

    Tag* readTag(int depth);
    Tag* readCompound(int depth);
    Tag* readList(int depth);
    Tag* readArray(wchar_t type, int depth);
    Tag* readBare();
    bool readKey(std::wstring& key);
    bool readQuotedText(std::wstring& text);
    void skipSpace();
    bool atEnd() const;
    wchar_t peek() const;
    bool take(wchar_t character);
    Tag* fail(const std::wstring& message);
    bool failBool(const std::wstring& message);

    const std::wstring& mText;
    size_t mPosition;
    std::wstring mError;
};

#endif
