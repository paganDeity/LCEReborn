#ifndef GAME_CORE_PLAYERIDENTITIES_HPP
#define GAME_CORE_PLAYERIDENTITIES_HPP

#include <string>
#include <vector>

class PlayerIdentityEntry
{
public:
    PlayerIdentityEntry() : defaultSkin(0), bundledSkin(false), usable(false), active(false) {}

    std::string stem;
    std::wstring name;
    std::string uuid;
    std::wstring skin;
    std::wstring skinPath;
    std::string friendCode;
    int defaultSkin;
    bool bundledSkin;
    bool usable;
    bool active;
    std::string problem;
};

class PlayerIdentities
{
public:
    virtual ~PlayerIdentities() {}

    static PlayerIdentities& Get();

    virtual bool Enabled() const = 0;
    virtual void Initialise(const std::string& directory, const std::string& preferredName) = 0;
    virtual std::vector<PlayerIdentityEntry> List() = 0;
    virtual std::wstring ActiveName() = 0;
    virtual std::string ActiveUuid() = 0;
    virtual std::string ActiveStem() = 0;
    virtual void ApplyToProfile() = 0;
    virtual bool Select(const std::string& stem, std::string* error) = 0;
    virtual bool Create(const std::wstring& name, std::string* error) = 0;
    virtual bool Rename(const std::string& stem, const std::wstring& name, std::string* error) = 0;
    virtual bool Remove(const std::string& stem, std::string* error) = 0;
    virtual bool SetBundledSkin(const std::string& stem, const std::vector<unsigned char>& png, std::string* error) = 0;
    virtual bool OpenFolder() = 0;
    virtual void BeginChooseSkinFile(const std::string& stem) = 0;
    virtual bool TakeSkinFileResult(std::string* stem, std::string* error) = 0;
    virtual void SkinChosen(int pad, const std::wstring& skinName) = 0;
    virtual void ApplyActiveSkin(int pad) = 0;
    virtual bool GoOnline(std::string* error) = 0;
    virtual void LeaveOnline() = 0;
    virtual bool Online() = 0;
};

#endif
