#include "game/command/ChatCommands.hpp"
#include "game/core/ModHost.hpp"

#include <wctype.h>

#include "game/command/ChatCommand.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/command/CommandSource.hpp"
#include "game/command/chat/ClearChatCommand.hpp"
#include "game/command/chat/DataChatCommand.hpp"
#include "game/command/chat/DefaultGameModeChatCommand.hpp"
#include "game/command/chat/DifficultyChatCommand.hpp"
#include "game/command/chat/EffectChatCommand.hpp"
#include "game/command/chat/EnchantChatCommand.hpp"
#include "game/command/chat/ExecuteChatCommand.hpp"
#include "game/command/chat/ExperienceChatCommand.hpp"
#include "game/command/chat/FillChatCommand.hpp"
#include "game/command/chat/GameModeChatCommand.hpp"
#include "game/command/chat/BanChatCommand.hpp"
#include "game/command/chat/GiveChatCommand.hpp"
#include "game/command/chat/LootChatCommand.hpp"
#include "game/command/chat/ReplaceItemChatCommand.hpp"
#include "game/command/chat/HelpChatCommand.hpp"
#include "game/command/chat/KickChatCommand.hpp"
#include "game/command/chat/KillChatCommand.hpp"
#include "game/command/chat/ListChatCommand.hpp"
#include "game/command/chat/MeChatCommand.hpp"
#include "game/command/chat/MessageChatCommand.hpp"
#include "game/command/chat/OperatorChatCommand.hpp"
#include "game/command/chat/SayChatCommand.hpp"
#include "game/command/chat/BossBarChatCommand.hpp"
#include "game/command/chat/CloneChatCommand.hpp"
#include "game/command/chat/LocateChatCommand.hpp"
#include "game/command/chat/ParticleChatCommand.hpp"
#include "game/command/chat/PlaySoundChatCommand.hpp"
#include "game/command/chat/SeedChatCommand.hpp"
#include "game/command/chat/ServerControlChatCommand.hpp"
#include "game/command/chat/SpreadPlayersChatCommand.hpp"
#include "game/command/chat/SetBlockChatCommand.hpp"
#include "game/command/chat/SetWorldSpawnChatCommand.hpp"
#include "game/command/chat/SpawnpointChatCommand.hpp"
#include "game/command/chat/SummonChatCommand.hpp"
#include "game/command/chat/TeleportChatCommand.hpp"
#include "game/command/chat/TimeChatCommand.hpp"
#include "game/command/chat/ToggleDownfallChatCommand.hpp"
#include "game/command/chat/WeatherChatCommand.hpp"
#include "game/command/chat/WhitelistChatCommand.hpp"
#include "game/core/ByteArrayInputStream.hpp"
#include "game/core/ByteArrayOutputStream.hpp"
#include "game/core/DataInputStream.hpp"
#include "game/core/DataOutputStream.hpp"
#include "game/network/ClientboundChatPacket.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerLevel.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/generation/PlayerList.hpp"

namespace
{
struct Registry
{
    Registry() : ready(false), commands() {}

    ~Registry()
    {
        for (size_t index = 0; index < commands.size(); ++index)
            delete commands[index];
    }

    bool ready;
    std::vector<ChatCommand*> commands;
};

std::vector<ChatCommand*>& Storage()
{
    static Registry registry;
    if (!registry.ready)
    {
        registry.ready = true;
        ChatCommand* const defaults[] = {new HelpChatCommand(),
                                         new ExecuteChatCommand(),
                                         new TimeChatCommand(),
                                         new WeatherChatCommand(),
                                         new GameModeChatCommand(),
                                         new TeleportChatCommand(),
                                         new KillChatCommand(),
                                         new GiveChatCommand(),
                                         new LootChatCommand(),
                                         new ReplaceItemChatCommand(),
                                         new ClearChatCommand(),
                                         new DataChatCommand(),
                                         new EffectChatCommand(),
                                         new EnchantChatCommand(),
                                         new ExperienceChatCommand(),
                                         new SpawnpointChatCommand(),
                                         new DifficultyChatCommand(),
                                         new DefaultGameModeChatCommand(),
                                         new SetWorldSpawnChatCommand(),
                                         new ToggleDownfallChatCommand(),
                                         new SeedChatCommand(),
                                         new SummonChatCommand(),
                                         new SetBlockChatCommand(),
                                         new FillChatCommand(),
                                         new SayChatCommand(),
                                         new MeChatCommand(),
                                         new MessageChatCommand(),
                                         new ListChatCommand(),
                                         new KickChatCommand(),
                                         new OperatorChatCommand(true),
                                         new OperatorChatCommand(false),
                                         new WhitelistChatCommand(),
                                         new BanChatCommand(BanChatCommand::eAction_Ban),
                                         new BanChatCommand(BanChatCommand::eAction_Pardon),
                                         new BanChatCommand(BanChatCommand::eAction_List),
                                         new BanChatCommand(BanChatCommand::eAction_BanAddress),
                                         new BanChatCommand(BanChatCommand::eAction_PardonAddress),
                                         new ServerControlChatCommand(ServerControlChatCommand::eAction_SaveAll),
                                         new ServerControlChatCommand(ServerControlChatCommand::eAction_Stop),
                                         new ServerControlChatCommand(ServerControlChatCommand::eAction_SaveOff),
                                         new ServerControlChatCommand(ServerControlChatCommand::eAction_SaveOn),
                                         new ParticleChatCommand(),
                                         new CloneChatCommand(),
                                         new PlaySoundChatCommand(),
                                         new LocateChatCommand(),
                                         new SpreadPlayersChatCommand(),
                                         new BossBarChatCommand()};
        registry.commands.assign(defaults, defaults + sizeof(defaults) / sizeof(defaults[0]));
    }
    return registry.commands;
}

bool Names(const ChatCommand* command, const std::wstring& name)
{
    if (command->getName() == name)
        return true;
    const std::vector<std::wstring>& aliases = command->getAliases();
    for (size_t index = 0; index < aliases.size(); ++index)
    {
        if (aliases[index] == name)
            return true;
    }
    return false;
}

void WriteUsage(DataOutputStream& output, const std::wstring& name, const ChatCommand* command)
{
    output.writeUTF(name);
    output.writeUTF(command->getUsage());
    output.writeUTF(command->getArguments());
}

void (*sConsoleOutput)(const std::wstring& text) = NULL;
std::wstring* sCaptured = NULL;

void Capture(const std::wstring& text)
{
    if (sCaptured != NULL)
    {
        if (!sCaptured->empty())
            *sCaptured += L"\n";
        *sCaptured += text;
    }
}

void Dispatch(const CommandSource& source, const ChatCommand::Words& words, const wchar_t* prefix)
{
    const std::wstring name = CommandArguments::Lower(words[0]);
    ChatCommand* command = ChatCommands::Find(name);
    if (command == NULL)
    {
        std::wstring modReply;
        if (ModHost::Get().Command(name, source.player ? source.player->getName() : std::wstring(L"Server"),
                                   CommandArguments::Join(words, 1), &modReply))
        {
            if (!modReply.empty())
            {
                if (source.output)
                    ChatCommands::Reply(source.output, modReply);
                else
                    ChatCommands::Echo(modReply);
            }
            return;
        }
        const std::wstring unknown = L"Unknown command " + std::wstring(prefix) + name + L". Type " + prefix
                                     + L"help for the commands.";
        if (source.output)
            ChatCommands::Error(source.output, unknown);
        else
            ChatCommands::Echo(L"§c" + unknown);
        return;
    }
    if (!command->canUse(source.permission))
    {
        if (source.output)
            ChatCommands::Error(source.output, L"You don't have permission to use this command.");
        return;
    }
    if (!command->run(source, words))
    {
        if (source.output)
            ChatCommands::Error(source.output, command->usageError());
        else
            ChatCommands::Echo(L"§c" + command->usageError());
    }
}
}

namespace ChatCommands
{
void Register(ChatCommand* command)
{
    if (command == NULL)
        return;
    std::vector<ChatCommand*>& commands = Storage();
    for (size_t index = 0; index < commands.size(); ++index)
    {
        if (commands[index]->getName() == command->getName())
        {
            delete commands[index];
            commands[index] = command;
            return;
        }
    }
    commands.push_back(command);
}

void Unregister(ChatCommand* command)
{
    std::vector<ChatCommand*>& commands = Storage();
    for (size_t index = 0; index < commands.size(); ++index)
    {
        if (commands[index] == command)
        {
            commands.erase(commands.begin() + index);
            delete command;
            return;
        }
    }
}

ChatCommand* Find(const std::wstring& name)
{
    const std::vector<ChatCommand*>& commands = Storage();
    for (size_t index = 0; index < commands.size(); ++index)
    {
        if (Names(commands[index], name))
            return commands[index];
    }
    return NULL;
}

const std::vector<ChatCommand*>& Commands()
{
    return Storage();
}

int PermissionOf(boost::shared_ptr<ServerPlayer> player)
{
    if (!player || !player->mConnection)
        return ChatCommand::ePermission_Everyone;
    return gMinecraftServer->getPlayers()->isOp(player) ? ChatCommand::ePermission_Operator
                                                        : ChatCommand::ePermission_Everyone;
}

arrayWithLength<unsigned char> Trimmed(ByteArrayOutputStream& stream)
{
    arrayWithLength<unsigned char> all = stream.toByteArray();
    arrayWithLength<unsigned char> exact(static_cast<unsigned int>(stream.mCount), true);
    for (int index = 0; index < stream.mCount; ++index)
        exact.data[index] = all.data[index];
    delete[] all.data;
    return exact;
}

std::vector<arrayWithLength<unsigned char> > WriteUsageChunks(int permission)
{
    const size_t kChunkBytes = 900;
    const std::vector<ChatCommand*>& commands = Storage();
    std::vector<arrayWithLength<unsigned char> > chunks;
    std::vector<arrayWithLength<unsigned char> > records;
    size_t size = 0;
    bool first = true;
    for (size_t index = 0; index <= commands.size(); ++index)
    {
        std::vector<arrayWithLength<unsigned char> > next;
        if (index < commands.size() && commands[index]->canUse(permission))
        {
            const ChatCommand* command = commands[index];
            std::vector<std::wstring> names(1, command->getName());
            names.insert(names.end(), command->getAliases().begin(), command->getAliases().end());
            for (size_t name = 0; name < names.size(); ++name)
            {
                ByteArrayOutputStream bytes;
                DataOutputStream output(&bytes);
                WriteUsage(output, names[name], command);
                next.push_back(Trimmed(bytes));
            }
        }
        for (size_t record = 0; record <= next.size(); ++record)
        {
            const bool last = index == commands.size() && record == next.size();
            const bool full = record < next.size() && size + next[record].length + 3 > kChunkBytes && !records.empty();
            if (last || full)
            {
                if (records.empty() && !first)
                    break;
                ByteArrayOutputStream bytes;
                DataOutputStream output(&bytes);
                output.writeByte(first ? 1 : 0);
                output.writeShort(static_cast<short>(records.size()));
                for (size_t item = 0; item < records.size(); ++item)
                {
                    output.write(records[item]);
                    delete[] records[item].data;
                }
                chunks.push_back(Trimmed(bytes));
                records.clear();
                size = 0;
                first = false;
            }
            if (record < next.size())
            {
                records.push_back(next[record]);
                size += next[record].length;
            }
        }
    }
    return chunks;
}

std::vector<CommandUsage> ReadUsages(arrayWithLength<unsigned char> data, bool* first)
{
    std::vector<CommandUsage> usages;
    ByteArrayInputStream bytes(data);
    DataInputStream input(&bytes);
    *first = input.readByte() != 0;
    const int count = input.readShort();
    for (int index = 0; index < count; ++index)
    {
        CommandUsage usage;
        usage.name = input.readUTF();
        usage.usage = input.readUTF();
        usage.arguments = input.readUTF();
        if (!usage.name.empty())
            usages.push_back(usage);
    }
    bytes.reset();
    input.close();
    return usages;
}

std::wstring CleanMessage(const std::wstring& message)
{
    return CleanMessage(message, kMaxMessageLength);
}

std::wstring CleanMessage(const std::wstring& message, unsigned int limit)
{
    std::wstring clean;
    for (size_t index = 0; index < message.size() && clean.size() < limit; ++index)
    {
        const wchar_t character = message[index];
        if (character < 0x20 || (character >= 0x7f && character < 0xa0) || character == 0xa7)
            continue;
        if (character == L' ' && (clean.empty() || clean[clean.size() - 1] == L' '))
            continue;
        clean.push_back(character);
    }
    while (!clean.empty() && clean[clean.size() - 1] == L' ')
        clean.erase(clean.size() - 1);
    return clean;
}

std::wstring FormatMessage(const std::wstring& message)
{
    std::wstring formatted;
    bool visible = false;
    for (size_t index = 0; index < message.size(); ++index)
    {
        const wchar_t code = index + 1 < message.size() ? static_cast<wchar_t>(towlower(message[index + 1])) : 0;
        if (message[index] == L'&'
            && ((code >= L'0' && code <= L'9') || (code >= L'a' && code <= L'f') || (code >= L'k' && code <= L'o')
                || code == L'r'))
        {
            formatted.push_back(static_cast<wchar_t>(0xA7));
            formatted.push_back(code);
            ++index;
            continue;
        }
        if (message[index] != L' ')
            visible = true;
        formatted.push_back(message[index]);
    }
    return visible ? formatted : std::wstring();
}

void Reply(boost::shared_ptr<ServerPlayer> player, const std::wstring& text)
{
    if (!player)
        return;
    std::wstring strings[2] = {std::wstring(), text};
    player->sendMessage(ClientboundChatPacket::eChatPacketMessage_PlayerChat, NULL, 0, strings, 2);
}

void Error(boost::shared_ptr<ServerPlayer> player, const std::wstring& text)
{
    Reply(player, L"§c" + text);
}

void Run(boost::shared_ptr<ServerPlayer> player, const std::wstring& line)
{
    const ChatCommand::Words words = CommandArguments::Split(line.substr(1));
    if (words.empty())
        return;
    CommandSource source;
    source.player = player;
    source.output = player;
    source.entity = player;
    source.level = player->mLevel;
    source.x = player->mX;
    source.y = player->mY;
    source.z = player->mZ;
    source.yRot = player->mYRot;
    source.xRot = player->mXRot;
    source.console = false;
    source.permission = PermissionOf(player);
    Dispatch(source, words, L"/");
}

void RunConsole(const std::wstring& line)
{
    const ChatCommand::Words words = CommandArguments::Split(line.size() > 0 && line[0] == L'/' ? line.substr(1)
                                                                                                : line);
    if (words.empty())
        return;
    CommandSource source;
    source.level = gMinecraftServer->getLevel(0);
    const BlockPos spawn = source.level->getSharedSpawnPos();
    source.x = spawn.x + 0.5;
    source.y = spawn.y;
    source.z = spawn.z + 0.5;
    source.yRot = 0.0f;
    source.xRot = 0.0f;
    source.console = true;
    source.permission = ChatCommand::ePermission_Console;
    Dispatch(source, words, L"");
}

void SetConsoleOutput(void (*output)(const std::wstring& text))
{
    sConsoleOutput = output;
}

void Announce(const std::wstring& text)
{
    std::wstring strings[2] = {std::wstring(), text};
    gMinecraftServer->getPlayers()->broadcastAll(boost::shared_ptr<Packet>(
        new ClientboundChatPacket(ClientboundChatPacket::eChatPacketMessage_PlayerChat, NULL, 0, strings, 2)));
    Echo(text);
}

bool RunFromBlock(Level* level, double x, double y, double z, const std::wstring& line, std::wstring* output)
{
    output->clear();
    const ChatCommand::Words words = CommandArguments::Split(line.size() > 0 && line[0] == L'/' ? line.substr(1) : line);
    if (words.empty())
        return false;
    CommandSource source;
    source.level = level;
    source.x = x;
    source.y = y;
    source.z = z;
    source.yRot = 0.0f;
    source.xRot = 0.0f;
    source.console = false;
    source.permission = ChatCommand::ePermission_Operator;
    void (*previous)(const std::wstring&) = sConsoleOutput;
    std::wstring* previousCapture = sCaptured;
    sCaptured = output;
    sConsoleOutput = &Capture;
    Dispatch(source, words, L"/");
    sConsoleOutput = previous;
    sCaptured = previousCapture;
    return output->compare(0, 2, L"\u00a7c") != 0;
}

bool RunCaptured(const std::wstring& line, std::wstring* output)
{
    void (*previous)(const std::wstring&) = sConsoleOutput;
    std::wstring* previousCapture = sCaptured;
    output->clear();
    sCaptured = output;
    sConsoleOutput = &Capture;
    RunConsole(line);
    sConsoleOutput = previous;
    sCaptured = previousCapture;
    return output->compare(0, 2, L"\u00a7c") != 0;
}

void Echo(const std::wstring& text)
{
    if (sConsoleOutput != NULL)
        sConsoleOutput(text);
}
}
