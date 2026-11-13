#include "game/command/TargetSelector.hpp"

#include <math.h>
#include <wctype.h>

#include <algorithm>
#include <set>

#include <boost/pointer_cast.hpp>

#include "game/command/CommandArguments.hpp"
#include "game/command/EntityData.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/NbtMatch.hpp"
#include "game/data/StringNbtReader.hpp"
#include "game/core/EntityType.hpp"
#include "game/core/GameType.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/entity/mob/EntityIO.hpp"
#include "game/server/MinecraftServer.hpp"
#include "game/server/ServerPlayer.hpp"
#include "game/util/Random.hpp"
#include "game/util/ResourceLocation.hpp"
#include "game/world/Level.hpp"
#include "game/world/generation/PlayerList.hpp"

namespace
{
struct Filter
{
    bool hasType;
    bool typeNegated;
    std::wstring type;
    bool hasName;
    bool nameNegated;
    std::wstring name;
    bool hasMode;
    bool modeNegated;
    int mode;
    int minimumLevel;
    int maximumLevel;
    double minimumRadius;
    double maximumRadius;
    int count;
    bool hasCount;
    double x;
    double y;
    double z;
    bool positioned;
    bool hasVolume;
    double dx;
    double dy;
    double dz;
    int sort;
    std::vector<std::wstring> tags;
    std::vector<std::wstring> notTags;
    bool noTags;
    bool anyTag;
    std::vector<CompoundTag*> nbt;
    std::vector<bool> nbtNegated;
    bool hasXRotation;
    double minimumXRotation;
    double maximumXRotation;
    bool hasYRotation;
    double minimumYRotation;
    double maximumYRotation;

    Filter()
        : sort(0), noTags(false), anyTag(false), hasXRotation(false), minimumXRotation(0.0), maximumXRotation(0.0),
          hasYRotation(false), minimumYRotation(0.0), maximumYRotation(0.0)
    {
    }

    ~Filter()
    {
        for (size_t index = 0; index < nbt.size(); ++index)
            delete nbt[index];
    }

private:
    Filter(const Filter&);
    Filter& operator=(const Filter&);
};

enum ESort
{
    eSort_Default,
    eSort_Nearest,
    eSort_Furthest,
    eSort_Random,
    eSort_Arbitrary
};

std::wstring Unquote(const std::wstring& value)
{
    if (value.size() >= 2 && (value[0] == L'"' || value[0] == L'\'') && value[value.size() - 1] == value[0])
        return value.substr(1, value.size() - 2);
    return value;
}

bool ParseRange(const std::wstring& value, double* minimum, double* maximum)
{
    const size_t dots = value.find(L"..");
    if (dots == std::wstring::npos)
    {
        if (!CommandArguments::ParseNumber(value, minimum))
            return false;
        *maximum = *minimum;
        return true;
    }
    const std::wstring low = value.substr(0, dots);
    const std::wstring high = value.substr(dots + 2);
    if (low.empty() && high.empty())
        return false;
    *minimum = -1.0e30;
    *maximum = 1.0e30;
    if (!low.empty() && !CommandArguments::ParseNumber(low, minimum))
        return false;
    if (!high.empty() && !CommandArguments::ParseNumber(high, maximum))
        return false;
    return *minimum <= *maximum;
}

std::vector<std::wstring> SplitArguments(const std::wstring& text)
{
    std::vector<std::wstring> parts;
    std::wstring current;
    int depth = 0;
    wchar_t quote = 0;
    for (size_t index = 0; index < text.size(); ++index)
    {
        const wchar_t character = text[index];
        if (quote != 0)
        {
            current += character;
            if (character == L'\\' && index + 1 < text.size())
                current += text[++index];
            else if (character == quote)
                quote = 0;
            continue;
        }
        if (character == L'"' || character == L'\'')
            quote = character;
        else if (character == L'{' || character == L'[')
            ++depth;
        else if (character == L'}' || character == L']')
            --depth;
        if (character == L',' && depth == 0)
        {
            parts.push_back(current);
            current.clear();
            continue;
        }
        current += character;
    }
    parts.push_back(current);
    return parts;
}

struct Ranked
{
    boost::shared_ptr<Entity> entity;
    double distance;
};

bool Nearer(const Ranked& left, const Ranked& right)
{
    return left.distance < right.distance;
}

bool Farther(const Ranked& left, const Ranked& right)
{
    return left.distance > right.distance;
}

bool ParseMode(const std::wstring& value, int* mode)
{
    const std::wstring lower = CommandArguments::Lower(value);
    if (lower == L"s" || lower == L"survival" || lower == L"0")
        *mode = 0;
    else if (lower == L"c" || lower == L"creative" || lower == L"1")
        *mode = 1;
    else if (lower == L"a" || lower == L"adventure" || lower == L"2")
        *mode = 2;
    else if (lower == L"sp" || lower == L"spectator" || lower == L"3")
        *mode = 3;
    else
        return false;
    return true;
}

bool ParseArguments(const std::wstring& text, const CommandSource& source, Filter* filter,
                    std::wstring* error)
{
    const std::vector<std::wstring> pairs = SplitArguments(text);
    for (size_t position = 0; position < pairs.size(); ++position)
    {
        const std::wstring pair = pairs[position];
        if (pair.empty())
            continue;
        const size_t equals = pair.find(L'=');
        if (equals == std::wstring::npos)
        {
            *error = L"Selector arguments are written as key=value.";
            return false;
        }
        const std::wstring key = CommandArguments::Lower(pair.substr(0, equals));
        std::wstring value = pair.substr(equals + 1);
        const bool negated = !value.empty() && value[0] == L'!';
        if (negated)
            value = value.substr(1);
        double number = 0.0;
        int integer = 0;
        if (key == L"type")
        {
            filter->hasType = true;
            filter->typeNegated = negated;
            filter->type = CommandArguments::StripNamespace(value);
            if (filter->type == L"lightningbolt")
                filter->type = L"lightning_bolt";
            if (filter->type != L"player" && filter->type != L"lightning_bolt"
                && !EntityIO::isValidEntityType(ResourceLocation(filter->type)))
            {
                *error = L"Unknown entity type " + value + L".";
                return false;
            }
        }
        else if (key == L"name")
        {
            filter->hasName = true;
            filter->nameNegated = negated;
            filter->name = Unquote(value);
        }
        else if (key == L"limit" && !negated && CommandArguments::ParseInt(value, 1, 1000000, &integer))
        {
            filter->count = integer;
            filter->hasCount = true;
        }
        else if (key == L"sort" && !negated)
        {
            const std::wstring order = CommandArguments::Lower(value);
            if (order == L"nearest")
                filter->sort = eSort_Nearest;
            else if (order == L"furthest")
                filter->sort = eSort_Furthest;
            else if (order == L"random")
                filter->sort = eSort_Random;
            else if (order == L"arbitrary")
                filter->sort = eSort_Arbitrary;
            else
            {
                *error = L"Unknown sort type " + value + L".";
                return false;
            }
        }
        else if (key == L"tag")
        {
            if (value.empty())
            {
                if (negated)
                    filter->anyTag = true;
                else
                    filter->noTags = true;
            }
            else
                (negated ? filter->notTags : filter->tags).push_back(Unquote(value));
        }
        else if (key == L"nbt")
        {
            std::wstring problem;
            Tag* parsed = StringNbtReader::parse(value, &problem);
            if (parsed == NULL || parsed->getId() != Tag::TAG_COMPOUND)
            {
                delete parsed;
                *error = L"The nbt argument needs a compound, like nbt={OnGround:1b}.";
                return false;
            }
            filter->nbt.push_back(static_cast<CompoundTag*>(parsed));
            filter->nbtNegated.push_back(negated);
        }
        else if (key == L"gamemode")
        {
            if (!ParseMode(value, &filter->mode))
            {
                *error = L"Unknown game mode " + value + L".";
                return false;
            }
            filter->hasMode = true;
            filter->modeNegated = negated;
        }
        else if (key == L"distance" && !negated && ParseRange(value, &number, &filter->maximumRadius))
        {
            filter->minimumRadius = number < 0.0 ? 0.0 : number;
            if (filter->maximumRadius >= 1.0e29)
                filter->maximumRadius = -1.0;
            filter->positioned = true;
        }
        else if (key == L"level" && !negated && ParseRange(value, &number, &filter->minimumRadius))
        {
            filter->minimumLevel = static_cast<int>(number < 0.0 ? 0.0 : number);
            filter->maximumLevel = filter->minimumRadius >= 1.0e29 ? -1 : static_cast<int>(filter->minimumRadius);
            filter->minimumRadius = 0.0;
        }
        else if ((key == L"x_rotation" || key == L"y_rotation") && !negated)
        {
            double low = 0.0, high = 0.0;
            if (!ParseRange(value, &low, &high))
            {
                *error = L"A rotation is a number or a range like -45..45.";
                return false;
            }
            if (key == L"x_rotation")
            {
                filter->hasXRotation = true;
                filter->minimumXRotation = low;
                filter->maximumXRotation = high;
            }
            else
            {
                filter->hasYRotation = true;
                filter->minimumYRotation = low;
                filter->maximumYRotation = high;
            }
        }
        else if (key == L"m")
        {
            if (!ParseMode(value, &filter->mode))
            {
                *error = L"Unknown game mode " + value + L".";
                return false;
            }
            filter->hasMode = true;
            filter->modeNegated = negated;
        }
        else if ((key == L"l" || key == L"lm") && !negated && CommandArguments::ParseInt(value, 0, 1000000, &integer))
        {
            if (key == L"l")
                filter->maximumLevel = integer;
            else
                filter->minimumLevel = integer;
        }
        else if ((key == L"r" || key == L"rm") && !negated && CommandArguments::ParseNumber(value, &number)
                 && number >= 0.0)
        {
            if (key == L"r")
                filter->maximumRadius = number;
            else
                filter->minimumRadius = number;
            filter->positioned = true;
        }
        else if (key == L"c" && !negated && CommandArguments::ParseInt(value, -1000000, 1000000, &integer)
                 && integer != 0)
        {
            filter->count = integer;
            filter->hasCount = true;
        }
        else if ((key == L"x" || key == L"y" || key == L"z") && !negated)
        {
            const double origin = key == L"x" ? source.x : (key == L"y" ? source.y : source.z);
            if (!CommandArguments::ParseCoordinate(value, origin, false, &number))
            {
                *error = L"Selector coordinate " + key + L" must be a number.";
                return false;
            }
            (key == L"x" ? filter->x : (key == L"y" ? filter->y : filter->z)) = number;
            filter->positioned = true;
        }
        else if ((key == L"dx" || key == L"dy" || key == L"dz") && !negated
                 && CommandArguments::ParseNumber(value, &number))
        {
            (key == L"dx" ? filter->dx : (key == L"dy" ? filter->dy : filter->dz)) = number;
            filter->hasVolume = true;
            filter->positioned = true;
        }
        else
        {
            *error = L"Unknown or invalid selector argument " + pair + L".";
            return false;
        }
    }
    return true;
}

bool Matches(const boost::shared_ptr<Entity>& entity, const Filter& filter)
{
    if (!entity->isAlive())
        return false;
    if (filter.hasType && (TargetSelector::TypeName(entity) == filter.type) == filter.typeNegated)
        return false;
    if (filter.hasName)
    {
        const bool player = Entity::instanceof(entity, EntityType::PLAYER);
        const bool same = player ? CommandArguments::Lower(entity->getName()) == CommandArguments::Lower(filter.name)
                                 : (!entity->getCustomName().empty() && entity->getCustomName() == filter.name);
        if (same == filter.nameNegated)
            return false;
    }
    if (!filter.tags.empty() || !filter.notTags.empty() || filter.noTags || filter.anyTag)
    {
        std::set<std::wstring>* own = entity->getTags();
        const bool empty = own == NULL || own->empty();
        if (filter.noTags && !empty)
            return false;
        if (filter.anyTag && empty)
            return false;
        for (size_t index = 0; index < filter.tags.size(); ++index)
            if (own == NULL || own->find(filter.tags[index]) == own->end())
                return false;
        for (size_t index = 0; index < filter.notTags.size(); ++index)
            if (own != NULL && own->find(filter.notTags[index]) != own->end())
                return false;
    }
    if (filter.hasXRotation && (entity->mXRot < filter.minimumXRotation || entity->mXRot > filter.maximumXRotation))
        return false;
    if (filter.hasYRotation)
    {
        float yaw = fmodf(entity->mYRot, 360.0f);
        if (yaw >= 180.0f)
            yaw -= 360.0f;
        else if (yaw < -180.0f)
            yaw += 360.0f;
        if (yaw < filter.minimumYRotation || yaw > filter.maximumYRotation)
            return false;
    }
    for (size_t index = 0; index < filter.nbt.size(); ++index)
    {
        CompoundTag* actual = EntityData::Read(entity);
        const bool found = NbtMatch::matches(filter.nbt[index], actual);
        delete actual;
        if (found == filter.nbtNegated[index])
            return false;
    }

    boost::shared_ptr<ServerPlayer> player = boost::dynamic_pointer_cast<ServerPlayer>(entity);
    if (filter.hasMode || filter.minimumLevel > 0 || filter.maximumLevel >= 0)
    {
        if (!player)
            return false;
        if (filter.hasMode && (player->GetGameType()->getId() == filter.mode) == filter.modeNegated)
            return false;
        if (player->mExperienceLevel < filter.minimumLevel)
            return false;
        if (filter.maximumLevel >= 0 && player->mExperienceLevel > filter.maximumLevel)
            return false;
    }

    const double distance = sqrt(entity->distanceToSqr(filter.x, filter.y, filter.z));
    if (filter.maximumRadius >= 0.0 && distance > filter.maximumRadius)
        return false;
    if (distance < filter.minimumRadius)
        return false;
    if (filter.hasVolume)
    {
        const double x0 = filter.dx < 0.0 ? filter.x + filter.dx : filter.x;
        const double y0 = filter.dy < 0.0 ? filter.y + filter.dy : filter.y;
        const double z0 = filter.dz < 0.0 ? filter.z + filter.dz : filter.z;
        if (entity->mX < floor(x0) || entity->mX >= floor(x0) + fabs(filter.dx) + 1.0 || entity->mY < floor(y0)
            || entity->mY >= floor(y0) + fabs(filter.dy) + 1.0 || entity->mZ < floor(z0)
            || entity->mZ >= floor(z0) + fabs(filter.dz) + 1.0)
            return false;
    }
    return true;
}
}

namespace TargetSelector
{
bool IsSelector(const std::wstring& word)
{
    return word.size() >= 2 && word[0] == L'@';
}

std::wstring TypeName(const boost::shared_ptr<Entity>& entity)
{
    if (Entity::instanceof(entity, EntityType::PLAYER))
        return L"player";
    const ResourceLocation key = EntityIO::getKey(entity);
    const std::wstring path = CommandArguments::Lower(key.getPath());
    // a mob of a mod is named with its mod, as it is summoned
    if (key.getNamespace() != L"minecraft" && !key.getNamespace().empty())
        return CommandArguments::Lower(key.getNamespace()) + L":" + path;
    return path == L"lightningbolt" ? std::wstring(L"lightning_bolt") : path;
}

std::wstring DisplayName(const boost::shared_ptr<Entity>& entity)
{
    const std::wstring name = entity->getName();
    return name.empty() ? CommandArguments::Pretty(TypeName(entity)) : name;
}

std::wstring Names(const std::vector<boost::shared_ptr<Entity> >& entities)
{
    std::wstring text;
    for (size_t index = 0; index < entities.size(); ++index)
    {
        if (index > 0)
            text += index + 1 == entities.size() ? L" and " : L", ";
        text += DisplayName(entities[index]);
    }
    return text;
}

std::wstring Names(const std::vector<boost::shared_ptr<ServerPlayer> >& players)
{
    std::vector<boost::shared_ptr<Entity> > entities(players.begin(), players.end());
    return Names(entities);
}

bool Select(const CommandSource& source, const std::wstring& word, std::vector<boost::shared_ptr<Entity> >& entities,
            std::wstring* error)
{
    entities.clear();
    if (!IsSelector(word))
    {
        boost::shared_ptr<ServerPlayer> player = gMinecraftServer->getPlayers()->getPlayerByNameIgnoringCase(word);
        if (player)
            entities.push_back(player);
        return true;
    }

    const wchar_t kind = static_cast<wchar_t>(towlower(word[1]));
    if (kind != L'p' && kind != L'a' && kind != L'r' && kind != L's' && kind != L'e')
    {
        *error = L"Unknown selector " + word.substr(0, 2) + L". Use @p, @a, @r, @s or @e.";
        return false;
    }

    Filter filter;
    filter.hasType = false;
    filter.typeNegated = false;
    filter.hasName = false;
    filter.nameNegated = false;
    filter.hasMode = false;
    filter.modeNegated = false;
    filter.mode = 0;
    filter.minimumLevel = 0;
    filter.maximumLevel = -1;
    filter.minimumRadius = 0.0;
    filter.maximumRadius = -1.0;
    filter.count = 0;
    filter.hasCount = false;
    filter.x = source.x;
    filter.y = source.y;
    filter.z = source.z;
    filter.positioned = false;
    filter.hasVolume = false;
    filter.dx = 0.0;
    filter.dy = 0.0;
    filter.dz = 0.0;
    if (word.size() > 2)
    {
        if (word[2] != L'[' || word[word.size() - 1] != L']')
        {
            *error = L"Selector arguments go in square brackets, like @e[type=zombie].";
            return false;
        }
        if (!ParseArguments(word.substr(3, word.size() - 4), source, &filter, error))
            return false;
    }

    const bool playersOnly = kind == L'p' || kind == L'a'
                             || (kind == L'r' && (!filter.hasType || filter.type == L"player"));
    std::vector<boost::shared_ptr<Entity> > candidates;
    if (kind == L's')
    {
        if (source.entity)
            candidates.push_back(source.entity);
    }
    else if (playersOnly)
    {
        std::vector<boost::shared_ptr<ServerPlayer> >* players = gMinecraftServer->getPlayers()->getPlayers();
        for (size_t index = 0; index < players->size(); ++index)
        {
            const boost::shared_ptr<ServerPlayer>& player = (*players)[index];
            if ((kind == L'p' || filter.positioned) && player->mLevel != source.level)
                continue;
            candidates.push_back(player);
        }
    }
    else
    {
        candidates = source.level->getAllEntities();
    }

    std::vector<Ranked> ranked;
    for (size_t index = 0; index < candidates.size(); ++index)
    {
        if (!Matches(candidates[index], filter))
            continue;
        Ranked entry;
        entry.entity = candidates[index];
        entry.distance = candidates[index]->distanceToSqr(filter.x, filter.y, filter.z);
        ranked.push_back(entry);
    }

    int limit = filter.hasCount ? filter.count : 0;
    int order = filter.sort;
    if (order == eSort_Default)
    {
        if (kind == L'r')
            order = eSort_Random;
        else if (kind == L'p' || filter.hasCount)
            order = limit < 0 ? eSort_Furthest : eSort_Nearest;
        else
            order = eSort_Arbitrary;
    }
    if ((kind == L'p' || kind == L'r') && !filter.hasCount)
        limit = 1;
    if (order == eSort_Random)
    {
        Random* random = source.level->mRandom;
        for (size_t index = ranked.size(); index > 1; --index)
        {
            const size_t other = static_cast<size_t>(random->nextInt(static_cast<int>(index)));
            std::swap(ranked[index - 1], ranked[other]);
        }
    }
    else if (order == eSort_Nearest || order == eSort_Furthest)
        std::stable_sort(ranked.begin(), ranked.end(), order == eSort_Furthest ? Farther : Nearer);
    const size_t wanted = limit == 0 ? ranked.size() : static_cast<size_t>(limit < 0 ? -limit : limit);
    for (size_t index = 0; index < ranked.size() && index < wanted; ++index)
        entities.push_back(ranked[index].entity);

    return true;
}

bool Entities(const CommandSource& source, const std::wstring& word,
              std::vector<boost::shared_ptr<Entity> >& entities, std::wstring* error)
{
    if (!Select(source, word, entities, error))
        return false;
    if (!entities.empty())
        return true;
    if (!IsSelector(word))
        *error = L"There is no player called " + word + L".";
    else
    {
        const wchar_t kind = static_cast<wchar_t>(towlower(word[1]));
        const bool typed = word.find(L"type=") != std::wstring::npos && word.find(L"type=player") == std::wstring::npos;
        *error = kind == L'e' || (kind == L'r' && typed) ? L"No entity was found." : L"No player was found.";
    }
    return false;
}

bool Players(const CommandSource& source, const std::wstring& word,
             std::vector<boost::shared_ptr<ServerPlayer> >& players, std::wstring* error)
{
    players.clear();
    std::vector<boost::shared_ptr<Entity> > entities;
    if (!Entities(source, word, entities, error))
        return false;
    for (size_t index = 0; index < entities.size(); ++index)
    {
        boost::shared_ptr<ServerPlayer> player = boost::dynamic_pointer_cast<ServerPlayer>(entities[index]);
        if (player)
            players.push_back(player);
    }
    if (players.empty())
    {
        *error = L"No player was found.";
        return false;
    }
    return true;
}
}
