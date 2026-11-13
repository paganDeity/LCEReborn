#ifndef GAME_UI_UISCENE_FRIENDSMENU_HPP
#define GAME_UI_UISCENE_FRIENDSMENU_HPP

#include <string>
#include <vector>

#include "game/ui/UIControl_Button.hpp"
#include "game/ui/UIControl_Label.hpp"
#include "game/ui/UIControl_PlayerButtonList.hpp"
#include "game/ui/UIScene.hpp"
#include "net/NetP2p.hpp"
#include "platform/C4JStorage.hpp"

class UILayer;

class UIScene_FriendsMenu : public UIScene
{
public:
    UIScene_FriendsMenu(int playerId, void* data, UILayer* layer);
    virtual ~UIScene_FriendsMenu();

    virtual std::wstring getMoviePath();
    virtual unsigned int mapElementsAndNames();
    virtual int getSceneType();
    virtual void updateTooltips();
    virtual void updateComponents();
    virtual void handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled);
    virtual void handlePress(int controlId, int childId);
    virtual void handleGainFocus(bool hadFocus);
    virtual void handleFocusChange(int focus, int childFocus);
    virtual void tick();

private:
    enum EControl
    {
        eControl_Add,
        eControl_List
    };

    void Reload();
    bool Hosting() const;
    int Selected() const;
    void Invite(int index);
    void EditNickname(int index);
    void AskRemove(int index);
    static int RemoveReturned(void* context, int pad, C4JStorage::EMessageResult result);

    UIControl_Button m_addButton;
    UIControl_PlayerButtonList m_list;
    UIControl_Label m_title;
    std::vector<lcenet::P2pFriend> m_friends;
    std::wstring m_titleText;
    int m_lastRevision;
    long long m_lastCheck;
};

#endif
