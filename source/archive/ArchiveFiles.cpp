#include "archive/ArchiveFiles.hpp"

namespace archive
{
bool Files::IsDirectory(const std::string& path)
{
    std::vector<std::string> none;
    return List(path, &none, NULL);
}

bool Files::ListDirectories(const std::string& directory, std::vector<std::string>& names)
{
    return List(directory, &names, NULL);
}

bool Files::ListFiles(const std::string& directory, std::vector<std::string>& names)
{
    std::vector<std::string> directories;
    std::vector<std::string> files;
    if (!List(directory, &directories, &files))
        return false;
    names.insert(names.end(), files.begin(), files.end());
    for (size_t i = 0; i < directories.size(); ++i)
    {
        std::vector<std::string> inner;
        if (!ListFiles(Join(directory, directories[i]), inner))
            continue;
        for (size_t j = 0; j < inner.size(); ++j)
            names.push_back(directories[i] + "/" + inner[j]);
    }
    return true;
}

std::string Files::Join(const std::string& directory, const std::string& name)
{
    return directory.empty() || directory[directory.size() - 1] == '/' ? directory + name : directory + "/" + name;
}
}
