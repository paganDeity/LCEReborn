#ifndef GAME_SERVER_ACCESSLIST_HPP
#define GAME_SERVER_ACCESSLIST_HPP

#include <ctype.h>
#include <string>
#include <vector>
#include <wctype.h>

// a list of players a server keeps by UUID, as operators, the whitelist and the bans all are: an entry names a player by UUID, or by name alone until
// that player joins, when Resolve gives it the UUID, so the player is kept by UUID from then on. Entry has an id and a name. Used on the server thread.
template <class Entry> class AccessList
{
public:
    typedef void (*Changed)(const std::vector<Entry>& entries);

    AccessList() : m_changed(NULL) {}

    void SetChanged(Changed changed) { m_changed = changed; }

    // replace the entries with these, without telling anyone it changed
    void Set(const std::vector<Entry>& entries) { m_entries = entries; }

    const std::vector<Entry>& Entries() const { return m_entries; }

    bool Contains(const std::wstring& id) const { return !id.empty() && Locate(id) != m_entries.end(); }

    // listed by UUID, or by name alone (a pending entry)
    bool Listed(const std::wstring& id, const std::wstring& name) const
    {
        return Contains(id) || (!name.empty() && LocatePending(name) != m_entries.end());
    }

    bool FindByName(const std::wstring& name, Entry* found) const
    {
        const std::wstring wanted = Lower(name);
        for (typename std::vector<Entry>::const_iterator it = m_entries.begin(); it != m_entries.end(); ++it)
        {
            if (!it->name.empty() && Lower(it->name) == wanted)
            {
                *found = *it;
                return true;
            }
        }
        return false;
    }

    // the entry for a UUID, or for a pending name
    bool Find(const std::wstring& id, const std::wstring& name, Entry* found) const
    {
        typename std::vector<Entry>::const_iterator it = id.empty() ? m_entries.end() : Locate(id);
        if (it == m_entries.end() && !name.empty())
            it = LocatePending(name);
        if (it == m_entries.end())
            return false;
        *found = *it;
        return true;
    }

    // add a player by UUID, or by name alone when the UUID is unknown; true when it was not listed before
    bool Add(const std::wstring& id, const std::wstring& name)
    {
        Entry entry;
        entry.id = id;
        entry.name = name;
        return Add(entry);
    }

    bool Add(const Entry& wanted)
    {
        if (wanted.id.empty())
        {
            if (wanted.name.empty() || LocatePending(wanted.name) != m_entries.end())
                return false;
            m_entries.push_back(wanted);
            Notify();
            return true;
        }
        bool modified = false;
        typename std::vector<Entry>::iterator pending = wanted.name.empty() ? m_entries.end() : LocatePending(wanted.name);
        if (pending != m_entries.end())
        {
            m_entries.erase(pending);
            modified = true;
        }
        typename std::vector<Entry>::iterator it = Locate(wanted.id);
        const bool added = it == m_entries.end();
        if (added)
        {
            m_entries.push_back(wanted);
            modified = true;
        }
        else if (!wanted.name.empty() && it->name != wanted.name)
        {
            it->name = wanted.name;
            modified = true;
        }
        if (modified)
            Notify();
        return added;
    }

    // take a player off by UUID or by pending name; true when anything was removed
    bool Remove(const std::wstring& id, const std::wstring& name)
    {
        bool removed = false;
        typename std::vector<Entry>::iterator it = id.empty() ? m_entries.end() : Locate(id);
        if (it != m_entries.end())
        {
            m_entries.erase(it);
            removed = true;
        }
        it = name.empty() ? m_entries.end() : LocatePending(name);
        if (it != m_entries.end())
        {
            m_entries.erase(it);
            removed = true;
        }
        if (removed)
            Notify();
        return removed;
    }

    // a player has joined with this UUID and name: a listed one has its name brought up to date, and a pending one is given the UUID; false when not listed
    bool Resolve(const std::wstring& id, const std::wstring& name)
    {
        typename std::vector<Entry>::iterator it = Locate(id);
        if (it != m_entries.end())
        {
            if (it->name != name)
            {
                it->name = name;
                Notify();
            }
            return true;
        }
        it = LocatePending(name);
        if (it == m_entries.end())
            return false;
        it->id = id;
        it->name = name;
        Notify();
        return true;
    }

private:
    static std::wstring Lower(const std::wstring& text)
    {
        std::wstring lowered(text);
        for (size_t index = 0; index < lowered.size(); ++index)
            lowered[index] = static_cast<wchar_t>(towlower(lowered[index]));
        return lowered;
    }

    typename std::vector<Entry>::iterator Locate(const std::wstring& id)
    {
        typename std::vector<Entry>::iterator it = m_entries.begin();
        while (it != m_entries.end() && it->id != id)
            ++it;
        return it;
    }

    typename std::vector<Entry>::const_iterator Locate(const std::wstring& id) const
    {
        typename std::vector<Entry>::const_iterator it = m_entries.begin();
        while (it != m_entries.end() && it->id != id)
            ++it;
        return it;
    }

    typename std::vector<Entry>::iterator LocatePending(const std::wstring& name)
    {
        const std::wstring wanted = Lower(name);
        typename std::vector<Entry>::iterator it = m_entries.begin();
        while (it != m_entries.end() && !(it->id.empty() && Lower(it->name) == wanted))
            ++it;
        return it;
    }

    typename std::vector<Entry>::const_iterator LocatePending(const std::wstring& name) const
    {
        const std::wstring wanted = Lower(name);
        typename std::vector<Entry>::const_iterator it = m_entries.begin();
        while (it != m_entries.end() && !(it->id.empty() && Lower(it->name) == wanted))
            ++it;
        return it;
    }

    void Notify()
    {
        if (m_changed != NULL)
            m_changed(m_entries);
    }

    std::vector<Entry> m_entries;
    Changed m_changed;
};

#endif
