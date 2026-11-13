#include "platform/pc/PcPlayerIdentities.hpp"

#include <string.h>

#include <SDL3/SDL.h>

#include <fstream>
#include <iterator>
#include <mutex>
#include <set>

#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/CMinecraftApp.hpp"
#include "game/core/GameSettings.hpp"
#include "game/core/PlayerIdentities.hpp"
#include "identity/IdLock.hpp"
#include "identity/IdProfile.hpp"
#include "identity/IdStore.hpp"
#include "net/NetLog.hpp"
#include "p2p/P2pFriendCode.hpp"
#include "platform/CProfile.hpp"
#include "platform/pc/PcLaunchOptions.hpp"
#include "platform/pc/PcPaths.hpp"
#include "platform/pc/PcUtf8.hpp"
#include "platform/pc/PcWindow.hpp"

namespace
{
std::string Narrow(const std::wstring& text)
{
    return PcWideToUtf8(text);
}

std::string Sanitise(const std::string& text)
{
    std::string name;
    for (size_t index = 0; index < text.size() && name.size() < 16; ++index)
    {
        const char letter = text[index];
        const bool plain = (letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z')
                           || (letter >= '0' && letter <= '9') || letter == '_';
        if (plain)
            name += letter;
    }
    while (name.size() < 3)
        name += '_';
    return name;
}

class PcPlayerIdentities : public PlayerIdentities
{
public:
    PcPlayerIdentities() : m_store(""), m_enabled(false), m_users(0) {}

    virtual bool Enabled() const { return m_enabled; }
    virtual void Initialise(const std::string& directory, const std::string& preferredName);
    virtual std::vector<PlayerIdentityEntry> List();
    virtual std::wstring ActiveName();
    virtual std::string ActiveUuid();
    virtual std::string ActiveStem() { return m_stem; }
    virtual void ApplyToProfile();
    virtual bool Select(const std::string& stem, std::string* error);
    virtual bool Create(const std::wstring& name, std::string* error);
    virtual bool Rename(const std::string& stem, const std::wstring& name, std::string* error);
    virtual bool Remove(const std::string& stem, std::string* error);
    virtual bool SetBundledSkin(const std::string& stem, const std::vector<unsigned char>& png, std::string* error);
    virtual bool OpenFolder();
    virtual void BeginChooseSkinFile(const std::string& stem);
    virtual bool TakeSkinFileResult(std::string* stem, std::string* error);
    virtual void SkinChosen(int pad, const std::wstring& skinName);
    virtual void ApplyActiveSkin(int pad);
    virtual bool GoOnline(std::string* error);
    virtual void LeaveOnline();
    virtual bool Online();

    bool Key(p2p::Identity& key)
    {
        if (!m_enabled)
            return false;
        key = m_active.Key();
        return true;
    }

private:
    bool Fail(std::string* error, const std::string& text)
    {
        if (error != NULL)
            *error = text;
        return false;
    }

    std::wstring BundledPath(const identity::Profile& profile);
    static void SDLCALL DialogDone(void* userdata, const char* const* files, int filter);

    struct Chosen
    {
        Chosen() : ready(false), request(0) {}
        std::mutex mutex;
        bool ready;
        long request;
        std::string stem;
        std::string path;
        std::string error;
    };

    identity::Store m_store;
    std::set<std::string> m_registered;
    Chosen m_chosen;
    bool m_enabled;
    std::string m_stem;
    identity::Profile m_active;
    identity::Lock m_lock;
    int m_users;
};

void PcPlayerIdentities::Initialise(const std::string& directory, const std::string& preferredName)
{
    m_store = identity::Store(directory);
    m_store.Prepare();
    std::string stem = m_store.Active();
    std::string error;
    if (stem.empty() || !m_store.Load(stem, m_active, &error))
    {
        stem.clear();
        const std::vector<std::string> all = m_store.List();
        for (size_t index = 0; index < all.size() && stem.empty(); ++index)
            if (m_store.Load(all[index], m_active, &error))
                stem = all[index];
        if (stem.empty() && !m_store.Create(Sanitise(preferredName), &stem, &m_active))
        {
            NET_LOG_WARN("Identities are off: the identity folder %s can't be used", directory.c_str());
            return;
        }
        m_store.SetActive(stem);
    }
    m_stem = stem;
    m_enabled = true;
}

std::vector<PlayerIdentityEntry> PcPlayerIdentities::List()
{
    std::vector<PlayerIdentityEntry> entries;
    if (!m_enabled)
        return entries;
    const std::vector<std::string> stems = m_store.List();
    for (size_t index = 0; index < stems.size(); ++index)
    {
        PlayerIdentityEntry entry;
        entry.stem = stems[index];
        entry.active = stems[index] == m_stem;
        identity::Profile profile;
        entry.usable = m_store.Load(stems[index], profile, &entry.problem);
        if (entry.usable)
        {
            entry.name = PcUtf8ToWide(profile.Name());
            entry.uuid = profile.Uuid();
            entry.bundledSkin = profile.HasBundledSkin();
            entry.skin = PcUtf8ToWide(profile.SkinName());
            entry.friendCode = p2p::FriendCode::Of(profile.Key().Public());
            entry.defaultSkin = identity::Profile::DefaultSkin(profile.Name());
            entry.skinPath = profile.HasBundledSkin() ? BundledPath(profile) : entry.skin;
        }
        else
            entry.name = PcUtf8ToWide(stems[index]);
        entries.push_back(entry);
    }
    return entries;
}

std::wstring PcPlayerIdentities::ActiveName()
{
    return m_enabled ? PcUtf8ToWide(m_active.Name()) : std::wstring();
}

void PcPlayerIdentities::ApplyToProfile()
{
    static_cast<CProfile*>(gProfile)->ApplyActiveIdentity();
}

std::string PcPlayerIdentities::ActiveUuid()
{
    return m_enabled ? m_active.Uuid() : std::string();
}

bool PcPlayerIdentities::Select(const std::string& stem, std::string* error)
{
    if (!m_enabled)
        return Fail(error, "identities are off");
    if (Online() && stem != m_stem)
        return Fail(error, "this identity is online; leave the game before choosing another");
    identity::Profile profile;
    std::string problem;
    if (!m_store.Load(stem, profile, &problem))
        return Fail(error, problem);
    m_active = profile;
    m_stem = stem;
    m_store.SetActive(stem);
    return true;
}

bool PcPlayerIdentities::Create(const std::wstring& name, std::string* error)
{
    if (!m_enabled)
        return Fail(error, "identities are off");
    if (!identity::Profile::ValidName(Narrow(name)))
        return Fail(error, "a name is 3 to 16 letters, digits and underscores");
    return m_store.Create(Narrow(name), NULL, NULL) || Fail(error, "the identity could not be saved");
}

bool PcPlayerIdentities::Rename(const std::string& stem, const std::wstring& name, std::string* error)
{
    if (!m_enabled)
        return Fail(error, "identities are off");
    if (Online() && stem == m_stem)
        return Fail(error, "this identity is online; leave the game before changing its name");
    identity::Profile profile;
    std::string problem;
    if (!m_store.Load(stem, profile, &problem))
        return Fail(error, problem);
    if (!profile.SetName(Narrow(name)))
        return Fail(error, "a name is 3 to 16 letters, digits and underscores");
    if (!m_store.Save(stem, profile))
        return Fail(error, "the identity could not be saved");
    if (stem == m_stem)
        m_active = profile;
    return true;
}

bool PcPlayerIdentities::Remove(const std::string& stem, std::string* error)
{
    if (!m_enabled)
        return Fail(error, "identities are off");
    if (stem == m_stem)
        return Fail(error, "this is the identity in use; choose another first");
    return m_store.Remove(stem) || Fail(error, "the identity could not be removed");
}

bool PcPlayerIdentities::SetBundledSkin(const std::string& stem, const std::vector<unsigned char>& png,
                                        std::string* error)
{
    if (!m_enabled)
        return Fail(error, "identities are off");
    identity::Profile profile;
    std::string problem;
    if (!m_store.Load(stem, profile, &problem))
        return Fail(error, problem);
    if (!profile.SetBundledSkin(png))
        return Fail(error, "a skin must be a PNG of at most 1 MiB");
    if (!m_store.Save(stem, profile))
        return Fail(error, "the identity could not be saved");
    if (stem == m_stem)
        m_active = profile;
    return true;
}

void PcPlayerIdentities::SkinChosen(int pad, const std::wstring& skinName)
{
    if (!m_enabled || pad != 0 || skinName.empty())
        return;
    if (m_active.SetNamedSkin(Narrow(skinName)))
        m_store.Save(m_stem, m_active);
}

std::wstring PcPlayerIdentities::BundledPath(const identity::Profile& profile)
{
    const std::string hash = profile.SkinHash();
    unsigned int id = 0;
    for (int index = 0; index < 4; ++index)
        id = (id << 8) | static_cast<unsigned char>(hash[index] >= 'a' ? hash[index] - 'a' + 10 : hash[index] - '0');
    id &= SKIN_ID_UGC_MASK;
    if (id == 0)
        id = 0x20;
    const std::wstring path = CMinecraftApp::getSkinPathFromId(id);
    if (m_registered.insert(hash).second)
    {
        const std::vector<uint8_t>& data = profile.SkinData();
        unsigned char* copy = new unsigned char[data.size()];
        memcpy(copy, &data[0], data.size());
        gApp.AddMemoryTextureFile(path, copy, static_cast<unsigned int>(data.size()));
    }
    return path;
}

void PcPlayerIdentities::ApplyActiveSkin(int pad)
{
    if (!m_enabled || pad != 0)
        return;
    if (m_active.HasBundledSkin())
        GameSettings::SetPlayerSkin(pad, BundledPath(m_active));
    else if (!m_active.SkinName().empty())
        GameSettings::SetPlayerSkin(pad, PcUtf8ToWide(m_active.SkinName()));
    else
        GameSettings::SetPlayerSkin(pad, CMinecraftApp::getSkinPathFromId(identity::Profile::DefaultSkin(m_active.Name())));
}

bool PcPlayerIdentities::OpenFolder()
{
    if (!m_enabled)
        return false;
    std::string path = m_store.Directory();
    const std::string url = "file://" + std::string(path.empty() || path[0] != '/' ? "/" : "") + path;
    return SDL_OpenURL(url.c_str());
}

void SDLCALL PcPlayerIdentities::DialogDone(void* userdata, const char* const* files, int)
{
    PcPlayerIdentities* self = static_cast<PcPlayerIdentities*>(userdata);
    std::lock_guard<std::mutex> lock(self->m_chosen.mutex);
    if (files == NULL)
        self->m_chosen.error = SDL_GetError();
    else if (files[0] != NULL)
        self->m_chosen.path = files[0];
    self->m_chosen.ready = true;
}

void PcPlayerIdentities::BeginChooseSkinFile(const std::string& stem)
{
    static const SDL_DialogFileFilter kFilters[] = {{"Skin images (*.png)", "png"}, {"All files", "*"}};
    {
        std::lock_guard<std::mutex> lock(m_chosen.mutex);
        m_chosen.ready = false;
        m_chosen.stem = stem;
        m_chosen.path.clear();
        m_chosen.error.clear();
    }
    SDL_ShowOpenFileDialog(&PcPlayerIdentities::DialogDone, this, PcWindowHandle(), kFilters, 2, NULL, false);
}

bool PcPlayerIdentities::TakeSkinFileResult(std::string* stem, std::string* error)
{
    std::string path;
    {
        std::lock_guard<std::mutex> lock(m_chosen.mutex);
        if (!m_chosen.ready)
            return false;
        m_chosen.ready = false;
        path = m_chosen.path;
        *stem = m_chosen.stem;
        *error = m_chosen.error;
    }
    if (!error->empty() || path.empty())
        return true;
    std::ifstream file(path.c_str(), std::ios::binary);
    std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (bytes.empty())
    {
        *error = "the file could not be read";
        return true;
    }
    SetBundledSkin(*stem, bytes, error);
    return true;
}

bool PcPlayerIdentities::GoOnline(std::string* error)
{
    if (!m_enabled)
        return true;
    if (m_users == 0 && !m_lock.Acquire(m_store.LockPathOf(m_stem)))
        return Fail(error, "this identity is already online in another copy of the game");
    ++m_users;
    return true;
}

void PcPlayerIdentities::LeaveOnline()
{
    if (m_users > 0 && --m_users == 0)
        m_lock.Release();
}

bool PcPlayerIdentities::Online()
{
    return m_users > 0;
}

PcPlayerIdentities& Instance()
{
    static PcPlayerIdentities identities;
    return identities;
}
}

PlayerIdentities& PlayerIdentities::Get()
{
    return Instance();
}

bool PcActiveIdentityKey(p2p::Identity& key)
{
    return Instance().Key(key);
}

void PcStartIdentities(const char*)
{
    const PcLaunchOptions& options = PcGetLaunchOptions();
    std::string directory = options.identityDir;
    if (directory.empty() && !options.p2pDir.empty())
        directory = options.p2pDir + "/identities";
    if (directory.empty())
        directory = PcSaveRoot() + "/identities";
    PlayerIdentities::Get().Initialise(directory, options.username);
}
