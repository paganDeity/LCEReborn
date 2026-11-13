#include "mod/ModManifest.hpp"

#include <stdlib.h>

#include "mod/ModVersion.hpp"

namespace mod
{
namespace
{
std::string Trim(const std::string& text)
{
    size_t first = 0;
    size_t last = text.size();
    while (first < last && (text[first] == ' ' || text[first] == '\t' || text[first] == '\r'))
        ++first;
    while (last > first && (text[last - 1] == ' ' || text[last - 1] == '\t' || text[last - 1] == '\r'))
        --last;
    return text.substr(first, last - first);
}
}

bool Manifest::ValidId(const std::string& id)
{
    if (id.size() < 3 || id.size() > 32)
        return false;
    for (size_t index = 0; index < id.size(); ++index)
    {
        const char letter = id[index];
        const bool plain = (letter >= 'a' && letter <= 'z') || (letter >= '0' && letter <= '9') || letter == '_' || letter == '-';
        if (!plain)
            return false;
    }
    return true;
}

bool Manifest::ValidEntry(const std::string& entry)
{
    if (entry.size() < 5 || entry.size() > 64 || entry[0] == '.')
        return false;
    for (size_t index = 0; index < entry.size(); ++index)
    {
        const char letter = entry[index];
        const bool plain = (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z') || (letter >= '0' && letter <= '9')
                           || letter == '_' || letter == '-' || letter == '.';
        if (!plain)
            return false;
    }
    const std::string tail = entry.substr(entry.size() - 4);
    return tail == ".lua" || (entry.size() > 5 && entry.substr(entry.size() - 5) == ".wasm");
}

bool Manifest::Parse(const std::string& text, Manifest& manifest, std::string* error)
{
    manifest = Manifest();
    size_t at = 0;
    while (at <= text.size())
    {
        size_t end = text.find('\n', at);
        if (end == std::string::npos)
            end = text.size();
        const std::string line = Trim(text.substr(at, end - at));
        at = end + 1;
        if (line.empty() || line[0] == '#')
            continue;
        const size_t equals = line.find('=');
        if (equals == std::string::npos)
        {
            if (error != NULL)
                *error = "a line without = in mod.txt";
            return false;
        }
        const std::string key = Trim(line.substr(0, equals));
        const std::string value = Trim(line.substr(equals + 1));
        if (key == "id")
            manifest.id = value;
        else if (key == "name")
            manifest.name = value;
        else if (key == "version")
            manifest.version = value;
        else if (key == "entry")
            manifest.entry = value;
        else if (key == "api")
            manifest.api = atoi(value.c_str());
        else if (key == "memory")
            manifest.memoryMegabytes = atoi(value.c_str());
        else if (key == "budget")
            manifest.budgetMillions = atoi(value.c_str());
        else if (key == "depends")
        {
            size_t start = 0;
            while (start <= value.size())
            {
                size_t end = value.find(',', start);
                if (end == std::string::npos)
                    end = value.size();
                const std::string item = Trim(value.substr(start, end - start));
                start = end + 1;
                if (item.empty())
                    continue;
                size_t split = 0;
                while (split < item.size() && item[split] != '>' && item[split] != '<' && item[split] != '=' && item[split] != '^')
                    ++split;
                Dependency dependency;
                dependency.id = Trim(item.substr(0, split));
                dependency.constraint = Trim(item.substr(split));
                if (!ValidId(dependency.id) || !Version::ValidConstraint(dependency.constraint))
                {
                    if (error != NULL)
                        *error = "bad dependency " + item;
                    return false;
                }
                manifest.depends.push_back(dependency);
            }
        }
    }
    if (!ValidId(manifest.id))
    {
        if (error != NULL)
            *error = "id must be 3 to 32 characters of a-z, 0-9, _ and -";
        return false;
    }
    if (manifest.api != kApi)
    {
        if (error != NULL)
            *error = "this game runs mods for api 1";
        return false;
    }
    if (!ValidEntry(manifest.entry))
    {
        if (error != NULL)
            *error = "entry must be a .lua or .wasm file name in the mod's folder";
        return false;
    }
    if (manifest.name.empty())
        manifest.name = manifest.id;
    return true;
}
}
