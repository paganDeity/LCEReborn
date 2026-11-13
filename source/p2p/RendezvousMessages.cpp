#include "p2p/RendezvousMessages.hpp"

#include "p2p/P2pWriter.hpp"
#include "p2p/RendezvousProtocol.hpp"

namespace p2p
{
namespace rendezvous
{
std::vector<uint8_t> Messages::Register(const std::vector<PublicKey>& allowed)
{
    Writer writer;
    writer.U8(eRequest_Register);
    writer.U16(static_cast<unsigned int>(allowed.size()));
    for (size_t index = 0; index < allowed.size(); ++index)
        writer.Key(allowed[index]);
    return writer.Data();
}

std::vector<uint8_t> Messages::Publish(const std::vector<uint8_t>& record)
{
    Writer writer;
    writer.U8(eRequest_Publish);
    writer.Bytes(record.empty() ? NULL : &record[0], record.size());
    return writer.Data();
}

std::vector<uint8_t> Messages::Unpublish()
{
    Writer writer;
    writer.U8(eRequest_Unpublish);
    return writer.Data();
}

std::vector<uint8_t> Messages::List(unsigned int offset)
{
    Writer writer;
    writer.U8(eRequest_List);
    writer.U16(offset);
    return writer.Data();
}

std::vector<uint8_t> Messages::Connect(const PublicKey& peer)
{
    Writer writer;
    writer.U8(eRequest_Connect);
    writer.Key(peer);
    return writer.Data();
}

std::vector<uint8_t> Messages::Relay(const PublicKey& peer, const uint8_t* data, size_t size)
{
    Writer writer;
    writer.U8(eRequest_Relay);
    writer.Key(peer);
    writer.U16(static_cast<unsigned int>(size));
    writer.Bytes(data, size);
    return writer.Data();
}

std::vector<uint8_t> Messages::Lookup(const std::string& code)
{
    Writer writer;
    writer.U8(eRequest_Lookup);
    writer.Text(code);
    return writer.Data();
}

std::vector<uint8_t> Messages::Reflect()
{
    Writer writer;
    writer.U8(eRequest_Reflect);
    return writer.Data();
}

std::vector<uint8_t> Messages::Invite(const PublicKey& peer)
{
    Writer writer;
    writer.U8(eRequest_Invite);
    writer.Key(peer);
    return writer.Data();
}

std::vector<uint8_t> Messages::Answer(unsigned int kind, unsigned int request)
{
    Writer writer;
    writer.U8(kind);
    writer.U8(request);
    return writer.Data();
}

std::vector<uint8_t> Messages::Lobbies(const std::vector<std::vector<uint8_t> >& records, bool more)
{
    Writer writer;
    writer.U8(eReply_Lobbies);
    writer.U8(static_cast<unsigned int>(records.size()));
    writer.U8(more ? 1 : 0);
    for (size_t index = 0; index < records.size(); ++index)
    {
        writer.U16(static_cast<unsigned int>(records[index].size()));
        writer.Bytes(&records[index][0], records[index].size());
    }
    return writer.Data();
}

std::vector<uint8_t> Messages::Punch(const PublicKey& peer, const Endpoint& address)
{
    Writer writer;
    writer.U8(eReply_Punch);
    writer.Key(peer);
    writer.U32(address.ip);
    writer.U16(address.port);
    return writer.Data();
}

std::vector<uint8_t> Messages::Found(const PublicKey& peer)
{
    Writer writer;
    writer.U8(eReply_Found);
    writer.Key(peer);
    return writer.Data();
}

std::vector<uint8_t> Messages::Reflected(const Endpoint& address)
{
    Writer writer;
    writer.U8(eReply_Reflected);
    writer.U32(address.ip);
    writer.U16(address.port);
    return writer.Data();
}

std::vector<uint8_t> Messages::Invited(const std::vector<uint8_t>& record)
{
    Writer writer;
    writer.U8(eReply_Invited);
    writer.Bytes(&record[0], record.size());
    return writer.Data();
}

std::vector<uint8_t> Messages::Relayed(const PublicKey& peer, const uint8_t* data, size_t size)
{
    Writer writer;
    writer.U8(eReply_Relayed);
    writer.Key(peer);
    writer.Bytes(data, size);
    return writer.Data();
}
}
}
