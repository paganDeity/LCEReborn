#include <SDL3/SDL.h>

#include <chrono>
#include <cstdio>
#include <string>

#include "game/world/light/LightEngine.hpp"
#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/ConsoleUIController.hpp"
#include "game/core/GameSettings.hpp"
#include "game/core/L10N.hpp"
#include "game/core/InfoBarManager.hpp"
#include "game/core/IntCache.hpp"
#include "game/core/Minecraft.hpp"
#include "game/core/ShutdownManager.hpp"
#include "game/core/VideoSettings.hpp"
#include "game/core/StatsCounter.hpp"
#include "game/client/render/Tesselator.hpp"
#include "game/world/block/Block.hpp"
#include "game/core/Compression.hpp"
#include "game/core/PIXNamedEvents.hpp"
#include "game/data/ElementStorage.hpp"
#include "game/network/CGameNetworkManager.hpp"
#include "game/network/PackTransfer.hpp"
#include "game/network/PacketCodec.hpp"
#include "game/util/AABB.hpp"
#include "game/util/Vec3.hpp"
#include "game/util/phys__Vec2.hpp"
#include "game/world/Level.hpp"
#include "game/world/chunk/LevelChunk.hpp"
#include "game/world/chunk/OldChunkStorage.hpp"
#include "game/item/FireworksRecipe.hpp"
#include "game/client/render/Textures.hpp"
#include "game/client/audio/SoundEngine.hpp"
#include "platform/pc/shaderpack/ShaderPackManager.hpp"
#include "game/core/LeaderboardManager.hpp"
#include "game/client/render/Renderer.hpp"
#include "game/core/CriticalSection.hpp"
#include "platform/C4JThreadImpl.hpp"
#include "platform/CInput.hpp"
#include "platform/pc/PcPaths.hpp"
#include "platform/pc/PcServerAccess.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerWatchdog.hpp"
#include "platform/CProfile.hpp"
#include "platform/CStorage.hpp"
#include "platform/FileApi.hpp"
#include "platform/pc/PcApplication.hpp"
#include "platform/pc/PcCrashReport.hpp"
#include "platform/pc/PcDebugServer.hpp"
#include "platform/pc/PcDiscord.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcLaunchOptions.hpp"
#include "platform/pc/PcModHost.hpp"
#include "game/core/PlayerIdentities.hpp"
#include "platform/pc/PcP2pStart.hpp"
#include "platform/pc/PcPlayerIdentities.hpp"
#include "platform/pc/PcFrameCapture.hpp"
#include "platform/pc/PcBedrockUI.hpp"
#include "platform/pc/PcStartWorld.hpp"
#include "platform/pc/PcWindow.hpp"

extern CConsoleMinecraftApp gApp;

void DefineActions();
void RegisterAwardsWithProfileManager();
void ShutdownAndExit();

static const Logger sLog("Main");

// the game's own server is watched as the dedicated one is, but a player can see a frozen world and close the game, so here a stall is
// only written to the log, once each time it happens
static void WarnOfStalledServer()
{
    static bool warned = false;
    const long long nowMs = static_cast<long long>(MinecraftServer::getCurrentTimeMillis());
    const bool stalled = ServerWatchdog::Stalled(nowMs);
    if (stalled && !warned)
        LOG_WARN(sLog, "The game's server has not ticked for %d seconds; it is stuck", ServerWatchdog::StalledSeconds(nowMs));
    warned = stalled;
}

int main(int argc, char** argv)
{
    int exitCode;
    if (!PcParseLaunchOptions(argc, argv, exitCode, PcProgram::Game))
        return exitCode;

    C4JThreadImpl::SetMainThreadID();
    const std::string& logDir = PcGetLaunchOptions().logDir;
    Log::Start(logDir.empty() ? std::string("logs") : logDir, PcGetLaunchOptions().logKeepDays);
    PcCrashReport::Install(logDir.empty() ? std::string("logs") : logDir);

    std::string folderError;
    if (!PcCheckGameFolders(folderError))
    {
        sLog.WriteText(LogLevel::Fatal, folderError);
        Log::Stop();
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "LCEReborn", folderError.c_str(), NULL);
        return 1;
    }
    int protocol = ConnectionProtocols::eProtocol_Legacy;
    const std::string& spoken = PcGetLaunchOptions().protocol;
    if (!spoken.empty()
        && (!ConnectionProtocols::Parse(spoken, &protocol)
            || (protocol != ConnectionProtocols::eProtocol_Legacy && protocol != ConnectionProtocols::eProtocol_Reborn)))
    {
        std::fprintf(stderr, "--protocol must be legacy or reborn, not %s\n", spoken.c_str());
        Log::Stop();
        return 1;
    }
    ConnectionProtocols::SetClientSpeaks(protocol);
    PcStartIdentities("game");
    PcServerAccessStart();
    PcStartMods("game");
    LOG_INFO(sLog, "Content: %s", PcContentRoot().c_str());
    LOG_INFO(sLog, "Saves: %s", PcSaveRoot().c_str());
    LOG_INFO(sLog, "Resources: %s", PcResourceRoot().c_str());
    const std::string& discordApp = PcGetLaunchOptions().discordApp;
    PcDiscordStart(discordApp.empty() ? std::string(LCE_DISCORD_APP_ID) : discordApp);

    ShutdownManager::Initialise();
    if (gProfile == 0)
        gProfile = new CProfile();
    Compression::CreateNewThreadStorage();
    gRenderer->Initialise();
    PcBedrockUI::Initialise();
    g_pInput->Initialise(1, EControllerActions_Count, 0x17);
    VideoSettings::Load(PcSaveRoot() + "/video.txt");

    DefineActions();
    g_pInput->SetJoypadMapArraySetupComplete();
    g_pInput->SetKeyRepeatRate(0.3f, 0.2f);
    g_pInput->SetDeadzoneAndMovementRange(0, 20000, 0x7fff);

    gApp.loadMediaArchive();
    if (gApp.ReadProductCodes() == 0)
        gApp.FatalLoadError(L"ReadProductCodes");

    switch (gApp.GetProductSKU())
    {
    case 0:
        gProfile->SetMinimumAge(7);
        break;
    case 1:
        gProfile->SetMinimumAge(10);
        break;
    case 2:
        gProfile->SetMinimumAge(0);
        break;
    }
    L10N::loadStringTable();
    g_pInput->SetCircleCrossSwapped(true);

    int uiWidth;
    int uiHeight;
    if (gRenderer->IsWidescreen())
    {
        uiWidth = 1280;
        uiHeight = 720;
    }
    else
    {
        uiWidth = 640;
        uiHeight = 480;
    }
    gConsoleUIController.init(uiWidth, uiHeight);
#if LCE_DEBUG_BUILD
    if (PcGetLaunchOptions().skipIntro)
        gConsoleUIController.SetSkipStartupScreens(true);
#endif
    gApp.InitTime();
    static unsigned int s_profileDefaults[5];
    gProfile->Initialise(L"", L"", 0x22, 5, 4, s_profileDefaults, 0x1e60, &gApp.mGameDefinedDataChangedBitmask);
    gProfile->SetSignInChoicesCallback(CConsoleMinecraftApp::RequestSignInUIChoices);
    gProfile->SetOnAwardHandler(PopupToast, 0);
    RegisterAwardsWithProfileManager();

    bool noSpaceForSaveQuota;
    gStorage->Init(0, L10N::GetString(0xc0b26be6), "savegame.dat", 51000000,
                   CConsoleMinecraftApp::DisplaySavingMessageWiiU, &gApp, "", &noSpaceForSaveQuota);
    gApp.SetNoSpaceForSaveQuota(noSpaceForSaveQuota);
    gProfile->SetSignInChangeCallback(
        [](void* context, bool signedIn, unsigned int users) { CMinecraftApp::SignInChangeCallback(context, signedIn, users); },
        &gApp);
    gProfile->SetProfileReadErrorCallback(CMinecraftApp::ProfileReadErrorCallback, &gApp);

    gStorage->InitialiseGenericData();
    gStorage->InitialiseProfileData(0x22, 5, 4, s_profileDefaults, 0x1e60, &gApp.mGameDefinedDataChangedBitmask);
    gStorage->SetDefaultOptionsCallback(CMinecraftApp::DefaultOptionsCallback, &gApp);
    gStorage->SetOptionsDataCallback(CMinecraftApp::OptionsDataCallback, &gApp);
    gStorage->SetOldProfileVersionCallback(GameSettings::OldProfileVersionCallback, &gApp);
    gStorage->SetGameSaveFolderPrefix("");
    gStorage->SetMaxSaves(99);
    arrayWithLength<unsigned char> thumbnail = gApp.getArchiveFile(L"DefaultSaveThumbnail64x64.png", false);
    gStorage->SetDefaultImages(0, 0, 0, 0, thumbnail.data, thumbnail.length);
    gStorage->SetIncompleteSaveCallback(CConsoleMinecraftApp::Callback_SaveGameIncomplete, &gApp);
    if (thumbnail.data != 0)
        delete[] thumbnail.data;
    gStorage->SetSaveTitle(L"Default Save");
    gStorage->SetOpenDirFn(OpenDir);
    gStorage->SetReadDirFn(ReadDir);
    gStorage->SetCloseDirFn(CloseDir);
    gStorage->SetDLCPackageRoot("DLCDrive");
    PackReceiver::TidyCache();

    Tesselator::CreateNewThreadStorage(0x100000);
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

    Minecraft::main();
    Minecraft* minecraft = Minecraft::GetInstance();
    gApp.InitGameSettings();
    gStorage->ReadFromProfile(0, 0);
    const std::string& uiMode = PcGetLaunchOptions().uiMode;
    if (!uiMode.empty())
    {
        if (uiMode != "legacy" && uiMode != "bedrock")
        {
            std::fprintf(stderr, "--ui-mode must be legacy or bedrock, not %s\n", uiMode.c_str());
            return 1;
        }
        const unsigned int mode = uiMode == "bedrock" ? eUIMode_Bedrock : eUIMode_Legacy;
        GameSettings::SetUIModeOverride(mode);
        LOG_INFO(sLog, "Interface mode %s for this run", uiMode.c_str());
    }
#if LCE_DEBUG_BUILD
    const std::string& hints = PcGetLaunchOptions().hints;
    if (!hints.empty())
    {
        if (hints != "on" && hints != "off")
        {
            std::fprintf(stderr, "--hints must be on or off, not %s\n", hints.c_str());
            return 1;
        }
        GameSettings::SetHintsOverride(hints == "on");
        LOG_INFO(sLog, "Hints %s for this run", hints.c_str());
    }
#endif
    gApp.InitialiseTips();
    g_pLeaderboardManager->OpenSession();
    CGameNetworkManager::sInstance.Initialise();
    CGameNetworkManager::sInstance.SetLocalGame(true);
    PcP2pStart();
    PlayerIdentities::Get().ApplyActiveSkin(0);
    ShaderPackManager::Get().Initialise();

    float timingColour = 0.0f;
#if LCE_DEBUG_BUILD
    if (PcGetLaunchOptions().frameRate > 0)
        PcWindowSetFrameRate(PcGetLaunchOptions().frameRate);
    if (PcGetLaunchOptions().debugPort > 0)
    {
        PcDebugStart(static_cast<unsigned short>(PcGetLaunchOptions().debugPort),
                     PcGetLaunchOptions().debugIdleQuit);
    }
#endif
    bool exitSaveRequested = false;
    bool exitSaveRunning = false;
    double exitSaveStart = 0.0;
    for (;;)
    {
        PcPumpEvents();
        InfoBarManager::StartTimingCategory(InfoBarManager::eTimingCagegory_Frame);
        gRenderer->StartFrame();
        gApp.UpdateTime();
        PIXBeginNamedEvent(timingColour, "Input manager tick");
        g_pInput->Tick();
        PIXEndNamedEvent();
        PIXBeginNamedEvent(timingColour, "Profile manager tick");
        gProfile->Tick();
        PIXEndNamedEvent();
        PIXBeginNamedEvent(timingColour, "Storage manager tick");
        gStorage->Tick();
        PIXEndNamedEvent();
        WarnOfStalledServer();
        PIXBeginNamedEvent(timingColour, "Render manager tick");
        gRenderer->Tick();
        PIXEndNamedEvent();
        PIXBeginNamedEvent(timingColour, "Network manager do work #1");
        CGameNetworkManager::sInstance.DoWork();
        PIXEndNamedEvent();
        g_pLeaderboardManager->Tick();

        if (gApp.GetGameStarted() != 0)
        {
            minecraft->run_middle();
            gApp.SetAppPaused(CGameNetworkManager::sInstance.IsLocalGame()
                              && CGameNetworkManager::sInstance.GetPlayerCount() == 1
                              && gConsoleUIController.IsPauseMenuDisplayed(gProfile->GetPrimaryPad()));
        }
        else
        {
            minecraft->mSoundEngine->tick(NULL, 0.0f);
            minecraft->mTextures->tick(true, false, false);
            IntCache::Reset();
            if (gApp.GetReallyChangingSessionType() != 0)
                minecraft->tickAllConnections();
        }

        minecraft->mSoundEngine->playMusicTick();
        InfoBarManager::StartTimingCategory(InfoBarManager::eTimingCagegory_UITick);
        gConsoleUIController.tick();
        InfoBarManager::EndTimingCategory(InfoBarManager::eTimingCagegory_UITick);
        gRenderer->ResetRenderState();
        InfoBarManager::StartTimingCategory(InfoBarManager::eTimingCagegory_UIRender);
        gRenderer->SetLayout(Renderer::ELayout_Interface);
        gConsoleUIController.render();
        gRenderer->SetLayout(Renderer::ELayout_Screen);
        InfoBarManager::EndTimingCategory(InfoBarManager::eTimingCagegory_UIRender);
        InfoBarManager::EndTimingCategory(InfoBarManager::eTimingCagegory_Frame);
        PIXBeginNamedEvent(timingColour, "Frame present");
        gRenderer->Present();
        PIXEndNamedEvent();
        PcWindowPaceFrame();
        gRenderer->Set_matrixDirty();
        gConsoleUIController.CheckMenuDisplayed();
        PIXBeginNamedEvent(timingColour, "Profile load check");
        if (gApp.mGameDefinedDataChangedBitmask != 0)
        {
            for (int player = 0; player < 4; ++player)
            {
                if ((gApp.mGameDefinedDataChangedBitmask & (1u << player)) != 0)
                {
                    unsigned char* data = gStorage->GetGameDefinedProfileData(player);
                    GameSettings::ClearGameSettingsChangedFlag(player);
                    GameSettings::ApplyGameSettingsChanged(player);
                    minecraft->mStats[player]->clear();
                    minecraft->mStats[player]->parse(data);
                }
            }
            gApp.mGameDefinedDataChangedBitmask = 0;
        }
        PIXEndNamedEvent();
        PIXBeginNamedEvent(timingColour, "Network manager do work #2");
        CGameNetworkManager::sInstance.DoWork();
        PIXEndNamedEvent();
        gApp.HandleXuiActions();
        PcStartWorldTick();
#if LCE_DEBUG_BUILD
        PcFrameCaptureTick();
        PcDebugTick();
#endif
        Vec3::resetPool();

        if (!PcExitRequested())
            continue;
        if (!exitSaveRequested && PcInWorld() && CGameNetworkManager::sInstance.IsHost())
        {
            gApp.SetAction(gProfile->GetPrimaryPad(), eXuiAction_SaveGame, NULL);
            exitSaveRequested = true;
            exitSaveStart = std::chrono::duration<double>(
                                std::chrono::steady_clock::now().time_since_epoch())
                                .count();
        }
        if (exitSaveRequested)
        {
            const int pad = gProfile->GetPrimaryPad();
            const int action = gApp.GetXuiServerAction(pad);
            if (action != eXuiServerAction_Idle)
                exitSaveRunning = true;
            const double now =
                std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
            const double limit = exitSaveRunning ? 120.0 : 10.0;
            if ((exitSaveRunning && action == eXuiServerAction_Idle) || now - exitSaveStart > limit)
            {
                if (now - exitSaveStart > limit)
                    LOG_WARN(sLog, "The world did not finish saving before exit");
                break;
            }
            continue;
        }
        break;
    }

    ShutdownAndExit();
    return 0;
}
