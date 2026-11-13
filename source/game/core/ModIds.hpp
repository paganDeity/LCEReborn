#ifndef GAME_CORE_MODIDS_HPP
#define GAME_CORE_MODIDS_HPP

#include <map>
#include <string>
#include <vector>

class CompoundTag;

class ModIds
{
public:
    ModIds(const wchar_t* tagName, int first, int last);

    void Clear();
    void Load(CompoundTag* tag);
    void Merge(CompoundTag* tag);
    void Save(CompoundTag* tag) const;
    void SaveOnly(CompoundTag* tag, const std::vector<std::wstring>& keys) const;
    static std::vector<std::wstring> NamesIn(CompoundTag* tag, const wchar_t* tagName);
    std::vector<int> Assign(const std::vector<std::wstring>& sortedKeys);

    // give the entries (each with a key and the id it has now) their ids; true when any of them changes, with the new ids in order
    template <class Entry> bool Reassign(const std::vector<Entry>& entries, std::vector<int>* assigned)
    {
        std::vector<std::wstring> keys;
        for (size_t index = 0; index < entries.size(); ++index)
            keys.push_back(entries[index].key);
        *assigned = Assign(keys);
        for (size_t index = 0; index < entries.size(); ++index)
        {
            if (entries[index].id != (*assigned)[index])
                return true;
        }
        return false;
    }

private:
    const wchar_t* m_tagName;
    int m_first;
    int m_last;
    std::map<std::wstring, int> m_saved;
};

#endif
