#ifndef GAME_UI_UISCENE_IDENTITYMENU_HPP
#define GAME_UI_UISCENE_IDENTITYMENU_HPP

#include <string>
#include <vector>

#include "game/core/PlayerIdentities.hpp"
#include "game/ui/UIControl.hpp"
#include "game/ui/UIControl_BitmapIcon.hpp"
#include "game/ui/UIControl_Label.hpp"
#include "game/ui/UIControl_MultiList.hpp"
#include "game/ui/UIControl_PlayerSkinPreview.hpp"
#include "game/ui/UIScene.hpp"
#include "platform/C4JStorage.hpp"

class UILayer;

class UIScene_IdentityMenu : public UIScene
{
public:
    UIScene_IdentityMenu(int playerId, void* data, UILayer* layer);
    virtual ~UIScene_IdentityMenu();

    virtual std::wstring getMoviePath();
    virtual unsigned int mapElementsAndNames();
    virtual int getSceneType();
    virtual void tick();
    virtual void updateTooltips();
    virtual void updateComponents();
    virtual void customDraw(const char* name, fuiRect* rect);
    virtual void handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled);
    virtual void handleAnimationEnd();
    virtual void handleGainFocus(bool hadFocus);
    virtual void handleFocusChange(int focus, int childFocus);

private:
    enum EAction
    {
        eAction_Use,
        eAction_Username,
        eAction_Skin,
        eAction_SkinFile,
        eAction_Folder,
        eAction_Remove,
        eAction_New
    };

    enum ECharacter
    {
        eCharacter_Current,
        eCharacter_Next1,
        eCharacter_Next2,
        eCharacter_Next3,
        eCharacter_Previous1,
        eCharacter_Previous2,
        eCharacter_Previous3,
        eCharacter_Count
    };

    void Reload();
    void Show();
    void ShowNeighbours();
    void Apply(UIControl_PlayerSkinPreview& preview, const PlayerIdentityEntry* entry);
    void PreviewSlot(int character, size_t slot);
    void BuildActions();
    void Run(int action);
    int SlotCount() const;
    size_t Next(size_t slot) const;
    size_t Previous(size_t slot) const;
    const PlayerIdentityEntry* Current() const;
    void Move(bool forward);
    void SetFlag(int flag, bool value);
    void RequestName(bool create);
    void NameEntered(const std::wstring& name);
    void AskRemove();
    void Tell(int messageId);
    static int KeyboardReturned(void* context, bool accepted);
    static int RemoveReturned(void* context, int pad, C4JStorage::EMessageResult result);

    UIControl_PlayerSkinPreview m_characters[eCharacter_Count];
    UIControl_Label m_labelName;
    UIControl_Label m_labelInfo;
    UIControl_Label m_labelTitle;
    UIControl_Label m_labelHint;
    UIControl m_controlCharacters;
    UIControl m_controlTimer;
    UIControl_BitmapIcon m_icon;
    UIControl_MultiList m_list;
    std::vector<PlayerIdentityEntry> m_entries;
    std::vector<int> m_actions;
    size_t m_index;
    int m_action;
    bool m_animating;
    bool m_sliding;
    bool m_pending;
    bool m_nameReady;
    bool m_nameAccepted;
    bool m_choosing;
    std::wstring m_typed;
    std::string m_editStem;
};

#endif
