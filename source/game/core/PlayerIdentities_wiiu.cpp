#include "game/core/PlayerIdentities.hpp"

namespace
{
class InertPlayerIdentities : public PlayerIdentities
{
public:
    virtual bool Enabled() const { return false; }
    virtual void Initialise(const std::string&, const std::string&) {}
    virtual std::vector<PlayerIdentityEntry> List() { return std::vector<PlayerIdentityEntry>(); }
    virtual std::wstring ActiveName() { return std::wstring(); }
    virtual std::string ActiveUuid() { return std::string(); }
    virtual std::string ActiveStem() { return std::string(); }
    virtual void ApplyToProfile() {}
    virtual bool Select(const std::string&, std::string*) { return false; }
    virtual bool Create(const std::wstring&, std::string*) { return false; }
    virtual bool Rename(const std::string&, const std::wstring&, std::string*) { return false; }
    virtual bool Remove(const std::string&, std::string*) { return false; }
    virtual bool SetBundledSkin(const std::string&, const std::vector<unsigned char>&, std::string*) { return false; }
    virtual bool OpenFolder() { return false; }
    virtual void BeginChooseSkinFile(const std::string&) {}
    virtual bool TakeSkinFileResult(std::string*, std::string*) { return false; }
    virtual void SkinChosen(int, const std::wstring&) {}
    virtual void ApplyActiveSkin(int) {}
    virtual bool GoOnline(std::string*) { return true; }
    virtual void LeaveOnline() {}
    virtual bool Online() { return false; }
};
}

PlayerIdentities& PlayerIdentities::Get()
{
    static InertPlayerIdentities identities;
    return identities;
}
