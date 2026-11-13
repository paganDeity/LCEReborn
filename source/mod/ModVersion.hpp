#ifndef MOD_MODVERSION_HPP
#define MOD_MODVERSION_HPP

#include <string>
#include <vector>

namespace mod
{
class Version
{
public:
    static bool Parse(const std::string& text, std::vector<int>& parts);
    static int Compare(const std::vector<int>& left, const std::vector<int>& right);
    static bool Satisfies(const std::string& version, const std::string& constraint);
    static bool ValidConstraint(const std::string& constraint);
};
}

#endif
