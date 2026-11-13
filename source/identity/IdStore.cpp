#include "identity/IdStore.hpp"

#include <stdio.h>

#include <algorithm>

#include "identity/IdFiles.hpp"

namespace identity
{
namespace
{
const char kExtension[] = ".lceid";
const char kActiveFile[] = "active";
}

Store::Store(const std::string& directory) : m_directory(directory)
{
}

bool Store::ValidStem(const std::string& stem)
{
    if (stem.empty() || stem.size() > 48)
        return false;
    for (size_t index = 0; index < stem.size(); ++index)
    {
        const char letter = stem[index];
        const bool plain = (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z')
                           || (letter >= '0' && letter <= '9') || letter == '_' || letter == '-';
        if (!plain)
            return false;
    }
    return true;
}

bool Store::Prepare()
{
    return Files::MakeDirectory(m_directory);
}

std::string Store::PathOf(const std::string& stem) const
{
    return m_directory + "/" + stem + kExtension;
}

std::string Store::LockPathOf(const std::string& stem) const
{
    return m_directory + "/" + stem + ".lock";
}

std::vector<std::string> Store::List() const
{
    std::vector<std::string> names;
    Files::List(m_directory, kExtension, names);
    std::vector<std::string> valid;
    for (size_t index = 0; index < names.size(); ++index)
        if (ValidStem(names[index]))
            valid.push_back(names[index]);
    std::sort(valid.begin(), valid.end());
    return valid;
}

bool Store::Load(const std::string& stem, Profile& profile, std::string* error) const
{
    std::vector<uint8_t> bytes;
    if (!ValidStem(stem) || !Files::Read(PathOf(stem), bytes))
    {
        if (error != NULL)
            *error = "the identity file could not be read";
        return false;
    }
    return Profile::Load(bytes, profile, error);
}

bool Store::Save(const std::string& stem, const Profile& profile) const
{
    return ValidStem(stem) && Files::WritePrivate(PathOf(stem), profile.Save());
}

bool Store::Create(const std::string& name, std::string* stem, Profile* profile)
{
    Profile created;
    if (!Profile::Generate(name, created))
        return false;
    std::string candidate = name;
    const std::vector<std::string> existing = List();
    for (int suffix = 2; std::find(existing.begin(), existing.end(), candidate) != existing.end(); ++suffix)
    {
        char text[16];
        snprintf(text, sizeof(text), "-%d", suffix);
        candidate = name + text;
    }
    if (!Save(candidate, created))
        return false;
    if (stem != NULL)
        *stem = candidate;
    if (profile != NULL)
        *profile = created;
    return true;
}

bool Store::Remove(const std::string& stem) const
{
    return ValidStem(stem) && Files::Remove(PathOf(stem));
}

std::string Store::Active() const
{
    std::vector<uint8_t> bytes;
    if (!Files::Read(m_directory + "/" + kActiveFile, bytes))
        return std::string();
    std::string stem(bytes.begin(), bytes.end());
    while (!stem.empty() && (stem[stem.size() - 1] == '\n' || stem[stem.size() - 1] == '\r'))
        stem.erase(stem.size() - 1);
    return ValidStem(stem) ? stem : std::string();
}

bool Store::SetActive(const std::string& stem) const
{
    if (!ValidStem(stem))
        return false;
    return Files::WritePrivate(m_directory + "/" + kActiveFile, std::vector<uint8_t>(stem.begin(), stem.end()));
}
}
