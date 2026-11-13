#include <stdio.h>

#include <string>
#include <vector>

#include "game/server/AccessList.hpp"
#include "game/data/NbtBudget.hpp"
#include "game/server/ServerAccess.hpp"
#include "game/server/ServerWatchdog.hpp"

namespace
{
int failures = 0;
int notified = 0;

void Check(bool ok, const char* what)
{
    printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok)
        ++failures;
}

void Count(const std::vector<WhitelistEntry>&)
{
    ++notified;
}
}

int main()
{
    AccessList<WhitelistEntry> list;
    list.SetChanged(Count);
    Check(list.Add(L"", L"Alex") && notified == 1, "a name alone is listed as pending and the change is told");
    Check(!list.Add(L"", L"alex") && notified == 1, "the same pending name again, in another case, changes nothing");
    Check(list.Listed(L"aa11", L"ALEX") && !list.Contains(L"aa11"), "a pending name lists whoever has it, by name only");
    Check(list.Resolve(L"aa11", L"Alex") && list.Contains(L"aa11") && notified == 2, "joining gives the pending entry its UUID");
    Check(!list.Listed(L"bb22", L"Alex"), "after that another player with the same name is not listed");
    Check(list.Resolve(L"aa11", L"Alexandra") && notified == 3, "a listed player's new name is kept");
    WhitelistEntry found;
    Check(list.FindByName(L"alexandra", &found) && found.id == L"aa11", "an entry is found by its name");
    Check(!list.Resolve(L"cc33", L"Nobody") && notified == 3, "a player who is not listed is not resolved");
    Check(list.Add(L"dd44", L"Steve") && !list.Add(L"dd44", L"Steve"), "a UUID is added once");
    Check(list.Remove(L"dd44", L"") && !list.Remove(L"dd44", L""), "and removed once");
    Check(list.Add(L"", L"Later") && list.Add(L"ee55", L"Later") && list.Entries().size() == 2,
          "adding a UUID with the name of a pending entry replaces the pending entry");

    std::wstring reason;
    Check(ServerAccess::Admit(L"aa11", L"Alex", 0, &reason) == ServerAccess::eVerdict_Admitted, "with the whitelist off anyone is admitted");
    ServerAccess::SetWhitelistOn(true);
    Check(ServerAccess::Admit(L"aa11", L"Alex", 0, &reason) == ServerAccess::eVerdict_NotWhitelisted, "with it on an unlisted player is not");
    ServerAccess::Whitelist().Add(L"", L"Alex");
    Check(ServerAccess::Admit(L"aa11", L"Alex", 0, &reason) == ServerAccess::eVerdict_Admitted && ServerAccess::Whitelist().Contains(L"aa11"),
          "a listed name is admitted and kept by UUID from then on");
    BanEntry ban;
    ban.id = L"aa11";
    ban.name = L"Alex";
    ban.reason = L"griefing";
    ServerAccess::Bans().Add(ban);
    Check(ServerAccess::Admit(L"aa11", L"Renamed", 0, &reason) == ServerAccess::eVerdict_Banned && reason == L"griefing",
          "a ban holds under a new name, wins over the whitelist and gives its reason");
    Check(ServerAccess::Admit(L"ff66", L"Alex", 0, &reason) != ServerAccess::eVerdict_Banned, "another player with the banned name is not banned");
    ServerAccess::SetWhitelistOn(false);
    ServerAccess::SetMaxPlayers(2);
    Check(ServerAccess::Admit(L"ff66", L"Sam", 1, &reason) == ServerAccess::eVerdict_Admitted
              && ServerAccess::Admit(L"ff66", L"Sam", 2, &reason) == ServerAccess::eVerdict_Full,
          "a full server refuses the next player");

    std::wstring address;
    Check(ServerAccess::ParseAddress(L"203.0.113.7", &address) && address == L"203.0.113.7", "an address is four numbers with dots");
    Check(!ServerAccess::ParseAddress(L"203.0.113", &address) && !ServerAccess::ParseAddress(L"1.2.3.256", &address)
              && !ServerAccess::ParseAddress(L"1..2.3", &address) && !ServerAccess::ParseAddress(L"a.b.c.d", &address)
              && !ServerAccess::ParseAddress(L"1.2.3.4.5", &address),
          "and nothing else is");
    ServerAccess::SetMaxConnectionsPerAddress(2);
    ServerAccess::SetMaxPendingConnections(3);
    Check(ServerAccess::AdmitConnection(L"203.0.113.7", false, 1, 0) == ServerAccess::eConnection_Accepted
              && ServerAccess::AdmitConnection(L"203.0.113.7", false, 2, 0) == ServerAccess::eConnection_TooManyFromAddress,
          "an address may hold only so many connections");
    Check(ServerAccess::AdmitConnection(L"203.0.113.7", false, 0, 3) == ServerAccess::eConnection_TooManyPending,
          "and only so many connections may be waiting to join");
    Check(ServerAccess::AdmitConnection(L"127.0.0.1", true, 9, 9) == ServerAccess::eConnection_Accepted,
          "the host's own machine is not held to the counts");
    BanEntry banned;
    banned.id = L"127.0.0.1";
    ServerAccess::AddressBans().Add(banned);
    Check(ServerAccess::AdmitConnection(L"127.0.0.1", true, 0, 0) == ServerAccess::eConnection_BannedAddress,
          "but a banned address is refused wherever it is");
    ServerAccess::SetMaxConnectionsPerAddress(0);
    Check(ServerAccess::AdmitConnection(L"203.0.113.7", false, 500, 0) == ServerAccess::eConnection_Accepted, "0 is no limit");

    ServerAccess::SetMaxPacketsPerSecond(100);
    Check(!ServerAccess::TooManyPackets(500) && ServerAccess::TooManyPackets(501), "a connection may send so many packets a second, averaged over the window");
    ServerAccess::SetMaxPacketsPerSecond(0);
    Check(!ServerAccess::TooManyPackets(4000000000u), "and any number when the limit is 0");

    Check(NbtBudget::Charge(1LL << 40), "with no budget open nothing is charged");
    NbtBudget::Open(100);
    Check(NbtBudget::Charge(60) && NbtBudget::Charge(40) && !NbtBudget::Charge(1), "an open budget covers what it holds and no more");
    Check(NbtBudget::Close(), "and closing it says it was spent");
    NbtBudget::Open(100);
    Check(!NbtBudget::Charge(-5) && NbtBudget::Close(), "a negative count spends the budget");
    NbtBudget::Open(100);
    Check(NbtBudget::Charge(100) && !NbtBudget::Close(), "a budget used exactly was not spent");

    ServerWatchdog::SetLimit(60);
    Check(!ServerWatchdog::Stalled(1000000), "a server that is not running is not stalled");
    ServerWatchdog::Beat(1000);
    Check(ServerWatchdog::StalledSeconds(31000) == 30 && !ServerWatchdog::Stalled(31000), "a loop that stood still for less than the limit is not stalled");
    Check(ServerWatchdog::Stalled(61000) && ServerWatchdog::StalledSeconds(61000) == 60, "one that stood still for the limit is");
    ServerWatchdog::Beat(61000);
    Check(!ServerWatchdog::Stalled(62000), "and a beat ends the stall");
    ServerWatchdog::Rest();
    Check(!ServerWatchdog::Stalled(9000000), "a server that stopped is not watched");
    ServerWatchdog::Beat(1000);
    ServerWatchdog::SetLimit(0);
    Check(!ServerWatchdog::Stalled(9000000), "a limit of 0 never acts");

    printf(failures == 0 ? "all passed\n" : "%d failed\n", failures);
    return failures == 0 ? 0 : 1;
}
