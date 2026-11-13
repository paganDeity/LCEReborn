#include "game/data/NbtPath.hpp"

#include <stdio.h>

#include "game/data/ByteArrayTag.hpp"
#include "game/data/ByteTag.hpp"
#include "game/data/CompoundTag.hpp"
#include "game/data/IntArrayTag.hpp"
#include "game/data/IntTag.hpp"
#include "game/data/ListTag.hpp"
#include "game/data/LongArrayTag.hpp"
#include "game/data/LongTag.hpp"
#include "game/data/NbtMatch.hpp"
#include "game/data/NbtSelection.hpp"
#include "game/data/NumericTag.hpp"
#include "game/data/StringNbtReader.hpp"
#include "game/data/Tag.hpp"

namespace
{
enum ENodeType
{
    ENode_Root,
    ENode_Name,
    ENode_All,
    ENode_Index,
    ENode_Element
};

struct Node
{
    ENodeType type;
    std::wstring name;
    int index;
    CompoundTag* filter;
};

class Path
{
public:
    Path()
    {
    }

    ~Path()
    {
        for (size_t index = 0; index < nodes.size(); ++index)
            delete nodes[index].filter;
    }

    std::vector<Node> nodes;

private:
    Path(const Path&);
    Path& operator=(const Path&);
};

struct Slot
{
    Tag* holder;
    std::wstring name;
    int element;
};

Node MakeNode(ENodeType type)
{
    Node node;
    node.type = type;
    node.index = 0;
    node.filter = NULL;
    return node;
}

bool Fail(std::wstring* error, const std::wstring& message, size_t position)
{
    if (error != NULL)
    {
        wchar_t text[32];
        swprintf(text, sizeof(text) / sizeof(text[0]), L" at character %d", (int)position);
        *error = message + text;
    }
    return false;
}

bool FailPlain(std::wstring* error, const std::wstring& message)
{
    if (error != NULL)
        *error = message;
    return false;
}

bool ReadQuoted(const std::wstring& path, size_t* position, std::wstring* name)
{
    ++*position;
    while (*position < path.size())
    {
        const wchar_t character = path[(*position)++];
        if (character == L'"')
            return true;
        if (character != L'\\')
        {
            name->push_back(character);
            continue;
        }
        if (*position >= path.size())
            break;
        const wchar_t escape = path[(*position)++];
        if (escape == L'n')
            name->push_back(L'\n');
        else if (escape == L'r')
            name->push_back(L'\r');
        else if (escape == L't')
            name->push_back(L'\t');
        else
            name->push_back(escape);
    }
    return false;
}

bool ParseIndex(const std::wstring& text, size_t start, size_t end, int* index)
{
    if (start >= end)
        return false;
    size_t position = start;
    bool negative = false;
    if (text[position] == L'-')
    {
        negative = true;
        ++position;
    }
    if (position >= end)
        return false;
    long long value = 0;
    for (; position < end; ++position)
    {
        const wchar_t character = text[position];
        if (character < L'0' || character > L'9')
            return false;
        value = value * 10 + (character - L'0');
        if (value > 2147483647LL)
            return false;
    }
    *index = (int)(negative ? -value : value);
    return true;
}

bool ParseFilter(const std::wstring& path, size_t* position, CompoundTag** filter, std::wstring* error)
{
    std::wstring reason;
    Tag* tag = StringNbtReader::parseCompound(path, position, &reason);
    if (tag == NULL)
        return FailPlain(error, L"a path filter is not valid: " + reason);
    *filter = static_cast<CompoundTag*>(tag);
    return true;
}

bool ParseList(const std::wstring& path, size_t* position, Path* parsed, std::wstring* error)
{
    ++*position;
    if (*position < path.size() && path[*position] == L']')
    {
        ++*position;
        parsed->nodes.push_back(MakeNode(ENode_All));
        return true;
    }
    if (*position < path.size() && path[*position] == L'{')
    {
        Node node = MakeNode(ENode_Element);
        if (!ParseFilter(path, position, &node.filter, error))
            return false;
        parsed->nodes.push_back(node);
        if (*position >= path.size() || path[*position] != L']')
            return Fail(error, L"a path list is not closed", *position);
        ++*position;
        return true;
    }
    const size_t start = *position;
    while (*position < path.size() && path[*position] != L']')
        ++*position;
    if (*position == path.size())
        return Fail(error, L"a path list is not closed", *position);
    Node node = MakeNode(ENode_Index);
    if (!ParseIndex(path, start, *position, &node.index))
        return Fail(error, L"a list index must be a whole number", start);
    ++*position;
    parsed->nodes.push_back(node);
    return true;
}

bool ParseName(const std::wstring& path, size_t* position, Path* parsed, std::wstring* error)
{
    Node node = MakeNode(ENode_Name);
    if (path[*position] == L'"')
    {
        if (!ReadQuoted(path, position, &node.name))
            return Fail(error, L"a quoted name is not closed", *position);
    }
    else
    {
        const size_t start = *position;
        while (*position < path.size() && path[*position] != L'.' && path[*position] != L'[' &&
               path[*position] != L'{')
        {
            if (path[*position] == L'"')
                return Fail(error, L"a quote may only start a name", *position);
            ++*position;
        }
        node.name = path.substr(start, *position - start);
        if (node.name.empty())
            return Fail(error, L"a name is missing", start);
    }
    if (*position < path.size() && path[*position] == L'{' && !ParseFilter(path, position, &node.filter, error))
        return false;
    parsed->nodes.push_back(node);
    return true;
}

bool ParsePath(const std::wstring& path, Path* parsed, std::wstring* error)
{
    size_t position = 0;
    if (path.empty())
        return Fail(error, L"the path is empty", 0);
    while (position < path.size())
    {
        const wchar_t character = path[position];
        if (character == L'.')
        {
            ++position;
            if (position == path.size())
                return Fail(error, L"a path cannot end with a dot", position);
            continue;
        }
        if (character == L'{')
        {
            if (!parsed->nodes.empty())
                return Fail(error, L"a filter must follow a name or start the path", position);
            Node node = MakeNode(ENode_Root);
            if (!ParseFilter(path, &position, &node.filter, error))
                return false;
            parsed->nodes.push_back(node);
            continue;
        }
        if (character == L'[')
        {
            if (!ParseList(path, &position, parsed, error))
                return false;
            continue;
        }
        if (!ParseName(path, &position, parsed, error))
            return false;
    }
    return true;
}

bool Passes(const Node& node, Tag* tag)
{
    return node.filter == NULL || NbtMatch::matches(node.filter, tag);
}

int ListElement(const ListTag* list, int index)
{
    return index < 0 ? list->size() + index : index;
}

bool IsArray(Tag* tag)
{
    return tag->getId() == Tag::TAG_BYTE_ARRAY || tag->getId() == Tag::TAG_INT_ARRAY
           || tag->getId() == Tag::TAG_LONG_ARRAY;
}

bool IsNumeric(Tag* tag)
{
    return tag->getId() >= Tag::TAG_BYTE && tag->getId() <= Tag::TAG_DOUBLE;
}

int ArraySize(Tag* tag)
{
    if (tag->getId() == Tag::TAG_BYTE_ARRAY)
        return static_cast<int>(static_cast<ByteArrayTag*>(tag)->getAsByteArray().length);
    if (tag->getId() == Tag::TAG_INT_ARRAY)
        return static_cast<int>(static_cast<IntArrayTag*>(tag)->getAsIntArray().length);
    return static_cast<int>(static_cast<LongArrayTag*>(tag)->getAsLongArray().length);
}

int ArrayElement(Tag* tag, int index)
{
    return index < 0 ? ArraySize(tag) + index : index;
}

Tag* ArrayValue(Tag* tag, int element)
{
    if (tag->getId() == Tag::TAG_BYTE_ARRAY)
        return new ByteTag(static_cast<ByteArrayTag*>(tag)->getAsByteArray().data[element]);
    if (tag->getId() == Tag::TAG_INT_ARRAY)
        return new IntTag(static_cast<IntArrayTag*>(tag)->getAsIntArray().data[element]);
    return new LongTag(static_cast<LongArrayTag*>(tag)->getAsLongArray().data[element]);
}

void SetArrayValue(Tag* tag, int element, Tag* value)
{
    NumericTag* number = static_cast<NumericTag*>(value);
    if (tag->getId() == Tag::TAG_BYTE_ARRAY)
        static_cast<ByteArrayTag*>(tag)->getAsByteArray().data[element] = number->getAsByte();
    else if (tag->getId() == Tag::TAG_INT_ARRAY)
        static_cast<IntArrayTag*>(tag)->getAsIntArray().data[element] = number->getAsInt();
    else
        static_cast<LongArrayTag*>(tag)->getAsLongArray().data[element] = number->getAsLong();
}

template <class Value>
arrayWithLength<Value> Resized(arrayWithLength<Value> old, int at, bool insert, Value value)
{
    const unsigned int length = insert ? old.length + 1 : old.length - 1;
    arrayWithLength<Value> resized(length, true);
    unsigned int target = 0;
    for (unsigned int source = 0; source < old.length; ++source)
    {
        if (static_cast<int>(source) == at)
        {
            if (!insert)
                continue;
            resized.data[target++] = value;
        }
        resized.data[target++] = old.data[source];
    }
    if (insert && at == static_cast<int>(old.length))
        resized.data[target] = value;
    return resized;
}

bool InsertIntoArray(Tag* tag, int index, Tag* value)
{
    const int size = ArraySize(tag);
    const int at = index < 0 ? size + index : index;
    if (at < 0 || at > size)
        return false;
    NumericTag* number = static_cast<NumericTag*>(value);
    if (tag->getId() == Tag::TAG_BYTE_ARRAY)
    {
        ByteArrayTag* array = static_cast<ByteArrayTag*>(tag);
        array->setData(Resized(array->getAsByteArray(), at, true, number->getAsByte()));
    }
    else if (tag->getId() == Tag::TAG_INT_ARRAY)
    {
        IntArrayTag* array = static_cast<IntArrayTag*>(tag);
        array->setData(Resized(array->getAsIntArray(), at, true, number->getAsInt()));
    }
    else
    {
        LongArrayTag* array = static_cast<LongArrayTag*>(tag);
        array->setData(Resized(array->getAsLongArray(), at, true, number->getAsLong()));
    }
    return true;
}

void RemoveFromArray(Tag* tag, int element)
{
    if (tag->getId() == Tag::TAG_BYTE_ARRAY)
    {
        ByteArrayTag* array = static_cast<ByteArrayTag*>(tag);
        array->setData(Resized(array->getAsByteArray(), element, false, static_cast<unsigned char>(0)));
    }
    else if (tag->getId() == Tag::TAG_INT_ARRAY)
    {
        IntArrayTag* array = static_cast<IntArrayTag*>(tag);
        array->setData(Resized(array->getAsIntArray(), element, false, 0));
    }
    else
    {
        LongArrayTag* array = static_cast<LongArrayTag*>(tag);
        array->setData(Resized(array->getAsLongArray(), element, false, 0LL));
    }
}

void SelectNode(const std::vector<Tag*>& current, const Node& node, std::vector<Tag*>* next, NbtSelection* owner)
{
    for (size_t index = 0; index < current.size(); ++index)
    {
        Tag* tag = current[index];
        if (node.type == ENode_Root)
        {
            if (Passes(node, tag))
                next->push_back(tag);
        }
        else if (node.type == ENode_Name)
        {
            if (tag->getId() != Tag::TAG_COMPOUND)
                continue;
            CompoundTag* compound = static_cast<CompoundTag*>(tag);
            if (compound->contains(node.name) && Passes(node, compound->get(node.name)))
                next->push_back(compound->get(node.name));
        }
        else if (tag->getId() == Tag::TAG_LIST)
        {
            ListTag* list = static_cast<ListTag*>(tag);
            if (node.type == ENode_Index)
            {
                const int element = ListElement(list, node.index);
                if (element >= 0 && element < list->size())
                    next->push_back(list->get(element));
                continue;
            }
            for (int element = 0; element < list->size(); ++element)
            {
                if (Passes(node, list->get(element)))
                    next->push_back(list->get(element));
            }
        }
        else if (IsArray(tag) && node.type != ENode_Element)
        {
            const int size = ArraySize(tag);
            const int first = node.type == ENode_Index ? ArrayElement(tag, node.index) : 0;
            const int last = node.type == ENode_Index ? first + 1 : size;
            for (int element = first < 0 ? last : first; element < last && element < size; ++element)
            {
                Tag* value = ArrayValue(tag, element);
                owner->own(value);
                next->push_back(value);
            }
        }
    }
}

void Walk(Tag* root, const Path& parsed, size_t steps, std::vector<Tag*>* current, NbtSelection* owner)
{
    current->clear();
    current->push_back(root);
    for (size_t step = 0; step < steps && !current->empty(); ++step)
    {
        std::vector<Tag*> next;
        SelectNode(*current, parsed.nodes[step], &next, owner);
        current->swap(next);
    }
}

bool CollectSlots(const std::vector<Tag*>& holders, const Node& last, bool existing, std::vector<Slot>* slots,
                  std::wstring* error)
{
    if (last.type == ENode_Root)
        return FailPlain(error, L"the whole data cannot be replaced or removed through a filter");
    for (size_t index = 0; index < holders.size(); ++index)
    {
        Tag* tag = holders[index];
        Slot slot;
        slot.holder = tag;
        slot.element = -1;
        if (last.type == ENode_Name)
        {
            if (tag->getId() != Tag::TAG_COMPOUND)
                continue;
            CompoundTag* compound = static_cast<CompoundTag*>(tag);
            const bool present = compound->contains(last.name);
            if (present ? !Passes(last, compound->get(last.name)) : existing || last.filter != NULL)
                continue;
            slot.name = last.name;
            slots->push_back(slot);
            continue;
        }
        if (IsArray(tag) && last.type != ENode_Element)
        {
            const int size = ArraySize(tag);
            if (last.type == ENode_Index)
            {
                slot.element = ArrayElement(tag, last.index);
                if (slot.element >= 0 && slot.element < size)
                    slots->push_back(slot);
                continue;
            }
            for (int element = 0; element < size; ++element)
            {
                slot.element = element;
                slots->push_back(slot);
            }
            continue;
        }
        if (tag->getId() != Tag::TAG_LIST)
            continue;
        ListTag* list = static_cast<ListTag*>(tag);
        if (last.type == ENode_Index)
        {
            slot.element = ListElement(list, last.index);
            if (slot.element >= 0 && slot.element < list->size())
                slots->push_back(slot);
            continue;
        }
        for (int element = 0; element < list->size(); ++element)
        {
            if (!Passes(last, list->get(element)))
                continue;
            slot.element = element;
            slots->push_back(slot);
        }
    }
    return true;
}

bool InsertIntoList(ListTag* list, int index, Tag* value)
{
    const int size = list->size();
    int at = index < 0 ? size + index : index;
    if (at < 0 || at > size)
        return false;
    std::vector<Tag*> items;
    while (list->size() > 0)
        items.push_back(list->remove(0));
    for (int position = 0; position < at; ++position)
        list->add(items[position]);
    list->add(value);
    for (size_t position = (size_t)at; position < items.size(); ++position)
        list->add(items[position]);
    return true;
}

void FlattenValues(const std::vector<Tag*>& values, std::vector<Tag*>* flat, NbtSelection* owner)
{
    for (size_t index = 0; index < values.size(); ++index)
    {
        if (values[index]->getId() == Tag::TAG_LIST)
        {
            ListTag* list = static_cast<ListTag*>(values[index]);
            for (int element = 0; element < list->size(); ++element)
                flat->push_back(list->get(element));
        }
        else if (IsArray(values[index]))
        {
            for (int element = 0; element < ArraySize(values[index]); ++element)
            {
                Tag* value = ArrayValue(values[index], element);
                owner->own(value);
                flat->push_back(value);
            }
        }
        else
        {
            flat->push_back(values[index]);
        }
    }
}

int CollectionSize(Tag* tag)
{
    return IsArray(tag) ? ArraySize(tag) : static_cast<ListTag*>(tag)->size();
}

bool InsertIntoCollection(Tag* tag, int operation, int index, const std::vector<Tag*>& values, int* changed,
                          std::wstring* error)
{
    const bool array = IsArray(tag);
    if (!array && tag->getId() != Tag::TAG_LIST)
        return FailPlain(error, L"the target is not a list");
    ListTag* list = array ? NULL : static_cast<ListTag*>(tag);
    for (size_t value = 0; value < values.size(); ++value)
    {
        if (array && !IsNumeric(values[value]))
            return FailPlain(error, L"an array can only hold numbers");
        if (!array && list->size() > 0 && list->getType() != values[value]->getId())
            return FailPlain(error, L"the value does not match the list's type");
    }
    const int size = CollectionSize(tag);
    int at = size;
    if (operation == NbtPath::EModify_Prepend)
        at = 0;
    else if (operation == NbtPath::EModify_Insert)
        at = index < 0 ? size + index + 1 : index;
    if (at < 0 || at > size)
        return FailPlain(error, L"the list index is out of range");
    for (size_t value = 0; value < values.size(); ++value, ++at)
    {
        if (array)
        {
            InsertIntoArray(tag, at, values[value]);
            continue;
        }
        Tag* copy = values[value]->copy();
        InsertIntoList(list, at, copy);
    }
    if (!values.empty())
        ++*changed;
    return true;
}
}

bool NbtPath::select(Tag* root, const std::wstring& path, NbtSelection* selected, std::wstring* error)
{
    selected->clear();
    Path parsed;
    if (!ParsePath(path, &parsed, error))
        return false;
    std::vector<Tag*> found;
    Walk(root, parsed, parsed.nodes.size(), &found, selected);
    for (size_t index = 0; index < found.size(); ++index)
        selected->add(found[index]);
    return true;
}

bool NbtPath::remove(Tag* root, const std::wstring& path, int* removed, std::wstring* error)
{
    *removed = 0;
    Path parsed;
    if (!ParsePath(path, &parsed, error))
        return false;
    NbtSelection owner;
    std::vector<Tag*> holders;
    Walk(root, parsed, parsed.nodes.size() - 1, &holders, &owner);
    if (holders.empty())
        return true;
    std::vector<Slot> slots;
    if (!CollectSlots(holders, parsed.nodes.back(), true, &slots, error))
        return false;
    for (size_t index = slots.size(); index > 0; --index)
    {
        const Slot& slot = slots[index - 1];
        if (slot.element < 0)
        {
            CompoundTag* compound = static_cast<CompoundTag*>(slot.holder);
            Tag* value = compound->get(slot.name);
            compound->remove(slot.name);
            delete value;
        }
        else if (IsArray(slot.holder))
        {
            RemoveFromArray(slot.holder, slot.element);
        }
        else
        {
            delete static_cast<ListTag*>(slot.holder)->remove(slot.element);
        }
        ++*removed;
    }
    return true;
}

bool NbtPath::modify(Tag* root, const std::wstring& path, int operation, int index, Tag* value, int* changed,
                     std::wstring* error)
{
    std::vector<Tag*> values;
    values.push_back(value);
    return modify(root, path, operation, index, values, changed, error);
}

bool NbtPath::modify(Tag* root, const std::wstring& path, int operation, int index, const std::vector<Tag*>& values,
                     int* changed, std::wstring* error)
{
    *changed = 0;
    if (values.empty())
        return Fail(error, L"a value is missing", 0);
    Path parsed;
    if (!ParsePath(path, &parsed, error))
        return false;
    if (operation == EModify_Set)
    {
        NbtSelection owner;
        std::vector<Tag*> holders;
        Walk(root, parsed, parsed.nodes.size() - 1, &holders, &owner);
        if (holders.empty())
            return true;
        std::vector<Slot> slots;
        if (!CollectSlots(holders, parsed.nodes.back(), false, &slots, error))
            return false;
        if (slots.empty())
            return true;
        if (values.size() != 1 && values.size() != slots.size())
            return FailPlain(error, L"the number of source values and target values must match");
        for (size_t target = 0; target < slots.size(); ++target)
        {
            const Slot& slot = slots[target];
            Tag* value = values.size() == 1 ? values[0] : values[target];
            if (slot.element < 0)
            {
                static_cast<CompoundTag*>(slot.holder)->put(slot.name, value->copy());
            }
            else if (IsArray(slot.holder))
            {
                if (!IsNumeric(value))
                    return FailPlain(error, L"an array element must be set to a number");
                SetArrayValue(slot.holder, slot.element, value);
            }
            else
            {
                ListTag* list = static_cast<ListTag*>(slot.holder);
                if (list->getType() != value->getId())
                    return FailPlain(error, L"the value does not match the list's type");
                list->set(slot.element, value->copy());
            }
            ++*changed;
        }
        return true;
    }
    NbtSelection selection;
    if (!select(root, path, &selection, error))
        return false;
    if (selection.empty())
        return true;
    const std::vector<Tag*>& selected = selection.tags();
    std::vector<Tag*> flat;
    if (operation == EModify_Merge)
    {
        for (size_t index = 0; index < values.size(); ++index)
            flat.push_back(values[index]);
        if (flat.size() != 1 && flat.size() != selected.size())
        {
            if (error != NULL)
                *error = L"the number of source values and target values must match";
            return false;
        }
        for (size_t index = 0; index < flat.size() && index < selected.size(); ++index)
        {
            if (flat[index]->getId() != Tag::TAG_COMPOUND)
            {
                if (error != NULL)
                    *error = L"merge needs a compound and a compound value";
                return false;
            }
        }
        for (size_t position = 0; position < selected.size(); ++position)
        {
            Tag* tag = selected[position];
            if (tag->getId() != Tag::TAG_COMPOUND)
            {
                if (error != NULL)
                    *error = L"merge needs a compound and a compound value";
                return false;
            }
            Tag* value = flat.size() == 1 ? flat[0] : flat[position];
            static_cast<CompoundTag*>(tag)->merge(static_cast<CompoundTag*>(value));
            ++*changed;
        }
        return true;
    }
    FlattenValues(values, &flat, &selection);
    for (size_t position = 0; position < selected.size(); ++position)
    {
        if (!InsertIntoCollection(selected[position], operation, index, flat, changed, error))
            return false;
    }
    return true;
}
