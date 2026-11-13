#include "platform/pc/FileApi.hpp"

#include "game/core/ModHost.hpp"
#include "platform/pc/PcFileSystem.hpp"
#include "platform/pc/PcPaths.hpp"
#include "platform/pc/PcVirtualMemory.hpp"

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <unordered_map>
#include <utility>

extern C4JStorage* gStorage;

namespace fs = std::filesystem;

enum class FileOpenMode
{
    Read,
    Append,
    Truncate
};

struct FileHandleEntry
{
    std::mutex mutex;
    std::FILE* stream;
    long long position;
    bool append;

    FileHandleEntry(std::FILE* fileStream, bool appendMode) : stream(fileStream), position(0), append(appendMode)
    {
    }

    ~FileHandleEntry()
    {
        std::fclose(stream);
    }

    FileHandleEntry(const FileHandleEntry&) = delete;
    FileHandleEntry& operator=(const FileHandleEntry&) = delete;
};

struct DirectoryHandleEntry
{
    std::mutex mutex;
    fs::directory_iterator iterator;

    explicit DirectoryHandleEntry(fs::directory_iterator directoryIterator) : iterator(std::move(directoryIterator))
    {
    }
};

struct FileHandleTable
{
    std::mutex mutex;
    std::unordered_map<int, std::shared_ptr<FileHandleEntry>> files;
    std::unordered_map<int, std::shared_ptr<DirectoryHandleEntry>> directories;
    int nextHandle = 1;
};

static FileHandleTable& Handles()
{
    static FileHandleTable table;
    return table;
}


static int AllocateHandle(FileHandleTable& table)
{
    for (;;)
    {
        int handle = table.nextHandle;
        table.nextHandle = handle == INT_MAX ? 1 : handle + 1;
        if (table.files.find(handle) == table.files.end()
            && table.directories.find(handle) == table.directories.end())
            return handle;
    }
}

static int AddFile(const std::shared_ptr<FileHandleEntry>& file)
{
    FileHandleTable& table = Handles();
    std::lock_guard<std::mutex> lock(table.mutex);
    int handle = AllocateHandle(table);
    table.files[handle] = file;
    return handle;
}

static int AddDirectory(const std::shared_ptr<DirectoryHandleEntry>& directory)
{
    FileHandleTable& table = Handles();
    std::lock_guard<std::mutex> lock(table.mutex);
    int handle = AllocateHandle(table);
    table.directories[handle] = directory;
    return handle;
}

static std::shared_ptr<FileHandleEntry> FindFile(int handle)
{
    FileHandleTable& table = Handles();
    std::lock_guard<std::mutex> lock(table.mutex);
    std::unordered_map<int, std::shared_ptr<FileHandleEntry>>::iterator it = table.files.find(handle);
    return it == table.files.end() ? std::shared_ptr<FileHandleEntry>() : it->second;
}

static std::shared_ptr<DirectoryHandleEntry> FindDirectory(int handle)
{
    FileHandleTable& table = Handles();
    std::lock_guard<std::mutex> lock(table.mutex);
    std::unordered_map<int, std::shared_ptr<DirectoryHandleEntry>>::iterator it = table.directories.find(handle);
    return it == table.directories.end() ? std::shared_ptr<DirectoryHandleEntry>() : it->second;
}

static bool MatchMount(const std::string& path, const char* mount, std::string& rest)
{
    const std::size_t length = std::strlen(mount);
    if (path.compare(0, length, mount) != 0)
        return false;
    if (path.size() != length && path[length] != '/')
        return false;
    rest = path.substr(length);
    return true;
}

static fs::path UnderRoot(const std::string& root, const std::string& rest)
{
    std::string::size_type start = rest.find_first_not_of('/');
    if (start == std::string::npos)
        return fs::path(root);
    return fs::path(root) / PcGamePath(rest.substr(start));
}

static fs::path ContentPath(const std::string& relative)
{
    return PcContentPath(UnderRoot(PcContentRoot(), relative));
}

std::string PcGameContentFile(const std::string& relative)
{
    return ContentPath(relative).u8string();
}

static fs::path ReadablePath(const std::string& relative)
{
    std::string overlaid;
    const std::string::size_type start = relative.find_first_not_of('/');
    if (start != std::string::npos && ModHost::Get().Overlay(relative.substr(start), &overlaid))
        return fs::path(overlaid);
    return ContentPath(relative);
}

static fs::path ResolvePath(const char* fileName, bool reading = false)
{
    std::string path;
    if (gStorage != nullptr)
        path = gStorage->GetMountedPath(std::string(fileName));
    if (path.empty())
        path = fileName;
    std::replace(path.begin(), path.end(), '\\', '/');

    std::string rest;
    if (MatchMount(path, "/vol/content", rest))
        return reading ? ReadablePath(rest) : ContentPath(rest);
    if (MatchMount(path, "/vol/save", rest))
        return UnderRoot(PcSaveRoot(), rest);
    if (MatchMount(path, "/vol/packs", rest))
        return UnderRoot(PcPacksRoot(), rest);

    const fs::path nativePath = PcGamePath(path);
    if ((!path.empty() && path[0] == '/') || nativePath.is_absolute())
        return nativePath;
    return reading ? ReadablePath(path) : ContentPath(path);
}

static long long StreamSize(std::FILE* stream)
{
    std::fflush(stream);
    if (!PcSeekFile(stream, 0, SEEK_END))
        return 0;
    long long size = PcTellFile(stream);
    return size < 0 ? 0 : size;
}

int CreateFileA(const char* fileName, unsigned int desiredAccess, unsigned int, void*, unsigned int creationDisposition,
                unsigned int, int)
{
    FileOpenMode mode;
    switch (creationDisposition)
    {
    case OPEN_EXISTING:
        if (desiredAccess == GENERIC_READ)
        {
            mode = FileOpenMode::Read;
            break;
        }
        [[fallthrough]];
    case OPEN_ALWAYS:
        mode = FileOpenMode::Append;
        break;
    case CREATE_NEW:
    case CREATE_ALWAYS:
    case TRUNCATE_EXISTING:
        mode = FileOpenMode::Truncate;
        break;
    default:
        return INVALID_HANDLE_VALUE;
    }

    const fs::path path = ResolvePath(fileName, mode == FileOpenMode::Read);
    std::error_code error;
    if (fs::is_directory(path, error))
        return INVALID_HANDLE_VALUE;

    std::FILE* stream = PcOpenFile(path, mode == FileOpenMode::Read ? "rb" : mode == FileOpenMode::Append ? "a+b" : "w+b");
    if (stream == nullptr)
        return INVALID_HANDLE_VALUE;

    return AddFile(std::make_shared<FileHandleEntry>(stream, mode == FileOpenMode::Append));
}

void CloseHandle(int file)
{
    std::shared_ptr<FileHandleEntry> openFile;
    {
        FileHandleTable& table = Handles();
        std::lock_guard<std::mutex> lock(table.mutex);
        std::unordered_map<int, std::shared_ptr<FileHandleEntry>>::iterator it = table.files.find(file);
        if (it == table.files.end())
            return;
        openFile = it->second;
        table.files.erase(it);
    }
    std::lock_guard<std::mutex> lock(openFile->mutex);
    std::fflush(openFile->stream);
}

unsigned int GetFileSize(int file, unsigned int*)
{
    std::shared_ptr<FileHandleEntry> openFile = FindFile(file);
    if (!openFile)
        return 0;
    std::lock_guard<std::mutex> lock(openFile->mutex);
    return static_cast<unsigned int>(StreamSize(openFile->stream));
}

int ReadFile(int file, void* buffer, unsigned int bytesToRead, unsigned int* bytesRead, _OVERLAPPED*)
{
    std::shared_ptr<FileHandleEntry> openFile = FindFile(file);
    if (!openFile)
        return 1;
    std::lock_guard<std::mutex> lock(openFile->mutex);
    std::size_t total = 0;
    if (bytesToRead != 0 && PcSeekFile(openFile->stream, openFile->position, SEEK_SET))
    {
        total = std::fread(buffer, 1, bytesToRead, openFile->stream);
        std::clearerr(openFile->stream);
        openFile->position += static_cast<long long>(total);
    }
    if (bytesRead != nullptr)
        *bytesRead = static_cast<unsigned int>(total);
    return 1;
}

int WriteFile(int file, const void* buffer, unsigned int bytesToWrite, unsigned int* bytesWritten, _OVERLAPPED*)
{
    std::shared_ptr<FileHandleEntry> openFile = FindFile(file);
    if (!openFile)
        return 0;
    std::lock_guard<std::mutex> lock(openFile->mutex);
    const bool positioned = openFile->append ? PcSeekFile(openFile->stream, 0, SEEK_END)
                                             : PcSeekFile(openFile->stream, openFile->position, SEEK_SET);
    if (!positioned)
        return 0;
    const std::size_t written = bytesToWrite == 0 ? 0 : std::fwrite(buffer, 1, bytesToWrite, openFile->stream);
    if (written != bytesToWrite)
    {
        std::clearerr(openFile->stream);
        return 0;
    }
    if (!openFile->append)
        openFile->position += static_cast<long long>(written);
    if (bytesWritten != nullptr)
        *bytesWritten = static_cast<unsigned int>(written);
    return 1;
}

int SetFilePointer(int file, long distanceToMove, int*, unsigned int moveMethod)
{
    std::shared_ptr<FileHandleEntry> openFile = FindFile(file);
    if (!openFile)
        return -1;
    std::lock_guard<std::mutex> lock(openFile->mutex);
    const int fileSize = static_cast<int>(StreamSize(openFile->stream));
    int position;
    if (moveMethod == 2)
        position = fileSize + static_cast<int>(distanceToMove);
    else if (moveMethod == 1)
        position = static_cast<int>(openFile->position) + static_cast<int>(distanceToMove);
    else
        position = static_cast<int>(distanceToMove);
    if (position >= 0 && position < fileSize)
        openFile->position = position;
    return position;
}

unsigned int GetFileAttributesA(const char* fileName)
{
    std::error_code error;
    const fs::file_status status = fs::status(ResolvePath(fileName, true), error);
    if (error || !fs::exists(status))
        return INVALID_FILE_ATTRIBUTES;
    return fs::is_directory(status) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
}

int CreateDirectoryA(const char* pathName, void*)
{
    std::error_code error;
    const bool created = fs::create_directory(ResolvePath(pathName), error);
    return created && !error;
}

bool PcRemoveAll(const char* path)
{
    std::error_code error;
    fs::remove_all(ResolvePath(path), error);
    return !error;
}

bool PcRename(const char* from, const char* to)
{
    std::error_code error;
    fs::rename(ResolvePath(from), ResolvePath(to), error);
    return !error;
}

bool PcCreateDirectories(const char* path)
{
    std::error_code error;
    fs::create_directories(ResolvePath(path), error);
    return !error;
}

int OpenDir(const char* path, int* directory)
{
    std::error_code error;
    fs::directory_iterator iterator(ResolvePath(path), error);
    if (error)
        return 0;
    *directory = AddDirectory(std::make_shared<DirectoryHandleEntry>(std::move(iterator)));
    return 1;
}

int ReadDir(int* directory, FSDirEntry* entry)
{
    std::shared_ptr<DirectoryHandleEntry> openDirectory = FindDirectory(*directory);
    if (!openDirectory)
        return 0;
    std::lock_guard<std::mutex> lock(openDirectory->mutex);
    if (openDirectory->iterator == fs::directory_iterator())
        return 0;

    const fs::directory_entry& current = *openDirectory->iterator;
    const std::string name = PcGameName(current.path().filename());
    const std::size_t length = std::min(name.size(), sizeof(entry->name) - 1);
    std::memcpy(entry->name, name.data(), length);
    entry->name[length] = '\0';

    std::error_code error;
    entry->isDirectory = current.is_directory(error);
    entry->size = 0;
    if (!entry->isDirectory)
    {
        const std::uintmax_t size = current.file_size(error);
        if (!error)
            entry->size = static_cast<unsigned long long>(size);
    }

    openDirectory->iterator.increment(error);
    if (error)
        openDirectory->iterator = fs::directory_iterator();
    return 1;
}

int CloseDir(int* directory)
{
    FileHandleTable& table = Handles();
    std::lock_guard<std::mutex> lock(table.mutex);
    return table.directories.erase(*directory) != 0;
}

static const unsigned int kMemCommit = 0x1000;
static const unsigned int kMemReserve = 0x2000;
static const unsigned int kMemDecommit = 0x4000;
static const unsigned int kMemRelease = 0x8000;

void* VirtualAlloc(void* address, unsigned long size, unsigned int allocationType, unsigned int)
{
    const bool commit = (allocationType & kMemCommit) != 0;
    if (address == nullptr)
        return (allocationType & kMemReserve) != 0 || commit ? PcVirtualReserve(size, commit) : nullptr;
    if (commit && !PcVirtualCommit(address, size))
        return nullptr;
    return address;
}

int VirtualFree(void* address, unsigned long size, unsigned int freeType)
{
    if (address == nullptr)
        return 0;
    if ((freeType & kMemRelease) != 0)
        return PcVirtualRelease(address, size) ? 1 : 0;
    if ((freeType & kMemDecommit) != 0)
        return PcVirtualDecommit(address, size) ? 1 : 0;
    return 1;
}
