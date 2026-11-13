#ifndef MOD_MODMANIFEST_HPP
#define MOD_MODMANIFEST_HPP

#include <string>
#include <vector>

namespace mod
{
class Dependency
{
public:
    std::string id;
    std::string constraint;
};

class Manifest
{
public:
    static const int kApi = 1;

    Manifest() : api(0), memoryMegabytes(0), budgetMillions(0) {}

    static bool ValidId(const std::string& id);
    static bool ValidEntry(const std::string& entry);
    static bool Parse(const std::string& text, Manifest& manifest, std::string* error);

    std::string id;
    std::string name;
    std::string version;
    std::string entry;
    int api;
    int memoryMegabytes;
    int budgetMillions;
    std::vector<Dependency> depends;
};
}

#endif
