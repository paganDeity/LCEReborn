#ifndef GAME_COMMAND_CHATCOMMANDS_HPP
#define GAME_COMMAND_CHATCOMMANDS_HPP

#include <stddef.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "game/network/ServerboundChatPacket.hpp"
#include "game/world/biome/arrayWithLength.hpp"

class ChatCommand;
class Level;
class ServerPlayer;

namespace ChatCommands
{
const unsigned int kMaxMessageLength = ServerboundChatPacket::kMaxLength;

struct CommandUsage
{
    std::wstring name;
    std::wstring usage;
    std::wstring arguments;
};

void Register(ChatCommand* command);
// take a command out of the registry and delete it
void Unregister(ChatCommand* command);
ChatCommand* Find(const std::wstring& name);
const std::vector<ChatCommand*>& Commands();
int PermissionOf(boost::shared_ptr<ServerPlayer> player);
std::vector<arrayWithLength<unsigned char> > WriteUsageChunks(int permission);
std::vector<CommandUsage> ReadUsages(arrayWithLength<unsigned char> data, bool* first);

std::wstring CleanMessage(const std::wstring& message);
std::wstring CleanMessage(const std::wstring& message, unsigned int limit);
std::wstring FormatMessage(const std::wstring& message);
void Reply(boost::shared_ptr<ServerPlayer> player, const std::wstring& text);
void Error(boost::shared_ptr<ServerPlayer> player, const std::wstring& text);
void Run(boost::shared_ptr<ServerPlayer> player, const std::wstring& line);
void RunConsole(const std::wstring& line);
void SetConsoleOutput(void (*output)(const std::wstring& text));
void Echo(const std::wstring& text);
void Announce(const std::wstring& text);
bool RunFromBlock(Level* level, double x, double y, double z, const std::wstring& line, std::wstring* output);
bool RunCaptured(const std::wstring& line, std::wstring* output);
}

#endif
