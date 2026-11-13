#include "game/command/chat/ExecuteChatCommand.hpp"

#include <math.h>

#include <boost/pointer_cast.hpp>

#include "game/command/ChatCommands.hpp"
#include "game/command/CommandArguments.hpp"
#include "game/command/CommandLimits.hpp"
#include "game/command/TargetSelector.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/NbtPath.hpp"
#include "game/data/NbtSelection.hpp"
#include "game/command/EntityData.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerLevel.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockEntity.hpp"
#include "game/world/block/BlockPos.hpp"
#include "game/world/block/BlockState.hpp"

using CommandArguments::Lower;
using CommandArguments::Number;

namespace
{

}

ExecuteChatCommand::ExecuteChatCommand()
    : ChatCommand(L"execute", L"<as|at|positioned|align|in|rotated|facing|if|unless|run> ...", L"execute", ePermission_Operator)
{
}

bool ExecuteChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 2)
        return false;
    std::vector<CommandSource> sources(1, source);
    std::vector<CommandSource> next;
    bool tested = false;
    size_t index = 1;
    while (index < words.size())
    {
        const std::wstring part = Lower(words[index]);
        tested = false;
        if (part == L"run")
        {
            if (index + 1 >= words.size())
                return false;
            const std::wstring name = Lower(words[index + 1]);
            ChatCommand* command = ChatCommands::Find(name);
            if (command == NULL)
            {
                Fail(source, L"Unknown command /" + name + L". Type /help for the commands.");
                return true;
            }
            if (!command->canUse(source.permission))
            {
                Fail(source, L"You don't have permission to use this command.");
                return true;
            }
            const Words rest(words.begin() + index + 1, words.end());
            for (size_t run = 0; run < sources.size(); ++run)
            {
                if (!command->run(sources[run], rest))
                {
                    Fail(source, command->usageError());
                    return true;
                }
            }
            return true;
        }
        if (part == L"as" || part == L"at")
        {
            if (index + 1 >= words.size())
                return false;
            const bool as = part == L"as";
            if (!Fork(source, sources, words[index + 1], !as, !as, as, next))
                return true;
            sources.swap(next);
            index += 2;
            continue;
        }
        if (part == L"positioned")
        {
            if (index + 2 < words.size() && Lower(words[index + 1]) == L"as")
            {
                if (!Fork(source, sources, words[index + 2], true, false, false, next))
                    return true;
                sources.swap(next);
                index += 3;
                continue;
            }
            if (index + 3 >= words.size())
                return false;
            for (size_t at = 0; at < sources.size(); ++at)
            {
                double result[3];
                const double origin[3] = {sources[at].x, sources[at].y, sources[at].z};
                std::wstring error;
                if (!CommandArguments::ParsePosition(words, index + 1, origin, sources[at].yRot, sources[at].xRot, true,
                                                     result, &error))
                {
                    Fail(source, error);
                    return true;
                }
                sources[at].x = result[0];
                sources[at].y = result[1];
                sources[at].z = result[2];
            }
            index += 4;
            continue;
        }
        if (part == L"align")
        {
            if (index + 1 >= words.size())
                return false;
            const std::wstring axes = Lower(words[index + 1]);
            for (size_t at = 0; at < sources.size(); ++at)
            {
                if (axes.find(L'x') != std::wstring::npos)
                    sources[at].x = floor(sources[at].x);
                if (axes.find(L'y') != std::wstring::npos)
                    sources[at].y = floor(sources[at].y);
                if (axes.find(L'z') != std::wstring::npos)
                    sources[at].z = floor(sources[at].z);
            }
            index += 2;
            continue;
        }
        if (part == L"in")
        {
            if (index + 1 >= words.size())
                return false;
            const std::wstring dimension = CommandArguments::StripNamespace(words[index + 1]);
            int id = 0;
            if (dimension == L"overworld")
                id = 0;
            else if (dimension == L"the_nether")
                id = -1;
            else if (dimension == L"the_end")
                id = 1;
            else
            {
                Fail(source, L"Unknown dimension " + words[index + 1] + L".");
                return true;
            }
            Level* level = gMinecraftServer->getLevel(id);
            if (level == NULL)
            {
                Fail(source, L"That dimension is not loaded.");
                return true;
            }
            for (size_t at = 0; at < sources.size(); ++at)
                sources[at].level = level;
            index += 2;
            continue;
        }
        if (part == L"rotated")
        {
            if (index + 1 >= words.size())
                return false;
            if (Lower(words[index + 1]) == L"as")
            {
                if (index + 2 >= words.size() || !Fork(source, sources, words[index + 2], false, true, false, next))
                    return index + 2 < words.size() ? true : false;
                sources.swap(next);
                index += 3;
                continue;
            }
            if (index + 2 >= words.size())
                return false;
            for (size_t at = 0; at < sources.size(); ++at)
            {
                double yaw = 0.0, pitch = 0.0;
                const std::wstring& yawWord = words[index + 1];
                const std::wstring& pitchWord = words[index + 2];
                const bool yawRelative = !yawWord.empty() && yawWord[0] == L'~';
                const bool pitchRelative = !pitchWord.empty() && pitchWord[0] == L'~';
                if ((yawRelative && yawWord.size() > 1 ? !CommandArguments::ParseNumber(yawWord.substr(1), &yaw)
                                                      : (!yawRelative && !CommandArguments::ParseNumber(yawWord, &yaw)))
                    || (pitchRelative && pitchWord.size() > 1 ? !CommandArguments::ParseNumber(pitchWord.substr(1), &pitch)
                                                              : (!pitchRelative && !CommandArguments::ParseNumber(pitchWord, &pitch))))
                {
                    Fail(source, L"A rotation is a yaw and a pitch in degrees, or ~ for relative.");
                    return true;
                }
                sources[at].yRot = static_cast<float>(yawRelative ? sources[at].yRot + yaw : yaw);
                sources[at].xRot = static_cast<float>(pitchRelative ? sources[at].xRot + pitch : pitch);
            }
            index += 3;
            continue;
        }
        if (part == L"facing")
        {
            if (index + 1 >= words.size())
                return false;
            const bool entityTarget = Lower(words[index + 1]) == L"entity";
            if (entityTarget && index + 3 >= words.size())
                return false;
            const size_t used = entityTarget ? 4 : 4;
            if (!entityTarget && index + 3 >= words.size())
                return false;
            for (size_t at = 0; at < sources.size(); ++at)
            {
                double toX = 0.0, toY = 0.0, toZ = 0.0;
                if (entityTarget)
                {
                    EntityList found;
                    std::wstring error;
                    if (!TargetSelector::Select(sources[at], words[index + 2], found, &error) || found.empty())
                    {
                        Fail(source, error.empty() ? std::wstring(L"No entity was found.") : error);
                        return true;
                    }
                    const bool eyes = Lower(words[index + 3]) == L"eyes";
                    toX = found[0]->mX;
                    toY = found[0]->mY + (eyes ? found[0]->getEyeHeight() : 0.0);
                    toZ = found[0]->mZ;
                }
                else
                {
                    double result[3];
                    const double origin[3] = {sources[at].x, sources[at].y, sources[at].z};
                    std::wstring error;
                    if (!CommandArguments::ParsePosition(words, index + 1, origin, sources[at].yRot, sources[at].xRot, false,
                                                         result, &error))
                    {
                        Fail(source, error);
                        return true;
                    }
                    toX = result[0];
                    toY = result[1];
                    toZ = result[2];
                }
                const double dx = toX - sources[at].x;
                const double dy = toY - sources[at].y;
                const double dz = toZ - sources[at].z;
                const double flat = sqrt(dx * dx + dz * dz);
                const double radians = 3.14159265358979323846 / 180.0;
                sources[at].yRot = static_cast<float>(atan2(dz, dx) / radians - 90.0);
                sources[at].xRot = static_cast<float>(-(atan2(dy, flat) / radians));
            }
            index += used;
            continue;
        }
        if (part == L"if" || part == L"unless")
        {
            const bool wanted = part == L"if";
            next.clear();
            size_t length = 0;
            for (size_t test = 0; test < sources.size(); ++test)
            {
                bool passed = false;
                if (!Condition(source, sources[test], words, index + 1, &length, &passed))
                    return length == 0 ? false : true;
                if (passed == wanted)
                    next.push_back(sources[test]);
            }
            if (sources.empty())
            {
                size_t probe = 0;
                bool ignored = false;
                CommandSource none = source;
                if (!Condition(source, none, words, index + 1, &probe, &ignored) && probe == 0)
                    return false;
                length = probe;
            }
            sources.swap(next);
            tested = true;
            index += 1 + length;
            continue;
        }
        return false;
    }
    if (!tested)
        return false;
    if (sources.empty())
        Fail(source, L"Test failed");
    else
        Tell(source, L"Test passed, count: " + Number(static_cast<int>(sources.size())));
    return true;
}

bool ExecuteChatCommand::Condition(const CommandSource& source, const CommandSource& at, const Words& words, size_t index,
                                   size_t* length, bool* passed)
{
    *length = 0;
    *passed = false;
    if (index >= words.size())
        return false;
    const std::wstring kind = Lower(words[index]);
    if (kind == L"entity")
    {
        if (index + 1 >= words.size())
            return false;
        *length = 2;
        EntityList found;
        std::wstring error;
        if (!TargetSelector::Select(at, words[index + 1], found, &error))
        {
            Fail(source, error);
            return false;
        }
        *passed = !found.empty();
        return true;
    }
    if (kind == L"block")
    {
        if (index + 4 >= words.size())
            return false;
        *length = 5;
        BlockPos pos;
        CommandSource here = at;
        if (!ParseBlockPos(here, words, index + 1, &pos))
            return false;
        Block* block = FindBlock(source, words[index + 4]);
        if (block == NULL)
            return false;
        int data = -1;
        if (index + 5 < words.size() && CommandArguments::ParseInt(words[index + 5], 0, 15, &data))
            *length = 6;
        if (!at.level->hasChunkAt(pos))
            return true;
        const BlockState* state = at.level->getBlockState(pos);
        *passed = state->getBlock() == block
                  && (data < 0 || state->getBlock()->convertBlockStateToLegacyData(state) == data);
        return true;
    }
    if (kind == L"blocks")
    {
        if (index + 10 >= words.size())
            return false;
        *length = 11;
        BlockPos from, to, destination;
        if (!ParseBlockPos(at, words, index + 1, &from) || !ParseBlockPos(at, words, index + 4, &to)
            || !ParseBlockPos(at, words, index + 7, &destination))
            return false;
        const std::wstring mode = Lower(words[index + 10]);
        if (mode != L"all" && mode != L"masked")
            return false;
        const int minX = from.x < to.x ? from.x : to.x, maxX = from.x < to.x ? to.x : from.x;
        const int minY = from.y < to.y ? from.y : to.y, maxY = from.y < to.y ? to.y : from.y;
        const int minZ = from.z < to.z ? from.z : to.z, maxZ = from.z < to.z ? to.z : from.z;
        if ((maxX - minX + 1) * (maxY - minY + 1) * (maxZ - minZ + 1) > CommandLimits::CompareBlocks())
        {
            Fail(source, L"That area is too big (the limit is " + Number(CommandLimits::CompareBlocks()) + L" blocks).");
            return false;
        }
        *passed = true;
        for (int x = minX; x <= maxX && *passed; ++x)
            for (int y = minY; y <= maxY && *passed; ++y)
                for (int z = minZ; z <= maxZ && *passed; ++z)
                {
                    const BlockPos first(x, y, z);
                    const BlockPos second(destination.x + x - minX, destination.y + y - minY, destination.z + z - minZ);
                    const BlockState* a = at.level->getBlockState(first);
                    const BlockState* b = at.level->getBlockState(second);
                    if (mode == L"masked" && Block::getId(a->getBlock()) == 0)
                        continue;
                    *passed = a->getBlock() == b->getBlock()
                              && a->getBlock()->convertBlockStateToLegacyData(a) == b->getBlock()->convertBlockStateToLegacyData(b);
                }
        return true;
    }
    if (kind == L"data")
    {
        if (index + 2 >= words.size())
            return false;
        const std::wstring holder = Lower(words[index + 1]);
        CompoundTag* root = NULL;
        std::wstring path;
        if (holder == L"entity")
        {
            if (index + 3 >= words.size())
                return false;
            *length = 4;
            EntityList found;
            std::wstring error;
            if (!TargetSelector::Select(at, words[index + 2], found, &error))
            {
                Fail(source, error);
                return false;
            }
            if (found.empty())
                return true;
            root = EntityData::Read(found[0]);
            path = words[index + 3];
        }
        else if (holder == L"block")
        {
            if (index + 5 >= words.size())
                return false;
            *length = 6;
            BlockPos pos;
            if (!ParseBlockPos(at, words, index + 2, &pos))
                return false;
            boost::shared_ptr<BlockEntity> blockEntity = at.level->getBlockEntity(pos);
            if (!blockEntity)
                return true;
            root = blockEntity->save(new CompoundTag());
            path = words[index + 5];
        }
        else
            return false;
        NbtSelection selected;
        std::wstring error;
        *passed = NbtPath::select(root, path, &selected, &error) && !selected.empty();
        delete root;
        return true;
    }
    return false;
}

bool ExecuteChatCommand::Fork(const CommandSource& source, const std::vector<CommandSource>& from,
                              const std::wstring& word, bool moveTo, bool rotate, bool becomeEntity,
                              std::vector<CommandSource>& to)
{
    to.clear();
    for (size_t index = 0; index < from.size(); ++index)
    {
        EntityList found;
        std::wstring error;
        if (!TargetSelector::Select(from[index], word, found, &error))
        {
            Fail(source, error);
            return false;
        }
        for (size_t entity = 0; entity < found.size(); ++entity)
        {
            CommandSource next = from[index];
            const EntityPtr& target = found[entity];
            if (becomeEntity)
            {
                next.entity = target;
                next.output = boost::dynamic_pointer_cast<ServerPlayer>(target);
            }
            if (moveTo)
            {
                next.level = target->mLevel;
                next.x = target->mX;
                next.y = target->mY;
                next.z = target->mZ;
            }
            if (rotate)
            {
                next.yRot = target->mYRot;
                next.xRot = target->mXRot;
            }
            to.push_back(next);
            if (to.size() > static_cast<size_t>(CommandLimits::ExecuteForks()))
            {
                Fail(source,
                     L"That would run the command more than " + Number(static_cast<int>(static_cast<size_t>(CommandLimits::ExecuteForks()))) + L" times.");
                return false;
            }
        }
    }
    return true;
}
