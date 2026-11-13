#include "platform/pc/PcBedrockHome.hpp"

#include <cmath>

#include "game/core/ConsoleUIController.hpp"
#include "game/ui/UILayer.hpp"
#include "platform/CInput.hpp"
#include "platform/CProfile.hpp"

extern ConsoleUIController gConsoleUIController;

namespace
{
const float kLogoHeight = 6.4f;
const float kTitleSize = 3.2f;
const float kHeadingSize = 2.0f;
const float kCaptionSize = 1.4f;
const float kActionHeight = 4.8f;
const float kHeroHeight = 6.4f;
const float kLinkHeight = 3.6f;
}

PcBedrockHome::PcBedrockHome(int player, void*, UILayer* layer)
    : UIScene(player, layer), mSelected(0), mHovered(-1), mWidth(0), mHeight(0), mPointer()
{
}

PcBedrockHome::~PcBedrockHome()
{
}

std::wstring PcBedrockHome::getMoviePath()
{
    return L"";
}

unsigned int PcBedrockHome::mapElementsAndNames()
{
    return 0;
}

bool PcBedrockHome::needsReloaded()
{
    return false;
}

void PcBedrockHome::reloadMovie(bool)
{
}

int PcBedrockHome::getSceneType()
{
    return EUIScene_MainMenu;
}

PcBedrockHome::Layout PcBedrockHome::ComputeLayout(int width, int height) const
{
    Layout layout;
    float rem = PcOreUI::Rem(width, height);
    const float margin = std::fmin(3.2f * rem, std::fmin(static_cast<float>(width) * 0.04f,
                                                         static_cast<float>(height) * 0.035f));
    const float availableWidth = static_cast<float>(width) - margin * 2.0f;
    float column = 86.0f;
    float fitted = std::fmin(column * rem, availableWidth);
    bool compact = fitted < 68.0f * rem || static_cast<float>(height) < 44.0f * rem
                   || static_cast<float>(width) < static_cast<float>(height) * 0.85f;
    column = compact ? 44.0f : 86.0f;
    float panelWidth = std::fmin(column * rem, availableWidth);
    bool compactFooter = panelWidth < 60.0f * rem;
    float footer = compactFooter ? 10.8f : 4.8f;
    float version = compactFooter ? 4.0f : 2.8f;
    const float aspect = PcOreUI::LogoAspect();
    float logoWidth = std::fmin(56.0f * rem, panelWidth * 0.78f);
    float logoHeight = aspect > 0.0f ? logoWidth / aspect : kLogoHeight * rem;
    float availableHeight = static_cast<float>(height) - margin * 2.0f;
    float outside = logoHeight + (3.2f + footer + version) * rem;
    float groupHeight = (compact ? 29.2f : 23.6f) * rem;
    float actionWidth = panelWidth * (compact ? 1.0f : 0.59f);
    float pad = std::fmin(rem * (compact ? 2.0f : 2.4f), actionWidth * 0.08f);
    float minimum = groupHeight + pad * 2.0f;
    float scale = 1.0f;
    if (minimum + outside > availableHeight)
    {
        const float logoRem = aspect > 0.0f ? 56.0f / aspect : kLogoHeight;
        const float required = logoRem + 3.2f + footer + version + (compact ? 29.2f : 23.6f)
                               + (compact ? 2.0f : 2.4f) * 2.0f;
        scale = availableHeight / (rem * required);
        if (scale > 1.0f)
            scale = 1.0f;
        if (scale < 0.2f)
            scale = 0.2f;
    }
    rem *= scale;
    panelWidth = std::fmin(column * rem, availableWidth);
    compactFooter = panelWidth < 60.0f * rem;
    footer = compactFooter ? 10.8f : 4.8f;
    version = compactFooter ? 4.0f : 2.8f;
    logoWidth = std::fmin(56.0f * rem, panelWidth * 0.78f);
    logoHeight = aspect > 0.0f ? logoWidth / aspect : kLogoHeight * rem;
    outside = logoHeight + (3.2f + footer + version) * rem;
    groupHeight = (compact ? 29.2f : 23.6f) * rem;
    actionWidth = panelWidth * (compact ? 1.0f : 0.59f);
    pad = std::fmin(rem * (compact ? 2.0f : 2.4f), actionWidth * 0.08f);
    minimum = groupHeight + pad * 2.0f;
    float panelHeight = std::fmin(34.4f * rem, availableHeight - outside);
    if (panelHeight < minimum)
        panelHeight = minimum;
    const float gap = 1.6f * rem;
    const float left = (static_cast<float>(width) - panelWidth) * 0.5f;
    const float groupTop = (static_cast<float>(height) - panelHeight - outside) * 0.5f;
    const float logoLeft = (static_cast<float>(width) - logoWidth) * 0.5f;
    const float actionTop = groupTop + logoHeight + gap;
    const float bottom = actionTop + panelHeight;
    const float actionRight = compact ? left + panelWidth : left + panelWidth * 0.59f;

    layout.rem = rem;
    layout.compact = compact;
    layout.compactFooter = compactFooter;
    layout.logo = {logoLeft, groupTop, logoLeft + logoWidth, groupTop + logoHeight};
    layout.panel = {left, actionTop, actionRight, bottom};
    layout.character = compact ? PcOreRect{0.0f, 0.0f, 0.0f, 0.0f}
                               : PcOreRect{actionRight + 1.6f * rem, actionTop, left + panelWidth, bottom};
    layout.pad = pad;
    layout.actionPadding = pad;

    const float x = left + pad;
    const float right = actionRight - pad;
    float y = actionTop + pad;
    y += (2.4f + 0.8f) * rem;
    y += (2.0f + 1.6f) * rem;
    layout.play = {x, y, right, y + kHeroHeight * rem};
    y += (kHeroHeight + 1.2f) * rem;
    const float midpoint = (x + right) * 0.5f;
    const float split = 1.2f * rem * 0.5f;
    layout.servers = {x, y, midpoint - split, y + kActionHeight * rem};
    layout.settings = {midpoint + split, y, right, y + kActionHeight * rem};
    y += (kActionHeight + 0.8f) * rem;
    layout.skins = {x, y, right, y + kLinkHeight * rem};
    y += (kLinkHeight + 0.8f) * rem;
    layout.dressing = compact ? PcOreRect{x, y, right, y + kActionHeight * rem}
                              : PcOreRect{0.0f, 0.0f, 0.0f, 0.0f};

    if (!compact && layout.character.right > layout.character.left)
    {
        const float pad2 = 2.4f * rem;
        const float cx = layout.character.left + pad2;
        const float cr = layout.character.right - pad2;
        const float buttonTop = layout.character.bottom - pad2 - kActionHeight * rem;
        layout.changeSkin = {cx, buttonTop, cr, buttonTop + kActionHeight * rem};
    }
    else
    {
        layout.changeSkin = PcOreRect{0.0f, 0.0f, 0.0f, 0.0f};
    }

    layout.footer = {left, bottom + gap, left + panelWidth, bottom + gap + footer * rem};
    return layout;
}

PcOreRect PcBedrockHome::ActionRect(const Layout& layout, int index) const
{
    switch (index)
    {
    case 0:
        return layout.play;
    case 1:
        return layout.servers;
    case 2:
        return layout.settings;
    case 3:
        return layout.skins;
    default:
        return layout.compact ? layout.dressing : layout.changeSkin;
    }
}

int PcBedrockHome::ActionCount(const Layout&) const
{
    return 5;
}

int PcBedrockHome::ActionAt(int width, int height, float pointerX, float pointerY) const
{
    const Layout layout = ComputeLayout(width, height);
    PcOreCanvas canvas;
    canvas.width = width;
    canvas.height = height;
    canvas.rem = layout.rem;
    for (int index = 0; index < ActionCount(layout); ++index)
        canvas.Hit(ActionRect(layout, index), index);
    return canvas.HitAt(pointerX, pointerY);
}

void PcBedrockHome::Activate(int index)
{
    EUIScene scene = EUIScene_Count;
    switch (index)
    {
    case 0:
        gProfile->SetPrimaryPad(m_playerId);
        scene = EUIScene_LoadCreateJoinMenu;
        break;
    case 1:
        scene = EUIScene_JoinMenu;
        break;
    case 2:
        scene = EUIScene_SettingsListMenu;
        break;
    default:
        scene = EUIScene_SkinSelectMenu;
        break;
    }
    if (scene != EUIScene_Count)
        gConsoleUIController.NavigateToScene(m_playerId, scene, NULL, EUILayer_Scene, EUIGroup_PlayerDefault);
}

void PcBedrockHome::tick()
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
        if (clicked >= 0)
        {
            mSelected = clicked;
            Activate(clicked);
        }
    }
}

void PcBedrockHome::render(int width, int height, C4JRender::eViewportType)
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

    if (!PcOreUI::Logo(canvas, layout.logo))
    {
        std::wstring title = L"MINECRAFT";
        float titleSize = kTitleSize;
        const float titleWidth = PcOreUI::TextWidth(canvas, title, titleSize);
        if (titleWidth > layout.logo.right - layout.logo.left && titleWidth > 0.0f)
            titleSize *= (layout.logo.right - layout.logo.left) / titleWidth;
        PcOreUI::TextCentred(canvas, title, layout.logo, titleSize, PcOreUI::TEXT, true);
    }

    PcOreUI::Panel(canvas, layout.panel);
    const float x = layout.panel.left + layout.actionPadding;
    const float right = layout.panel.right - layout.actionPadding;
    float y = layout.panel.top + layout.actionPadding;
    PcOreUI::Text(canvas, L"LET'S PLAY", x, y, kHeadingSize, PcOreUI::TEXT, false);
    y += (2.4f + 0.8f) * layout.rem;
    PcOreUI::Text(canvas, L"Your worlds, friends and favorite servers.", x, y, kCaptionSize, PcOreUI::TEXT_DIMMER,
                  false);

    const int count = ActionCount(layout);
    for (int index = 0; index < count; ++index)
    {
        const PcOreRect rect = ActionRect(layout, index);
        if (rect.right <= rect.left || rect.bottom <= rect.top)
            continue;
        PcOreButtonState state;
        state.hovered = index == mSelected || index == mHovered;
        state.pressed = false;
        state.focused = index == mSelected;
        state.enabled = true;
        if (index == 0)
            PcOreUI::Button(canvas, rect, EPcOreVariant_Hero, L"Play", state);
        else if (index == 1)
            PcOreUI::Button(canvas, rect, EPcOreVariant_Secondary, L"Servers", state);
        else if (index == 2)
            PcOreUI::Button(canvas, rect, EPcOreVariant_Secondary, L"Settings", state);
        else if (index == 3)
        {
            if (state.focused)
                PcOreUI::Frame(canvas, rect, layout.rem * 0.2f, PcOreUI::OUTLINE);
            const bool lit = index == mSelected || index == mHovered;
            PcOreUI::TextIn(canvas, L"Skins", {rect.left + layout.rem, rect.top, rect.right, rect.bottom},
                            kHeadingSize, lit ? PcOreUI::TEXT : PcOreUI::TEXT_DIMMER, false);
            PcOreUI::Chevron(canvas, {rect.right - layout.rem * 2.4f, rect.top + layout.rem * 0.6f, rect.right,
                                      rect.bottom - layout.rem * 0.6f},
                             lit ? PcOreUI::TEXT : PcOreUI::TEXT_DIMMER, true);
        }
        else if (layout.compact)
            PcOreUI::Button(canvas, rect, EPcOreVariant_Neutral, L"Dressing Room", state);
        else
            PcOreUI::Button(canvas, rect, EPcOreVariant_Secondary, L"Change Skin", state);
        canvas.Hit(rect, index);
    }

    if (!layout.compact && layout.character.right > layout.character.left)
    {
        PcOreUI::Panel(canvas, layout.character);
        const float pad2 = 2.4f * layout.rem;
        const float cx = layout.character.left + pad2;
        const float cr = layout.character.right - pad2;
        float cy = layout.character.top + pad2;
        PcOreUI::Text(canvas, L"YOUR CHARACTER", cx, cy, kHeadingSize, PcOreUI::TEXT, false);
        cy += 2.4f * layout.rem;
        PcOreUI::Text(canvas, gProfile->GetDisplayName(m_playerId), cx, cy, kCaptionSize, PcOreUI::TEXT_DIMMER,
                      false);

        const float buttonTop = layout.changeSkin.top;
        PcOreRect preview = {cx, cy + 4.8f * layout.rem, cr, buttonTop - 1.6f * layout.rem};
        PcOreUI::Character(canvas, preview);

        PcOreUI::TextCentred(canvas, L"Change your skin", {cx, buttonTop - 4.0f * layout.rem, cr, buttonTop},
                             kCaptionSize, PcOreUI::TEXT_DIMMER, false);
        PcOreButtonState skinState;
        skinState.hovered = mSelected == 4 || mHovered == 4;
        skinState.pressed = false;
        skinState.focused = mSelected == 4;
        skinState.enabled = true;
        PcOreUI::Button(canvas, layout.changeSkin, EPcOreVariant_Secondary, L"Change Skin", skinState);
        canvas.Hit(layout.changeSkin, 4);
    }

    if (layout.compactFooter)
    {
        const float middle = (layout.footer.top + layout.footer.bottom) * 0.5f;
        PcOreUI::TextCentred(canvas, L"LCEReborn", {layout.footer.left, layout.footer.top, layout.footer.right,
                                                     middle},
                             1.1f, PcOreUI::TEXT_DIMMEST, false);
        PcOreUI::TextCentred(canvas, L"Minecraft: Legacy Console Edition",
                             {layout.footer.left, middle, layout.footer.right, layout.footer.bottom}, 1.1f,
                             PcOreUI::TEXT_DIMMEST, false);
    }
    else
    {
        PcOreUI::TextCentred(canvas, L"LCEReborn  /  Minecraft: Legacy Console Edition", layout.footer, 1.1f,
                             PcOreUI::TEXT_DIMMEST, false);
    }

    PcOreUI::Present(canvas);
}

void PcBedrockHome::handleInput(int, int action, bool repeat, bool pressed, bool, bool& handled)
{
    if (!pressed || repeat)
        return;
    const Layout layout = ComputeLayout(mWidth, mHeight);
    const int count = ActionCount(layout);
    if (action == EControllerActions_MenuUp || action == EControllerActions_MenuDown)
    {
        const int direction = action == EControllerActions_MenuUp ? -1 : 1;
        mSelected = (mSelected + direction + count) % count;
        handled = true;
    }
    else if (action == EControllerActions_MenuLeft || action == EControllerActions_MenuRight)
    {
        if (mSelected == 1 || mSelected == 2)
            mSelected = mSelected == 1 ? 2 : 1;
        else
        {
            const int direction = action == EControllerActions_MenuLeft ? -1 : 1;
            mSelected = (mSelected + direction + count) % count;
        }
        handled = true;
    }
    else if (action == EControllerActions_MenuA || action == EControllerActions_MenuOk)
    {
        Activate(mSelected);
        handled = true;
    }
}
