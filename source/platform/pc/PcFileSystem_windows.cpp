#include "platform/pc/PcFileSystem.hpp"

#include <share.h>
#include <stdio.h>

#include "platform/pc/PcUtf8.hpp"

std::filesystem::path PcGamePath(const std::string& text)
{
    std::wstring wide;
    if (!PcUtf8ToWideStrict(text, &wide))
        wide.assign(text.begin(), text.end());
    return std::filesystem::path(wide);
}

std::string PcGameName(const std::filesystem::path& path)
{
    return PcWideToUtf8(path.native());
}

std::filesystem::path PcContentPath(const std::filesystem::path& path)
{
    return path;
}

std::FILE* PcOpenFile(const std::filesystem::path& path, const char* mode)
{
    const std::wstring wideMode(mode, mode + std::char_traits<char>::length(mode));
    return _wfsopen(path.c_str(), wideMode.c_str(), _SH_DENYNO);
}

bool PcSeekFile(std::FILE* stream, long long offset, int origin)
{
    return _fseeki64(stream, offset, origin) == 0;
}

long long PcTellFile(std::FILE* stream)
{
    return _ftelli64(stream);
}
