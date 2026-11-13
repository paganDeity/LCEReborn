#include <stdio.h>

#include <string>
#include <vector>

#include "platform/pc/PcWorldBackup.hpp"

namespace
{
int failures = 0;

void Check(bool ok, const char* what)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok)
        ++failures;
}
}

int main()
{
    std::vector<std::string> copies;
    copies.push_back("world-20261111-120000");
    copies.push_back("world-20261109-080000");
    copies.push_back("world-20261110-235959");
    copies.push_back("world-20261111-000001");
    const std::vector<std::string> surplus = PcWorldBackup::Surplus(copies, 2);
    Check(surplus.size() == 2 && surplus[0] == "world-20261109-080000" && surplus[1] == "world-20261110-235959",
          "the copies past the number to keep are the oldest ones");
    Check(PcWorldBackup::Surplus(copies, 4).empty() && PcWorldBackup::Surplus(copies, 9).empty(), "nothing is surplus within the number to keep");
    Check(PcWorldBackup::Surplus(std::vector<std::string>(), 1).empty(), "no copies, no surplus");
    printf(failures == 0 ? "all passed\n" : "%d failed\n", failures);
    return failures == 0 ? 0 : 1;
}
