#include "mod/ModVersion.hpp"

#include <stdlib.h>

namespace mod
{
bool Version::Parse(const std::string& text, std::vector<int>& parts)
{
    parts.clear();
    if (text.empty())
        return false;
    size_t at = 0;
    while (at <= text.size())
    {
        size_t end = text.find('.', at);
        if (end == std::string::npos)
            end = text.size();
        if (end == at || end - at > 9)
            return false;
        int value = 0;
        for (size_t index = at; index < end; ++index)
        {
            if (text[index] < '0' || text[index] > '9')
                return false;
            value = value * 10 + (text[index] - '0');
        }
        parts.push_back(value);
        at = end + 1;
    }
    return true;
}

int Version::Compare(const std::vector<int>& left, const std::vector<int>& right)
{
    const size_t count = left.size() > right.size() ? left.size() : right.size();
    for (size_t index = 0; index < count; ++index)
    {
        const int a = index < left.size() ? left[index] : 0;
        const int b = index < right.size() ? right[index] : 0;
        if (a != b)
            return a < b ? -1 : 1;
    }
    return 0;
}

bool Version::ValidConstraint(const std::string& constraint)
{
    if (constraint.empty())
        return true;
    size_t at = 0;
    while (at < constraint.size() && (constraint[at] == '>' || constraint[at] == '<' || constraint[at] == '=' || constraint[at] == '^'))
        ++at;
    std::vector<int> parts;
    return at <= 2 && Parse(constraint.substr(at), parts);
}

bool Version::Satisfies(const std::string& version, const std::string& constraint)
{
    if (constraint.empty())
        return true;
    std::string op;
    size_t at = 0;
    while (at < constraint.size() && (constraint[at] == '>' || constraint[at] == '<' || constraint[at] == '=' || constraint[at] == '^'))
        op += constraint[at++];
    std::vector<int> have, want;
    if (!Parse(version, have) || !Parse(constraint.substr(at), want))
        return false;
    const int order = Compare(have, want);
    if (op == ">=")
        return order >= 0;
    if (op == ">")
        return order > 0;
    if (op == "<=")
        return order <= 0;
    if (op == "<")
        return order < 0;
    if (op == "^")
        return order >= 0 && !have.empty() && !want.empty() && have[0] == want[0];
    return order == 0;
}
}
