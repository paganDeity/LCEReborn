#include "platform/pc/PcStartWorld.hpp"
#include "platform/pc/PcWorldBackup.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "game/client/render/TexturePackRepository.hpp"
#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/CMinecraftApp.hpp"
#include "game/core/ConsoleUIController.hpp"
#include "game/core/FileHeader.hpp"
#include "game/core/Minecraft.hpp"
#include "game/minigame/MiniGameDef.hpp"
#include "game/network/CGameNetworkManager.hpp"
#include "game/ui/UIGroup.hpp"
#include "game/ui/UILayer.hpp"
#include "game/ui/UIScene_FullscreenProgress.hpp"
#include "game/world/LevelData.hpp"
#include "game/world/generation/MultiplayerLocalPlayer.hpp"
#include "game/world/generation/Player.hpp"
#include "platform/CInput.hpp"
#include "platform/CProfile.hpp"
#include "platform/CStorage.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcApplication.hpp"
#include "platform/pc/PcLaunchOptions.hpp"
#include "platform/pc/PcPaths.hpp"
#include "vendor/DiskSaveBuffer.hpp"
#include "vendor/_LoadingInputParams.hpp"
#include "vendor/_NetworkGameInitData.hpp"

extern CConsoleMinecraftApp gApp;

namespace
{
enum StartWorldState
{
    StartWorldState_Idle,
    StartWorldState_WaitForMenu,
    StartWorldState_Loading,
    StartWorldState_Done
};

const int kMenuSettleFrames = 60;
const double kTimeoutSeconds = 300.0;

const Logger sLog("StartWorld");
StartWorldState sState = StartWorldState_Idle;
int sMenuFrames = 0;
double sStateStart = 0.0;

double NowSeconds()
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void Enter(StartWorldState state)
{
    sState = state;
    sStateStart = NowSeconds();
}

void Fail(const char* message, const std::string& detail)
{
    LOG_ERROR(sLog, "%s: %s", message, detail.c_str());
    PcRequestExit();
    Enter(StartWorldState_Done);
}

long long SeedFromText(const std::wstring& seed)
{
    if (seed.empty())
        return static_cast<long long>(std::chrono::steady_clock::now().time_since_epoch().count());
    bool numeric = true;
    for (size_t index = 0; index < seed.size(); ++index)
    {
        if ((seed[index] < L'0' || seed[index] > L'9') && (index != 0 || seed[0] != L'-'))
        {
            numeric = false;
            break;
        }
    }
    if (numeric && seed != L"-")
        return std::wcstoll(seed.c_str(), NULL, 10);
    int hash = 0;
    for (size_t index = 0; index < seed.size(); ++index)
        hash = hash * 31 + seed[index];
    return hash;
}

void SetSaveName(const std::string& folder, const std::wstring& title)
{
    std::vector<char> uniqueName(folder.begin(), folder.end());
    uniqueName.push_back('\0');
    gStorage->ResetSaveData();
    gStorage->SetSaveTitle(title.c_str());
    gStorage->SetSaveUniqueFilename(&uniqueName[0]);
}

bool StartHosting(_NetworkGameInitData* initData)
{
    gApp.SetTutorialMode(false, boost::shared_ptr<Player>());
    CGameNetworkManager::sInstance.HostGame(0, false, false, static_cast<unsigned char>(initData->m_gameType),
                                            eMiniGameId_Any, 0, 0);
    CGameNetworkManager::sInstance.FakeLocalPlayerJoined();

    _LoadingInputParams* params = new _LoadingInputParams();
    params->m_threadProc = &CGameNetworkManager::RunNetworkGameThreadProc;
    params->m_threadParam = initData;
    FullscreenProgressState* progressState = new FullscreenProgressState();
    progressState->closeMode = 3;
    progressState->playerId = 0;
    params->m_completionData = progressState;
    gApp.SetAutosaveTimerTime();
    gConsoleUIController.NavigateToScene(gProfile->GetPrimaryPad(), EUIScene_FullscreenProgress, params, EUILayer_Scene,
                                         EUIGroup_PlayerDefault);
    return true;
}

bool HostWorld(const PcLaunchOptions& options, std::string& error)
{
    if (options.world.find_first_of("/\\:") != std::string::npos)
    {
        error = "--world must be a folder name, not a path: " + options.world;
        return false;
    }
    if (PcSaveFolderExists(options.world))
    {
        if (!options.seed.empty() || !options.gameMode.empty())
            LOG_WARN(sLog, "%s already exists, so --seed and --game-mode are not used", options.world.c_str());
        _NetworkGameInitData* initData = PcPrepareWorldLoad(options.world, error);
        if (initData == NULL)
            return false;
        LOG_INFO(sLog, "Loading %s", options.world.c_str());
        return StartHosting(initData);
    }
    PcNewWorld world;
    world.folder = options.world;
    world.title.assign(options.world.begin(), options.world.end());
    world.seed.assign(options.seed.begin(), options.seed.end());
    if (!options.gameMode.empty())
    {
        world.gameType = PcGameTypeFromName(options.gameMode);
        if (world.gameType < 0)
        {
            error = "--game-mode must be survival, creative or adventure, not " + options.gameMode;
            return false;
        }
    }
    LOG_INFO(sLog, "Making a new world in %s", options.world.c_str());
    return StartHosting(PcPrepareNewWorld(world));
}
}

bool PcSaveFolderExists(const std::string& folder)
{
    std::error_code error;
    return std::filesystem::is_regular_file(std::filesystem::path(PcSaveRoot()) / "games" / folder / "world.bin", error);
}

bool PcReadSaveFile(const std::string& folder, const char* file, std::vector<unsigned char>& data, std::string& error)
{
    std::filesystem::path path = std::filesystem::path(PcSaveRoot()) / "games" / folder / file;
    std::ifstream stream(path, std::ios::binary);
    if (!stream)
    {
        error = path.string() + " could not be opened";
        return false;
    }
    stream.seekg(0, std::ios::end);
    std::streamoff length = stream.tellg();
    stream.seekg(0, std::ios::beg);
    if (length <= 0)
    {
        error = path.string() + " is empty";
        return false;
    }
    data.resize(static_cast<size_t>(length));
    stream.read(reinterpret_cast<char*>(&data[0]), length);
    if (!stream)
    {
        error = path.string() + " could not be read";
        return false;
    }
    return true;
}

_NetworkGameInitData* PcPrepareWorldLoad(const std::string& folder, std::string& error)
{
    PcWorldBackup::BeforeLoad(std::filesystem::path(PcSaveRoot()) / "games" / std::filesystem::u8path(folder));
    std::vector<unsigned char> world;
    if (!PcReadSaveFile(folder, "world.bin", world, error))
        return NULL;

    int texturePackId = -1;
    std::vector<unsigned char> thumbnail;
    std::string thumbnailError;
    if (PcReadSaveFile(folder, "image.png", thumbnail, thumbnailError))
    {
        CGameNetworkManager::sInstance.SetWorldThumbnail(&thumbnail[0], static_cast<unsigned int>(thumbnail.size()));
        CMinecraftApp::PngTagData tagData;
        gApp.GetImageTextData(&thumbnail[0], static_cast<unsigned int>(thumbnail.size()), &tagData);
        tagData.flags |= CMinecraftApp::PngTagData::eFlag_NumLoads;
        ++tagData.numLoads;
        gApp.SetCurrentImageTextData(&tagData);
        if ((tagData.flags & CMinecraftApp::PngTagData::eFlag_TexturePack) != 0)
            texturePackId = static_cast<int>(tagData.texturePack);
    }
    else
    {
        CGameNetworkManager::sInstance.SetWorldThumbnail(NULL, 0);
        LOG_WARN(sLog, "Loading with default settings: %s", thumbnailError.c_str());
    }

    unsigned char* data = static_cast<unsigned char*>(std::malloc(world.size()));
    if (data == NULL)
    {
        error = "out of memory for the save";
        return NULL;
    }
    std::memcpy(data, &world[0], world.size());

    std::wstring name(folder.begin(), folder.end());
    SetSaveName(folder, name);

    if (texturePackId >= 0)
        Minecraft::GetInstance()->mTexturePackRepository->selectTexturePackById(texturePackId, false);

    DiskSaveBuffer* saveBuffer = new DiskSaveBuffer;
    saveBuffer->data = data;
    saveBuffer->length = static_cast<unsigned int>(world.size());
    saveBuffer->name = name;

    _NetworkGameInitData* initData = new _NetworkGameInitData();
    initData->m_seed = 0;
    initData->m_saveData = saveBuffer;
    initData->m_recordLevelResume = 1;
    initData->m_hostOptions = gApp.GetGameHostOption(eGameHostOption_All);
    initData->m_miniGameHostOptions = Minecraft::GetInstance()->GetMiniGame()->GetHostOption(
        eCustomGameModeHostOption_All);
    if (texturePackId >= 0)
        initData->m_texturePackId = texturePackId;
    initData->m_savePlatform = SAVE_FILE_PLATFORM_LOCAL;
    initData->m_gameType = gApp.GetGameHostOption(eGameHostOption_GameType);
    return initData;
}

_NetworkGameInitData* PcPrepareNewWorld(const PcNewWorld& world)
{
    gApp.SetDefaultHostOptions();
    gApp.SetGameHostOption(eGameHostOption_Difficulty, world.difficulty);
    gApp.SetGameHostOption(eGameHostOption_GameType, world.gameType);
    gApp.SetGameHostOption(eGameHostOption_LevelType, 0);
    gApp.SetGameHostOption(eGameHostOption_Structures, 1);
    gApp.SetGameHostOption(eGameHostOption_BonusChest, 0);
    gApp.SetGameHostOption(eGameHostOption_PvP, world.pvp ? 1 : 0);
    gApp.SetGameHostOption(eGameHostOption_TrustPlayers, 1);
    gApp.SetGameHostOption(eGameHostOption_FireSpreads, 1);
    gApp.SetGameHostOption(eGameHostOption_TNT, 1);
    gApp.SetGameHostOption(eGameHostOption_CheatsEnabled,
                           world.allowCheats || PcGetLaunchOptions().cheats ? 1 : 0);
    gApp.SetGameHostOption(eGameHostOption_WorldSize, world.worldSize);
    gApp.ClearTerrainFeaturePosition();

    SetSaveName(world.folder, world.title);
    CGameNetworkManager::sInstance.SetWorldThumbnail(NULL, 0);

    _NetworkGameInitData* initData = new _NetworkGameInitData();
    initData->m_seed = SeedFromText(world.seed);
    initData->m_saveData = NULL;
    initData->m_hostOptions = gApp.GetGameHostOption(eGameHostOption_All);
    initData->m_miniGameHostOptions = Minecraft::GetInstance()->GetMiniGame()->GetHostOption(
        eCustomGameModeHostOption_All);
    initData->m_xzSize = LevelData::getWorldSizeWidth(world.worldSize);
    initData->m_hellScale = static_cast<unsigned char>(LevelData::getWorldSizeHellScale(world.worldSize));
    initData->m_savePlatform = SAVE_FILE_PLATFORM_LOCAL;
    initData->m_gameType = world.gameType;
    return initData;
}

int PcGameTypeFromName(const std::string& name)
{
    static const char* const kNames[] = {"survival", "creative", "adventure"};
    for (int index = 0; index < 3; ++index)
    {
        if (name == kNames[index])
            return index;
    }
    return -1;
}

bool PcInWorld()
{
    Minecraft* minecraft = Minecraft::GetInstance();
    return gApp.GetGameStarted() != 0 && minecraft->mLevel != NULL && minecraft->mPlayer != NULL
           && !gConsoleUIController.IsSceneInStack(gProfile->GetPrimaryPad(), EUIScene_FullscreenProgress);
}

void PcStartWorldTick()
{
    const PcLaunchOptions& options = PcGetLaunchOptions();
    if (options.world.empty() && options.join.empty())
        return;
    const std::string& target = options.world.empty() ? options.join : options.world;

    switch (sState)
    {
    case StartWorldState_Idle:
        Enter(StartWorldState_WaitForMenu);
        break;
    case StartWorldState_WaitForMenu:
        if (++sMenuFrames < kMenuSettleFrames)
            break;
        if (gConsoleUIController.IsSceneInStack(gProfile->GetPrimaryPad(), EUIScene_SaveMessage))
        {
            gConsoleUIController.NavigateToHomeMenu(true);
            sMenuFrames = 0;
            break;
        }
        if (!gConsoleUIController.IsSceneInStack(gProfile->GetPrimaryPad(), EUIScene_MainMenu))
        {
            if (NowSeconds() - sStateStart > kTimeoutSeconds)
                Fail("The main menu never appeared", target);
            break;
        }
        gProfile->SetPrimaryPad(g_pInput->LastInputPad());
        {
            std::string error;
            if (!options.world.empty() && !HostWorld(options, error))
            {
                Fail("The world could not be started", error);
                break;
            }
            if (options.world.empty() && CGameNetworkManager::sInstance.JoinServerAddress(options.join) != 0)
            {
                Fail("Could not join", options.join);
                break;
            }
        }
        if (options.world.empty())
            LOG_INFO(sLog, "Joining %s", target.c_str());
        Enter(StartWorldState_Loading);
        break;
    case StartWorldState_Loading:
        if (PcInWorld())
        {
            Minecraft* minecraft = Minecraft::GetInstance();
            LOG_INFO(sLog, "In %s at %.2f,%.2f,%.2f facing %.1f,%.1f", target.c_str(), minecraft->mPlayer->mX,
                     minecraft->mPlayer->mY, minecraft->mPlayer->mZ, minecraft->mPlayer->mYRot,
                     minecraft->mPlayer->mXRot);
            Enter(StartWorldState_Done);
        }
        else if (NowSeconds() - sStateStart > kTimeoutSeconds)
        {
            Fail("The world did not finish loading", target);
        }
        break;
    case StartWorldState_Done:
        break;
    }
}
