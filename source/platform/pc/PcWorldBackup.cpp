#include "platform/pc/PcWorldBackup.hpp"

#include <ctime>

#include "platform/pc/Log.hpp"
#include "platform/pc/PcPaths.hpp"

namespace fs = std::filesystem;

namespace
{
const Logger sLog("Backup");
bool sEnabled = true;
int sKeep = 3;

std::string Stamp()
{
    const std::time_t now = std::time(nullptr);
    std::tm utc{};
#if defined(_WIN32)
    gmtime_s(&utc, &now);
#else
    gmtime_r(&now, &utc);
#endif
    char text[32];
    std::strftime(text, sizeof(text), "%Y%m%d-%H%M%S", &utc);
    return text;
}
}

void PcWorldBackup::SetPolicy(bool enabled, int keep)
{
    sEnabled = enabled;
    sKeep = keep < 1 ? 1 : keep;
}

bool PcWorldBackup::BeforeLoad(const fs::path& worldFolder)
{
    std::error_code error;
    if (!sEnabled || !fs::is_directory(worldFolder, error))
        return true;
    const std::string world = worldFolder.filename().u8string();
    const fs::path backups = fs::path(PcSaveRoot()) / "backups";
    fs::create_directories(backups, error);
    const fs::path copy = backups / fs::u8path(world + "-" + Stamp());
    fs::copy(worldFolder, copy, fs::copy_options::recursive | fs::copy_options::overwrite_existing, error);
    if (error)
    {
        LOG_WARN(sLog, "Could not back up %s before loading it: %s", world.c_str(), error.message().c_str());
        return false;
    }
    LOG_INFO(sLog, "Backed up %s to %s", world.c_str(), copy.u8string().c_str());

    // the copies of this world are the folders named after it and a stamp of the same length
    std::vector<std::string> copies;
    const std::string prefix = world + "-";
    for (fs::directory_iterator entry(backups, error), end; !error && entry != end; entry.increment(error))
    {
        const std::string name = entry->path().filename().u8string();
        if (entry->is_directory(error) && name.size() == prefix.size() + 15 && name.compare(0, prefix.size(), prefix) == 0)
            copies.push_back(name);
    }
    const std::vector<std::string> surplus = Surplus(copies, sKeep);
    for (size_t index = 0; index < surplus.size(); ++index)
    {
        std::error_code removeError;
        fs::remove_all(backups / fs::u8path(surplus[index]), removeError);
    }
    return true;
}
