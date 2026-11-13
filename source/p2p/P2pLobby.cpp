#include "p2p/P2pLobby.hpp"

#include "p2p/P2pReader.hpp"
#include "p2p/P2pWriter.hpp"

namespace p2p
{
namespace
{
const char kLabel[] = "lce-p2p lobby v1";

void Body(const Lobby& lobby, Writer& writer)
{
    writer.Key(lobby.owner);
    writer.U8(lobby.visibility);
    writer.U8(lobby.maxPlayers);
    writer.U8(lobby.players);
    writer.U8(lobby.protocol);
    writer.U16(lobby.version);
    writer.U16(lobby.port);
    writer.Text(lobby.name);
    writer.Text(lobby.mode);
}

std::vector<uint8_t> SigningInput(const Writer& body)
{
    std::vector<uint8_t> input(kLabel, kLabel + sizeof(kLabel) - 1);
    input.insert(input.end(), body.Data().begin(), body.Data().end());
    return input;
}
}

std::vector<uint8_t> Lobby::Sign(const Identity& owner, Lobby lobby)
{
    lobby.owner = owner.Public();
    Writer writer;
    Body(lobby, writer);
    const std::vector<uint8_t> input = SigningInput(writer);
    uint8_t signature[Identity::kSignatureSize];
    owner.Sign(&input[0], input.size(), signature);
    writer.Bytes(signature, sizeof(signature));
    return writer.Data();
}

bool Lobby::Read(const uint8_t* data, size_t size, Lobby& lobby, size_t* used)
{
    Reader reader(data, size);
    Lobby result;
    result.owner = reader.Key();
    result.visibility = reader.U8();
    result.maxPlayers = reader.U8();
    result.players = reader.U8();
    result.protocol = reader.U8();
    result.version = reader.U16();
    result.port = reader.U16();
    result.name = reader.Text(kMaxName);
    result.mode = reader.Text(kMaxMode);
    if (!reader.Ok() || result.visibility > eVisibility_Invite || result.protocol == 0 || result.players > result.maxPlayers)
        return false;
    const size_t bodySize = static_cast<size_t>(reader.Position() - data);
    uint8_t signature[Identity::kSignatureSize];
    if (!reader.Bytes(signature, sizeof(signature)))
        return false;
    Writer body;
    Body(result, body);
    const std::vector<uint8_t> input = SigningInput(body);
    if (body.Data().size() != bodySize || !Identity::Verify(result.owner, &input[0], input.size(), signature))
        return false;
    lobby = result;
    if (used != NULL)
        *used = bodySize + Identity::kSignatureSize;
    return true;
}
}
