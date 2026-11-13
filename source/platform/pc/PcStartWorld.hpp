#ifndef PLATFORM_PC_PCSTARTWORLD_HPP
#define PLATFORM_PC_PCSTARTWORLD_HPP

#include <string>
#include <vector>

class _NetworkGameInitData;

struct PcNewWorld
{
    std::string folder;
    std::wstring title;
    std::wstring seed;
    int gameType = 0;
    int difficulty = 2;
    int worldSize = 0;
    bool allowCheats = false;
    bool pvp = true;
};

void PcStartWorldTick();
bool PcInWorld();
bool PcSaveFolderExists(const std::string& folder);
bool PcReadSaveFile(const std::string& folder, const char* file, std::vector<unsigned char>& data, std::string& error);
_NetworkGameInitData* PcPrepareWorldLoad(const std::string& folder, std::string& error);
_NetworkGameInitData* PcPrepareNewWorld(const PcNewWorld& world);
int PcGameTypeFromName(const std::string& name);

#endif
