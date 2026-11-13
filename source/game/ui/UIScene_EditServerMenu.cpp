#include "game/ui/UIScene_EditServerMenu.hpp"
#include "game/core/StringIds.hpp"

#include <cstring>
#include <stdint.h>

#include "game/client/audio/SoundEvents.hpp"
#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/ConsoleUIController.hpp"
#include "game/core/ExtraStrings.hpp"
#include "game/core/L10N.hpp"
#include "game/ui/UILayer.hpp"
#include "game/ui/UIString.hpp"
#include "game/ui/fuiFile.hpp"
#include "game/ui/fuiRenderNode.hpp"
#include "game/ui/fuiRenderNodeTimeline.hpp"
#include "game/core/PlayerIdentities.hpp"
#include "net/NetP2p.hpp"
#include "net/ServerList.hpp"
#include "platform/CInput.hpp"

extern ConsoleUIController gConsoleUIController;

namespace
{
const wchar_t* const kDefaultServerName = L"Minecraft Server";
const unsigned int kNameLimit = 32;
const unsigned int kAddressLimit = 128;
const unsigned int kNicknameLimit = 24;
const unsigned int kCodeLimit = 24;

std::wstring Widen(const std::string& text)
{
    return std::wstring(text.begin(), text.end());
}

std::string Narrow(const std::wstring& text)
{
    std::string result;
    for (size_t index = 0; index < text.size(); ++index)
        result.push_back(text[index] < 0x80 ? static_cast<char>(text[index]) : '?');
    return result;
}

fuiRenderNode* FindParentOf(fuiRenderNode* node, const char* name)
{
    if (node == NULL || node->asTimeline() == NULL)
        return NULL;
    fuiRenderNodeTimeline* timeline = static_cast<fuiRenderNodeTimeline*>(node);
    for (int index = 0; index < timeline->getNumChildren(); ++index)
        if (std::strcmp(timeline->getChildAt(index)->getName(), name) == 0)
            return node;
    for (int index = 0; index < timeline->getNumChildren(); ++index)
    {
        fuiRenderNode* parent = FindParentOf(timeline->getChildAt(index), name);
        if (parent != NULL)
            return parent;
    }
    return NULL;
}


fuiRenderNode* FindChild(fuiRenderNodeTimeline* parent, const char* name)
{
    for (int index = 0; index < parent->getNumChildren(); ++index)
        if (std::strcmp(parent->getChildAt(index)->getName(), name) == 0)
            return parent->getChildAt(index);
    return NULL;
}

std::wstring Trim(const std::wstring& text)
{
    size_t first = text.find_first_not_of(L" \t");
    if (first == std::wstring::npos)
        return std::wstring();
    size_t last = text.find_last_not_of(L" \t");
    return text.substr(first, last - first + 1);
}
}

UIScene_EditServerMenu::UIScene_EditServerMenu(int playerId, void* data, UILayer* layer)
    : UIScene(playerId, layer),
      m_serverIndex(-1),
      m_friendMode(false),
      m_friendKnown(false),
      m_name(kDefaultServerName),
      m_keyboardControl(-1)
{
    EditServerSceneData* sceneData = static_cast<EditServerSceneData*>(data);
    if (sceneData != NULL && sceneData->friendMode)
    {
        m_friendMode = true;
        m_friendKey = sceneData->friendKey;
        m_friendKnown = !sceneData->friendKey.empty();
        m_name = sceneData->nickname;
        m_address = Widen(sceneData->code);
        delete sceneData;
    }
    else if (sceneData != NULL)
    {
        const std::vector<lcenet::SavedServer>& servers = lcenet::SavedServers().Servers();
        if (sceneData->serverIndex >= 0 && static_cast<unsigned int>(sceneData->serverIndex) < servers.size())
        {
            m_serverIndex = sceneData->serverIndex;
            m_name = servers[m_serverIndex].name;
            m_address = servers[m_serverIndex].address;
        }
        delete sceneData;
    }

    initialiseMovie();
    m_title.init(UIString(m_friendMode ? std::wstring(L10N::GetString(IDS_LCE_ADD_FRIEND))
                                       : std::wstring(L"Edit Server Info")));
    m_nameLabel.init(UIString(m_friendMode ? std::wstring(L10N::GetString(IDS_LCE_NICKNAME))
                                           : std::wstring(L"Server Name")));
    m_addressLabel.init(UIString(m_friendMode ? std::wstring(L10N::GetString(IDS_LCE_FRIEND_CODE))
                                              : std::wstring(L"Server Address")));
    m_nameInput.init(UIString(m_name), eControl_Name);
    m_nameInput.SetCharLimit(m_friendMode ? kNicknameLimit : kNameLimit);
    m_addressInput.init(UIString(m_address), eControl_Address);
    m_addressInput.SetCharLimit(m_friendMode ? kCodeLimit : kAddressLimit);
    m_okButton.init(UIString(L10N::GetString(IDS_CONFIRM_OK)), eControl_Ok);
    CloseTitleGap();
    gConsoleUIController.TouchBoxRebuild(this);
}

UIScene_EditServerMenu::~UIScene_EditServerMenu()
{
}

std::wstring UIScene_EditServerMenu::getMoviePath()
{
    return gApp.GetLocalPlayerCount() > 1 ? std::wstring(L"SocialPostSplit") : std::wstring(L"SocialPost");
}

void UIScene_EditServerMenu::CloseTitleGap()
{
    fuiRenderNode* rootNode = m_movie->getRootNode();
    if (rootNode->asTimeline() == NULL)
        return;
    fuiRenderNodeTimeline* root = static_cast<fuiRenderNodeTimeline*>(rootNode);
    fuiRenderNode* title = FindChild(root, "LabelText");
    fuiRenderNode* nameLabel = FindChild(root, "LabelCaption");
    fuiRenderNode* nameInput = FindChild(root, "EditCaption");
    fuiRenderNode* addressLabel = FindChild(root, "LabelDescription");
    if (title == NULL || nameLabel == NULL || nameInput == NULL || addressLabel == NULL || root->getNumChildren() == 0)
        return;
    float spacing = addressLabel->getY() - (nameInput->getY() + nameInput->getHeight());
    float gap = nameLabel->getY() - (title->getY() + title->getHeight() + spacing);
    if (gap <= 0.0f)
        return;
    fuiRenderNode* panel = root->getChildAt(0);
    float half = gap * 0.5f;
    panel->setY(panel->getY() + half);
    panel->setHeight(panel->getHeight() - gap);
    title->setY(title->getY() + half);
    for (int index = 1; index < root->getNumChildren(); ++index)
    {
        fuiRenderNode* node = root->getChildAt(index);
        if (node != title)
            node->setY(node->getY() - half);
    }
    for (size_t index = 0; index < m_controls.size(); ++index)
        m_controls[index]->UpdateControl();
}

void UIScene_EditServerMenu::SetupControl(UIControl& control, const char* name)
{
    fuiRenderNode* parent = FindParentOf(m_movie->getRootNode(), name);
    if (parent == NULL)
        parent = m_movie->getRootNode();
    control.setupControl(this, parent, std::string(name));
    m_controls.push_back(&control);
}

unsigned int UIScene_EditServerMenu::mapElementsAndNames()
{
    UIScene::mapElementsAndNames();
    SetupControl(m_title, "LabelText");
    SetupControl(m_nameLabel, "LabelCaption");
    SetupControl(m_nameInput, "EditCaption");
    SetupControl(m_addressLabel, "LabelDescription");
    SetupControl(m_addressInput, "EditDescription");
    SetupControl(m_okButton, "OK");
    return 1;
}

int UIScene_EditServerMenu::getSceneType()
{
    return EUIScene_EditServerMenu;
}

void UIScene_EditServerMenu::updateTooltips()
{
    gConsoleUIController.SetTooltips(m_playerId, IDS_TOOLTIPS_SELECT, IDS_TOOLTIPS_BACK, -1, -1, -1, -1, -1, -1, -1,
                                     -1, -1, -1, -1, -1, -1, -1, false, false);
}

void UIScene_EditServerMenu::updateComponents()
{
    m_layer->showComponent(m_playerId, EUIScene_Panorama, true);
    m_layer->showComponent(m_playerId, EUIScene_Logo, false);
}

void UIScene_EditServerMenu::handleDestroy()
{
    g_pInput->DestroyKeyboard();
}

void UIScene_EditServerMenu::handleInput(int playerId, int action, bool repeat, bool pressed, bool released,
                                         bool& handled)
{
    if (m_keyboardControl != -1)
        return;
    gConsoleUIController.AnimateKeyPress(playerId, action, repeat, pressed, released);
    switch (action)
    {
    case EControllerActions_MenuCancel:
        if (pressed)
        {
            navigateBack();
            handled = true;
        }
        break;
    case EControllerActions_MenuOk:
    case EControllerActions_MenuUp:
    case EControllerActions_MenuDown:
    case EControllerActions_MenuLeft:
    case EControllerActions_MenuRight:
        sendInputToMovie(action, repeat, pressed, released);
        handled = true;
        break;
    }
}

void UIScene_EditServerMenu::handlePress(int controlId, int)
{
    if (m_keyboardControl != -1)
        return;
    gConsoleUIController.PlayUISFX(SoundEvents::UI_PRESS);
    switch (controlId)
    {
    case eControl_Name:
        RequestText(controlId);
        break;
    case eControl_Address:
        if (!m_friendKnown)
            RequestText(controlId);
        break;
    case eControl_Ok:
        Save();
        break;
    }
}

void UIScene_EditServerMenu::RequestText(int control)
{
    wchar_t initialText[kAddressLimit + 1];
    std::memset(initialText, 0, sizeof(initialText));
    const std::wstring& text = control == eControl_Name ? m_name : m_address;
    unsigned int limit = control == eControl_Name ? (m_friendMode ? kNicknameLimit : kNameLimit)
                                                  : (m_friendMode ? kCodeLimit : kAddressLimit);
    text.copy(initialText, limit);
    m_keyboardControl = control;
    const wchar_t* title = m_friendMode ? (control == eControl_Name ? L10N::GetString(IDS_LCE_NICKNAME)
                                                                    : L10N::GetString(IDS_LCE_FRIEND_CODE))
                                        : (control == eControl_Name ? L"Server Name" : L"Server Address");
    unsigned int busy = g_pInput->RequestKeyboard(title,
                                                  initialText, m_playerId, limit, &KeyboardReturned,
                                                  reinterpret_cast<void*>(static_cast<uintptr_t>(GetCallbackUniqueId())),
                                                  C_4JInput::EKeyboardMode_0);
    if (busy != 0)
        m_keyboardControl = -1;
}

int UIScene_EditServerMenu::KeyboardReturned(void* context, bool accepted)
{
    gConsoleUIController.EnterCallbackIdCriticalSection();
    UIScene_EditServerMenu* scene = static_cast<UIScene_EditServerMenu*>(
        gConsoleUIController.GetSceneFromCallbackId(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(context))));
    if (scene != NULL)
    {
        int control = scene->m_keyboardControl;
        scene->m_keyboardControl = -1;
        if (accepted)
        {
            wchar_t text[kAddressLimit + 1];
            std::memset(text, 0, sizeof(text));
            g_pInput->GetText(text, kAddressLimit);
            std::wstring value = Trim(text);
            if (control == eControl_Name)
            {
                scene->m_name = value.empty() && !scene->m_friendMode ? std::wstring(kDefaultServerName) : value;
                scene->m_nameInput.setLabel(UIString(scene->m_name), false, false);
            }
            else if (control == eControl_Address)
            {
                scene->m_address = value;
                scene->m_addressInput.setLabel(UIString(scene->m_address), false, false);
            }
        }
    }
    gConsoleUIController.LeaveCallbackIdCriticalSection();
    return 0;
}

void UIScene_EditServerMenu::Save()
{
    if (m_friendMode)
    {
        lcenet::P2pService& service = lcenet::P2pService::Get();
        if (m_friendKnown)
            service.SetNickname(m_friendKey, Narrow(m_name));
        else if (service.ValidFriendCode(Narrow(m_address)))
            service.AddFriendByCode(Narrow(m_address), Narrow(m_name));
        else
        {
            static int button = IDS_CONFIRM_OK;
            gConsoleUIController.RequestErrorMessage(IDS_LCE_ADD_FRIEND, IDS_LCE_FRIEND_CODE_INVALID, &button, 1,
                                                     m_playerId, NULL, NULL, NULL, false);
            return;
        }
        navigateBack();
        return;
    }
    if (m_address.empty())
    {
        RequestText(eControl_Address);
        return;
    }
    lcenet::SavedServer server;
    server.name = m_name.empty() ? std::wstring(kDefaultServerName) : m_name;
    server.address = m_address;
    if (m_serverIndex >= 0)
        lcenet::SavedServers().Update(static_cast<size_t>(m_serverIndex), server);
    else
        lcenet::SavedServers().Add(server);
    lcenet::SavedServers().Save();
    navigateBack();
}
