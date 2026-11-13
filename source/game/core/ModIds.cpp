#include "game/core/ModIds.hpp"

#include <set>

#include "game/data/CompoundTag.hpp"
#include "game/data/ListTag.hpp"

ModIds::ModIds(const wchar_t* tagName, int first, int last) : m_tagName(tagName), m_first(first), m_last(last) {}

void ModIds::Clear()
{
    m_saved.clear();
}

void ModIds::Load(CompoundTag* tag)
{
    m_saved.clear();
    ListTag* list = tag->getList(m_tagName, 10);
    for (int index = 0; list != NULL && index < list->size(); ++index)
    {
        CompoundTag* entry = list->getCompound(index);
        m_saved[entry->getString(L"name")] = entry->getInt(L"id");
    }
}

void ModIds::Merge(CompoundTag* tag)
{
    ListTag* list = tag->getList(m_tagName, 10);
    for (int index = 0; list != NULL && index < list->size(); ++index)
    {
        CompoundTag* entry = list->getCompound(index);
        const std::wstring name = entry->getString(L"name");
        const int id = entry->getInt(L"id");
        for (std::map<std::wstring, int>::iterator it = m_saved.begin(); it != m_saved.end();)
        {
            if (it->second == id && it->first != name)
                m_saved.erase(it++);
            else
                ++it;
        }
        m_saved[name] = id;
    }
}

void ModIds::Save(CompoundTag* tag) const
{
    if (m_saved.empty())
        return;
    ListTag* list = new ListTag();
    for (std::map<std::wstring, int>::const_iterator it = m_saved.begin(); it != m_saved.end(); ++it)
    {
        CompoundTag* entry = new CompoundTag();
        entry->putString(L"name", it->first);
        entry->putInt(L"id", it->second);
        list->add(entry);
    }
    tag->put(m_tagName, list);
}

void ModIds::SaveOnly(CompoundTag* tag, const std::vector<std::wstring>& keys) const
{
    ListTag* list = new ListTag();
    for (size_t index = 0; index < keys.size(); ++index)
    {
        std::map<std::wstring, int>::const_iterator found = m_saved.find(keys[index]);
        if (found == m_saved.end())
            continue;
        CompoundTag* entry = new CompoundTag();
        entry->putString(L"name", found->first);
        entry->putInt(L"id", found->second);
        list->add(entry);
    }
    tag->put(m_tagName, list);
}

std::vector<std::wstring> ModIds::NamesIn(CompoundTag* tag, const wchar_t* tagName)
{
    std::vector<std::wstring> names;
    ListTag* list = tag->getList(tagName, 10);
    for (int index = 0; list != NULL && index < list->size(); ++index)
        names.push_back(list->getCompound(index)->getString(L"name"));
    return names;
}

std::vector<int> ModIds::Assign(const std::vector<std::wstring>& sortedKeys)
{
    std::set<int> taken;
    for (std::map<std::wstring, int>::const_iterator it = m_saved.begin(); it != m_saved.end(); ++it)
        taken.insert(it->second);
    std::vector<int> ids;
    int next = m_first;
    for (size_t index = 0; index < sortedKeys.size(); ++index)
    {
        std::map<std::wstring, int>::const_iterator saved = m_saved.find(sortedKeys[index]);
        if (saved != m_saved.end())
        {
            ids.push_back(saved->second);
            continue;
        }
        while (next <= m_last && taken.count(next) != 0)
            ++next;
        if (next > m_last)
        {
            ids.push_back(-1);
            continue;
        }
        taken.insert(next);
        m_saved[sortedKeys[index]] = next;
        ids.push_back(next);
    }
    return ids;
}
