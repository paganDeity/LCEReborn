#ifndef GAME_DATA_STRINGNBTWRITER_HPP
#define GAME_DATA_STRINGNBTWRITER_HPP

#include <string>

class Tag;

class StringNbtWriter
{
public:
    static std::wstring write(Tag* tag);
};

#endif
