#ifndef PLATFORM_PC_PCFILESYSTEM_HPP
#define PLATFORM_PC_PCFILESYSTEM_HPP

#include <cstdio>
#include <filesystem>
#include <string>

std::filesystem::path PcGamePath(const std::string& text);
std::string PcGameName(const std::filesystem::path& path);
std::filesystem::path PcContentPath(const std::filesystem::path& path);
std::FILE* PcOpenFile(const std::filesystem::path& path, const char* mode);
bool PcSeekFile(std::FILE* stream, long long offset, int origin);
long long PcTellFile(std::FILE* stream);

#endif
