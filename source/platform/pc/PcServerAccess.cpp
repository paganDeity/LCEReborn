#include "platform/pc/PcServerAccess.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "game/core/GameSessionData.hpp"
#include "game/network/PacketCodec.hpp"
#include "game/server/ServerAccess.hpp"
#include "game/server/ServerWatchdog.hpp"
#include "game/world/generation/PlayerList.hpp"
#include "platform/pc/Log.hpp"
#include "platform/pc/PcJson.hpp"
#include "platform/pc/PcPaths.hpp"
#include "platform/pc/PcWorldBackup.hpp"

namespace
{
const Logger sLog("Access");

std::string Trim(const std::string& text)
{
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && (text[begin] == ' ' || text[begin] == '\t' || text[begin] == '\r'))
        ++begin;
    while (end > begin && (text[end - 1] == ' ' || text[end - 1] == '\t' || text[end - 1] == '\r'))
        --end;
    return text.substr(begin, end - begin);
}

std::string Lower(std::string text)
{
    for (size_t index = 0; index < text.size(); ++index)
        text[index] = static_cast<char>(tolower(static_cast<unsigned char>(text[index])));
    return text;
}

// whether a line of server.properties sets this key, and to what
bool LineSets(const std::string& line, const std::string& key, std::string* value)
{
    const std::string trimmed = Trim(line);
    const size_t equals = trimmed.find('=');
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '!' || equals == std::string::npos
        || Lower(Trim(trimmed.substr(0, equals))) != key)
        return false;
    *value = Trim(trimmed.substr(equals + 1));
    return true;
}

// write a file whole or not at all: to a temporary beside it, then renamed over it
bool WriteFileAtomically(const std::filesystem::path& path, const std::string& text)
{
    std::filesystem::path temporary = path;
    temporary += ".tmp";
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        file << text;
        if (!file)
            return false;
    }
    std::error_code error;
    std::filesystem::rename(temporary, path, error);
    return !error;
}

// the reason an entry carries, for the lists that keep one
const std::wstring& ReasonOf(const BanEntry& entry)
{
    return entry.reason;
}

template <class Entry> std::wstring ReasonOf(const Entry&)
{
    return std::wstring();
}

void SetReason(BanEntry& entry, const std::wstring& reason)
{
    entry.reason = reason;
}

template <class Entry> void SetReason(Entry&, const std::wstring&)
{
}

std::filesystem::path ListPath(const char* file)
{
    return std::filesystem::path(PcSaveRoot()) / file;
}

// a list of players beside server.properties (ops.json, whitelist.json, banned-players.json): each entry has a uuid, a name, or both, and a
// ban has its reason. An entry with neither, or with a uuid that is not one, is skipped with a warning
template <class Entry>
std::vector<Entry> LoadPlayerList(const char* fileName, const wchar_t* idKey = L"uuid",
                                  bool (*parseId)(const std::wstring&, std::wstring*) = &PlayerList::ParsePlayerId)
{
    std::vector<Entry> entries;
    const std::filesystem::path path = ListPath(fileName);
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return entries;
    const std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    JsonValue root;
    if (!PcJsonParse(source, root) || root.type != JsonValue::Array)
    {
        LOG_WARN(sLog, "%s is not a list of players, so it is read as empty", path.string().c_str());
        return entries;
    }
    for (std::vector<JsonValue>::const_iterator it = root.items.begin(); it != root.items.end(); ++it)
    {
        std::map<std::wstring, JsonValue>::const_iterator uuid = it->members.find(idKey);
        std::map<std::wstring, JsonValue>::const_iterator name = it->members.find(L"name");
        std::map<std::wstring, JsonValue>::const_iterator reason = it->members.find(L"reason");
        Entry entry;
        if (name != it->members.end() && name->second.type == JsonValue::String)
            entry.name = name->second.text;
        if (reason != it->members.end() && reason->second.type == JsonValue::String)
            SetReason(entry, reason->second.text);
        const bool hasUuid =
            uuid != it->members.end() && uuid->second.type == JsonValue::String && !uuid->second.text.empty();
        if (it->type != JsonValue::Object || (hasUuid ? !parseId(uuid->second.text, &entry.id)
                                                      : entry.name.empty()))
        {
            LOG_WARN(sLog, "%s: skipping an entry without a valid %ls or name", fileName, idKey);
            continue;
        }
        entries.push_back(entry);
    }
    return entries;
}

template <class Entry> void SavePlayerList(const char* fileName, const std::vector<Entry>& entries, const char* idKey = "uuid")
{
    std::string text = "[";
    for (typename std::vector<Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it)
    {
        text += it == entries.begin() ? "\n" : ",\n";
        text += std::string("  {\n    \"") + idKey + "\": " + PcJsonQuote(it->id);
        if (!it->name.empty() || std::string(idKey) == "uuid")
            text += ",\n    \"name\": " + PcJsonQuote(it->name);
        if (!ReasonOf(*it).empty())
            text += ",\n    \"reason\": " + PcJsonQuote(ReasonOf(*it));
        text += "\n  }";
    }
    text += entries.empty() ? "]\n" : "\n]\n";

    const std::filesystem::path path = ListPath(fileName);
    if (!WriteFileAtomically(path, text))
        LOG_WARN(sLog, "Could not save %s", path.string().c_str());
}

void SaveOperators(const std::vector<ServerOperator>& operators)
{
    SavePlayerList("ops.json", operators);
}

void SaveWhitelist(const std::vector<WhitelistEntry>& entries)
{
    SavePlayerList("whitelist.json", entries);
}

void SaveBans(const std::vector<BanEntry>& entries)
{
    SavePlayerList("banned-players.json", entries);
}

void SaveAddressBans(const std::vector<BanEntry>& entries)
{
    SavePlayerList("banned-ips.json", entries, "ip");
}

// a whole number setting of server.properties between two bounds, or the value it has when the line is missing or wrong
int ReadCount(const std::string& key, int minimum, int maximum, int fallback);

// keep a setting that a command changed: its line in server.properties is rewritten, or added, and every other line stays as it was
void SaveProperty(const std::string& key, const std::string& value)
{
    const std::filesystem::path path = ListPath("server.properties");
    std::ifstream file(path);
    std::string text;
    std::string line;
    bool written = false;
    while (std::getline(file, line))
    {
        std::string old;
        if (!written && LineSets(line, key, &old))
        {
            line = key + "=" + value;
            written = true;
        }
        text += line + "\n";
    }
    if (!written)
        text += key + "=" + value + "\n";
    if (!WriteFileAtomically(path, text))
        LOG_WARN(sLog, "Could not save %s to %s", key.c_str(), path.string().c_str());
}

void SaveWhitelistOn(bool on)
{
    SaveProperty("white-list", on ? "true" : "false");
}

// a setting of server.properties, or an empty string when the file or the line is missing
std::string ReadProperty(const std::string& key)
{
    std::ifstream file(ListPath("server.properties"));
    std::string line;
    std::string value;
    while (std::getline(file, line))
    {
        if (LineSets(line, key, &value))
            return value;
    }
    return std::string();
}
}

namespace
{
int ReadCount(const std::string& key, int minimum, int maximum, int fallback)
{
    const std::string wanted = ReadProperty(key);
    if (wanted.empty())
        return fallback;
    char* end = NULL;
    const long parsed = std::strtol(wanted.c_str(), &end, 10);
    if (end != wanted.c_str() && *end == 0 && parsed >= minimum && parsed <= maximum)
        return static_cast<int>(parsed);
    LOG_WARN(sLog, "server.properties: %s=%s is not a number from %d to %d, keeping %d", key.c_str(), wanted.c_str(), minimum, maximum,
             fallback);
    return fallback;
}
}

void PcServerAccessStart()
{
    PlayerList::SetOperators(LoadPlayerList<ServerOperator>("ops.json"));
    PlayerList::SetOperatorsChanged(SaveOperators);
    ServerAccess::Whitelist().Set(LoadPlayerList<WhitelistEntry>("whitelist.json"));
    ServerAccess::Whitelist().SetChanged(SaveWhitelist);
    ServerAccess::Bans().Set(LoadPlayerList<BanEntry>("banned-players.json"));
    ServerAccess::Bans().SetChanged(SaveBans);

    const std::string whitelist = Lower(ReadProperty("white-list"));
    const bool whitelistOn = whitelist == "true" || whitelist == "on" || whitelist == "yes" || whitelist == "1";
    ServerAccess::SetWhitelistOn(whitelistOn);
    ServerAccess::SetWhitelistOnChanged(SaveWhitelistOn);

    const int maxPlayers = ReadCount("max-players", 1, GameSessionData::MAX_SESSION_PLAYERS, GameSessionData::MAX_SESSION_PLAYERS);
    ServerAccess::SetMaxPlayers(maxPlayers);
    ServerAccess::AddressBans().Set(LoadPlayerList<BanEntry>("banned-ips.json", L"ip", &ServerAccess::ParseAddress));
    ServerAccess::AddressBans().SetChanged(SaveAddressBans);
    ServerAccess::SetMaxConnectionsPerAddress(ReadCount("max-connections-per-ip", 0, 1024, ServerAccess::MaxConnectionsPerAddress()));
    ServerAccess::SetMaxPendingConnections(ReadCount("max-pending-connections", 0, 4096, ServerAccess::MaxPendingConnections()));
    ServerAccess::SetMaxPacketsPerSecond(ReadCount("max-packets-per-second", 0, 100000, ServerAccess::MaxPacketsPerSecond()));
    ServerWatchdog::SetLimit(ReadCount("max-tick-seconds", 0, 86400, ServerWatchdog::Limit()));
    const std::string backup = Lower(ReadProperty("backup-on-load"));
    PcWorldBackup::SetPolicy(backup != "false" && backup != "off" && backup != "no" && backup != "0", ReadCount("backup-keep", 1, 1000, 3));
    int protocols = ConnectionProtocols::eProtocol_Legacy;
    const std::string accepted = Lower(ReadProperty("protocols"));
    if (!accepted.empty() && (!ConnectionProtocols::Parse(accepted, &protocols) || protocols == 0))
    {
        LOG_WARN(sLog, "server.properties: protocols=%s is not legacy, reborn or both, keeping legacy", accepted.c_str());
        protocols = ConnectionProtocols::eProtocol_Legacy;
    }
    ConnectionProtocols::SetServerAccepts(protocols);
    LOG_INFO(sLog, "Whitelist %s (%d listed), %d players and %d addresses banned, at most %d players, %d connections an address, %d not yet joined",
             whitelistOn ? "on" : "off", static_cast<int>(ServerAccess::Whitelist().Entries().size()),
             static_cast<int>(ServerAccess::Bans().Entries().size()), static_cast<int>(ServerAccess::AddressBans().Entries().size()),
             maxPlayers, ServerAccess::MaxConnectionsPerAddress(), ServerAccess::MaxPendingConnections());
    if ((protocols & ConnectionProtocols::eProtocol_Reborn) != 0)
        LOG_INFO(sLog, "Remote players may connect with %s", protocols == ConnectionProtocols::eProtocol_Reborn ? "the Reborn protocol only" : "the Reborn or the Legacy protocol");
}
