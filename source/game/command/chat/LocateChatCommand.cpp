#include "game/command/chat/LocateChatCommand.hpp"

#include <math.h>

#include "game/command/CommandArguments.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/generation/LargeFeature.hpp"

using CommandArguments::Lower;
using CommandArguments::Number;
using CommandArguments::StripNamespace;

namespace
{
struct Structure
{
    const wchar_t* name;
    const std::wstring* feature;
};

// Java 1.14's names; the game keeps the desert and jungle pyramids, the swamp hut and the igloo as one feature, so each of
// those names finds the nearest of any of them
const Structure kStructures[] = {{L"buried_treasure", &LargeFeature::BURIED_TREASURE},
                                 {L"desert_pyramid", &LargeFeature::TEMPLE},
                                 {L"endcity", &LargeFeature::END_CITY},
                                 {L"fortress", &LargeFeature::FORTRESS},
                                 {L"igloo", &LargeFeature::TEMPLE},
                                 {L"jungle_pyramid", &LargeFeature::TEMPLE},
                                 {L"mansion", &LargeFeature::MANSION},
                                 {L"mineshaft", &LargeFeature::MINESHAFT},
                                 {L"monument", &LargeFeature::MONUMENT},
                                 {L"ocean_ruin", &LargeFeature::RUIN},
                                 {L"shipwreck", &LargeFeature::SHIPWRECK},
                                 {L"stronghold", &LargeFeature::STRONGHOLD},
                                 {L"swamp_hut", &LargeFeature::TEMPLE},
                                 {L"village", &LargeFeature::VILLAGE}};

std::wstring Names()
{
    std::wstring names;
    for (size_t index = 0; index < sizeof(kStructures) / sizeof(kStructures[0]); ++index)
        names += std::wstring(index == 0 ? L"" : L"|") + kStructures[index].name;
    return names;
}
}

LocateChatCommand::LocateChatCommand() : ChatCommand(L"locate", L"<structure>", Names(), ePermission_Operator)
{
}

bool LocateChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() != 2)
        return false;
    const std::wstring wanted = Lower(StripNamespace(words[1]));
    const std::wstring* feature = NULL;
    for (size_t index = 0; index < sizeof(kStructures) / sizeof(kStructures[0]) && feature == NULL; ++index)
    {
        if (wanted == kStructures[index].name)
            feature = kStructures[index].feature;
    }
    if (feature == NULL)
        return false;
    const BlockPos from(static_cast<int>(floor(source.x)), static_cast<int>(floor(source.y)), static_cast<int>(floor(source.z)));
    BlockPos* found = source.level->findNearestMapFeature(*feature, from, false);
    if (found == NULL)
    {
        Fail(source, L"Could not find that structure nearby");
        return true;
    }
    const double dx = found->x - from.x;
    const double dz = found->z - from.z;
    const int distance = static_cast<int>(floor(sqrt(dx * dx + dz * dz)));
    Tell(source, L"The nearest " + words[1] + L" is at [" + Number(found->x) + L", ~, " + Number(found->z) + L"] (" + Number(distance)
                     + L" blocks away)");
    delete found;
    return true;
}
