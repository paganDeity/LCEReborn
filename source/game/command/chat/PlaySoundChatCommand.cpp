#include "game/command/chat/PlaySoundChatCommand.hpp"

#include <math.h>

#include "game/client/audio/SoundEvent.hpp"
#include "game/client/audio/SoundEvents.hpp"
#include "game/client/audio/SoundSource.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/network/ClientboundSoundPacket.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/network/ServerGamePacketListenerImpl.hpp"

using CommandArguments::ParseNumber;
using CommandArguments::StripNamespace;

namespace
{
// Java's names for the sound categories, in the game's order
const wchar_t* const kSources[] = {L"master", L"music", L"record", L"weather", L"block", L"hostile", L"neutral", L"player", L"ambient", L"voice"};

bool ParseSource(const std::wstring& word, SoundSource::ESoundSource* source)
{
    for (size_t index = 0; index < sizeof(kSources) / sizeof(kSources[0]); ++index)
    {
        if (word == kSources[index])
        {
            *source = static_cast<SoundSource::ESoundSource>(index);
            return true;
        }
    }
    return false;
}
}

PlaySoundChatCommand::PlaySoundChatCommand()
    : ChatCommand(L"playsound", L"<sound> <source> <targets> [x y z] [volume] [pitch] [minVolume]",
                  L"text master|music|record|weather|block|hostile|neutral|player|ambient|voice targets coord coord coord number number number",
                  ePermission_Operator)
{
}

bool PlaySoundChatCommand::run(const CommandSource& source, const Words& words)
{
    const size_t count = words.size();
    if (count != 4 && count != 7 && count != 8 && count != 9 && count != 10)
        return false;
    const SoundEvent* sound = SoundEvents::byName(StripNamespace(words[1]));
    if (sound == NULL)
    {
        Fail(source, L"Unknown sound " + words[1]);
        return true;
    }
    SoundSource::ESoundSource category;
    if (!ParseSource(words[2], &category))
        return false;
    PlayerVector players;
    if (!ResolvePlayers(source, words, 3, players))
        return true;
    double position[3] = {source.x, source.y, source.z};
    if (count >= 7 && !ParsePosition(source, words, 4, false, position))
        return true;
    double numbers[3] = {1.0, 1.0, 0.0};
    const double lowest[3] = {0.0, 0.0, 0.0};
    const double highest[3] = {1.0e9, 2.0, 1.0};
    const wchar_t* const names[3] = {L"volume", L"pitch", L"minimum volume"};
    for (size_t index = 0; index < 3 && 7 + index < count; ++index)
    {
        if (!ParseNumber(words[7 + index], &numbers[index]) || numbers[index] < lowest[index] || numbers[index] > highest[index])
        {
            Fail(source, std::wstring(L"The ") + names[index] + L" is not a number in range: " + words[7 + index]);
            return true;
        }
    }
    // a sound is heard within sixteen blocks for each unit of volume above one; a farther player hears it at the minimum
    // volume, from just in front of them, or not at all
    const double range = numbers[0] > 1.0 ? 16.0 * numbers[0] : 16.0;
    int played = 0;
    for (size_t index = 0; index < players.size(); ++index)
    {
        ServerPlayer* player = players[index].get();
        double x = position[0], y = position[1], z = position[2];
        double volume = numbers[0];
        const double dx = x - player->mX, dy = y - player->mY, dz = z - player->mZ;
        const double distance = sqrt(dx * dx + dy * dy + dz * dz);
        if (distance > range)
        {
            if (numbers[2] <= 0.0)
                continue;
            x = player->mX + dx / distance * 2.0;
            y = player->mY + dy / distance * 2.0;
            z = player->mZ + dz / distance * 2.0;
            volume = numbers[2];
        }
        player->mConnection->send(boost::shared_ptr<ClientboundSoundPacket>(new ClientboundSoundPacket(
            sound, category, x, y, z, static_cast<float>(volume), static_cast<float>(numbers[1]))));
        ++played;
    }
    if (played == 0)
    {
        Fail(source, L"The player is too far away to hear the sound");
        return true;
    }
    Tell(source, L"Played sound " + words[1] + L" to " + Count(played, L"player", L"players"));
    return true;
}
