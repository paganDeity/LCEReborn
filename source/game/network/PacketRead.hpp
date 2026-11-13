#ifndef GAME_NETWORK_PACKETREAD_HPP
#define GAME_NETWORK_PACKETREAD_HPP

// How a packet's reader says the bytes it was given cannot be a real packet (a count no real packet has, a string longer than its field
// allows): it calls Refuse and returns, and Packet::readPacket, which asks TakeRefused once the reader is back, reports a protocol error,
// which ends the connection. The mark is per thread, as a connection reads on its own thread.
namespace PacketRead
{
void Refuse();
bool TakeRefused();
// set before a packet reads itself; a client takes over-long strings from its server, as the original game does, since
// the server writes some longer than their fields say (a chat line over 123 characters), and only the server refuses them
void Begin(bool isServer);
bool ForServer();
// the longest chat message the connection being read agreed to; its codec sets it before a read, and a connection
// that never does reads the Legacy 256
void SetChatLimit(unsigned int limit);
unsigned int ChatLimit();
// whether a packet number is one the game reads in this direction, so a framed stream can skip one it does not know
bool Known(int packetId, bool isServer);
}

#endif
