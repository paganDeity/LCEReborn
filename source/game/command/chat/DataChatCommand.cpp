#include "game/command/chat/DataChatCommand.hpp"

#include <stdio.h>

#include <vector>

#include "game/command/CommandArguments.hpp"
#include "game/command/EntityData.hpp"
#include "game/command/TargetSelector.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/NbtPath.hpp"
#include "game/data/NbtSelection.hpp"
#include "game/data/StringNbtWriter.hpp"
#include "game/data/Tag.hpp"
#include "game/entity/mob/Entity.hpp"
#include "game/server/CommandStorage.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/BlockEntity.hpp"
#include "game/world/block/BlockPos.hpp"

using CommandArguments::Lower;
using CommandArguments::ParseInt;

DataChatCommand::DataChatCommand()
    : ChatCommand(L"data", L"get|merge|modify|remove block|entity|storage <target> [path] [operation value <data>|from <source> [path]]",
                  L"get merge modify remove block|entity|storage coord coord coord path", ePermission_Operator)
{
}

bool DataChatCommand::run(const CommandSource& source, const Words& words)
{
    if (words.size() < 4)
        return false;
    const std::wstring operation = Lower(words[1]);
    if (operation != L"get" && operation != L"merge" && operation != L"modify" && operation != L"remove")
    {
        Fail(source, L"Only data get, merge, modify and remove are available.");
        return true;
    }
    Holder holder;
    if (!resolveHolder(source, words, 2, &holder))
        return holder.usage ? false : true;
    const size_t rest = words.size() - holder.next;
    if (operation == L"get")
    {
        if (rest > 1)
            return false;
        CompoundTag* tag = holder.read();
        report(source, holder.subject, tag, rest == 1 ? words[holder.next] : std::wstring(), rest == 1);
        delete tag;
        return true;
    }
    if (operation == L"merge")
    {
        if (rest != 1)
            return false;
        CompoundTag* tag = NULL;
        if (!ReadCompound(source, words[holder.next], L"The data is not valid: ", &tag))
            return true;
        CompoundTag* current = holder.read();
        current->merge(tag);
        holder.write(current);
        delete current;
        delete tag;
        Tell(source, holder.subject + L" merged.");
        return true;
    }
    if (operation == L"modify")
    {
        if (rest < 3)
            return false;
        CompoundTag* current = holder.read();
        const bool done = modifyFrom(source, holder, current, words[holder.next], words, holder.next + 1);
        delete current;
        return done;
    }
    if (rest != 1)
        return false;
    CompoundTag* current = holder.read();
    const bool done = removeFrom(source, holder, current, words[holder.next]);
    delete current;
    return done;
}

CompoundTag* DataChatCommand::Holder::read() const
{
    if (blockEntity)
        return blockEntity->save(new CompoundTag());
    if (entity)
        return EntityData::Read(entity);
    return CommandStorage::get()->read(storageId);
}

void DataChatCommand::Holder::write(CompoundTag* tag) const
{
    if (blockEntity)
        blockEntity->load(tag);
    else if (entity)
        entity->load(tag);
    else
        CommandStorage::get()->write(storageId, tag);
}

bool DataChatCommand::resolveHolder(const CommandSource& source, const Words& words, size_t kindIndex, Holder* holder)
{
    holder->usage = false;
    const std::wstring kind = Lower(words[kindIndex]);
    if (kind == L"block")
    {
        if (words.size() < kindIndex + 4)
        {
            holder->usage = true;
            return false;
        }
        BlockPos pos;
        if (!resolveBlock(source, words, kindIndex + 1, &pos, &holder->blockEntity, &holder->subject))
            return false;
        holder->next = kindIndex + 4;
        return true;
    }
    if (kind == L"entity")
    {
        if (words.size() < kindIndex + 2)
        {
            holder->usage = true;
            return false;
        }
        if (!resolveEntity(source, words[kindIndex + 1], &holder->entity, &holder->subject))
            return false;
        holder->next = kindIndex + 2;
        return true;
    }
    if (kind == L"storage")
    {
        if (words.size() < kindIndex + 2)
        {
            holder->usage = true;
            return false;
        }
        if (!CommandStorage::get())
        {
            Fail(source, L"Storage needs a world the server runs.");
            return false;
        }
        // an id without a namespace is the game's own, as in Java
        holder->storageId = words[kindIndex + 1].find(L':') == std::wstring::npos ? L"minecraft:" + words[kindIndex + 1] : words[kindIndex + 1];
        holder->subject = L"Storage " + holder->storageId;
        holder->next = kindIndex + 2;
        return true;
    }
    holder->usage = true;
    return false;
}

void DataChatCommand::report(const CommandSource& source, const std::wstring& subject, CompoundTag* root,
                             const std::wstring& path, bool hasPath)
{
    if (!hasPath)
    {
        Tell(source, subject + L" has the following data: " + StringNbtWriter::write(root));
        return;
    }
    NbtSelection selected;
    std::wstring error;
    if (!NbtPath::select(root, path, &selected, &error))
    {
        Fail(source, L"The path is not valid: " + error);
        return;
    }
    if (selected.empty())
    {
        Fail(source, L"The path found no data.");
        return;
    }
    if (selected.size() != 1)
    {
        Fail(source, L"The path found more than one value.");
        return;
    }
    Tell(source, subject + L" has the following data at " + path + L": " + StringNbtWriter::write(selected.get(0)));
}

bool DataChatCommand::resolveBlock(const CommandSource& source, const Words& words, size_t index, BlockPos* pos,
                                   boost::shared_ptr<BlockEntity>* blockEntity, std::wstring* subject)
{
    double position[3];
    if (!ParsePosition(source, words, index, true, position))
        return false;
    *pos = BlockPos(position[0], position[1], position[2]);
    Level* level = source.level;
    if (!level->isInWorldBounds(*pos) || !level->hasChunkAt(*pos))
    {
        Fail(source, L"The block is outside the world.");
        return false;
    }
    *blockEntity = level->getBlockEntity(*pos);
    if (!*blockEntity)
    {
        Fail(source, L"The block has no data.");
        return false;
    }
    wchar_t text[64];
    swprintf(text, sizeof(text) / sizeof(text[0]), L"The block at %d, %d, %d", pos->x, pos->y, pos->z);
    *subject = text;
    return true;
}

bool DataChatCommand::resolveEntity(const CommandSource& source, const std::wstring& selector, EntityPtr* entity,
                                    std::wstring* subject)
{
    EntityList targets;
    if (!ResolveEntities(source, selector, targets))
        return false;
    if (targets.size() != 1)
    {
        Fail(source, L"The selector must find exactly one entity.");
        return false;
    }
    *entity = targets[0];
    *subject = TargetSelector::DisplayName(targets[0]);
    return true;
}

bool DataChatCommand::removeFrom(const CommandSource& source, const Holder& holder, CompoundTag* current, const std::wstring& path)
{
    int removed = 0;
    std::wstring error;
    if (!NbtPath::remove(current, path, &removed, &error))
    {
        Fail(source, L"The path is not valid: " + error);
        return true;
    }
    if (removed == 0)
    {
        Fail(source, L"Nothing was removed.");
        return true;
    }
    holder.write(current);
    Tell(source, holder.subject + L": removed " + Count(removed, L"value", L"values") + L".");
    return true;
}

bool DataChatCommand::modifyFrom(const CommandSource& source, const Holder& holder, CompoundTag* current, const std::wstring& path,
                                 const Words& words, size_t index)
{
    const std::wstring operation = Lower(words[index]);
    int kind = NbtPath::EModify_Set;
    int insertIndex = 0;
    bool hasInsertIndex = false;
    if (operation == L"set")
        kind = NbtPath::EModify_Set;
    else if (operation == L"merge")
        kind = NbtPath::EModify_Merge;
    else if (operation == L"append")
        kind = NbtPath::EModify_Append;
    else if (operation == L"prepend")
        kind = NbtPath::EModify_Prepend;
    else if (operation == L"insert")
        kind = NbtPath::EModify_Insert;
    else
    {
        Fail(source, L"Only set, merge, append, prepend and insert modify data.");
        return true;
    }
    if (kind == NbtPath::EModify_Insert)
    {
        if (words.size() < index + 3 || (Lower(words[index + 2]) != L"value" && Lower(words[index + 2]) != L"from"))
        {
            Fail(source, L"Usage: data modify <target> <path> insert <index> value|from ...");
            return true;
        }
        if (!ParseInt(words[index + 1], -2147483648, 2147483647, &insertIndex))
        {
            Fail(source, L"The index must be a whole number from -2147483648 to 2147483647.");
            return true;
        }
        hasInsertIndex = true;
    }
    if (words.size() < index + 2)
    {
        Fail(source, L"Usage: data modify <target> <path> " + operation + L" value <data> or from <source> ...");
        return true;
    }
    const std::wstring formWord = Lower(words[index + 1 + (kind == NbtPath::EModify_Insert ? 1 : 0)]);
    int changed = 0;
    if (formWord == L"value")
    {
        const size_t valueIndex = index + 2 + (kind == NbtPath::EModify_Insert ? 1 : 0);
        if (words.size() != valueIndex + 1)
        {
            Fail(source, L"Usage: data modify <target> <path> " + operation + L" value <data>");
            return true;
        }
        Tag* value = NULL;
        if (!ReadData(source, words[valueIndex], L"The data is not valid: ", &value))
            return true;
        std::wstring error;
        const bool done = NbtPath::modify(current, path, kind, insertIndex, value, &changed, &error);
        delete value;
        if (!done)
        {
            Fail(source, L"The data could not be modified: " + error);
            return true;
        }
    }
    else if (formWord == L"from")
    {
        const size_t kindIndex = index + (kind == NbtPath::EModify_Insert ? 3 : 2);
        if (words.size() <= kindIndex)
        {
            Fail(source, L"Usage: data modify <target> <path> " + operation + L" from block|entity|storage <source> [path]");
            return true;
        }
        CompoundTag* root = NULL;
        NbtSelection values;
        std::wstring error;
        if (!resolveModifySource(source, words, kindIndex, &root, &values, &error))
            return true;
        const bool done = NbtPath::modify(current, path, kind, insertIndex, values.tags(), &changed, &error);
        delete root;
        if (!done)
        {
            Fail(source, L"The data could not be modified: " + error);
            return true;
        }
    }
    else
    {
        Fail(source, L"Usage: data modify <target> <path> " + operation + L" value <data> or from <source> ...");
        return true;
    }
    if (changed == 0)
    {
        Fail(source, L"Nothing was modified.");
        return true;
    }
    holder.write(current);
    Tell(source, holder.subject + L": modified.");
    return true;
}

bool DataChatCommand::resolveModifySource(const CommandSource& source, const Words& words, size_t kindIndex,
                                          CompoundTag** root, NbtSelection* values, std::wstring* error)
{
    values->clear();
    Holder holder;
    if (!resolveHolder(source, words, kindIndex, &holder))
    {
        if (holder.usage)
            Fail(source, L"Usage: ... from block <x y z> [path], from entity <target> [path] or from storage <id> [path]");
        return false;
    }
    if (words.size() > holder.next + 1)
    {
        Fail(source, L"Usage: ... from block <x y z> [path], from entity <target> [path] or from storage <id> [path]");
        return false;
    }
    CompoundTag* rootTag = holder.read();
    if (words.size() == holder.next)
    {
        values->add(rootTag);
    }
    else if (!NbtPath::select(rootTag, words[holder.next], values, error))
    {
        Fail(source, L"The path is not valid: " + *error);
        delete rootTag;
        return false;
    }
    if (values->empty())
    {
        Fail(source, L"The path found no data.");
        delete rootTag;
        return false;
    }
    *root = rootTag;
    return true;
}
