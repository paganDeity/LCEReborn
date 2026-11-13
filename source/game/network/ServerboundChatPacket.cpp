#include "game/network/ServerboundChatPacket.hpp"

#include "game/core/DataInputStream.hpp"
#include "game/core/DataOutputStream.hpp"
#include "game/network/PacketListener.hpp"
#include "game/network/PacketRead.hpp"

ServerboundChatPacket::ServerboundChatPacket()
{
}

ServerboundChatPacket::ServerboundChatPacket(const std::wstring& message)
{
#if LCE_PLATFORM_PC
    // Parity Not Checked
    // the chat box already holds a message to what its connection carries
    const unsigned int kMaxLength = kLongLength;
#endif
    if (message.length() > kMaxLength)
    {
        this->message = message.substr(0, kMaxLength);
    }
    else
    {
        this->message = message;
    }
}

ServerboundChatPacket::~ServerboundChatPacket()
{
}

void ServerboundChatPacket::read(DataInputStream* input)
{
#if LCE_PLATFORM_PC
    // Parity Not Checked
    message = Packet::readUtf(input, PacketRead::ChatLimit());
#else
    message = Packet::readUtf(input, kMaxLength);
#endif
}

void ServerboundChatPacket::write(DataOutputStream* output)
{
    Packet::writeUtf(message, output);
}

void ServerboundChatPacket::handle(PacketListener* listener)
{
    listener->handleChat(shared_from_this());
}

int ServerboundChatPacket::getPacketId()
{
    return 222;
}

std::wstring ServerboundChatPacket::getMessage()
{
    return message;
}
