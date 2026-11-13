#ifndef PLATFORM_PC_PCWORLDBACKUP_HPP
#define PLATFORM_PC_PCWORLDBACKUP_HPP

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

// A copy of a world taken as it is loaded, so a bad save, a broken mod or a stuck server never costs more than one session. Both programs load
// worlds through it (the dedicated server and a world opened from the game), into backups/ in the save folder, one folder per copy named after
// the world and the time, keeping the newest few of each world.
namespace PcWorldBackup
{
// whether to copy and how many copies of a world to keep (server.properties: backup-on-load, backup-keep)
void SetPolicy(bool enabled, int keep);

// copy the world folder before it is read; false only when a copy was wanted and could not be made, which is logged and does not stop the load
bool BeforeLoad(const std::filesystem::path& worldFolder);

// the copies of one world that are past the number to keep, oldest first (names sort by time)
inline std::vector<std::string> Surplus(std::vector<std::string> copies, int keep)
{
    std::sort(copies.begin(), copies.end());
    if (keep < 1 || static_cast<int>(copies.size()) <= keep)
        return std::vector<std::string>();
    copies.resize(copies.size() - static_cast<size_t>(keep));
    return copies;
}
}

#endif
