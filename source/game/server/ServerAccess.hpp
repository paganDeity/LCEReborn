#ifndef GAME_SERVER_SERVERACCESS_HPP
#define GAME_SERVER_SERVERACCESS_HPP

#include <string>
#include <vector>

#include "game/server/AccessList.hpp"

// a player the whitelist lets in
struct WhitelistEntry
{
    std::wstring id;
    std::wstring name;
};

// a player kept out, and why
struct BanEntry
{
    std::wstring id;
    std::wstring name;
    std::wstring reason;
};

// who may join a server: the whitelist, when it is on, and the bans. A player is named by UUID, or by name until they first join (AccessList).
namespace ServerAccess
{
enum EVerdict
{
    eVerdict_Admitted,
    eVerdict_Banned,
    eVerdict_NotWhitelisted,
    eVerdict_Full
};

enum EConnectionVerdict
{
    eConnection_Accepted,
    eConnection_BannedAddress,
    eConnection_TooManyFromAddress,
    eConnection_TooManyPending
};

AccessList<WhitelistEntry>& Whitelist();
AccessList<BanEntry>& Bans();
// the addresses kept out: an entry's id is the address as text (1.2.3.4), and it has no name
AccessList<BanEntry>& AddressBans();

bool WhitelistOn();
void SetWhitelistOn(bool on);
void SetWhitelistOnChanged(void (*changed)(bool on));
// turn it on or off and tell whoever keeps the setting
void ChangeWhitelistOn(bool on);

// the most players the server takes; 0 leaves it to the game's own limit
int MaxPlayers();
void SetMaxPlayers(int count);

// an address as the lists keep it, four numbers and three dots; false when the text is not one
bool ParseAddress(const std::wstring& text, std::wstring* address);

// how many connections one address may hold at once, and how many may be open without having joined; 0 is no limit
int MaxConnectionsPerAddress();
void SetMaxConnectionsPerAddress(int count);
int MaxPendingConnections();
void SetMaxPendingConnections(int count);

// whether a new connection from this address is taken, given how many this address already holds and how many connections have not joined
// yet. The host's own machine is never refused by the two counts.
EConnectionVerdict AdmitConnection(const std::wstring& address, bool loopback, int fromThisAddress, int pending);

// a connection is cut when it sends more than this many packets a second, averaged over the window; 0 is no limit
const int kPacketWindowTicks = 100;
int MaxPacketsPerSecond();
void SetMaxPacketsPerSecond(int count);
bool TooManyPackets(unsigned int packetsInWindow);

// whether a player with this UUID and name may join a server that has this many players; a ban gives its reason. A listed player whose name
// or UUID was missing has it filled in.
EVerdict Admit(const std::wstring& id, const std::wstring& name, int playersOnline, std::wstring* reason);
}

#endif
