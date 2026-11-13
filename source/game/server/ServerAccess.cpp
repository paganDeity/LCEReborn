#include "game/server/ServerAccess.hpp"

namespace
{
AccessList<WhitelistEntry> sWhitelist;
AccessList<BanEntry> sBans;
AccessList<BanEntry> sAddressBans;
int sMaxConnectionsPerAddress = 4;
int sMaxPendingConnections = 16;
int sMaxPacketsPerSecond = 500;
bool sWhitelistOn = false;
void (*sWhitelistOnChanged)(bool on) = NULL;
int sMaxPlayers = 0;
}

AccessList<WhitelistEntry>& ServerAccess::Whitelist()
{
    return sWhitelist;
}

AccessList<BanEntry>& ServerAccess::Bans()
{
    return sBans;
}

AccessList<BanEntry>& ServerAccess::AddressBans()
{
    return sAddressBans;
}

bool ServerAccess::ParseAddress(const std::wstring& text, std::wstring* address)
{
    int parts = 0;
    int value = -1;
    for (size_t index = 0; index <= text.size(); ++index)
    {
        const wchar_t character = index < text.size() ? text[index] : L'.';
        if (character >= L'0' && character <= L'9')
        {
            value = (value < 0 ? 0 : value) * 10 + (character - L'0');
            if (value > 255)
                return false;
        }
        else if (character == L'.' && value >= 0)
        {
            ++parts;
            value = -1;
        }
        else
            return false;
    }
    if (parts != 4)
        return false;
    *address = text;
    return true;
}

int ServerAccess::MaxConnectionsPerAddress()
{
    return sMaxConnectionsPerAddress;
}

void ServerAccess::SetMaxConnectionsPerAddress(int count)
{
    sMaxConnectionsPerAddress = count;
}

int ServerAccess::MaxPendingConnections()
{
    return sMaxPendingConnections;
}

void ServerAccess::SetMaxPendingConnections(int count)
{
    sMaxPendingConnections = count;
}

ServerAccess::EConnectionVerdict ServerAccess::AdmitConnection(const std::wstring& address, bool loopback, int fromThisAddress,
                                                               int pending)
{
    if (sAddressBans.Contains(address))
        return eConnection_BannedAddress;
    if (loopback)
        return eConnection_Accepted;
    if (sMaxConnectionsPerAddress > 0 && fromThisAddress >= sMaxConnectionsPerAddress)
        return eConnection_TooManyFromAddress;
    if (sMaxPendingConnections > 0 && pending >= sMaxPendingConnections)
        return eConnection_TooManyPending;
    return eConnection_Accepted;
}

int ServerAccess::MaxPacketsPerSecond()
{
    return sMaxPacketsPerSecond;
}

void ServerAccess::SetMaxPacketsPerSecond(int count)
{
    sMaxPacketsPerSecond = count;
}

bool ServerAccess::TooManyPackets(unsigned int packetsInWindow)
{
    return sMaxPacketsPerSecond > 0 && packetsInWindow > static_cast<unsigned int>(sMaxPacketsPerSecond) * (kPacketWindowTicks / 20);
}

bool ServerAccess::WhitelistOn()
{
    return sWhitelistOn;
}

void ServerAccess::SetWhitelistOn(bool on)
{
    sWhitelistOn = on;
}

void ServerAccess::SetWhitelistOnChanged(void (*changed)(bool on))
{
    sWhitelistOnChanged = changed;
}

void ServerAccess::ChangeWhitelistOn(bool on)
{
    sWhitelistOn = on;
    if (sWhitelistOnChanged != NULL)
        sWhitelistOnChanged(on);
}

int ServerAccess::MaxPlayers()
{
    return sMaxPlayers;
}

void ServerAccess::SetMaxPlayers(int count)
{
    sMaxPlayers = count;
}

ServerAccess::EVerdict ServerAccess::Admit(const std::wstring& id, const std::wstring& name, int playersOnline, std::wstring* reason)
{
    BanEntry ban;
    if (sBans.Find(id, name, &ban))
    {
        sBans.Resolve(id, name);
        *reason = ban.reason;
        return eVerdict_Banned;
    }
    if (sWhitelistOn)
    {
        if (!sWhitelist.Listed(id, name))
            return eVerdict_NotWhitelisted;
        sWhitelist.Resolve(id, name);
    }
    if (sMaxPlayers > 0 && playersOnline >= sMaxPlayers)
        return eVerdict_Full;
    return eVerdict_Admitted;
}
