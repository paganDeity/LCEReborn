#include "platform/pc/PcDebugServer.hpp"

#if LCE_DEBUG_BUILD

#include <SDL3/SDL.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "game/command/ChatCommands.hpp"
#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/ChunkStats.hpp"
#include "game/core/ConsoleUIController.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/NbtPath.hpp"
#include "game/data/NbtSelection.hpp"
#include "game/data/StringNbtWriter.hpp"
#include "game/core/GameSettings.hpp"
#include "game/core/ModHost.hpp"
#include "game/core/InfoBarManager.hpp"
#include "game/core/Minecraft.hpp"
#include "game/core/PlayerIdentities.hpp"
#include "net/NetP2p.hpp"
#include "game/ui/UIGroup.hpp"
#include "game/ui/UIScene.hpp"
#include "game/ui/UIScene_SettingsMenu.hpp"
#include "game/network/CGameNetworkManager.hpp"
#include "game/core/DataOutputStream.hpp"
#include "game/network/Packet.hpp"
#include "game/network/ClientPacketListener.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerChunkCache.hpp"
#include "game/server/ServerControl.hpp"
#include "game/server/ServerLevel.hpp"
#include "game/world/Dimension.hpp"
#include "game/world/DimensionType.hpp"
#include "game/world/Level.hpp"
#include "game/world/LevelData.hpp"
#include "game/world/generation/MultiplayerLocalPlayer.hpp"
#include "net/NetSocket.hpp"
#include "platform/pc/Log.hpp"
#include "platform/CProfile.hpp"
#include "platform/DebugGpuTiming.hpp"
#include "platform/pc/CInput.hpp"
#include "platform/pc/PcAgentChatCommand.hpp"
#include "platform/pc/PcAgentTasks.hpp"
#include "platform/pc/PcApplication.hpp"
#include "platform/pc/PcFrameCapture.hpp"
#include "platform/pc/PcJson.hpp"
#include "platform/pc/PcStartWorld.hpp"
#include "platform/pc/PcSystemInfo.hpp"
#include "platform/pc/PcUtf8.hpp"
#include "platform/pc/PcWindow.hpp"

extern ConsoleUIController gConsoleUIController;

extern CConsoleMinecraftApp gApp;

namespace
{
const Logger sLog("Debug");
const size_t kMaxConsoleLines = 1024;
const size_t kMaxRequestSize = 65536;

struct ConsoleLine
{
    unsigned long long seq;
    std::string quoted;
};

struct PendingCapture
{
    PendingCapture() : active(false), id("null"), path() {}

    bool active;
    std::string id;
    std::string path;
};

struct QueuedCommand
{
    std::wstring line;
    bool barrier;
};

struct PendingCommand
{
    PendingCommand() : active(false), id("null"), since(0), deadline(0.0) {}

    bool active;
    std::string id;
    unsigned long long since;
    double deadline;
};

std::mutex sConsoleMutex;
std::deque<ConsoleLine> sConsoleLines;
unsigned long long sConsoleNext = 1;

std::mutex sCommandMutex;
std::deque<QueuedCommand> sCommands;
bool sBarrierReached = false;

lcenet::TcpSocket sListener;
lcenet::TcpSocket sClient;
std::string sReceived;
PendingCapture sCapture;
PendingCommand sCommand;

int sIdleQuitSeconds = 0;
double sLastRequest = 0.0;
double sLastTick = 0.0;
double sFrameWindowStart = 0.0;
int sFramesInWindow = 0;
double sFrameRate = 0.0;


enum EWaitKind
{
    EWait_None,
    EWait_Frames,
    EWait_Ticks,
    EWait_Steps
};

struct PendingWait
{
    PendingWait() : kind(EWait_None), id("null"), target(0), deadline(0.0) {}

    EWaitKind kind;
    std::string id;
    long long target;
    double deadline;
};

PendingWait sWait;

SDL_Keymod sHeldModifiers = SDL_KMOD_NONE;
// pushed motion events do not move the mouse SDL reports, so a click goes where the last pointer request put it
bool sPointerSet = false;
float sPointerX = 0.0f;
float sPointerY = 0.0f;
const int kTypedSlots = 16;
std::string sTyped[kTypedSlots];
int sNextTyped = 0;
long long sFramesDrawn = 0;
const double kWaitSecondsPerUnit = 2.0;
const double kWaitMinimumSeconds = 10.0;
const size_t kMaxRequestLength = 400;
const double kStoppedGapSeconds = 2.0;

double Now()
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

std::string Number(long long value)
{
    char text[32];
    std::snprintf(text, sizeof(text), "%lld", value);
    return text;
}

std::string Number(double value)
{
    char text[64];
    std::snprintf(text, sizeof(text), "%.6g", value);
    return text;
}

std::string Bool(bool value)
{
    return value ? "true" : "false";
}

std::string Quote(const std::wstring& text)
{
    return PcJsonQuote(text);
}

std::string Quote(const std::string& text)
{
    std::string out = "\"";
    for (size_t index = 0; index < text.size(); ++index)
    {
        const char character = text[index];
        if (character == '"' || character == '\\')
        {
            out += '\\';
            out += character;
        }
        else if ((unsigned char)character < 0x20)
        {
            char escaped[8];
            std::snprintf(escaped, sizeof(escaped), "\\u%04x", (unsigned int)(unsigned char)character);
            out += escaped;
        }
        else
        {
            out += character;
        }
    }
    return out + "\"";
}

std::string IdNumber(double value)
{
    char text[64];
    std::snprintf(text, sizeof(text), "%.0f", value);
    return text;
}

const JsonValue* Member(const JsonValue& object, const wchar_t* name)
{
    std::map<std::wstring, JsonValue>::const_iterator found = object.members.find(name);
    return found == object.members.end() ? NULL : &found->second;
}

bool MemberText(const JsonValue& object, const wchar_t* name, std::wstring& value)
{
    const JsonValue* member = Member(object, name);
    if (member == NULL || member->type != JsonValue::String)
        return false;
    value = member->text;
    return true;
}

bool MemberNumber(const JsonValue& object, const wchar_t* name, double& value)
{
    const JsonValue* member = Member(object, name);
    if (member == NULL || member->type != JsonValue::Number)
        return false;
    value = member->number;
    return true;
}

// a packet whose number and bytes the harness chose, sent on the client's own connection, so a test can send a
// server what a hostile client would without a second implementation of the protocol
class RawPacket : public Packet
{
public:
    RawPacket(int id, std::vector<unsigned char> bytes) : mId(id), mBytes(std::move(bytes)) {}

    int getPacketId() override { return mId; }
    void read(DataInputStream*) override {}
    void write(DataOutputStream* output) override
    {
        for (unsigned char byte : mBytes)
            output->writeByte(byte);
    }
    int getEstimatedSize() override { return static_cast<int>(mBytes.size()); }

private:
    int mId;
    std::vector<unsigned char> mBytes;
};

// hex digits, two to a byte; false on anything else
bool ParseHex(const std::wstring& text, std::vector<unsigned char>& bytes)
{
    bytes.clear();
    if (text.size() % 2 != 0)
        return false;
    for (size_t index = 0; index < text.size(); index += 2)
    {
        unsigned int value = 0;
        for (size_t digit = index; digit < index + 2; ++digit)
        {
            const wchar_t character = text[digit];
            value <<= 4;
            if (character >= L'0' && character <= L'9')
                value |= character - L'0';
            else if (character >= L'a' && character <= L'f')
                value |= character - L'a' + 10;
            else if (character >= L'A' && character <= L'F')
                value |= character - L'A' + 10;
            else
                return false;
        }
        bytes.push_back(static_cast<unsigned char>(value));
    }
    return true;
}

std::string IdText(const JsonValue& request)
{
    const JsonValue* id = Member(request, L"id");
    if (id == NULL)
        return "null";
    if (id->type == JsonValue::Number)
        return IdNumber(id->number);
    if (id->type == JsonValue::String)
        return Quote(id->text);
    return "null";
}

void SendLine(const std::string& line)
{
    if (!sClient.IsOpen())
        return;
    std::string text = line;
    text += '\n';
    if (!sClient.SendAll(text.data(), text.size()))
        sClient.Close();
}

void SendResult(const std::string& id, const std::string& result)
{
    SendLine("{\"id\":" + id + ",\"ok\":true,\"result\":" + result + "}");
}

void SendError(const std::string& id, const std::string& message)
{
    SendLine("{\"id\":" + id + ",\"ok\":false,\"error\":" + Quote(message) + "}");
}

void ConsoleOutput(const std::wstring& text)
{
    ConsoleLine line;
    line.quoted = PcJsonQuote(text);
    std::lock_guard<std::mutex> lock(sConsoleMutex);
    line.seq = sConsoleNext++;
    sConsoleLines.push_back(line);
    while (sConsoleLines.size() > kMaxConsoleLines)
        sConsoleLines.pop_front();
}

bool NextCommand(std::wstring& line)
{
    std::lock_guard<std::mutex> lock(sCommandMutex);
    if (sCommands.empty())
        return false;
    if (sCommands.front().barrier)
    {
        sCommands.pop_front();
        sBarrierReached = true;
        return false;
    }
    line = sCommands.front().line;
    sCommands.pop_front();
    return true;
}

std::string ConsoleLines(unsigned long long since)
{
    std::string lines = "[";
    bool first = true;
    std::lock_guard<std::mutex> lock(sConsoleMutex);
    for (size_t index = 0; index < sConsoleLines.size(); ++index)
    {
        const ConsoleLine& line = sConsoleLines[index];
        if (line.seq < since)
            continue;
        if (!first)
            lines += ",";
        lines += "{\"seq\":" + Number((long long)line.seq) + ",\"text\":" + line.quoted + "}";
        first = false;
    }
    lines += "]";
    return lines;
}

std::string TaskList()
{
    const std::vector<PcAgentTask> tasks = PcAgentTaskList();
    std::string list = "[";
    for (size_t index = 0; index < tasks.size(); ++index)
    {
        const PcAgentTask& task = tasks[index];
        if (index > 0)
            list += ",";
        list += "{\"id\":" + Number((long long)task.id) + ",\"text\":" + Quote(task.text) + ",\"options\":[";
        for (size_t option = 0; option < task.options.size(); ++option)
            list += (option > 0 ? "," : "") + Quote(task.options[option]);
        list += "],\"answer\":" + (task.answered ? Quote(task.answer) : std::string("null")) + "}";
    }
    return list + "]";
}

long long WaitPosition(EWaitKind kind)
{
    if (kind == EWait_Frames)
        return sFramesDrawn;
    if (gMinecraftServer == NULL)
        return -1;
    if (kind == EWait_Ticks)
        return gMinecraftServer->getTickCount();
    return MinecraftServer::TicksStepped();
}

void CheckWait()
{
    const long long position = WaitPosition(sWait.kind);
    const std::string id = sWait.id;
    if (position < 0)
    {
        sWait.kind = EWait_None;
        SendError(id, "the hosted world closed while waiting");
        return;
    }
    if (position >= sWait.target)
    {
        sWait.kind = EWait_None;
        SendResult(id, "{\"reached\":" + Number(position) + "}");
        return;
    }
    if (Now() > sWait.deadline)
    {
        sWait.kind = EWait_None;
        SendError(id, "the game did not get there in time; it is at " + Number(position) + " of "
                          + Number(sWait.target));
    }
}

std::string ProfileResult()
{
    static const char* const kSectionNames[InfoBarManager::eTimingCagegory_Count] = {
        "frame", "terrain", "terrainTransparent", "entities", "blockEntities", "particles", "uiTick", "uiRender"};
    std::string result = "{\"sectionsMs\":{";
    for (int section = 0; section < InfoBarManager::eTimingCagegory_Count; ++section)
    {
        if (section > 0)
            result += ",";
        result += Quote(std::string(kSectionNames[section])) + ":"
                  + Number((double)InfoBarManager::AverageMilliseconds(
                      static_cast<InfoBarManager::eTimingCagegory>(section)));
    }
    result += "},\"gpuSectionsMs\":{";
    for (int section = 0; section < InfoBarManager::eTimingCagegory_Count; ++section)
    {
        float milliseconds = 0.0f;
        if (section > 0)
            result += ",";
        result += Quote(std::string(kSectionNames[section])) + ":"
                  + (DebugGpuTiming::AverageMilliseconds(section, &milliseconds) ? Number((double)milliseconds)
                                                                                  : std::string("null"));
    }
    result += "},\"serverTickMs\":" + Number((double)MinecraftServer::sAverageTickMilliseconds);
    unsigned long long used = 0;
    unsigned long long total = 0;
    result += ",\"residentBytes\":" + (PcMemoryUsage(&used, &total) ? Number((long long)used) : std::string("null"));
    return result + "}";
}

int MouseButtonOf(const std::string& name)
{
    if (name == "MouseLeft")
        return SDL_BUTTON_LEFT;
    if (name == "MouseMiddle")
        return SDL_BUTTON_MIDDLE;
    if (name == "MouseRight")
        return SDL_BUTTON_RIGHT;
    return 0;
}

SDL_Keymod ModifierOf(SDL_Scancode scancode)
{
    switch (scancode)
    {
    case SDL_SCANCODE_LCTRL:
        return SDL_KMOD_LCTRL;
    case SDL_SCANCODE_RCTRL:
        return SDL_KMOD_RCTRL;
    case SDL_SCANCODE_LSHIFT:
        return SDL_KMOD_LSHIFT;
    case SDL_SCANCODE_RSHIFT:
        return SDL_KMOD_RSHIFT;
    case SDL_SCANCODE_LALT:
        return SDL_KMOD_LALT;
    case SDL_SCANCODE_RALT:
        return SDL_KMOD_RALT;
    default:
        return SDL_KMOD_NONE;
    }
}

std::string SceneName(UIScene* scene)
{
    const std::wstring movie = scene->getMoviePath();
    if (!movie.empty())
        return std::string(movie.begin(), movie.end());
    return "scene type " + Number((long long)scene->getSceneType());
}

std::string SceneList()
{
    static const char* const kLayerNames[EUILayer_Count] = {"debug", "tooltips", "alert",  "popup",
                                                            "fullscreen", "tutorial", "scene", "hud"};
    const int pad = gProfile->GetPrimaryPad();
    EUIGroup groups[2] = {EUIGroup_Fullscreen, EUIGroup_Fullscreen};
    int groupCount = 1;
    if (gApp.GetGameStarted())
    {
        groups[1] = gConsoleUIController.GetGroup(pad);
        groupCount = groups[1] == EUIGroup_Fullscreen ? 1 : 2;
    }
    std::string list = "[";
    for (int group = 0; group < groupCount; ++group)
    {
        for (int layer = 0; layer < EUILayer_Count; ++layer)
        {
            UIScene* scene = gConsoleUIController.GetTopScene(pad, static_cast<EUILayer>(layer), groups[group]);
            if (scene == NULL)
                continue;
            if (list.size() > 1)
                list += ",";
            list += "{\"group\":" + Quote(std::string(groups[group] == EUIGroup_Fullscreen ? "fullscreen" : "player"))
                    + ",\"layer\":" + Quote(std::string(kLayerNames[layer])) + ",\"name\":"
                    + Quote(SceneName(scene)) + "}";
        }
    }
    return list + "]";
}

std::string StateResult()
{
    Minecraft* minecraft = Minecraft::GetInstance();
    const bool inWorld = PcInWorld();
    std::string result = "{";
    result += "\"inWorld\":" + Bool(inWorld);
    result += ",\"eula\":" + Bool(gConsoleUIController.IsSceneInStack(0, EUIScene_EULA));
    result += ",\"uiMode\":" + Number((long long)GameSettings::UIMode(gProfile->GetPrimaryPad()));
    result += ",\"menuDisplayed\":" + Bool(gConsoleUIController.GetMenuDisplayed(gProfile->GetPrimaryPad()));
    result += ",\"pauseMenu\":"
              + Bool(gConsoleUIController.IsPauseMenuDisplayed(gProfile->GetPrimaryPad()));
    result += ",\"ticks\":" + Number((long long)minecraft->mTicks);
    result += ",\"frameRate\":" + Number(sFrameRate);
    result += ",\"scenes\":" + SceneList();
    result += ",\"tasks\":" + TaskList();
    result += ",\"profile\":" + ProfileResult();
    result += ",\"ticksHeld\":" + Bool(MinecraftServer::TicksHeld());
    if (gMinecraftServer != NULL)
        result += ",\"serverTicks\":" + Number((long long)gMinecraftServer->getTickCount());
    if (inWorld && minecraft->mPlayer)
    {
        MultiplayerLocalPlayer* player = minecraft->mPlayer.get();
        result += ",\"player\":{";
        result += "\"x\":" + Number(player->mX);
        result += ",\"y\":" + Number(player->mY);
        result += ",\"z\":" + Number(player->mZ);
        result += ",\"yaw\":" + Number((double)player->mYRot);
        result += ",\"pitch\":" + Number((double)player->mXRot);
        result += ",\"dimension\":" + Quote(player->mLevel->mDimension->getType()->getName());
        result += "}";
    }
    if (minecraft->mLevel != NULL)
    {
        Level* level = minecraft->mLevel;
        std::wstring name = level->getLevelData()->getLevelName();
        if (name.empty())
            name = minecraft->mLevelName.empty() ? minecraft->mLevelDirName : minecraft->mLevelName;
        result += ",\"level\":{";
        result += "\"name\":" + Quote(name);
        result += ",\"dir\":" + Quote(minecraft->mLevelDirName);
        result += ",\"seed\":" + Quote(Number(level->getSeed()));
        result += ",\"dayTime\":" + Number(level->getDayTime());
        result += ",\"raining\":" + Bool(level->isRaining());
        result += ",\"thundering\":" + Bool(level->isThundering());
        result += ",\"host\":" + Bool(CGameNetworkManager::sInstance.IsHost());
        if (gMinecraftServer != NULL)
            result += ",\"nbtCapabilities\":"
                      + Number((long long)gMinecraftServer->getLevel(0)->getLevelData()->getNbtCapabilities());
        result += "}";
    }
    result += "}";
    return result;
}

std::string ConsoleResult(double since)
{
    const unsigned long long start = since <= 0.0 ? 0 : (unsigned long long)since;
    unsigned long long next = start;
    {
        std::lock_guard<std::mutex> lock(sConsoleMutex);
        if (!sConsoleLines.empty() && sConsoleLines.back().seq >= next)
            next = sConsoleLines.back().seq + 1;
    }
    return "{\"lines\":" + ConsoleLines(start) + ",\"next\":" + Number((long long)next) + "}";
}

void HandleRequest(const std::string& text)
{
    JsonValue request;
    std::string id = "null";
    if (!PcJsonParse(text, request) || request.type != JsonValue::Object)
    {
        SendError(id, "the request is not a JSON object");
        return;
    }
    id = IdText(request);

    std::wstring command;
    if (!MemberText(request, L"cmd", command))
    {
        SendError(id, "the request has no cmd");
        return;
    }

    if (command == L"state")
    {
        SendResult(id, StateResult());
        return;
    }
    if (command == L"console")
    {
        double since = 0.0;
        MemberNumber(request, L"since", since);
        SendResult(id, ConsoleResult(since));
        return;
    }
    if (command == L"command")
    {
        std::wstring line;
        if (!MemberText(request, L"line", line) || line.empty())
        {
            SendError(id, "command needs a line");
            return;
        }
        if (!PcInWorld() || !CGameNetworkManager::sInstance.IsHost())
        {
            SendError(id, "commands need a hosted world");
            return;
        }
        if (sCommand.active)
        {
            SendError(id, "a command is already pending");
            return;
        }
        unsigned long long since = 0;
        {
            std::lock_guard<std::mutex> lock(sConsoleMutex);
            since = sConsoleNext;
        }
        {
            std::lock_guard<std::mutex> lock(sCommandMutex);
            QueuedCommand entry;
            entry.line = line;
            entry.barrier = false;
            sCommands.push_back(entry);
            entry.line.clear();
            entry.barrier = true;
            sCommands.push_back(entry);
            sBarrierReached = false;
        }
        sCommand.active = true;
        sCommand.id = id;
        sCommand.since = since;
        sCommand.deadline = Now() + 5.0;
        return;
    }
    if (command == L"capture")
    {
        std::wstring path;
        if (!MemberText(request, L"path", path) || path.empty())
        {
            SendError(id, "capture needs a path");
            return;
        }
        if (sCapture.active)
        {
            SendError(id, "a capture is already pending");
            return;
        }
        sCapture.active = true;
        sCapture.id = id;
        sCapture.path.assign(path.begin(), path.end());
        return;
    }
    if (command == L"key")
    {
        std::wstring name;
        double down = 0.0;
        if (!MemberText(request, L"name", name) || name.empty())
        {
            SendError(id, "key needs a name");
            return;
        }
        MemberNumber(request, L"down", down);
        const std::string scancodeName(name.begin(), name.end());
        const int mouseButton = MouseButtonOf(scancodeName);
        SDL_Scancode scancode = SDL_GetScancodeFromName(scancodeName.c_str());
        if (mouseButton == 0 && scancode == SDL_SCANCODE_UNKNOWN)
        {
            SendError(id, "unknown key " + scancodeName);
            return;
        }
        SDL_Window* window = PcWindowHandle();
        if (window == NULL)
        {
            SendError(id, "there is no window");
            return;
        }
        SDL_Event event;
        std::memset(&event, 0, sizeof(event));
        const bool pressed = down != 0.0;
        if (mouseButton != 0)
        {
            float x = sPointerX;
            float y = sPointerY;
            if (!sPointerSet)
                SDL_GetMouseState(&x, &y);
            event.type = pressed ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
            event.button.windowID = SDL_GetWindowID(window);
            event.button.button = static_cast<Uint8>(mouseButton);
            event.button.down = pressed;
            event.button.clicks = 1;
            event.button.x = x;
            event.button.y = y;
            if (!SDL_PushEvent(&event))
            {
                SendError(id, std::string("the button event was not queued: ") + SDL_GetError());
                return;
            }
            SendResult(id, "{\"queued\":true}");
            return;
        }
        if (pressed)
            sHeldModifiers = static_cast<SDL_Keymod>(sHeldModifiers | ModifierOf(scancode));
        else
            sHeldModifiers = static_cast<SDL_Keymod>(sHeldModifiers & ~ModifierOf(scancode));
        event.type = pressed ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
        event.key.windowID = SDL_GetWindowID(window);
        event.key.scancode = scancode;
        event.key.key = SDL_GetKeyFromScancode(scancode, sHeldModifiers, false);
        event.key.mod = sHeldModifiers;
        event.key.repeat = false;
        event.key.down = pressed;
        if (!SDL_PushEvent(&event))
        {
            SendError(id, std::string("the key event was not queued: ") + SDL_GetError());
            return;
        }
        SendResult(id, "{\"queued\":true}");
        return;
    }
    if (command == L"pointer")
    {
        double fractionX = -1.0;
        double fractionY = -1.0;
        MemberNumber(request, L"x", fractionX);
        MemberNumber(request, L"y", fractionY);
        if (fractionX < 0.0 || fractionX > 1.0 || fractionY < 0.0 || fractionY > 1.0)
        {
            SendError(id, "pointer needs x and y from 0 to 1");
            return;
        }
        SDL_Window* window = PcWindowHandle();
        if (window == NULL)
        {
            SendError(id, "there is no window");
            return;
        }
        int width = 0;
        int height = 0;
        SDL_GetWindowSize(window, &width, &height);
        const float x = static_cast<float>(fractionX * width);
        const float y = static_cast<float>(fractionY * height);
        SDL_Event event;
        std::memset(&event, 0, sizeof(event));
        event.type = SDL_EVENT_MOUSE_MOTION;
        event.motion.windowID = SDL_GetWindowID(window);
        event.motion.x = x;
        event.motion.y = y;
        event.motion.xrel = sPointerSet ? x - sPointerX : 0.0f;
        event.motion.yrel = sPointerSet ? y - sPointerY : 0.0f;
        if (!SDL_PushEvent(&event))
        {
            SendError(id, std::string("the motion event was not queued: ") + SDL_GetError());
            return;
        }
        sPointerSet = true;
        sPointerX = x;
        sPointerY = y;
        SendResult(id, "{\"queued\":true}");
        return;
    }
    if (command == L"mods")
    {
        std::wstring action;
        MemberText(request, L"action", action);
        if (action == L"reload")
            ModHost::Get().Reload();
        const std::vector<ModHost::Entry> entries = ModHost::Get().List();
        std::string result = "{\"mods\":[";
        for (size_t index = 0; index < entries.size(); ++index)
        {
            const ModHost::Entry& entry = entries[index];
            std::string depends;
            for (size_t dependency = 0; dependency < entry.depends.size(); ++dependency)
                depends += std::string(dependency != 0 ? "," : "") + Quote(entry.depends[dependency]);
            std::string commands;
            for (size_t command = 0; command < entry.commands.size(); ++command)
                commands += std::string(command != 0 ? "," : "") + Quote(entry.commands[command]);
            result += std::string(index != 0 ? "," : "") + "{\"folder\":" + Quote(entry.folder) + ",\"id\":" + Quote(entry.id)
                      + ",\"name\":" + Quote(entry.name) + ",\"version\":" + Quote(entry.version) + ",\"state\":" + Quote(entry.state)
                      + ",\"error\":" + Quote(entry.error) + ",\"commands\":[" + commands + "],\"depends\":[" + depends + "]}";
        }
        result += "]}";
        SendResult(id, result);
        return;
    }
    if (command == L"save")
    {
        if (!PcInWorld() || !CGameNetworkManager::sInstance.IsHost())
        {
            SendError(id, "save needs a hosted world");
            return;
        }
        ServerControl::RequestSave();
        SendResult(id, "{\"queued\":true}");
        return;
    }
    if (command == L"rawpacket")
    {
        std::wstring hex;
        double count = 1.0;
        MemberText(request, L"hex", hex);
        MemberNumber(request, L"count", count);
        std::vector<unsigned char> bytes;
        if (!ParseHex(hex, bytes) || bytes.empty() || count < 1.0 || count > 100000.0)
        {
            SendError(id, "rawpacket needs hex (the packet number, then its bytes) and a count from 1 to 100000");
            return;
        }
        Minecraft* minecraft = Minecraft::GetInstance();
        ClientPacketListener* connection =
            PcInWorld() && minecraft->mLocalPlayers[0] != NULL ? minecraft->getConnection(0) : NULL;
        if (connection == NULL)
        {
            SendError(id, "rawpacket needs a client connected to a server");
            return;
        }
        const int packetId = bytes[0];
        const std::vector<unsigned char> payload(bytes.begin() + 1, bytes.end());
        const int sends = static_cast<int>(count);
        for (int index = 0; index < sends; ++index)
            connection->send(boost::shared_ptr<Packet>(new RawPacket(packetId, payload)));
        SendResult(id, "{\"queued\":" + std::to_string(sends) + "}");
        return;
    }
    if (command == L"chunkstats")
    {
        double reset = 0.0;
        MemberNumber(request, L"reset", reset);
        std::string result = "{";
        for (int stage = 0; stage < ChunkStats::eStage_Count; ++stage)
        {
            const ChunkStats::Entry entry = ChunkStats::Get(static_cast<ChunkStats::EStage>(stage));
            char text[256];
            std::snprintf(text, sizeof(text),
                          "%s\"%s\":{\"count\":%llu,\"ms\":%.3f,\"slowestMs\":%.3f,\"bytesIn\":%llu,\"bytesOut\":%llu}",
                          stage == 0 ? "" : ",", ChunkStats::Name(static_cast<ChunkStats::EStage>(stage)),
                          static_cast<unsigned long long>(entry.count), entry.nanoseconds / 1.0e6,
                          entry.slowestNanoseconds / 1.0e6, static_cast<unsigned long long>(entry.bytesIn),
                          static_cast<unsigned long long>(entry.bytesOut));
            result += text;
        }
        ChunkStats::Memory memory;
        if (gMinecraftServer != NULL)
        {
            const int dimensions[3] = {0, -1, 1};
            for (int index = 0; index < 3; ++index)
            {
                ServerLevel* level = gMinecraftServer->getLevel(dimensions[index]);
                ServerChunkCache* cache = level != NULL ? dynamic_cast<ServerChunkCache*>(level->getChunkSource()) : NULL;
                if (cache != NULL)
                    cache->addLoadedChunkMemory(memory);
            }
        }
        char text[256];
        std::snprintf(text, sizeof(text), ",\"memory\":{\"chunks\":%llu,\"elementBytes\":%llu,\"lightBytes\":%llu,\"fixedBytes\":%llu}",
                      static_cast<unsigned long long>(memory.chunks), static_cast<unsigned long long>(memory.elementBytes),
                      static_cast<unsigned long long>(memory.lightBytes), static_cast<unsigned long long>(memory.fixedBytes));
        result += text;
        result += "}";
        if (reset != 0.0)
            ChunkStats::Reset();
        SendResult(id, result);
        return;
    }
    if (command == L"p2p")
    {
        lcenet::P2pService& service = lcenet::P2pService::Get();
        std::wstring action, text, nickname;
        MemberText(request, L"action", action);
        MemberText(request, L"text", text);
        MemberText(request, L"nickname", nickname);
        const std::string narrowText(text.begin(), text.end());
        const std::string narrowNickname(nickname.begin(), nickname.end());
        if (!service.Enabled())
        {
            SendError(id, "peer to peer is off; start the client with --p2p-server, --p2p-server-key and --p2p-dir");
            return;
        }
        if (action == L"status")
        {
            SendResult(id, "{\"status\":" + Quote(PcUtf8ToWide(service.Describe()))
                               + ",\"online\":" + Bool(service.Online()) + ",\"code\":"
                               + Quote(PcUtf8ToWide(service.FriendCode())) + "}");
        }
        else if (action == L"add-friend")
        {
            service.AddFriendByCode(narrowText, narrowNickname);
            SendResult(id, "{\"queued\":true}");
        }
        else if (action == L"friends")
        {
            std::vector<lcenet::P2pFriend> friends = service.Friends();
            std::string list = "[";
            for (size_t index = 0; index < friends.size(); ++index)
            {
                if (index > 0)
                    list += ",";
                list += "{\"key\":" + Quote(PcUtf8ToWide(friends[index].key))
                        + ",\"nickname\":"
                        + Quote(PcUtf8ToWide(friends[index].nickname))
                        + ",\"code\":" + Quote(PcUtf8ToWide(friends[index].code))
                        + "}";
            }
            SendResult(id, "{\"friends\":" + list + "]}");
        }
        else if (action == L"lobbies")
        {
            service.RefreshLobbies();
            std::vector<lcenet::P2pLobbyInfo> lobbies = service.Lobbies();
            std::string list = "[";
            for (size_t index = 0; index < lobbies.size(); ++index)
            {
                if (index > 0)
                    list += ",";
                list += "{\"owner\":" + Quote(PcUtf8ToWide(lobbies[index].owner))
                        + ",\"name\":" + Quote(PcUtf8ToWide(lobbies[index].name))
                        + ",\"players\":" + Number((long long)lobbies[index].players) + ",\"max\":"
                        + Number((long long)lobbies[index].maxPlayers) + ",\"protocol\":"
                        + Number((long long)lobbies[index].protocol) + ",\"version\":"
                        + Number((long long)lobbies[index].version) + "}";
            }
            SendResult(id, "{\"lobbies\":" + list + "]}");
        }
        else if (action == L"open")
        {
            SendResult(id, "{\"open\":" + Bool(CGameNetworkManager::sInstance.OpenToNetwork()) + "}");
        }
        else if (action == L"invite")
        {
            service.Invite(narrowText);
            SendResult(id, "{\"queued\":true}");
        }
        else if (action == L"join")
        {
            const int result = CGameNetworkManager::sInstance.JoinServerAddress("p2p:" + narrowText);
            SendResult(id, "{\"result\":" + Number((long long)result) + "}");
        }
        else
        {
            SendError(id, "p2p needs an action: status, add-friend, friends, lobbies, open, invite or join");
        }
        return;
    }
    if (command == L"identity")
    {
        PlayerIdentities& identities = PlayerIdentities::Get();
        std::wstring action, text, other;
        MemberText(request, L"action", action);
        MemberText(request, L"text", text);
        MemberText(request, L"other", other);
        const std::string stem(text.begin(), text.end());
        std::string error;
        if (!identities.Enabled())
        {
            SendError(id, "identities are off");
            return;
        }
        bool ok = true;
        if (action == L"list" || action == L"status")
        {
            std::vector<PlayerIdentityEntry> entries = identities.List();
            std::string list = "[";
            for (size_t index = 0; index < entries.size(); ++index)
            {
                if (index > 0)
                    list += ",";
                list += "{\"stem\":" + Quote(PcUtf8ToWide(entries[index].stem)) + ",\"name\":"
                        + Quote(entries[index].name) + ",\"uuid\":" + Quote(PcUtf8ToWide(entries[index].uuid))
                        + ",\"skin\":" + Quote(entries[index].skin) + ",\"bundledSkin\":"
                        + Bool(entries[index].bundledSkin) + ",\"usable\":" + Bool(entries[index].usable)
                        + ",\"active\":" + Bool(entries[index].active) + "}";
            }
            SendResult(id, "{\"online\":" + Bool(identities.Online()) + ",\"identities\":" + list + "]}");
            return;
        }
        if (action == L"select")
            ok = identities.Select(stem, &error);
        else if (action == L"create")
            ok = identities.Create(text, &error);
        else if (action == L"rename")
            ok = identities.Rename(stem, other, &error);
        else if (action == L"remove")
            ok = identities.Remove(stem, &error);
        else if (action == L"skin")
        {
            std::ifstream file(std::string(other.begin(), other.end()).c_str(), std::ios::binary);
            std::vector<unsigned char> png((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            ok = identities.SetBundledSkin(stem, png, &error);
        }
        else
        {
            SendError(id, "identity needs an action: list, select, create, rename, remove or skin");
            return;
        }
        if (!ok)
        {
            SendError(id, error);
            return;
        }
        if (action == L"select" || action == L"rename")
            identities.ApplyToProfile();
        SendResult(id, "{\"done\":true}");
        return;
    }
    if (command == L"client-nbt")
    {
        Minecraft* minecraft = Minecraft::GetInstance();
        if (!PcInWorld() || !minecraft->mPlayer)
        {
            SendError(id, "client-nbt needs a world");
            return;
        }
        std::wstring path;
        MemberText(request, L"path", path);
        CompoundTag* data = minecraft->mPlayer->saveWithoutId(new CompoundTag());
        std::wstring text;
        std::wstring error;
        bool found = true;
        if (path.empty())
        {
            text = StringNbtWriter::write(data);
        }
        else
        {
            NbtSelection selected;
            if (!NbtPath::select(data, path, &selected, &error))
                found = false;
            else if (selected.size() != 1)
                error = selected.empty() ? L"the path found no data" : L"the path found more than one value";
            else
                text = StringNbtWriter::write(selected.get(0));
            found = found && error.empty();
        }
        delete data;
        if (!found)
        {
            SendError(id, PcWideToUtf8(error));
            return;
        }
        SendResult(id, "{\"nbt\":" + Quote(text) + "}");
        return;
    }
    if (command == L"type")
    {
        std::wstring text;
        if (!MemberText(request, L"text", text) || text.empty())
        {
            SendError(id, "type needs text");
            return;
        }
        SDL_Window* window = PcWindowHandle();
        if (window == NULL)
        {
            SendError(id, "there is no window");
            return;
        }
        std::string& slot = sTyped[sNextTyped];
        sNextTyped = (sNextTyped + 1) % kTypedSlots;
        slot = PcWideToUtf8(text);
        SDL_Event event;
        std::memset(&event, 0, sizeof(event));
        event.type = SDL_EVENT_TEXT_INPUT;
        event.text.windowID = SDL_GetWindowID(window);
        event.text.text = slot.c_str();
        if (!SDL_PushEvent(&event))
        {
            SendError(id, std::string("the text event was not queued: ") + SDL_GetError());
            return;
        }
        SendResult(id, "{\"queued\":true}");
        return;
    }
    if (command == L"screen")
    {
        std::wstring name;
        if (!MemberText(request, L"name", name) || name.empty())
        {
            SendError(id, "screen needs a name");
            return;
        }
        EUIScene scene = EUIScene_Unknown;
        if (name == L"pause")
            scene = EUIScene_PauseMenu;
        else if (name == L"help")
            scene = EUIScene_HelpAndOptionsMenu;
        else if (name == L"controls")
            scene = EUIScene_ControlsMenu;
        else if (name == L"settings")
            scene = EUIScene_SettingsListMenu;
        else if (name == L"skins")
            scene = EUIScene_SkinSelectMenu;
        else if (name == L"main-menu")
            scene = EUIScene_MainMenu;
        else if (name == L"shaders")
            scene = EUIScene_ShaderMenu;
        else if (name == L"debug-overlay")
            scene = EUIScene_DebugOverlayMenu;
        else if (name == L"friends")
            scene = EUIScene_FriendsMenu;
        else if (name == L"identities")
            scene = EUIScene_IdentityMenu;
        if (scene == EUIScene_Unknown)
        {
            SendError(id, "unknown screen");
            return;
        }
        void* data = NULL;
        if (scene == EUIScene_SettingsListMenu)
        {
            UIScene_SettingsMenu::ListMenuInitData* settings = new UIScene_SettingsMenu::ListMenuInitData();
            settings->playerId = gProfile->GetPrimaryPad();
            settings->mode = 0;
            data = settings;
        }
        if (!gConsoleUIController.NavigateToScene(gProfile->GetPrimaryPad(), scene, data, EUILayer_Scene,
                                                  EUIGroup_PlayerDefault))
        {
            SendError(id, gConsoleUIController.GetMenuDisplayed(gProfile->GetPrimaryPad())
                              ? "the screen could not be opened over the menu that is displayed"
                              : "the screen could not be opened");
            return;
        }
        SendResult(id, "{\"opened\":true}");
        return;
    }
    if (command == L"hold")
    {
        double hold = 1.0;
        MemberNumber(request, L"hold", hold);
        MinecraftServer::HoldTicks(hold != 0.0);
        LOG_INFO(sLog, hold != 0.0 ? "Server ticks held" : "Server ticks released");
        SendResult(id, "{\"held\":" + Bool(hold != 0.0) + "}");
        return;
    }
    if (command == L"wait" || command == L"step")
    {
        double count = 0.0;
        std::wstring unit = command == L"step" ? L"steps" : L"ticks";
        if (!MemberNumber(request, L"count", count) || count < 1.0 || count != (long long)count)
        {
            SendError(id, "count must be a whole number of at least 1");
            return;
        }
        if (command == L"wait")
            MemberText(request, L"unit", unit);
        EWaitKind kind = EWait_None;
        if (unit == L"frames")
            kind = EWait_Frames;
        else if (unit == L"ticks")
            kind = EWait_Ticks;
        else if (unit == L"steps")
            kind = EWait_Steps;
        if (kind == EWait_None)
        {
            SendError(id, "unit must be frames or ticks");
            return;
        }
        if (sWait.kind != EWait_None)
        {
            SendError(id, "another wait is still running");
            return;
        }
        if (kind != EWait_Frames && gMinecraftServer == NULL)
        {
            SendError(id, "ticks need a hosted world");
            return;
        }
        if (kind == EWait_Ticks && MinecraftServer::TicksHeld())
        {
            SendError(id, "server ticks are held; step them instead");
            return;
        }
        if (kind == EWait_Steps && !MinecraftServer::TicksHeld())
        {
            SendError(id, "step needs the server ticks held first");
            return;
        }
        sWait.kind = kind;
        sWait.id = id;
        if (kind == EWait_Steps)
            sWait.target = MinecraftServer::StepTicks((int)count);
        else
            sWait.target = WaitPosition(kind) + (long long)count;
        const double seconds = count * kWaitSecondsPerUnit / (kind == EWait_Frames ? 1.0 : 20.0);
        sWait.deadline = Now() + (seconds > kWaitMinimumSeconds ? seconds : kWaitMinimumSeconds);
        return;
    }
    if (command == L"ask")
    {
        std::wstring text;
        if (!MemberText(request, L"text", text) || text.empty())
        {
            SendError(id, "ask needs text");
            return;
        }
        if (text.size() > kMaxRequestLength)
        {
            SendError(id, "ask text is longer than " + Number((long long)kMaxRequestLength) + " characters");
            return;
        }
        if (!PcInWorld() || !CGameNetworkManager::sInstance.IsHost())
        {
            SendError(id, "ask needs a hosted world, so the request can reach the chat");
            return;
        }
        std::vector<std::wstring> options;
        std::map<std::wstring, JsonValue>::const_iterator member = request.members.find(L"options");
        if (member != request.members.end())
        {
            for (size_t index = 0; index < member->second.items.size(); ++index)
                options.push_back(member->second.items[index].text);
        }
        const int task = PcAgentTaskAdd(text, options);
        {
            std::lock_guard<std::mutex> lock(sCommandMutex);
            QueuedCommand entry;
            entry.line = L"agent show " + std::to_wstring(task);
            entry.barrier = false;
            sCommands.push_back(entry);
        }
        LOG_INFO(sLog, "Request %d sent to the chat", task);
        SendResult(id, "{\"id\":" + Number((long long)task) + "}");
        return;
    }
    if (command == L"touch")
    {
        double finger = 1.0;
        double x = 0.0;
        double y = 0.0;
        double phase = 0.0;
        MemberNumber(request, L"id", finger);
        if (!MemberNumber(request, L"x", x) || !MemberNumber(request, L"y", y)
            || !MemberNumber(request, L"phase", phase))
        {
            SendError(id, "touch needs x, y and phase");
            return;
        }
        if (phase < 0.0 || phase > 3.0)
        {
            SendError(id, "phase must be 0 for down, 1 for move, 2 for up or 3 for cancel");
            return;
        }
        g_pInput->DebugTouch((unsigned long long)finger, (float)x, (float)y, (int)phase);
        SendResult(id, "{\"queued\":true}");
        return;
    }
    if (command == L"quit")
    {
        SendResult(id, "{\"quitting\":true}");
        PcRequestExit();
        return;
    }
    SendError(id, "unknown command");
}

void HandleLine(const std::string& line)
{
    if (line.empty())
        return;
    HandleRequest(line);
}

void DiscardPending()
{
    std::lock_guard<std::mutex> lock(sCommandMutex);
    sCommands.clear();
    sBarrierReached = false;
}

void ReadClient()
{
    char buffer[4096];
    for (;;)
    {
        int received = sClient.Receive(buffer, sizeof(buffer), 0);
        if (received == 0)
            return;
        if (received < 0)
        {
            sClient.Close();
            sReceived.clear();
            sCapture.active = false;
            sCommand.active = false;
            DiscardPending();
            return;
        }
        sReceived.append(buffer, (size_t)received);
        if (sReceived.size() > kMaxRequestSize)
        {
            sReceived.clear();
            SendError("null", "the request is too large");
            return;
        }
    }
}

void DrainLines()
{
    size_t start = 0;
    for (;;)
    {
        size_t newline = sReceived.find('\n', start);
        if (newline == std::string::npos)
            break;
        std::string line = sReceived.substr(start, newline - start);
        start = newline + 1;
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        sLastRequest = Now();
        HandleLine(line);
        if (!sClient.IsOpen())
            break;
    }
    if (start > 0)
        sReceived.erase(0, start);
}
}

void PcDebugStart(unsigned short port, int idleQuitSeconds)
{
    if (sListener.IsOpen())
        return;
    lcenet::Address address;
    address.ip = 0x7f000001u;
    address.port = port;
    std::string error;
    if (!sListener.Listen(address, error))
    {
        LOG_ERROR(sLog, "The debug protocol could not listen: %s", error.c_str());
        return;
    }
    sIdleQuitSeconds = idleQuitSeconds;
    sLastRequest = Now();
    sLastTick = sLastRequest;
    sFrameWindowStart = sLastRequest;
    MinecraftServer::SetConsoleCommandSource(NextCommand);
    ChatCommands::SetConsoleOutput(ConsoleOutput);
    ChatCommands::Register(new PcAgentChatCommand());
    LOG_INFO(sLog, "Debug protocol on 127.0.0.1:%u", (unsigned int)port);
}

void PcDebugTick()
{
    if (!sListener.IsOpen())
        return;
    const double now = Now();
    if (now - sLastTick > kStoppedGapSeconds)
    {
        sLastRequest += now - sLastTick;
        sFrameWindowStart = now;
        sFramesInWindow = 0;
    }
    sLastTick = now;
    ++sFramesDrawn;
    ++sFramesInWindow;
    if (now - sFrameWindowStart >= 1.0)
    {
        sFrameRate = sFramesInWindow / (now - sFrameWindowStart);
        sFrameWindowStart = now;
        sFramesInWindow = 0;
    }
    if (!sClient.IsOpen())
    {
        lcenet::TcpSocket accepted;
        lcenet::Address peer;
        if (!sListener.Accept(0, accepted, peer))
        {
            if (sIdleQuitSeconds > 0 && Now() - sLastRequest > (double)sIdleQuitSeconds)
            {
                LOG_INFO(sLog, "The debug protocol has been idle for %d seconds", sIdleQuitSeconds);
                PcRequestExit();
            }
            return;
        }
        sClient.Swap(accepted);
        sReceived.clear();
        sLastRequest = Now();
        LOG_TRACE(sLog, "Debug client connected from %s", peer.Text().c_str());
    }
    ReadClient();
    DrainLines();

    if (sCommand.active)
    {
        bool reached = false;
        {
            std::lock_guard<std::mutex> lock(sCommandMutex);
            reached = sBarrierReached;
            if (reached)
                sBarrierReached = false;
        }
        if (reached)
        {
            const std::string id = sCommand.id;
            const unsigned long long since = sCommand.since;
            sCommand.active = false;
            SendResult(id, "{\"lines\":" + ConsoleLines(since) + "}");
        }
        else if (Now() > sCommand.deadline)
        {
            const std::string id = sCommand.id;
            sCommand.active = false;
            SendError(id, "the command did not run; a hosted world is needed");
        }
    }

    if (sWait.kind != EWait_None)
        CheckWait();

    if (sIdleQuitSeconds > 0 && Now() - sLastRequest > (double)sIdleQuitSeconds)
    {
        LOG_INFO(sLog, "The debug protocol has been idle for %d seconds", sIdleQuitSeconds);
        PcRequestExit();
    }
}

void PcDebugFrame(int width, int height)
{
    if (!sCapture.active)
        return;
    const bool written = PcFrameCaptureWrite(sCapture.path, width, height);
    std::string id = sCapture.id;
    std::string path = sCapture.path;
    sCapture.active = false;
    sCapture.id = "null";
    sCapture.path.clear();
    if (written)
        SendResult(id, "{\"path\":" + Quote(path) + ",\"width\":" + Number((long long)width) + ",\"height\":"
                            + Number((long long)height) + "}");
    else
        SendError(id, "the frame could not be written to " + path);
}

#endif
