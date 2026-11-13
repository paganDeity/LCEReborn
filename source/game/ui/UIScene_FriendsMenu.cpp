#include "game/ui/UIScene_FriendsMenu.hpp"
#include "game/core/StringIds.hpp"

#include <stdint.h>

#include "game/client/audio/SoundEvents.hpp"
#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/ConsoleUIController.hpp"
#include "game/core/ExtraStrings.hpp"
#include "game/core/L10N.hpp"
#include "game/core/System.hpp"
#include "game/network/CGameNetworkManager.hpp"
#include "game/ui/UILayer.hpp"
#include "game/ui/UIScene_EditServerMenu.hpp"
#include "game/ui/UIString.hpp"
#include "game/ui/fuiFile.hpp"
#include "game/ui/fuiRenderNode.hpp"
#include "platform/CInput.hpp"

extern ConsoleUIController gConsoleUIController;

namespace
{
std::wstring Widen(const std::string& text)
{
    return std::wstring(text.begin(), text.end());
}

std::wstring Label(const lcenet::P2pFriend& entry)
{
    return entry.nickname.empty() ? Widen(entry.code) : Widen(entry.nickname) + L"  " + Widen(entry.code);
}
}

UIScene_FriendsMenu::UIScene_FriendsMenu(int playerId, void*, UILayer* layer)
    : UIScene(playerId, layer), m_lastRevision(-1), m_lastCheck(0)
{
    initialiseMovie();
    m_titleText = std::wstring(L10N::GetString(IDS_LCE_YOUR_FRIEND_CODE)) + L": "
                  + Widen(lcenet::P2pService::Get().FriendCode());
    m_addButton.init(UIString(L10N::GetString(IDS_LCE_ADD_FRIEND)), eControl_Add);
    m_title.init(UIString(m_titleText));
    m_list.init(eControl_List);
    Reload();
    updateTooltips();
    gConsoleUIController.TouchBoxRebuild(this);
}

UIScene_FriendsMenu::~UIScene_FriendsMenu()
{
}

std::wstring UIScene_FriendsMenu::getMoviePath()
{
    return gApp.GetLocalPlayerCount() > 1 ? std::wstring(L"InGameInfoMenuSplit") : std::wstring(L"InGameInfoMenu");
}

unsigned int UIScene_FriendsMenu::mapElementsAndNames()
{
    fuiRenderNode* node = m_movie->getRootNode();
    m_addButton.setupControl(this, node, std::string("GameOptions"));
    m_controls.push_back(&m_addButton);
    m_list.setupControl(this, node, std::string("GamePlayers"));
    m_controls.push_back(&m_list);
    m_title.setupControl(this, node, std::string("Title"));
    m_controls.push_back(&m_title);
    return 1;
}

int UIScene_FriendsMenu::getSceneType()
{
    return EUIScene_FriendsMenu;
}

bool UIScene_FriendsMenu::Hosting() const
{
    return CGameNetworkManager::sInstance.IsHost() && !CGameNetworkManager::sInstance.IsLocalGame();
}

int UIScene_FriendsMenu::Selected() const
{
    if (!const_cast<UIControl_PlayerButtonList&>(m_list).hasFocus() || m_friends.empty())
        return -1;
    const unsigned int selection = const_cast<UIControl_PlayerButtonList&>(m_list).getCurrentSelection();
    return selection < m_friends.size() ? static_cast<int>(selection) : -1;
}

void UIScene_FriendsMenu::Reload()
{
    const unsigned int selection = m_list.getCurrentSelection();
    m_friends = lcenet::P2pService::Get().Friends();
    m_list.clearList();
    for (size_t index = 0; index < m_friends.size(); ++index)
        m_list.addItem(Label(m_friends[index]), 0, 0, true);
    if (!m_friends.empty())
        m_list.setCurrentSelection(selection < m_friends.size() ? static_cast<int>(selection) : 0, false, true);
}

void UIScene_FriendsMenu::updateTooltips()
{
    const int selected = Selected();
    const int invite = selected >= 0 && Hosting() ? static_cast<int>(IDS_LCE_INVITE_FRIEND)
                                                    : (m_addButton.hasFocus() ? static_cast<int>(IDS_TOOLTIPS_SELECT) : -1);
    const int remove = selected >= 0 ? static_cast<int>(IDS_LCE_TOOLTIP_REMOVE) : -1;
    const int nickname = selected >= 0 ? static_cast<int>(IDS_LCE_TOOLTIP_NICKNAME) : -1;
    gConsoleUIController.SetTooltips(m_playerId, invite, static_cast<int>(IDS_TOOLTIPS_BACK), remove, nickname, -1, -1,
                                     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, false, false);
}

void UIScene_FriendsMenu::updateComponents()
{
    m_layer->showComponent(m_playerId, EUIScene_MenuBackground, true);
    m_layer->showComponent(m_playerId, EUIScene_Logo, false);
}

void UIScene_FriendsMenu::handleGainFocus(bool)
{
    Reload();
    updateTooltips();
}

void UIScene_FriendsMenu::handleFocusChange(int focus, int childFocus)
{
    if (focus == eControl_List)
        m_list.updateChildFocus(childFocus);
    updateTooltips();
}

void UIScene_FriendsMenu::tick()
{
    UIScene::tick();
    if (lcenet::P2pService::Get().TakeLookupFailure())
    {
        static int button = IDS_CONFIRM_OK;
        gConsoleUIController.RequestErrorMessage(IDS_LCE_ADD_FRIEND, IDS_LCE_FRIEND_NOT_FOUND, &button, 1, m_playerId, NULL,
                                                 NULL, NULL, false);
    }
    const long long now = static_cast<long long>(System::processTimeInMilliSecs());
    if (now - m_lastCheck < 1000)
        return;
    m_lastCheck = now;
    const std::vector<lcenet::P2pFriend> current = lcenet::P2pService::Get().Friends();
    bool changed = current.size() != m_friends.size();
    for (size_t index = 0; !changed && index < current.size(); ++index)
        changed = current[index].key != m_friends[index].key || current[index].nickname != m_friends[index].nickname;
    if (changed)
    {
        Reload();
        updateTooltips();
    }
}

void UIScene_FriendsMenu::handleInput(int playerId, int action, bool repeat, bool pressed, bool released,
                                      bool& handled)
{
    gConsoleUIController.AnimateKeyPress(m_playerId, action, repeat, pressed, released);
    switch (action)
    {
    case EControllerActions_MenuCancel:
        if (pressed && !repeat)
        {
            gConsoleUIController.PlayUISFX(SoundEvents::UI_BACK);
            navigateBack();
            handled = true;
        }
        break;
    case EControllerActions_MenuX:
        if (pressed && !repeat && Selected() >= 0)
        {
            AskRemove(Selected());
            handled = true;
        }
        break;
    case EControllerActions_MenuY:
        if (pressed && !repeat && Selected() >= 0)
        {
            EditNickname(Selected());
            handled = true;
        }
        break;
    case EControllerActions_MenuUp:
    case EControllerActions_MenuDown:
    case EControllerActions_MenuPageUp:
    case EControllerActions_MenuPageDown:
    case EControllerActions_MenuOk:
        sendInputToMovie(action, repeat, pressed, released);
        handled = true;
        break;
    default:
        break;
    }
}

void UIScene_FriendsMenu::handlePress(int controlId, int childId)
{
    gConsoleUIController.PlayUISFX(SoundEvents::UI_PRESS);
    if (controlId == eControl_Add)
    {
        gConsoleUIController.NavigateToScene(m_playerId, EUIScene_EditServerMenu,
                                             new EditServerSceneData(std::string(), std::wstring(), std::string()),
                                             EUILayer_Scene, EUIGroup_PlayerDefault);
    }
    else if (controlId == eControl_List && childId >= 0 && static_cast<size_t>(childId) < m_friends.size())
    {
        if (Hosting())
            Invite(childId);
        else
            EditNickname(childId);
    }
}

void UIScene_FriendsMenu::Invite(int index)
{
    lcenet::P2pService::Get().Invite(m_friends[index].key);
    static int button = IDS_CONFIRM_OK;
    gConsoleUIController.RequestErrorMessage(IDS_LCE_FRIENDS, IDS_LCE_FRIEND_INVITED, &button, 1, m_playerId, NULL, NULL,
                                             NULL, false);
}

void UIScene_FriendsMenu::EditNickname(int index)
{
    gConsoleUIController.NavigateToScene(
        m_playerId, EUIScene_EditServerMenu,
        new EditServerSceneData(m_friends[index].key, Widen(m_friends[index].nickname), m_friends[index].code),
        EUILayer_Scene, EUIGroup_PlayerDefault);
}

void UIScene_FriendsMenu::AskRemove(int index)
{
    std::string* key = new std::string(m_friends[index].key);
    int parameters[2] = {static_cast<int>(IDS_CONFIRM_OK), static_cast<int>(IDS_CONFIRM_CANCEL)};
    gConsoleUIController.RequestAlertMessage(IDS_LCE_REMOVE_FRIEND_TITLE, IDS_LCE_REMOVE_FRIEND, parameters, 2,
                                             m_playerId, &UIScene_FriendsMenu::RemoveReturned, key, NULL, true);
}

int UIScene_FriendsMenu::RemoveReturned(void* context, int, C4JStorage::EMessageResult result)
{
    std::string* key = static_cast<std::string*>(context);
    if (result == C4JStorage::EMessageResult_Accept)
        lcenet::P2pService::Get().RemoveFriend(*key);
    delete key;
    return 0;
}
