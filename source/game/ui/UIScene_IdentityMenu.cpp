#include "game/ui/UIScene_IdentityMenu.hpp"
#include "game/core/StringIds.hpp"

#include <cstdio>
#include <cstring>
#include <stdint.h>

#include "game/client/audio/SoundEvents.hpp"
#include "game/core/CConsoleMinecraftApp.hpp"
#include "game/core/ConsoleUIController.hpp"
#include "game/core/ExtraStrings.hpp"
#include "game/core/L10N.hpp"
#include "game/ui/FJ_SkinSelect.hpp"
#include "game/ui/UIScene_SkinSelectMenu.hpp"
#include "game/ui/UILayer.hpp"
#include "game/ui/UIString.hpp"
#include "game/ui/fuiFile.hpp"
#include "game/ui/fuiRenderNode.hpp"
#include "platform/CInput.hpp"

extern ConsoleUIController gConsoleUIController;

namespace
{
const unsigned int kNameLimit = 16;

std::wstring Widen(const std::string& text)
{
    return std::wstring(text.begin(), text.end());
}

std::wstring Text(int id)
{
    return std::wstring(L10N::GetString(id));
}

std::wstring Info(const PlayerIdentityEntry& entry)
{
    if (!entry.usable)
        return entry.problem.empty() ? Text(IDS_LCE_IDENTITY_UNUSABLE)
                                     : Text(IDS_LCE_IDENTITY_UNUSABLE) + L": " + Widen(entry.problem);
    std::wstring skin = entry.bundledSkin ? Text(IDS_LCE_CUSTOM_SKIN) : entry.skin;
    return skin.empty() ? Widen(entry.friendCode) : Widen(entry.friendCode) + L"   " + skin;
}
}

UIScene_IdentityMenu::UIScene_IdentityMenu(int playerId, void*, UILayer* layer)
    : UIScene(playerId, layer),
      m_index(0),
      m_action(0),
      m_animating(false),
      m_sliding(false),
      m_pending(false),
      m_nameReady(false),
      m_nameAccepted(false),
      m_choosing(false)
{
    initialiseMovie();
    m_list.init(0);
    m_labelName.init(UIString(L""));
    m_labelInfo.init(UIString(L""));
    m_labelTitle.init(UIString(Text(IDS_LCE_IDENTITIES)));
    m_labelHint.init(UIString(L""));
    m_controlTimer.setVisible(false);
    m_icon.setVisible(false);
    m_characters[eCharacter_Current].SetFacing(UIControl_PlayerSkinPreview::eFacingFront, false);
    m_characters[eCharacter_Next1].SetFacing(UIControl_PlayerSkinPreview::eFacingRight, false);
    m_characters[eCharacter_Next2].SetFacing(UIControl_PlayerSkinPreview::eFacingRight, false);
    m_characters[eCharacter_Next3].SetFacing(UIControl_PlayerSkinPreview::eFacingRight, false);
    m_characters[eCharacter_Previous1].SetFacing(UIControl_PlayerSkinPreview::eFacingLeft, false);
    m_characters[eCharacter_Previous2].SetFacing(UIControl_PlayerSkinPreview::eFacingLeft, false);
    m_characters[eCharacter_Previous3].SetFacing(UIControl_PlayerSkinPreview::eFacingLeft, false);
    Reload();
    for (size_t index = 0; index < m_entries.size(); ++index)
        if (m_entries[index].active)
            m_index = index;
    Show();
}

UIScene_IdentityMenu::~UIScene_IdentityMenu()
{
}

std::wstring UIScene_IdentityMenu::getMoviePath()
{
    return gApp.GetLocalPlayerCount() > 1 ? std::wstring(L"SkinSelectMenuSplit") : std::wstring(L"SkinSelectMenu");
}

unsigned int UIScene_IdentityMenu::mapElementsAndNames()
{
    UIScene::mapElementsAndNames();
    fuiRenderNode* node = m_movie->getRootNode();
    m_labelTitle.setupControl(this, node, std::string("Pack_Name"));
    m_controls.push_back(&m_labelTitle);
    m_labelHint.setupControl(this, node, std::string("Pack_Type"));
    m_controls.push_back(&m_labelHint);
    m_labelName.setupControl(this, node, std::string("SkinTitle1"));
    m_controls.push_back(&m_labelName);
    m_labelInfo.setupControl(this, node, std::string("SkinTitle2"));
    m_controls.push_back(&m_labelInfo);
    m_controlTimer.setupControl(this, node, std::string("Timer"));
    m_controls.push_back(&m_controlTimer);
    m_controlCharacters.setupControl(this, node, std::string("IggyCharacters"));
    m_controls.push_back(&m_controlCharacters);
    fuiRenderNode* characters = m_controlCharacters.getFuiRenderNode();
    for (int index = 0; index < eCharacter_Count; ++index)
    {
        char name[24];
        std::snprintf(name, sizeof(name), "iggy_Character%d", index);
        m_characters[index].setupControl(this, characters, std::string(name));
        m_controls.push_back(&m_characters[index]);
    }
    m_icon.setupControl(this, node, std::string("TexturePackIcon"));
    m_controls.push_back(&m_icon);
    m_list.setupControl(this, node, std::string("SkinButtonList"));
    m_controls.push_back(&m_list);
    return 1;
}

int UIScene_IdentityMenu::getSceneType()
{
    return EUIScene_IdentityMenu;
}

int UIScene_IdentityMenu::SlotCount() const
{
    return static_cast<int>(m_entries.size());
}

size_t UIScene_IdentityMenu::Next(size_t slot) const
{
    return (slot + 1) % static_cast<size_t>(SlotCount());
}

size_t UIScene_IdentityMenu::Previous(size_t slot) const
{
    return slot == 0 ? static_cast<size_t>(SlotCount()) - 1 : slot - 1;
}

const PlayerIdentityEntry* UIScene_IdentityMenu::Current() const
{
    return m_index < m_entries.size() ? &m_entries[m_index] : NULL;
}

void UIScene_IdentityMenu::Reload()
{
    m_entries = PlayerIdentities::Get().List();
    if (m_index >= m_entries.size())
        m_index = 0;
}

void UIScene_IdentityMenu::SetFlag(int flag, bool value)
{
    FJ_SkinSelect* skin = static_cast<FJ_SkinSelect*>(m_movie->getRootNode()->getFuiNode());
    switch (flag)
    {
    case 0:
        skin->SetPlayerCharacterSelected(value, false);
        break;
    case 1:
        skin->SetCharacterLocked(value);
        break;
    case 2:
        skin->SetCharacterFavourite(value);
        break;
    default:
        skin->SetCharacterBlocked(value);
        break;
    }
}

void UIScene_IdentityMenu::BuildActions()
{
    m_actions.clear();
    const PlayerIdentityEntry* entry = Current();
    if (entry != NULL && entry->usable && !entry->active)
        m_actions.push_back(eAction_Use);
    if (entry != NULL)
        m_actions.push_back(eAction_Username);
    if (entry != NULL && entry->usable)
    {
        if (entry->active)
            m_actions.push_back(eAction_Skin);
        m_actions.push_back(eAction_SkinFile);
    }
    m_actions.push_back(eAction_New);
    m_actions.push_back(eAction_Folder);
    if (entry != NULL && !entry->active)
        m_actions.push_back(eAction_Remove);

    m_list.clearList();
    for (size_t index = 0; index < m_actions.size(); ++index)
    {
        std::wstring label;
        switch (m_actions[index])
        {
        case eAction_Use:
            label = Text(IDS_LCE_USE_IDENTITY);
            break;
        case eAction_Username:
            label = Text(IDS_LCE_USERNAME);
            break;
        case eAction_New:
            label = Text(IDS_LCE_NEW_IDENTITY);
            break;
        case eAction_Skin:
            label = Text(IDS_LCE_CHANGE_SKIN);
            break;
        case eAction_SkinFile:
            label = Text(IDS_LCE_SKIN_FILE);
            break;
        case eAction_Folder:
            label = Text(IDS_LCE_OPEN_IDENTITY_FOLDER);
            break;
        default:
            label = Text(IDS_LCE_TOOLTIP_REMOVE);
            break;
        }
        m_list.AddNewButton(label, static_cast<int>(index), true);
    }
    m_action = 0;
}

void UIScene_IdentityMenu::Apply(UIControl_PlayerSkinPreview& preview, const PlayerIdentityEntry* entry)
{
    if (entry != NULL && !entry->skinPath.empty())
        preview.SetTexture(entry->skinPath, TN_MOB_STEVE);
    else
        preview.SetTexture(std::wstring(),
                           static_cast<_TEXTURE_NAME>(UIScene_SkinSelectMenu::getTextureId(entry != NULL ? entry->defaultSkin : 0)));
}

void UIScene_IdentityMenu::PreviewSlot(int character, size_t slot)
{
    UIControl_PlayerSkinPreview& preview = m_characters[character];
    preview.setVisible(true);
    Apply(preview, slot < m_entries.size() ? &m_entries[slot] : NULL);
}

void UIScene_IdentityMenu::ShowNeighbours()
{
    for (int index = eCharacter_Next1; index < eCharacter_Count; ++index)
        m_characters[index].setVisible(false);
    const int count = SlotCount();
    int nextCount = 3;
    int previousCount = 3;
    if (count < 7)
    {
        nextCount = (count - 1) / 2;
        previousCount = count - 1 - nextCount;
    }
    size_t slot = m_index;
    for (int index = 0; index < nextCount; ++index)
    {
        slot = Next(slot);
        PreviewSlot(eCharacter_Next1 + index, slot);
    }
    slot = m_index;
    for (int index = 0; index < previousCount; ++index)
    {
        slot = Previous(slot);
        PreviewSlot(eCharacter_Previous1 + index, slot);
    }
}

void UIScene_IdentityMenu::Show()
{
    const PlayerIdentityEntry* entry = Current();
    if (entry == NULL)
    {
        m_labelName.setLabel(UIString(Text(IDS_LCE_USERNAME_PLACEHOLDER)), false, false);
        m_labelInfo.setLabel(UIString(Text(IDS_LCE_NEW_IDENTITY)), false, false);
        m_labelHint.setLabel(UIString(L""), false, false);
        m_labelName.getFuiRenderNode()->setMultiply(0.55f, 0.55f, 0.55f);
    }
    else
    {
        m_labelName.setLabel(UIString(entry->name), false, false);
        m_labelInfo.setLabel(UIString(Info(*entry)), false, false);
        m_labelHint.setLabel(UIString(Widen(entry->uuid)), false, false);
        m_labelName.getFuiRenderNode()->setMultiply(1.0f, 1.0f, 1.0f);
    }
    SetFlag(0, entry != NULL && entry->active);
    SetFlag(1, false);
    SetFlag(2, false);
    SetFlag(3, entry != NULL && !entry->usable);
    m_characters[eCharacter_Current].setVisible(true);
    Apply(m_characters[eCharacter_Current], entry);
    ShowNeighbours();
    BuildActions();
    updateTooltips();
}

void UIScene_IdentityMenu::Move(bool forward)
{
    if (m_sliding || SlotCount() < 2)
        return;
    gConsoleUIController.PlayUISFX(SoundEvents::UI_SCROLL);
    m_index = forward ? Next(m_index) : Previous(m_index);
    m_animating = true;
    m_sliding = true;
    if (forward)
    {
        m_characters[eCharacter_Current].SetFacing(UIControl_PlayerSkinPreview::eFacingLeft, true);
        m_characters[eCharacter_Next1].SetFacing(UIControl_PlayerSkinPreview::eFacingFront, true);
    }
    else
    {
        m_characters[eCharacter_Current].SetFacing(UIControl_PlayerSkinPreview::eFacingRight, true);
        m_characters[eCharacter_Previous1].SetFacing(UIControl_PlayerSkinPreview::eFacingFront, true);
    }
    sendInputToMovie(forward ? 7 : 6, false, true, false);
}

void UIScene_IdentityMenu::handleAnimationEnd()
{
    if (!m_animating)
        return;
    m_animating = false;
    m_pending = true;
}

void UIScene_IdentityMenu::customDraw(const char* name, fuiRect* rect)
{
    if (m_pending)
    {
        std::string charactersName("IggyCharacters");
        if (m_movie->getRootNode()->getFuiNode()->getChildByName(charactersName)->currentFrame() == 1)
        {
            m_characters[eCharacter_Current].SetFacing(UIControl_PlayerSkinPreview::eFacingFront, false);
            m_characters[eCharacter_Next1].SetFacing(UIControl_PlayerSkinPreview::eFacingRight, false);
            m_characters[eCharacter_Previous1].SetFacing(UIControl_PlayerSkinPreview::eFacingLeft, false);
            m_sliding = false;
            m_pending = false;
            Show();
        }
    }
    int character = -1;
    std::sscanf(name, "Character%d", &character);
    if (character >= 0 && character < eCharacter_Count)
        m_characters[character].render(rect);
}

void UIScene_IdentityMenu::tick()
{
    UIScene::tick();
    if (m_nameReady)
    {
        m_nameReady = false;
        if (m_nameAccepted)
            NameEntered(m_typed);
    }
    if (m_choosing)
    {
        std::string stem;
        std::string problem;
        if (PlayerIdentities::Get().TakeSkinFileResult(&stem, &problem))
        {
            m_choosing = false;
            if (!problem.empty())
                Tell(IDS_LCE_SKIN_FILE_FAILED);
            Reload();
            Show();
        }
    }
}

void UIScene_IdentityMenu::updateTooltips()
{
    const PlayerIdentityEntry* entry = Current();
    const int remove = entry != NULL && !entry->active ? static_cast<int>(IDS_LCE_TOOLTIP_REMOVE) : -1;
    const int edit = static_cast<int>(IDS_LCE_RENAME_IDENTITY);
    gConsoleUIController.SetTooltips(m_playerId, static_cast<int>(IDS_TOOLTIPS_SELECT), static_cast<int>(IDS_TOOLTIPS_BACK),
                                     remove, edit, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                     static_cast<int>(IDS_TOOLTIPS_NAVIGATE), false, false);
}

void UIScene_IdentityMenu::updateComponents()
{
    m_layer->showComponent(m_playerId, EUIScene_Logo, false);
}

void UIScene_IdentityMenu::handleGainFocus(bool)
{
    Reload();
    Show();
}

void UIScene_IdentityMenu::handleFocusChange(int focus, int childFocus)
{
    if (focus == 0)
    {
        m_list.updateChildFocus(childFocus);
        m_action = childFocus;
    }
}

void UIScene_IdentityMenu::handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled)
{
    switch (action)
    {
    case EControllerActions_MenuCancel:
        if (pressed && !repeat)
        {
            gConsoleUIController.AnimateKeyPress(playerId, action, repeat, pressed, released);
            gConsoleUIController.PlayUISFX(SoundEvents::UI_BACK);
            navigateBack();
            handled = true;
        }
        break;
    case EControllerActions_MenuX:
        if (pressed && !repeat && Current() != NULL && !Current()->active)
        {
            AskRemove();
            handled = true;
        }
        break;
    case EControllerActions_MenuY:
        if (pressed && !repeat)
        {
            Run(Current() != NULL ? eAction_Username : eAction_New);
            handled = true;
        }
        break;
    case EControllerActions_MenuUp:
    case EControllerActions_MenuDown:
    case EControllerActions_MenuPageUp:
    case EControllerActions_MenuPageDown:
        if (pressed)
        {
            sendInputToMovie(action, repeat, pressed, released);
            handled = true;
        }
        break;
    case EControllerActions_MenuRight:
        if (pressed)
        {
            gConsoleUIController.AnimateKeyPress(playerId, action, repeat, pressed, released);
            Move(true);
            handled = true;
        }
        break;
    case EControllerActions_MenuLeft:
        if (pressed)
        {
            gConsoleUIController.AnimateKeyPress(playerId, action, repeat, pressed, released);
            Move(false);
            handled = true;
        }
        break;
    case EControllerActions_MenuOk:
        if (pressed && !repeat && m_action >= 0 && static_cast<size_t>(m_action) < m_actions.size())
        {
            gConsoleUIController.PlayUISFX(SoundEvents::UI_PRESS);
            Run(m_actions[m_action]);
            handled = true;
        }
        break;
    default:
        break;
    }
}

void UIScene_IdentityMenu::Tell(int messageId)
{
    static int button = IDS_CONFIRM_OK;
    gConsoleUIController.RequestErrorMessage(IDS_LCE_IDENTITIES, messageId, &button, 1, m_playerId, NULL, NULL, NULL, false);
}

void UIScene_IdentityMenu::Run(int action)
{
    const PlayerIdentityEntry* entry = Current();
    switch (action)
    {
    case eAction_Use:
    {
        std::string problem;
        if (!PlayerIdentities::Get().Select(entry->stem, &problem))
        {
            Tell(PlayerIdentities::Get().Online() ? IDS_LCE_IDENTITY_ONLINE : IDS_LCE_IDENTITY_UNUSABLE);
            return;
        }
        PlayerIdentities::Get().ApplyToProfile();
        Reload();
        Show();
        break;
    }
    case eAction_Username:
        if (entry != NULL && entry->active && PlayerIdentities::Get().Online())
            Tell(IDS_LCE_IDENTITY_ONLINE);
        else
            RequestName(false);
        break;
    case eAction_New:
        RequestName(true);
        break;
    case eAction_Skin:
        if (PlayerIdentities::Get().Online())
            Tell(IDS_LCE_IDENTITY_ONLINE);
        else
            gConsoleUIController.NavigateToScene(m_playerId, EUIScene_SkinSelectMenu, NULL, EUILayer_Scene, EUIGroup_PlayerDefault);
        break;
    case eAction_SkinFile:
        if (entry != NULL && entry->active && PlayerIdentities::Get().Online())
            Tell(IDS_LCE_IDENTITY_ONLINE);
        else if (!m_choosing)
        {
            m_choosing = true;
            PlayerIdentities::Get().BeginChooseSkinFile(entry->stem);
        }
        break;
    case eAction_Folder:
        PlayerIdentities::Get().OpenFolder();
        break;
    case eAction_Remove:
        AskRemove();
        break;
    }
}

void UIScene_IdentityMenu::RequestName(bool create)
{
    const PlayerIdentityEntry* entry = create ? NULL : Current();
    m_editStem = entry != NULL ? entry->stem : std::string();
    wchar_t initial[kNameLimit + 1];
    std::memset(initial, 0, sizeof(initial));
    if (entry != NULL)
        entry->name.copy(initial, kNameLimit);
    g_pInput->RequestKeyboard(L10N::GetString(IDS_LCE_USERNAME), initial, m_playerId, kNameLimit, &KeyboardReturned,
                              reinterpret_cast<void*>(static_cast<uintptr_t>(GetCallbackUniqueId())),
                              C_4JInput::EKeyboardMode_0);
}

int UIScene_IdentityMenu::KeyboardReturned(void* context, bool accepted)
{
    gConsoleUIController.EnterCallbackIdCriticalSection();
    UIScene_IdentityMenu* scene = static_cast<UIScene_IdentityMenu*>(
        gConsoleUIController.GetSceneFromCallbackId(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(context))));
    if (scene != NULL)
    {
        wchar_t text[kNameLimit + 1];
        std::memset(text, 0, sizeof(text));
        if (accepted)
            g_pInput->GetText(text, kNameLimit);
        scene->m_typed = text;
        scene->m_nameAccepted = accepted;
        scene->m_nameReady = true;
    }
    gConsoleUIController.LeaveCallbackIdCriticalSection();
    return 0;
}

void UIScene_IdentityMenu::NameEntered(const std::wstring& name)
{
    std::string problem;
    const bool done = m_editStem.empty() ? PlayerIdentities::Get().Create(name, &problem)
                                         : PlayerIdentities::Get().Rename(m_editStem, name, &problem);
    if (!done)
    {
        Tell(IDS_LCE_IDENTITY_NAME_INVALID);
        return;
    }
    if (!m_editStem.empty() && m_editStem == PlayerIdentities::Get().ActiveStem())
        PlayerIdentities::Get().ApplyToProfile();
    Reload();
    for (size_t index = 0; index < m_entries.size(); ++index)
        if (m_editStem.empty() ? m_entries[index].name == name : m_entries[index].stem == m_editStem)
            m_index = index;
    Show();
}

void UIScene_IdentityMenu::AskRemove()
{
    if (Current() == NULL || Current()->active)
        return;
    std::string* stem = new std::string(Current()->stem);
    int parameters[2] = {static_cast<int>(IDS_CONFIRM_OK), static_cast<int>(IDS_CONFIRM_CANCEL)};
    gConsoleUIController.RequestAlertMessage(IDS_LCE_REMOVE_IDENTITY_TITLE, IDS_LCE_REMOVE_IDENTITY, parameters, 2,
                                             m_playerId, &UIScene_IdentityMenu::RemoveReturned, stem, NULL, true);
}

int UIScene_IdentityMenu::RemoveReturned(void* context, int, C4JStorage::EMessageResult result)
{
    std::string* stem = static_cast<std::string*>(context);
    std::string problem;
    if (result == C4JStorage::EMessageResult_Accept)
        PlayerIdentities::Get().Remove(*stem, &problem);
    delete stem;
    return 0;
}
