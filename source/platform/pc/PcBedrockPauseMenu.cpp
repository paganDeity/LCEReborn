#include "platform/pc/PcBedrockPauseMenu.hpp"

#include <cmath>

#include "game/core/ConsoleUIController.hpp"
#include "game/core/L10N.hpp"
#include "game/core/StringIds.hpp"
#include "game/ui/UILayer.hpp"
#include "game/ui/UIScene_PauseMenu.hpp"
#include "platform/C4JInput.hpp"
#include "platform/CInput.hpp"
#include "platform/CProfile.hpp"

extern ConsoleUIController gConsoleUIController;

namespace
{
const float kActionHeight = 4.8f;
const float kActionGap = 1.2f;
const float kHeadingHeight = 2.4f + 0.8f;
const float kCaptionHeight = 2.0f + 1.6f;
const float kLogoTopGap = 0.8f;
const float kLogoHeight = 8.8f;
const float kLogoBottomGap = 2.0f;
const int kSkinsHit = 1000;
}

PcBedrockPauseMenu::PcBedrockPauseMenu(int player, void*, UILayer* layer)
    : UIScene(player, layer), mSelected(0), mHovered(-1), mWidth(0), mHeight(0), mPointer()
{
    Entry resume;
    resume.label = L10N::GetString(IDS_RESUME_GAME);
    resume.action = EAction_Resume;
    resume.variant = EPcOreVariant_Hero;
    mEntries.push_back(resume);
    Entry settings;
    settings.label = L10N::GetString(IDS_SETTINGS);
    settings.action = EAction_Settings;
    settings.variant = EPcOreVariant_Secondary;
    mEntries.push_back(settings);
    Entry skins;
    skins.label = L10N::GetString(IDS_CHANGE_SKIN);
    skins.action = EAction_Skins;
    skins.variant = EPcOreVariant_Neutral;
    mEntries.push_back(skins);
    Entry quit;
    quit.label = L"Save & Quit";
    quit.action = EAction_Quit;
    quit.variant = EPcOreVariant_Neutral;
    mEntries.push_back(quit);
}

PcBedrockPauseMenu::~PcBedrockPauseMenu()
{
}

std::wstring PcBedrockPauseMenu::getMoviePath()
{
    return L"";
}

unsigned int PcBedrockPauseMenu::mapElementsAndNames()
{
    return 0;
}

bool PcBedrockPauseMenu::needsReloaded()
{
    return false;
}

void PcBedrockPauseMenu::reloadMovie(bool)
{
}

int PcBedrockPauseMenu::getSceneType()
{
    return EUIScene_PauseMenu;
}

int PcBedrockPauseMenu::VisibleCount(const Layout& layout) const
{
    return layout.compact ? 4 : 3;
}

int PcBedrockPauseMenu::EntryIndex(const Layout& layout, int position) const
{
    if (layout.compact)
        return position;
    return position < 2 ? position : position + 1;
}

PcBedrockPauseMenu::Layout PcBedrockPauseMenu::ComputeLayout(int width, int height) const
{
    Layout layout;
    float rem = PcOreUI::Rem(width, height);
    const float margin = std::fmin(2.4f * rem, std::fmin(static_cast<float>(width) * 0.04f,
                                                         static_cast<float>(height) * 0.025f));
    float panelWidth = std::fmin(78.0f * rem, static_cast<float>(width) - margin * 2.0f);
    bool compact = panelWidth < 58.0f * rem || static_cast<float>(height) < 44.0f * rem;
    int count = compact ? 4 : 3;
    float padding = compact ? 1.6f : 3.2f;
    float logo = compact ? 0.0f : kLogoTopGap + kLogoHeight + kLogoBottomGap;
    float minimum = padding * 2.0f + logo + kHeadingHeight + kCaptionHeight + kActionHeight * count
                    + kActionGap * (count - 1);
    float scale = 1.0f;
    const float availableHeight = static_cast<float>(height) - margin * 2.0f;
    if (availableHeight < rem * minimum)
    {
        scale = availableHeight / (rem * minimum);
        if (scale < 0.2f)
            scale = 0.2f;
    }
    rem *= scale;
    panelWidth = std::fmin(78.0f * rem, static_cast<float>(width) - margin * 2.0f);
    compact = panelWidth < 58.0f * rem || static_cast<float>(height) < 44.0f * rem;
    count = compact ? 4 : 3;
    padding = compact ? 1.6f : 3.2f;
    logo = compact ? 0.0f : kLogoTopGap + kLogoHeight + kLogoBottomGap;
    minimum = padding * 2.0f + logo + kHeadingHeight + kCaptionHeight + kActionHeight * count
              + kActionGap * (count - 1);
    float panelHeight = (compact ? 32.0f : 49.6f) * rem;
    if (panelHeight > availableHeight)
        panelHeight = availableHeight;
    if (panelHeight < rem * minimum)
        panelHeight = rem * minimum;
    const float gap = kActionGap * rem;
    const float actionHeight = kActionHeight * rem;
    const float left = (static_cast<float>(width) - panelWidth) * 0.5f;
    const float top = (static_cast<float>(height) - panelHeight) * 0.5f;
    const float bottom = top + panelHeight;
    const float actionRight = compact ? left + panelWidth : left + panelWidth * 0.54f;
    layout.rem = rem;
    layout.compact = compact;
    layout.panel = {left, top, actionRight, bottom};
    layout.character = compact ? PcOreRect{0.0f, 0.0f, 0.0f, 0.0f}
                               : PcOreRect{actionRight + 1.6f * rem, top, left + panelWidth, bottom};
    layout.actionPadding = compact ? padding * rem : 2.4f * rem;
    layout.verticalPadding = compact ? padding * rem : 3.2f * rem;
    layout.actionHeight = actionHeight;

    const float x = left + layout.actionPadding;
    const float right = actionRight - layout.actionPadding;
    float y = top + layout.verticalPadding;
    if (!compact)
    {
        y += kLogoTopGap * rem;
        layout.logo = {x, y, right, y + kLogoHeight * rem};
        y += (kLogoHeight + kLogoBottomGap) * rem;
    }
    else
    {
        layout.logo = PcOreRect{0.0f, 0.0f, 0.0f, 0.0f};
    }
    const float groupHeight = kHeadingHeight * rem + kCaptionHeight * rem + actionHeight * count + gap * (count - 1);
    if (!compact)
        y = std::fmax(y, bottom - layout.verticalPadding - groupHeight);
    y += kHeadingHeight * rem + kCaptionHeight * rem;

    layout.actions.clear();
    for (int position = 0; position < count; ++position)
    {
        const float actionTop = y + static_cast<float>(position) * (actionHeight + gap);
        layout.actions.push_back({x, actionTop, right, actionTop + actionHeight});
    }
    layout.skins = PcOreRect{0.0f, 0.0f, 0.0f, 0.0f};
    if (!compact && layout.character.right > layout.character.left)
    {
        const float pad2 = 2.4f * rem;
        const float cx = layout.character.left + pad2;
        const float cr = layout.character.right - pad2;
        const float buttonTop = layout.character.bottom - pad2 - actionHeight;
        layout.skins = {cx, buttonTop, cr, buttonTop + actionHeight};
    }
    return layout;
}

int PcBedrockPauseMenu::ActionAt(int width, int height, float pointerX, float pointerY) const
{
    const Layout layout = ComputeLayout(width, height);
    PcOreCanvas canvas;
    canvas.width = width;
    canvas.height = height;
    canvas.rem = layout.rem;
    for (int position = 0; position < VisibleCount(layout); ++position)
        canvas.Hit(layout.actions[position], position);
    if (layout.skins.right > layout.skins.left)
        canvas.Hit(layout.skins, kSkinsHit);
    return canvas.HitAt(pointerX, pointerY);
}

void PcBedrockPauseMenu::Activate(int position)
{
    const Layout layout = ComputeLayout(mWidth, mHeight);
    if (position < 0 || position >= VisibleCount(layout))
        return;
    switch (mEntries[EntryIndex(layout, position)].action)
    {
    case EAction_Resume:
        navigateBack();
        break;
    case EAction_Settings:
        gConsoleUIController.NavigateToScene(m_playerId, EUIScene_InGameSettingsListMenu, NULL, EUILayer_Scene,
                                             EUIGroup_PlayerDefault);
        break;
    case EAction_Skins:
        gConsoleUIController.NavigateToScene(m_playerId, EUIScene_SkinSelectMenu, NULL, EUILayer_Scene,
                                             EUIGroup_PlayerDefault);
        break;
    case EAction_Quit:
        Quit();
        break;
    default:
        break;
    }
}

void PcBedrockPauseMenu::Quit()
{
    UIScene_PauseMenu::OpenExitGameDialog(m_playerId, reinterpret_cast<void*>(GetCallbackUniqueId()),
                                           UIScene_PauseMenu::ExitGameDialogReturned,
                                           UIScene_PauseMenu::ExitGameSaveDialogReturned,
                                           UIScene_PauseMenu::EOpenExitGameDialog_ExitGame);
}

void PcBedrockPauseMenu::tick()
{
    UIScene::tick();
    float x = 0.0f;
    float y = 0.0f;
    const bool moved = g_pInput->ConsumeMenuPointer(m_playerId, &x, &y);
    if (!moved)
        g_pInput->GetMenuPointer(m_playerId, &x, &y);
    if (moved)
        mHovered = ActionAt(mWidth, mHeight, x, y);
    if (mPointer.Update(m_playerId, &x, &y))
    {
        const int clicked = ActionAt(mWidth, mHeight, x, y);
        if (clicked == kSkinsHit)
        {
            gConsoleUIController.NavigateToScene(m_playerId, EUIScene_SkinSelectMenu, NULL, EUILayer_Scene,
                                                 EUIGroup_PlayerDefault);
        }
        else if (clicked >= 0)
        {
            mSelected = clicked;
            Activate(clicked);
        }
    }
}

void PcBedrockPauseMenu::render(int width, int height, C4JRender::eViewportType)
{
    mWidth = width;
    mHeight = height;
    if (width <= 0 || height <= 0)
        return;
    const Layout layout = ComputeLayout(width, height);
    PcOreCanvas canvas;
    canvas.width = width;
    canvas.height = height;
    canvas.rem = layout.rem;

    PcOreUI::Overlay(canvas, 128);
    PcOreUI::Panel(canvas, layout.panel);

    const float x = layout.panel.left + layout.actionPadding;
    const float right = layout.panel.right - layout.actionPadding;
    float headingY = layout.actions.empty() ? layout.panel.top : layout.actions[0].top;
    headingY -= (kHeadingHeight + kCaptionHeight) * layout.rem;
    if (layout.logo.right > layout.logo.left)
    {
        const float aspect = PcOreUI::LogoAspect();
        PcOreRect art = layout.logo;
        if (aspect > 0.0f)
        {
            const float height = layout.logo.bottom - layout.logo.top;
            const float width = std::fmin(height * aspect, layout.logo.right - layout.logo.left);
            const float left = (layout.logo.left + layout.logo.right - width) * 0.5f;
            art = {left, layout.logo.top, left + width, layout.logo.bottom};
        }
        if (!PcOreUI::Logo(canvas, art))
        {
            std::wstring title = L"MINECRAFT";
            float size = 3.2f;
            const float titleWidth = PcOreUI::TextWidth(canvas, title, size);
            if (titleWidth > layout.logo.right - layout.logo.left && titleWidth > 0.0f)
                size *= (layout.logo.right - layout.logo.left) / titleWidth;
            PcOreUI::TextCentred(canvas, title, layout.logo, size, PcOreUI::TEXT, true);
        }
    }
    PcOreUI::Text(canvas, L"GAME MENU", x, headingY, 2.0f, PcOreUI::TEXT, false);
    PcOreUI::Text(canvas, L"Pick up where you left off.", x, headingY + kHeadingHeight * layout.rem, 1.4f,
                  PcOreUI::TEXT_DIMMER, false);

    const int count = VisibleCount(layout);
    for (int position = 0; position < count; ++position)
    {
        const Entry& entry = mEntries[EntryIndex(layout, position)];
        PcOreButtonState state;
        state.hovered = position == mSelected || position == mHovered;
        state.pressed = false;
        state.focused = position == mSelected;
        state.enabled = true;
        PcOreUI::Button(canvas, layout.actions[position], entry.variant, entry.label, state);
        canvas.Hit(layout.actions[position], position);
    }

    if (!layout.compact && layout.character.right > layout.character.left)
    {
        PcOreUI::Panel(canvas, layout.character);
        const float pad2 = 2.4f * layout.rem;
        const float cx = layout.character.left + pad2;
        const float cr = layout.character.right - pad2;
        float cy = layout.character.top + pad2;
        PcOreUI::Text(canvas, L"YOUR CHARACTER", cx, cy, 2.0f, PcOreUI::TEXT, false);
        cy += 2.4f * layout.rem;
        PcOreUI::Text(canvas, gProfile->GetDisplayName(m_playerId), cx, cy, 1.4f, PcOreUI::TEXT_DIMMER, false);
        PcOreRect preview = {cx, cy + 4.8f * layout.rem, cr, layout.skins.top - 1.6f * layout.rem};
        PcOreUI::Character(canvas, preview);
        PcOreUI::TextCentred(canvas, L"Change your skin",
                             {cx, layout.skins.top - 4.0f * layout.rem, cr, layout.skins.top}, 1.4f,
                             PcOreUI::TEXT_DIMMER, false);
        PcOreButtonState skinState;
        skinState.hovered = false;
        skinState.pressed = false;
        skinState.focused = false;
        skinState.enabled = true;
        PcOreUI::Button(canvas, layout.skins, EPcOreVariant_Secondary, L"Change Skin", skinState);
        canvas.Hit(layout.skins, kSkinsHit);
    }

    PcOreUI::Present(canvas);
}

void PcBedrockPauseMenu::handleInput(int, int action, bool repeat, bool pressed, bool, bool& handled)
{
    if (!pressed || repeat)
        return;
    const Layout layout = ComputeLayout(mWidth, mHeight);
    const int count = VisibleCount(layout);
    if (action == EControllerActions_MenuUp || action == EControllerActions_MenuDown)
    {
        const int direction = action == EControllerActions_MenuUp ? -1 : 1;
        mSelected = (mSelected + direction + count) % count;
        handled = true;
    }
    else if (action == EControllerActions_MenuLeft || action == EControllerActions_MenuRight)
    {
        const int direction = action == EControllerActions_MenuLeft ? -1 : 1;
        mSelected = (mSelected + direction + count) % count;
        handled = true;
    }
    else if (action == EControllerActions_MenuA || action == EControllerActions_MenuOk)
    {
        Activate(mSelected);
        handled = true;
    }
    else if (action == EControllerActions_MenuB || action == EControllerActions_MenuCancel)
    {
        navigateBack();
        handled = true;
    }
}
