#include "archive/Archive.hpp"

#include "archive/ArchiveFiles.hpp"
#include "archive/FolderArchive.hpp"
#include "archive/ZipArchive.hpp"

namespace archive
{
Archive* Archive::Open(const std::string& path, std::string* error)
{
    if (Files::IsDirectory(path))
    {
        FolderArchive* folder = new FolderArchive();
        if (folder->Open(path, error))
            return folder;
        delete folder;
        return NULL;
    }
    ZipArchive* zip = new ZipArchive();
    if (zip->Open(path, error))
        return zip;
    delete zip;
    return NULL;
}
}
