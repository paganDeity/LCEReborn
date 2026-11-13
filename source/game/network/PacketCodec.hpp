#ifndef GAME_NETWORK_PACKETCODEC_HPP
#define GAME_NETWORK_PACKETCODEC_HPP

#include <string>

#include <boost/shared_ptr.hpp>

class DataInputStream;
class DataOutputStream;
class Packet;

class PacketCodec
{
public:
    virtual ~PacketCodec() {}
    virtual boost::shared_ptr<Packet> Read(DataInputStream* input, bool isServer, unsigned int& packetsRead,
                                           bool& protocolError) = 0;
    virtual void Write(boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer,
                       unsigned int& packetsWritten, bool wide) = 0;
    // sends what Write held back; the connection calls it before each flush of its output
    virtual void Flush(DataOutputStream*) {}
    // the longest chat message this connection carries, which the chat box and the server's chat handling both use
    virtual unsigned int ChatLimit();
};

class LegacyPacketCodec : public PacketCodec
{
public:
    virtual boost::shared_ptr<Packet> Read(DataInputStream* input, bool isServer, unsigned int& packetsRead,
                                           bool& protocolError);
    virtual void Write(boost::shared_ptr<Packet> packet, DataOutputStream* output, bool isServer,
                       unsigned int& packetsWritten, bool wide);
};

// the codec a new connection owns and deletes: a mod's when one listens to packet_encode, then Reborn where the
// settings below allow it, the game's otherwise; local is the host's own player, isServer the side this process holds
PacketCodec* CreateCodec(bool local, bool isServer);

// which wire a remote connection may use; Legacy alone is the original game's. A server has the protocols it accepts,
// a client the one it speaks when it joins
namespace ConnectionProtocols
{
enum EProtocol
{
    eProtocol_Legacy = 1,
    eProtocol_Reborn = 2
};

void SetServerAccepts(int protocols);
void SetClientSpeaks(int protocol);
int ServerAccepts();
int ClientSpeaks();
// "legacy", "reborn", or both with a comma between; false when a word is neither
bool Parse(const std::string& text, int* protocols);
}

#endif
