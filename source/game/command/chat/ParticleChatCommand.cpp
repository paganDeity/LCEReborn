#include "game/command/chat/ParticleChatCommand.hpp"

#include "game/client/render/ParticleType.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/server/ServerLevel.hpp"

using CommandArguments::ParseInt;
using CommandArguments::ParseNumber;
using CommandArguments::StripNamespace;

ParticleChatCommand::ParticleChatCommand()
    : ChatCommand(L"particle", L"<name> [x y z] [dx dy dz] [speed] [count] [force|normal]",
                  L"text coord coord coord number number number number number force|normal", ePermission_Operator)
{
}

bool ParticleChatCommand::run(const CommandSource& source, const Words& words)
{
    const size_t count = words.size();
    if (count != 2 && count != 5 && count != 8 && count != 9 && count != 10 && count != 11)
        return false;
    const std::wstring name = StripNamespace(words[1]);
    // the game's own particle names, which are the ones Java 1.12 used
    const ParticleType* type = ParticleType::byName(std::string(name.begin(), name.end()));
    if (type == NULL)
    {
        Fail(source, L"Unknown particle " + words[1]);
        return true;
    }
    if (type->getParamCount() > 0)
    {
        Fail(source, L"The particle " + words[1] + L" needs a block or an item, which this command does not take yet");
        return true;
    }
    double position[3] = {source.x, source.y, source.z};
    if (count >= 5 && !ParsePosition(source, words, 2, false, position))
        return true;
    double spread[3] = {0.0, 0.0, 0.0};
    for (size_t axis = 0; axis < 3 && count >= 8; ++axis)
    {
        if (!ParseNumber(words[5 + axis], &spread[axis]))
        {
            Fail(source, L"The spread must be a number, not " + words[5 + axis]);
            return true;
        }
    }
    double speed = 0.0;
    if (count >= 9 && (!ParseNumber(words[8], &speed) || speed < 0.0))
    {
        Fail(source, L"The speed must be a number of 0 or more, not " + words[8]);
        return true;
    }
    int amount = 1;
    if (count >= 10 && !ParseInt(words[9], 0, 1000, &amount))
    {
        Fail(source, L"The count must be a whole number from 0 to 1000, not " + words[9]);
        return true;
    }
    bool force = false;
    if (count == 11)
    {
        if (words[10] != L"force" && words[10] != L"normal")
            return false;
        force = words[10] == L"force";
    }
    ServerLevel* level = dynamic_cast<ServerLevel*>(source.level);
    if (level == NULL)
    {
        Fail(source, L"Particles can only be shown in a world the server runs");
        return true;
    }
    level->sendParticles(type, force, position[0], position[1], position[2], amount, spread[0], spread[1], spread[2], speed,
                         arrayWithLength<int>());
    Tell(source, L"Displaying " + words[1]);
    return true;
}
