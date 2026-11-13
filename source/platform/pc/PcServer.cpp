#include "game/world/light/LightEngine.hpp"
#include "platform/pc/PcServer.hpp"
#include "platform/pc/PcServerAccess.hpp"
#include "game/server/ServerWatchdog.hpp"

#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cwctype>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/Compression.hpp"
#include "game/core/DLCManager.hpp"
#include "game/core/DLCPack.hpp"
#include "game/core/Motd.hpp"
#include "game/core/GameSettings.hpp"
#include "game/core/IntCache.hpp"
#include "game/core/L10N.hpp"
#include "game/core/Minecraft.hpp"
#include "game/core/ShutdownManager.hpp"
#include "game/core/VideoSettings.hpp"
#include "game/command/ChatCommands.hpp"
#include "game/client/render/TexturePackRepository.hpp"
#include "game/data/ElementStorage.hpp"
#include "game/data/SparseDataStorage.hpp"
#include "game/data/SparseLightStorage.hpp"
#include "game/item/FireworksRecipe.hpp"
#include "game/minigame/MiniGameDef.hpp"
#include "game/network/CGameNetworkManager.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerControl.hpp"
#include "game/util/AABB.hpp"
#include "game/util/Vec3.hpp"
#include "game/util/phys__Vec2.hpp"
#include "game/world/Level.hpp"
#include "game/world/LevelData.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/CompressedBlockStorage.hpp"
#include "game/world/chunk/LevelChunk.hpp"
#include "game/world/chunk/OldChunkStorage.hpp"
#include "game/world/generation/PlayerList.hpp"
#include "net/NetMessages.hpp"
#include "net/NetSocket.hpp"
#include "platform/C4JThreadImpl.hpp"
#include "platform/CProfile.hpp"
#include "platform/CStorage.hpp"
#include "platform/FileApi.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcCrashReport.hpp"
#include "platform/pc/PcJson.hpp"
#include "platform/pc/PcLaunchOptions.hpp"
#include "platform/pc/PcModHost.hpp"
#include "platform/pc/PcP2pStart.hpp"
#include "platform/pc/PcPlayerIdentities.hpp"
#include "platform/pc/PcPaths.hpp"
#include "platform/pc/PcStartWorld.hpp"
#include "platform/pc/PcStopSignals.hpp"
#include "platform/pc/PcUtf8.hpp"
#include "vendor/_NetworkGameInitData.hpp"

extern CConsoleMinecraftApp gApp;
void ShutdownAndExit();

namespace
{
const Logger sLog("Server");
const int kIconSize = 64;

const char* const kDefaultProperties =
    "# LCEReborn dedicated server. Changes apply the next time the server starts.\n"
    "# world: the folder in saves/games to load; a new world is made there if it is missing\n"
    "# gamemode (survival, creative, adventure), difficulty (peaceful, easy, normal, hard),\n"
    "# world-size (classic, small, medium, large), level-seed, allow-cheats and pvp apply to new worlds\n"
    "# show-name-tags: show each player's name above their head\n"
    "# Operators may use commands while cheats are allowed. They are kept by UUID in ops.json beside this file;\n"
    "# add and remove them with the op and deop commands.\n"
    "# motd: shown under the server in the Join tab, as on Java servers: \\u00A7 or & codes, &#RRGGBB colours,\n"
    "# \\n for a second line, or a JSON text component. A 64x64 server-icon.png beside this file is its icon.\n"
    "# texture-pack: a texture or mash-up pack for the world, by its folder name in the packs folder or its pack\n"
    "# ID; players who don't have it are offered it when they join. Leave it empty to keep the world's own.\n"
    "server-name=LCEReborn Server\n"
    "motd=An LCEReborn server\n"
    "port=25570\n"
    "world=world\n"
    "level-seed=\n"
    "gamemode=survival\n"
    "difficulty=normal\n"
    "world-size=classic\n"
    "allow-cheats=false\n"
    "pvp=true\n"
    "show-name-tags=true\n"
    "view-distance=10\n"
    "simulation-distance=10\n"
    "autosave-minutes=5\n"
    "lan-visible=true\n"
    "texture-pack=\n"
    "# white-list: only the players in whitelist.json may join (the whitelist command adds and removes them, and turns this on and off).\n"
    "# Banned players are kept in banned-players.json (the ban, pardon and banlist commands). max-players: 1 up to the game's own limit.\n"
    "# max-connections-per-ip: connections one address may hold at once; max-pending-connections: connections that have not joined yet.\n"
    "# 0 is no limit. Addresses kept out are in banned-ips.json (the ban-ip and pardon-ip commands).\n"
    "white-list=false\n"
    "max-players=8\n"
    "max-connections-per-ip=4\n"
    "max-pending-connections=16\n"
    "# max-packets-per-second: a remote player who sends more than this, averaged over five seconds, is disconnected; 0 is no limit.\n"
    "max-packets-per-second=500\n"
    "# max-tick-seconds: a server whose loop has stood still this long is stuck and stops itself, without saving; 0 is never.\n"
    "max-tick-seconds=60\n"
    "# backup-on-load: copy a world into backups/ in the save folder each time it is loaded, keeping the newest backup-keep copies of it.\n"
    "backup-on-load=true\n"
    "backup-keep=3\n"
    "# protocols: the wires remote players may use: legacy (the original game's, which Wii U consoles speak), reborn, or reborn,legacy.\n"
    "protocols=legacy\n";

struct ServerProperties
{
    std::string serverName = "LCEReborn Server";
    std::wstring motd = L"An LCEReborn server";
    int port = lcenet::kDefaultPort;
    std::string world = "world";
    std::string seed;
    int gameType = 0;
    int difficulty = 2;
    int worldSize = 0;
    bool allowCheats = false;
    bool pvp = true;
    bool showNameTags = true;
    int viewDistance = 10;
    int simulationDistance = 10;
    int autosaveMinutes = 5;
    bool lanVisible = true;
    std::string texturePack;
};

std::mutex sConsoleMutex;
std::deque<std::string> sConsoleLines;
std::deque<std::wstring> sServerCommands;
std::atomic<int> sHostResult(0);

std::string Trim(const std::string& text)
{
    size_t begin = 0;
    while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])))
        ++begin;
    size_t end = text.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])))
        --end;
    return text.substr(begin, end - begin);
}

std::string Lower(std::string text)
{
    for (size_t index = 0; index < text.size(); ++index)
        text[index] = static_cast<char>(std::tolower(static_cast<unsigned char>(text[index])));
    return text;
}

bool ContinuesOnNextLine(const std::string& line)
{
    size_t backslashes = 0;
    while (backslashes < line.size() && line[line.size() - 1 - backslashes] == '\\')
        ++backslashes;
    return backslashes % 2 == 1;
}

bool ParseInt(const std::string& value, int minimum, int maximum, int& result)
{
    char* end = NULL;
    long number = std::strtol(value.c_str(), &end, 10);
    if (value.empty() || *end != '\0' || number < minimum || number > maximum)
        return false;
    result = static_cast<int>(number);
    return true;
}

bool ParseBool(const std::string& value, bool& result)
{
    std::string lower = Lower(value);
    if (lower == "true" || lower == "on" || lower == "yes")
        result = true;
    else if (lower == "false" || lower == "off" || lower == "no")
        result = false;
    else
        return false;
    return true;
}

bool ParseChoice(const std::string& value, const char* const* names, int count, int& result)
{
    std::string lower = Lower(value);
    for (int index = 0; index < count; ++index)
    {
        if (lower == names[index])
        {
            result = index;
            return true;
        }
    }
    return false;
}

void ApplyProperty(ServerProperties& properties, const std::string& key, const std::string& value)
{
    static const char* const kDifficulties[] = {"peaceful", "easy", "normal", "hard"};
    static const char* const kWorldSizes[] = {"classic", "small", "medium", "large"};

    bool valid = true;
    if (key == "server-name")
        properties.serverName = value.empty() ? properties.serverName : value;
    else if (key == "motd")
        properties.motd = Motd::Parse(Motd::Unescape(PcUtf8ToWide(value)));
    else if (key == "port")
        valid = ParseInt(value, 1, 65535, properties.port);
    else if (key == "world")
        valid = !value.empty() && value.find_first_of("/\\:") == std::string::npos
                && (properties.world = value, true);
    else if (key == "level-seed")
        properties.seed = value;
    else if (key == "gamemode")
    {
        const int gameType = PcGameTypeFromName(Lower(value));
        valid = gameType >= 0;
        if (valid)
            properties.gameType = gameType;
    }
    else if (key == "difficulty")
        valid = ParseChoice(value, kDifficulties, 4, properties.difficulty);
    else if (key == "world-size")
        valid = ParseChoice(value, kWorldSizes, LevelData::WORLD_SIZE_COUNT, properties.worldSize);
    else if (key == "allow-cheats")
        valid = ParseBool(value, properties.allowCheats);
    else if (key == "pvp")
        valid = ParseBool(value, properties.pvp);
    else if (key == "show-name-tags")
        valid = ParseBool(value, properties.showNameTags);
    else if (key == "ops")
    {
        if (!value.empty())
            LOG_WARN(sLog, "server.properties: ops is no longer read; operators are kept by UUID in ops.json, set "
                           "with the op command");
        return;
    }
    else if (key == "view-distance")
        valid = ParseInt(value, VideoSettings::RENDER_DISTANCE_MIN, VideoSettings::RENDER_DISTANCE_MAX,
                         properties.viewDistance);
    else if (key == "simulation-distance")
        valid = ParseInt(value, VideoSettings::SIMULATION_DISTANCE_MIN, VideoSettings::SIMULATION_DISTANCE_MAX,
                         properties.simulationDistance);
    else if (key == "white-list" || key == "max-players" || key == "max-connections-per-ip" || key == "max-pending-connections"
             || key == "max-packets-per-second" || key == "max-tick-seconds"
             || key == "backup-on-load" || key == "backup-keep" || key == "protocols")
    {
        // read by PcServerAccess, which a game hosted from the client starts too
    }
    else if (key == "autosave-minutes")
        valid = ParseInt(value, 0, 1440, properties.autosaveMinutes);
    else if (key == "lan-visible")
        valid = ParseBool(value, properties.lanVisible);
    else if (key == "texture-pack")
        properties.texturePack = value;
    else
    {
        LOG_WARN(sLog, "server.properties: unknown setting %s", key.c_str());
        return;
    }
    if (!valid)
        LOG_WARN(sLog, "server.properties: %s=%s is not valid, keeping the default", key.c_str(), value.c_str());
}

ServerProperties LoadProperties()
{
    ServerProperties properties;
    std::filesystem::path path = std::filesystem::path(PcSaveRoot()) / "server.properties";
    std::ifstream file(path);
    if (!file)
    {
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);
        std::ofstream created(path);
        created << kDefaultProperties;
        if (created)
            LOG_INFO(sLog, "Wrote default settings to %s", path.string().c_str());
        else
            LOG_WARN(sLog, "Could not write %s; using the default settings", path.string().c_str());
        return properties;
    }

    std::string line;
    while (std::getline(file, line))
    {
        line = Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '!')
            continue;
        std::string next;
        while (ContinuesOnNextLine(line) && std::getline(file, next))
            line = line.substr(0, line.size() - 1) + Trim(next);
        size_t equals = line.find('=');
        if (equals == std::string::npos)
        {
            LOG_WARN(sLog, "server.properties: ignoring %s", line.c_str());
            continue;
        }
        ApplyProperty(properties, Lower(Trim(line.substr(0, equals))), Trim(line.substr(equals + 1)));
    }
    return properties;
}

int HostThreadProc(void* data)
{
    AABB::UseDefaultThreadStorage();
    Vec3::UseDefaultThreadStorage();
    Compression::UseDefaultThreadStorage();
    Block::CreateNewThreadStorage();
    IntCache::CreateNewThreadStorage();
    FireworksRecipe::UseDefaultThreadStorage();
    ElementStorage::CreateNewThreadStorage(ElementStorage::eHESThreadId_NetworkGame);
    LevelChunk::CreateNewThreadStorage();
    sHostResult = CGameNetworkManager::sInstance.RunDedicatedServer(data) ? 1 : -1;
    Block::ReleaseThreadStorage();
    return 0;
}

void ReadConsole()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::lock_guard<std::mutex> lock(sConsoleMutex);
        sConsoleLines.push_back(line);
    }
}

bool NextServerCommand(std::wstring& line)
{
    std::lock_guard<std::mutex> lock(sConsoleMutex);
    if (sServerCommands.empty())
        return false;
    line = sServerCommands.front();
    sServerCommands.pop_front();
    return true;
}

void PrintCommandOutput(const std::wstring& text)
{
    std::wstring plain;
    for (size_t index = 0; index < text.size(); ++index)
    {
        if (text[index] == L'\x00a7' && index + 1 < text.size())
        {
            ++index;
            continue;
        }
        plain += text[index];
    }
    if (plain.empty())
        return;
    LOG_INFO(sLog, "%s", PcWideToUtf8(plain).c_str());
}

void HandleConsoleLine(const std::string& rawLine)
{
    static const char kByteOrderMark[] = "\xEF\xBB\xBF";
    std::string line = rawLine.compare(0, 3, kByteOrderMark) == 0 ? rawLine.substr(3) : rawLine;
    line = Trim(line);
    if (!line.empty() && line[0] == '/')
        line = Trim(line.substr(1));
    if (line.empty())
        return;
    std::string command = Lower(line.substr(0, line.find(' ')));
    // stop does not wait for the server's tick, so a server whose loop is busy still stops
    if (command == "stop")
    {
        ServerControl::RequestStop();
        return;
    }
    std::lock_guard<std::mutex> lock(sConsoleMutex);
    sServerCommands.push_back(PcUtf8ToWide(line));
}

void InitialiseGame(const std::wstring& serverName)
{
    ShutdownManager::Initialise();
    if (gProfile == 0)
        gProfile = new CProfile();
    static_cast<CProfile*>(gProfile)->SetPlayerName(serverName);
    Compression::CreateNewThreadStorage();

    gApp.loadMediaArchive();
    if (gApp.ReadProductCodes() == 0)
        gApp.FatalLoadError(L"ReadProductCodes");
    L10N::loadStringTable();
    gApp.InitTime();

    static unsigned int s_profileDefaults[5];
    gProfile->Initialise(L"", L"", 0x22, 5, 4, s_profileDefaults, 0x1e60, &gApp.mGameDefinedDataChangedBitmask);

    bool noSpaceForSaveQuota;
    gStorage->Init(0, L10N::GetString(0xc0b26be6), "savegame.dat", 51000000, NULL, &gApp, "", &noSpaceForSaveQuota);
    gApp.SetNoSpaceForSaveQuota(noSpaceForSaveQuota);
    gStorage->InitialiseGenericData();
    gStorage->InitialiseProfileData(0x22, 5, 4, s_profileDefaults, 0x1e60, &gApp.mGameDefinedDataChangedBitmask);
    gStorage->SetDefaultOptionsCallback(CMinecraftApp::DefaultOptionsCallback, &gApp);
    gStorage->SetOptionsDataCallback(CMinecraftApp::OptionsDataCallback, &gApp);
    gStorage->SetOldProfileVersionCallback(GameSettings::OldProfileVersionCallback, &gApp);
    gStorage->SetGameSaveFolderPrefix("");
    gStorage->SetMaxSaves(99);
    arrayWithLength<unsigned char> thumbnail = gApp.getArchiveFile(L"DefaultSaveThumbnail64x64.png", false);
    gStorage->SetDefaultImages(0, 0, 0, 0, thumbnail.data, thumbnail.length);
    if (thumbnail.data != 0)
        delete[] thumbnail.data;
    gStorage->SetOpenDirFn(OpenDir);
    gStorage->SetReadDirFn(ReadDir);
    gStorage->SetCloseDirFn(CloseDir);
    gStorage->SetDLCPackageRoot("DLCDrive");

    AABB::CreateNewThreadStorage();
    Vec3::CreateNewThreadStorage();
    phys::Vec2::CreateNewThreadStorage();
    IntCache::CreateNewThreadStorage();
    OldChunkStorage::CreateNewThreadStorage();
    LightEngine::EnableThreadCache();
    Block::CreateNewThreadStorage();
    FireworksRecipe::CreateNewThreadStorage();
    LevelChunk::CreateNewThreadStorage();
    ElementStorage::staticCtor();
    ElementStorage::CreateNewThreadStorage(ElementStorage::eHESThreadId_Main);

    Minecraft::mainHeadless();
    gApp.InitGameSettings();
    gStorage->ReadFromProfile(0, 0);
    CGameNetworkManager::sInstance.Initialise();
    CGameNetworkManager::sInstance.DoWork();
}

void LoadServerIcon()
{
    std::filesystem::path path = std::filesystem::path(PcSaveRoot()) / "server-icon.png";
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return;
    std::vector<unsigned char> icon((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    static const unsigned char kSignature[16] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n', 0, 0, 0, 13,
                                                 'I',  'H', 'D', 'R'};
    if (icon.size() < 24 || std::memcmp(&icon[0], kSignature, sizeof(kSignature)) != 0)
    {
        LOG_WARN(sLog, "%s is not a PNG image, so the server has no icon", path.string().c_str());
        return;
    }
    const unsigned int width = (icon[16] << 24) | (icon[17] << 16) | (icon[18] << 8) | icon[19];
    const unsigned int height = (icon[20] << 24) | (icon[21] << 16) | (icon[22] << 8) | icon[23];
    if (width != kIconSize || height != kIconSize)
    {
        LOG_WARN(sLog, "server-icon.png is %ux%u; it must be %dx%d", width, height, kIconSize, kIconSize);
        return;
    }
    if (icon.size() > lcenet::kMaxIconSize)
    {
        LOG_WARN(sLog, "server-icon.png is larger than %u bytes", static_cast<unsigned int>(lcenet::kMaxIconSize));
        return;
    }
    CGameNetworkManager::sInstance.SetServerIcon(icon, true);
    LOG_INFO(sLog, "Using server-icon.png as the server's icon");
}

void StopWithError()
{
    ShutdownManager::MainThreadHandleShutdown();
    Log::Stop();
    _Exit(1);
}

void TickMainThread()
{
    gProfile->Tick();
    gStorage->Tick();
    CGameNetworkManager::sInstance.DoWork();
    CompressedBlockStorage::tick();
    SparseLightStorage::tick();
    SparseDataStorage::tick();
    Vec3::resetPool();
}

void LoadPacks()
{
    gApp.StartInstallDLCProcess(0);
    const std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::now() + std::chrono::minutes(2);
    while (gApp.DLCInstallProcessCompleted() == 0 && std::chrono::steady_clock::now() < deadline)
    {
        TickMainThread();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    LOG_INFO(sLog, "Loaded %d packs; packs folder: %s", gApp.mDLCManager.getPackCount(DLCManager::eDLCType_All),
             PcPacksRoot().c_str());
}

int FindTexturePack(const std::string& name)
{
    const std::wstring wanted = PcUtf8ToWide(Lower(name));
    char* end = NULL;
    const long number = std::strtol(name.c_str(), &end, 10);
    const int count = gApp.mDLCManager.getPackCount(DLCManager::eDLCType_All);
    for (int index = 0; index < count; ++index)
    {
        DLCPack* pack = gApp.mDLCManager.getPack(static_cast<unsigned int>(index), DLCManager::eDLCType_All);
        if (pack == NULL || pack->GetRootPath().empty())
            continue;
        const int id = static_cast<int>(pack->GetPackId() & 0x00ffffffu);
        std::wstring folder = pack->getName();
        for (size_t character = 0; character < folder.size(); ++character)
            folder[character] = static_cast<wchar_t>(std::towlower(folder[character]));
        const bool matches = (end != NULL && *end == '\0' && !name.empty() && number == id) || folder == wanted;
        if (matches && Minecraft::GetInstance()->mTexturePackRepository->getTexturePackById(id) != NULL)
            return id;
    }
    return -1;
}
}

int PcServerMain()
{
    C4JThreadImpl::SetMainThreadID();
    const std::string& logDir = PcGetLaunchOptions().logDir;
    Log::Start(logDir.empty() ? std::string("logs") : logDir, PcGetLaunchOptions().logKeepDays);
    PcCrashReport::Install(logDir.empty() ? std::string("logs") : logDir);

    std::string folderError;
    if (!PcCheckGameFolders(folderError, false))
    {
        sLog.WriteText(LogLevel::Fatal, folderError);
        Log::Stop();
        return 1;
    }
    LOG_INFO(sLog, "Content: %s", PcContentRoot().c_str());
    LOG_INFO(sLog, "Saves: %s", PcSaveRoot().c_str());

    const ServerProperties properties = LoadProperties();
    CMinecraftApp::SetDedicatedServer(true);
    VideoSettings::SetRenderDistance(properties.viewDistance);
    VideoSettings::SetSimulationDistance(properties.simulationDistance);
    PcServerAccessStart();

    PcStartIdentities("server");
    PcStartMods("server");
    InitialiseGame(PcUtf8ToWide(properties.serverName));
    PcP2pStart();
    LoadPacks();
    gApp.SetGameHostOption(eGameHostOption_Gamertags, properties.showNameTags ? 1 : 0);

    _NetworkGameInitData* initData;
    if (PcSaveFolderExists(properties.world))
    {
        std::string error;
        initData = PcPrepareWorldLoad(properties.world, error);
        if (initData == NULL)
        {
            LOG_FATAL(sLog, "The world %s could not be loaded: %s", properties.world.c_str(), error.c_str());
            StopWithError();
        }
        LOG_INFO(sLog, "Loading the world %s", properties.world.c_str());
    }
    else
    {
        PcNewWorld world;
        world.folder = properties.world;
        world.title = PcUtf8ToWide(properties.world);
        world.seed = PcUtf8ToWide(properties.seed);
        world.gameType = properties.gameType;
        world.difficulty = properties.difficulty;
        world.worldSize = properties.worldSize;
        world.allowCheats = properties.allowCheats;
        world.pvp = properties.pvp;
        initData = PcPrepareNewWorld(world);
        LOG_INFO(sLog, "Making a new world in %s", properties.world.c_str());
    }

    if (!properties.texturePack.empty())
    {
        const int texturePack = FindTexturePack(properties.texturePack);
        if (texturePack < 0)
            LOG_WARN(sLog, "server.properties: texture-pack %s is not a loaded texture or mash-up pack",
                     properties.texturePack.c_str());
        else
        {
            initData->m_texturePackId = texturePack;
            LOG_INFO(sLog, "Using texture pack %d", texturePack);
        }
    }

    LoadServerIcon();
    ChatCommands::SetConsoleOutput(PrintCommandOutput);
    MinecraftServer::SetConsoleCommandSource(NextServerCommand);
    CGameNetworkManager::sInstance.SetListenPort(static_cast<unsigned short>(properties.port));
    CGameNetworkManager::sInstance.SetMotd(properties.motd);
    CGameNetworkManager::sInstance.HostGame(0, true, !properties.lanVisible,
                                            static_cast<unsigned char>(initData->m_gameType), eMiniGameId_Any, 0, 0);
    if (CGameNetworkManager::sInstance.GetListenPort() == 0)
    {
        LOG_FATAL(sLog, "Can't accept players on port %d. Choose another port in server.properties.", properties.port);
        StopWithError();
    }

    PcInstallStopSignals();

    C4JThreadImpl* hostThread = new C4JThreadImpl(HostThreadProc, initData, "Host", 0x100000);
    hostThread->Run();
    hostThread->SetDeleteOnExit(true);

    bool started = false;
    std::chrono::steady_clock::time_point nextAutosave;
    while (!ServerControl::StopRequested() && !PcStopSignalled())
    {
        TickMainThread();

        if (!started && sHostResult != 0)
        {
            if (sHostResult < 0)
            {
                LOG_FATAL(sLog, "The server could not start");
                StopWithError();
            }
            started = true;
            std::thread(ReadConsole).detach();
            nextAutosave = std::chrono::steady_clock::now() + std::chrono::minutes(properties.autosaveMinutes);
            LOG_INFO(sLog, "Ready for players on port %u. Type help for commands.",
                     static_cast<unsigned int>(CGameNetworkManager::sInstance.GetListenPort()));
        }

        if (started)
        {
            if (gMinecraftServer == NULL || MinecraftServer::s_haltRequested)
            {
                LOG_WARN(sLog, "The server stopped");
                break;
            }
            std::deque<std::string> lines;
            {
                std::lock_guard<std::mutex> lock(sConsoleMutex);
                lines.swap(sConsoleLines);
            }
            for (std::deque<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
                HandleConsoleLine(*it);
            const long long nowMs = static_cast<long long>(MinecraftServer::getCurrentTimeMillis());
            if (ServerWatchdog::Stalled(nowMs))
            {
                LOG_FATAL(sLog,
                          "The server has not ticked for %d seconds, so it is stuck. Stopping without saving: a save taken now could be half "
                          "written. The last autosave is what is kept.",
                          ServerWatchdog::StalledSeconds(nowMs));
                StopWithError();
            }
            if (properties.autosaveMinutes > 0 && std::chrono::steady_clock::now() >= nextAutosave)
            {
                ServerControl::RequestAutosave();
                nextAutosave = std::chrono::steady_clock::now() + std::chrono::minutes(properties.autosaveMinutes);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (started && gMinecraftServer != NULL)
    {
        LOG_INFO(sLog, "Saving and stopping");
        gMinecraftServer->saveOnExit = true;
        MinecraftServer::HaltServer(false);
        CGameNetworkManager::sInstance.ServerStoppedWait();
        while (gStorage->GetSaveState() != C4JStorage::ESaveGame_Idle)
        {
            gStorage->Tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    LOG_INFO(sLog, "Stopped");
    ShutdownAndExit();
    return 0;
}
