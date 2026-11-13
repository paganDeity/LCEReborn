#ifndef P2P_RENDEZVOUSPROTOCOL_HPP
#define P2P_RENDEZVOUSPROTOCOL_HPP

#include <stddef.h>
#include <stdint.h>

#include "p2p/P2pEndpoint.hpp"

namespace p2p
{
namespace rendezvous
{
enum EDatagram
{
    eDatagram_Handshake = 1,
    eDatagram_Session = 2
};

enum ERequest
{
    eRequest_Register = 1,
    eRequest_Publish = 2,
    eRequest_Unpublish = 3,
    eRequest_List = 4,
    eRequest_Connect = 5,
    eRequest_Relay = 6,
    eRequest_Lookup = 7,
    eRequest_Reflect = 8,
    eRequest_Invite = 9
};

enum EReply
{
    eReply_Ok = 0x81,
    eReply_Denied = 0x82,
    eReply_Lobbies = 0x83,
    eReply_Punch = 0x84,
    eReply_Relayed = 0x85,
    eReply_Found = 0x86,
    eReply_Reflected = 0x87,
    eReply_Invited = 0x88,
    eReply_InUse = 0x89
};

const size_t kHeaderSize = 9;
const size_t kMaxAllowed = 512;
const size_t kMaxRelay = 1000;
const size_t kLobbiesPerReply = 6;
}
}

#endif
