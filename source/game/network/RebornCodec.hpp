#ifndef GAME_NETWORK_REBORNCODEC_HPP
#define GAME_NETWORK_REBORNCODEC_HPP

class PacketCodec;

// the client's side of a Reborn connection: its first write is the preamble and its hello, its first read the server's
// answer
PacketCodec* CreateRebornClientCodec();
// the server's side of a connection that may be Reborn: its first read tells the two protocols apart by the preamble,
// and a Legacy peer is refused unless acceptLegacy
PacketCodec* CreateRebornServerCodec(bool acceptLegacy);

#endif
