#ifndef PLATFORM_PC_PCSERVERACCESS_HPP
#define PLATFORM_PC_PCSERVERACCESS_HPP

// The players a server keeps and its two limits, for the dedicated server and for a game hosted from the client alike: operators (ops.json),
// the whitelist (whitelist.json) and the bans (banned-players.json) beside server.properties in the save folder, and the white-list and
// max-players settings in server.properties. Start reads them and keeps them saved as the op, whitelist, ban and pardon commands change them.
void PcServerAccessStart();

#endif
