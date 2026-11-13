#ifndef PLATFORM_PC_PCUTF8_HPP
#define PLATFORM_PC_PCUTF8_HPP

#include <cstddef>
#include <string>

void PcAppendUtf8(std::string& text, unsigned int codepoint);
std::string PcWideToUtf8(const std::wstring& text);
bool PcUtf8ToWideStrict(const std::string& text, std::wstring* wide);
std::wstring PcUtf8ToWide(const std::string& text);
bool PcUtf8DecodeOne(const std::string& text, std::size_t* index, unsigned int* codepoint);
void PcAppendWide(std::wstring& wide, unsigned int codepoint);

#endif
