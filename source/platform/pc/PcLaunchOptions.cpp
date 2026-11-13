#include "platform/pc/PcLaunchOptions.hpp"

#include "platform/pc/PcCommandLine.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <vector>


static PcLaunchOptions sOptions;

static void PrintUsage(std::FILE* stream, PcProgram program)
{
    if (program == PcProgram::Server)
    {
        std::fputs("Usage: LCERebornServer [options]\n"
                   "  --content-dir <dir>         game content (default: assets in the working folder)\n"
                   "  --save-dir <dir>            worlds, server.properties and server-icon.png (default: saves in the\n"
                   "                              working folder)\n"
                   "  --packs-dir <dir>           packs to load (default: packs in the working folder)\n"
                   "  --log-dir <dir>             logs (default: logs in the working folder)\n"
               "  --log-keep-days <days>      remove archived logs older than this when starting; 0 keeps them all (default: 30)\n"
                   "  --p2p-server <host:port>    publish this server to friends through a rendezvous server\n"
                   "  --p2p-server-key <hex>      the rendezvous server's public key, 64 hex digits\n"
                   "  --p2p-dir <dir>             where the identity and the friends who may join are kept\n"
                   "  --identity-dir <dir>        where the identity files are kept (default: identities in the saves folder)\n"
                   "  --mods-dir <dir>            mods to load (default: mods in the working folder)\n"
                   "  --command-limits <list>     command limits as name=number,... (execute-forks, compare-blocks, command-chain)\n"
                   "  --help                      show this message\n",
                   stream);
        return;
    }
    std::fputs("Usage: LCEReborn [options]\n"
               "  --username <name>           player name\n"
               "  --uuid <uuid>               player identity, 32 hex digits (default: made from the name)\n"
               "  --content-dir <dir>         game content (default: assets in the working folder)\n"
               "  --save-dir <dir>            saves and settings (default: saves in the working folder)\n"
               "  --packs-dir <dir>           packs to load (default: packs in the working folder)\n"
               "  --resource-dir <dir>        PC interface art (default: resources in the working folder)\n"
               "  --language <code>           language, such as en-GB or ja (default: the system language)\n"
               "  --log-dir <dir>             logs (default: logs in the working folder)\n"
               "  --log-keep-days <days>      remove archived logs older than this when starting; 0 keeps them all (default: 30)\n"
               "  --split-screen <on|off>     other controllers can join as more players (default: on)\n"
               "  --join-prompt <on|off>      prompt other controllers to join when a world loads (default: off)\n"
               "  --cheats <on|off>           allow commands for the host in a world started from the command line (default: off)\n"
               "  --world <folder>            load this save from the saves folder's games folder at start, or\n"
               "                              make a new world there if the folder has no save\n"
               "  --seed <text>               seed for a new world made by --world (default: random)\n"
               "  --game-mode <mode>          survival, creative or adventure for a new world (default: survival)\n"
               "  --join <address>            join the game at this address (host or host:port) at start\n"
               "  --protocol <legacy|reborn>  the wire this game speaks when it joins a server (default: legacy)\n"
               "  --p2p-server <host:port>    peer-to-peer rendezvous server (needs --p2p-server-key and --p2p-dir)\n"
               "  --p2p-server-key <hex>      the server's public key, 64 hex digits\n"
               "  --p2p-dir <dir>             where this install keeps its identity and friends\n"
               "  --identity-dir <dir>        where the identity files are kept (default: identities in the saves folder)\n"
               "  --mods-dir <dir>            mods to load (default: mods in the working folder)\n"
               "  --command-limits <list>     command limits as name=number,... (execute-forks, compare-blocks, command-chain)\n"
               "  --discord-app <id>          show what you are doing in Discord, through this Discord application\n"
               "  --profiles <file>           the launcher's profiles, offered to split-screen players who join\n"
               "  --ui-mode <legacy|bedrock>  interface mode for this run (default: the player's setting)\n",
               stream);
#if LCE_DEBUG_BUILD
    std::fputs("  --position <x,y,z>          move the player here once a world has loaded\n"
               "  --rotation <yaw,pitch>      turn the player to face this way once a world has loaded\n"
               "  --render-size <w,h>         draw at this size, at least 320,180, and open the window at it (default: 1280,720)\n"
               "  --capture-frames <count>    save this many frames as PNG files, then quit\n"
               "  --capture-delay <seconds>   wait this long in the world before capturing (default: 10)\n"
               "  --capture-dir <dir>         where captured frames go (default: capture in the working folder)\n"
               "  --debug-port <port>         accept debug protocol connections on localhost at this port\n"
               "  --debug-idle-quit <seconds> quit when no debug request has arrived for this long (default: never)\n"
               "  --frame-rate <count>        draw at most this many frames a second (default: 60)\n"
               "  --skip-intro <on|off>       go past the splash screen and the save-icon message without waiting (default: off)\n"
               "  --hints <on|off>            show or hide the in-game hints for this run, whatever the profile says\n",
               stream);
#endif
    std::fputs("  --help                      show this message\n", stream);
}

bool PcParseLaunchOptions(int argc, char** argv, int& exitCode, PcProgram program)
{
    exitCode = 1;
    const std::vector<std::string> arguments = PcCommandLineArguments(argc, argv);

    for (std::vector<std::string>::size_type index = 0; index < arguments.size(); ++index)
    {
        std::string name = arguments[index];
        std::string value;
        bool hasValue = false;

        const std::string::size_type equals = name.find('=');
        if (name.compare(0, 2, "--") == 0 && equals != std::string::npos)
        {
            value = name.substr(equals + 1);
            name.resize(equals);
            hasValue = true;
        }

        if (name == "--help" || name == "-h")
        {
            PrintUsage(stdout, program);
            exitCode = 0;
            return false;
        }

        if (program == PcProgram::Server && name != "--content-dir" && name != "--save-dir" && name != "--packs-dir"
            && name != "--log-dir" && name != "--log-keep-days" && name != "--p2p-server" && name != "--p2p-server-key" && name != "--p2p-dir"
            && name != "--identity-dir" && name != "--mods-dir" && name != "--command-limits")
        {
            std::fprintf(stderr, "Unknown option: %s\n", arguments[index].c_str());
            PrintUsage(stderr, program);
            return false;
        }

        std::string* target = NULL;
        bool* flag = NULL;
        int* count = NULL;
        double* seconds = NULL;
#if LCE_DEBUG_BUILD
        std::vector<double>* list = NULL;
        size_t listSize = 0;
#endif
        if (name == "--split-screen")
            flag = &sOptions.splitScreen;
        else if (name == "--join-prompt")
            flag = &sOptions.joinPrompt;
        else if (name == "--cheats")
            flag = &sOptions.cheats;
        else if (name == "--username")
            target = &sOptions.username;
        else if (name == "--content-dir")
            target = &sOptions.contentDir;
        else if (name == "--save-dir")
            target = &sOptions.saveDir;
        else if (name == "--packs-dir")
            target = &sOptions.packsDir;
        else if (name == "--resource-dir")
            target = &sOptions.resourceDir;
        else if (name == "--language")
            target = &sOptions.language;
        else if (name == "--log-dir")
            target = &sOptions.logDir;
        else if (name == "--log-keep-days")
            count = &sOptions.logKeepDays;
        else if (name == "--uuid")
            target = &sOptions.uuid;
        else if (name == "--world")
            target = &sOptions.world;
        else if (name == "--seed")
            target = &sOptions.seed;
        else if (name == "--game-mode")
            target = &sOptions.gameMode;
        else if (name == "--join")
            target = &sOptions.join;
        else if (name == "--protocol")
            target = &sOptions.protocol;
        else if (name == "--p2p-server")
            target = &sOptions.p2pServer;
        else if (name == "--p2p-server-key")
            target = &sOptions.p2pServerKey;
        else if (name == "--p2p-dir")
            target = &sOptions.p2pDir;
        else if (name == "--identity-dir")
            target = &sOptions.identityDir;
        else if (name == "--mods-dir")
            target = &sOptions.modsDir;
        else if (name == "--command-limits")
            target = &sOptions.commandLimits;
        else if (name == "--discord-app")
            target = &sOptions.discordApp;
        else if (name == "--profiles")
            target = &sOptions.profiles;
        else if (name == "--ui-mode")
            target = &sOptions.uiMode;
#if LCE_DEBUG_BUILD
        else if (name == "--position")
        {
            list = &sOptions.position;
            listSize = 3;
        }
        else if (name == "--rotation")
        {
            list = &sOptions.rotation;
            listSize = 2;
        }
        else if (name == "--render-size")
        {
            list = &sOptions.renderSize;
            listSize = 2;
        }
        else if (name == "--capture-dir")
            target = &sOptions.captureDir;
        else if (name == "--capture-frames")
            count = &sOptions.captureFrames;
        else if (name == "--capture-delay")
            seconds = &sOptions.captureDelay;
        else if (name == "--debug-port")
            count = &sOptions.debugPort;
        else if (name == "--debug-idle-quit")
            count = &sOptions.debugIdleQuit;
        else if (name == "--frame-rate")
            count = &sOptions.frameRate;
        else if (name == "--skip-intro")
            flag = &sOptions.skipIntro;
        else if (name == "--hints")
            target = &sOptions.hints;
#endif
        else
        {
            std::fprintf(stderr, "Unknown option: %s\n", arguments[index].c_str());
            PrintUsage(stderr, program);
            return false;
        }

        if (!hasValue)
        {
            if (index + 1 >= arguments.size())
            {
                std::fprintf(stderr, "Missing value for %s\n", name.c_str());
                PrintUsage(stderr, program);
                return false;
            }
            value = arguments[++index];
        }

        if (value.empty())
        {
            std::fprintf(stderr, "Empty value for %s\n", name.c_str());
            return false;
        }

        if (flag != NULL)
        {
            if (value == "on")
                *flag = true;
            else if (value == "off")
                *flag = false;
            else
            {
                std::fprintf(stderr, "%s must be on or off, not %s\n", name.c_str(), value.c_str());
                return false;
            }
            continue;
        }

#if LCE_DEBUG_BUILD
        if (list != NULL)
        {
            list->clear();
            const char* cursor = value.c_str();
            for (;;)
            {
                char* end = NULL;
                const double number = std::strtod(cursor, &end);
                if (end == cursor)
                    break;
                list->push_back(number);
                cursor = end;
                if (*cursor != ',')
                    break;
                ++cursor;
            }
            if (list->size() != listSize || *cursor != '\0')
            {
                std::fprintf(stderr, "%s must be %u numbers separated by commas, not %s\n", name.c_str(),
                             static_cast<unsigned int>(listSize), value.c_str());
                return false;
            }
            continue;
        }
#endif

        if (count != NULL || seconds != NULL)
        {
            char* end = NULL;
            const double number = std::strtod(value.c_str(), &end);
            if (end == value.c_str() || *end != '\0' || number < 0.0 || (count != NULL && number != (int)number))
            {
                std::fprintf(stderr, "%s must be a %s, not %s\n", name.c_str(),
                             count != NULL ? "whole number" : "number of seconds", value.c_str());
                return false;
            }
            if (count != NULL)
            {
                *count = (int)number;
#if LCE_DEBUG_BUILD
                if (count == &sOptions.frameRate && (*count < 1 || *count > 1000))
                {
                    std::fprintf(stderr, "%s must be between 1 and 1000, not %s\n", name.c_str(), value.c_str());
                    return false;
                }
                if (count == &sOptions.debugPort && (*count < 1 || *count > 65535))
                {
                    std::fprintf(stderr, "%s must be a port between 1 and 65535, not %s\n", name.c_str(),
                                 value.c_str());
                    return false;
                }
#endif
            }
            else
                *seconds = number;
            continue;
        }

        *target = value;
    }

    if (!sOptions.uuid.empty())
    {
        std::string digits;
        for (char character : sOptions.uuid)
        {
            if (character == '-')
                continue;
            if (!std::isxdigit(static_cast<unsigned char>(character)))
            {
                digits.clear();
                break;
            }
            digits += static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        }
        if (digits.size() != 32)
        {
            std::fprintf(stderr, "--uuid must be 32 hexadecimal digits, not %s\n", sOptions.uuid.c_str());
            return false;
        }
        sOptions.uuid = digits;
    }

    return true;
}

const PcLaunchOptions& PcGetLaunchOptions()
{
    return sOptions;
}
