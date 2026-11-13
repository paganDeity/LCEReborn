#include "platform/pc/PcBedrockSettings.hpp"

#include <cmath>

#include "game/core/Difficulty.hpp"
#include "game/core/ExtraStrings.hpp"
#include "game/core/L10N.hpp"
#include "game/core/StringIds.hpp"
#include "game/ui/UILayer.hpp"
#include "game/ui/UIScene_SettingsMenu.hpp"
#include "platform/C4JInput.hpp"
#include "platform/CInput.hpp"

namespace
{
const float kHeaderHeight = 4.4f;
const float kHeaderStrip = 0.4f;
const float kBackWidth = 4.4f;
const float kTabHeight = 4.8f;
const float kTabTop = 1.6f;
const float kRowHeight = 4.6f;
const float kSliderRowHeight = 7.2f;
const float kThumbHalf = 1.6f;

const PcOreColour kHeaderFill = {0xe6, 0xe8, 0xeb, 255};
const PcOreColour kHeaderStripColour = {0xb1, 0xb2, 0xb5, 255};
const PcOreColour kTabSelected = {0x48, 0x49, 0x4a, 255};
const PcOreColour kTabHovered = {0x58, 0x58, 0x5a, 255};
const PcOreColour kPill = {0xd0, 0xd1, 0xd4, 255};
const PcOreColour kPillHovered = {0xb1, 0xb2, 0xb5, 255};

std::wstring Word(int id)
{
    const wchar_t* text = L10N::GetString(id);
    return text != NULL ? std::wstring(text) : std::wstring();
}

bool Contains(const PcOreRect& rect, float x, float y)
{
    return x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom;
}

PcOreColour TabColour(int index)
{
    switch (index)
    {
    case 0:
        return {0x98, 0x64, 0x32, 255};
    case 1:
        return {0xc1, 0x8e, 0x3e, 255};
    case 2:
        return {0x8d, 0x5a, 0x3b, 255};
    default:
        return {0x93, 0x91, 0x84, 255};
    }
}
}

PcBedrockSettings::PcBedrockSettings(int player, void* data, UILayer* layer, bool inGame)
    : UIScene(player, layer), mSection(0), mRow(0), mZone(0), mHoveredRow(-1), mHoveredTab(-1), mWidth(0),
      mHeight(0), mInGame(inGame), mHeld(false), mPressBack(false), mPressTab(-1), mPressRow(-1), mDragRow(-1)
{
    delete static_cast<UIScene_SettingsMenu::ListMenuInitData*>(data);
    BuildSections();
    mZone = 0;
}

PcBedrockSettings::~PcBedrockSettings()
{
}

std::wstring PcBedrockSettings::getMoviePath()
{
    return L"";
}

unsigned int PcBedrockSettings::mapElementsAndNames()
{
    return 0;
}

bool PcBedrockSettings::needsReloaded()
{
    return false;
}

void PcBedrockSettings::reloadMovie(bool)
{
}

int PcBedrockSettings::getSceneType()
{
    return mInGame ? EUIScene_InGameSettingsListMenu : EUIScene_SettingsListMenu;
}

void PcBedrockSettings::AddToggle(Section& section, int label, const wchar_t* description, eGameSetting setting)
{
    Row row;
    row.label = Word(label);
    row.description = description != NULL ? description : L"";
    row.kind = EKind_Toggle;
    row.setting = setting;
    row.minimum = 0;
    row.maximum = 1;
    section.rows.push_back(row);
}

void PcBedrockSettings::AddSlider(Section& section, int label, const wchar_t* description, eGameSetting setting,
                                  int minimum, int maximum)
{
    Row row;
    row.label = Word(label);
    row.description = description != NULL ? description : L"";
    row.kind = EKind_Slider;
    row.setting = setting;
    row.minimum = minimum;
    row.maximum = maximum;
    section.rows.push_back(row);
}

void PcBedrockSettings::AddChoice(Section& section, int label, const wchar_t* description, eGameSetting setting,
                                  const std::vector<int>& choices)
{
    Row row;
    row.label = Word(label);
    row.description = description != NULL ? description : L"";
    row.kind = EKind_Choice;
    row.setting = setting;
    row.minimum = 0;
    row.maximum = static_cast<int>(choices.size()) - 1;
    for (size_t index = 0; index < choices.size(); ++index)
        row.choices.push_back(Word(choices[index]));
    section.rows.push_back(row);
}

void PcBedrockSettings::BuildSections()
{
    mSections.clear();

    Section general;
    general.title = L"General";
    general.description = L"Gameplay and interface options.";
    {
        std::vector<int> difficulty;
        for (int index = 0; index < 4; ++index)
            difficulty.push_back(Difficulty::byId(index)->getKey());
        AddChoice(general, IDS_SLIDER_DIFFICULTY, L"Changes how hard the world is.", eGameSetting_Difficulty,
                  difficulty);
        AddSlider(general, IDS_SLIDER_SENSITIVITY_INGAME, L"How quickly the camera turns.",
                  eGameSetting_GameSensitivity, 0, 200);
        AddToggle(general, IDS_VIEW_BOBBING, L"Move the camera while walking.", eGameSetting_ViewBob);
        AddToggle(general, IDS_AUTOJUMP, L"Jump automatically at a block.", eGameSetting_AutoJump);
        AddToggle(general, IDS_HINTS, L"Show hints while you play.", eGameSetting_Hints);
    }
    mSections.push_back(general);

    Section video;
    video.title = L"Video";
    video.description = L"Brightness, clouds and the on-screen display.";
    AddSlider(video, IDS_SLIDER_GAMMA, L"Adjust the brightness of the world.", eGameSetting_Gamma, 0, 100);
    AddSlider(video, IDS_SLIDER_INTERFACEOPACITY, L"Set how opaque the interface is.",
              eGameSetting_InterfaceOpacity, 0, 100);
    AddToggle(video, IDS_CHECKBOX_RENDER_CLOUDS, L"Draw clouds in the sky.", eGameSetting_Clouds);
    AddToggle(video, IDS_CHECKBOX_DISPLAY_HUD, L"Show the heads-up display.", eGameSetting_DisplayHUD);
    AddToggle(video, IDS_CHECKBOX_DISPLAY_HAND, L"Show the held item.", eGameSetting_DisplayHand);
    mSections.push_back(video);

    Section audio;
    audio.title = Word(IDS_AUDIO);
    audio.description = L"Music and sound effect volume.";
    AddSlider(audio, IDS_SLIDER_MUSIC, L"Music volume.", eGameSetting_MusicVolume, 0, 100);
    AddSlider(audio, IDS_SLIDER_SOUND, L"Sound effect volume.", eGameSetting_SoundVolume, 0, 100);
    AddToggle(audio, IDS_CAVE_SOUNDS, L"Play sounds in caves.", eGameSetting_CaveSounds);
    AddToggle(audio, IDS_MINECART_SOUNDS, L"Play sounds while riding a minecart.", eGameSetting_MinecartSounds);
    mSections.push_back(audio);

    Section controls;
    controls.title = Word(IDS_CONTROLS);
    controls.description = L"How the camera and menus respond.";
    AddSlider(controls, IDS_SLIDER_SENSITIVITY_INMENU, L"How quickly the cursor moves in menus.",
              eGameSetting_InterfaceSensitivity, 0, 100);
    AddToggle(controls, IDS_INVERT_LOOK, L"Move the camera the other way.", eGameSetting_InvertLook);
    AddToggle(controls, IDS_SOUTHPAW, L"Swap the movement and look sticks.", eGameSetting_SouthPaw);
    AddToggle(controls, IDS_LCE_HOLD_TO_SNEAK, L"Keep sneaking while the button is held.",
              eGameSetting_HoldToSneak);
    AddToggle(controls, IDS_AUTOJUMP, L"Jump automatically at a block.", eGameSetting_AutoJump);
    mSections.push_back(controls);

    Section touch;
    touch.title = L"Touch";
    touch.description = L"On-screen controls for a phone or tablet.";
    {
        std::vector<int> scheme;
        scheme.push_back(IDS_LCE_TOUCH_JOYSTICK_TAP);
        scheme.push_back(IDS_LCE_TOUCH_JOYSTICK_CROSSHAIR);
        scheme.push_back(IDS_LCE_TOUCH_DPAD_TAP);
        AddChoice(touch, IDS_LCE_TOUCH_CONTROLS, L"Choose how movement and looking work.",
                  eGameSetting_TouchControls, scheme);
        std::vector<int> joystick;
        joystick.push_back(IDS_LCE_TOUCH_JOYSTICK_VISIBLE);
        joystick.push_back(IDS_LCE_TOUCH_JOYSTICK_HIDDEN_UNUSED);
        joystick.push_back(IDS_LCE_TOUCH_JOYSTICK_HIDDEN);
        AddChoice(touch, IDS_LCE_TOUCH_JOYSTICK, L"When the joystick is visible.", eGameSetting_TouchJoystick,
                  joystick);
        AddToggle(touch, IDS_LCE_TOUCH_LEFT_HANDED, L"Put the inventory controls on the left.",
                  eGameSetting_TouchLeftHanded);
        AddToggle(touch, IDS_LCE_TOUCH_EASY_SPRINT, L"Sprint by pushing the joystick further.",
                  eGameSetting_TouchEasySprint);
        AddToggle(touch, IDS_LCE_TOUCH_PICK_BLOCK, L"Show the pick block button.", eGameSetting_TouchPickBlock);
        AddToggle(touch, IDS_LCE_TOUCH_PERSPECTIVE, L"Show the camera perspective button.",
                  eGameSetting_TouchPerspective);
        AddToggle(touch, IDS_LCE_TOUCH_SWAP_JUMP_SNEAK, L"Swap the jump and sneak buttons.",
                  eGameSetting_TouchSwapJumpSneak);
    }
    mSections.push_back(touch);
}

int PcBedrockSettings::Value(const Row& row) const
{
    return static_cast<int>(GameSettings::GetGameSettings(m_playerId, row.setting));
}

void PcBedrockSettings::Store(const Row& row, int value)
{
    if (value < row.minimum)
        value = row.minimum;
    if (value > row.maximum)
        value = row.maximum;
    GameSettings::SetGameSettings(m_playerId, row.setting, static_cast<unsigned char>(value));
    GameSettings::CheckGameSettingsChanged(true, m_playerId);
}

void PcBedrockSettings::ActivateRow(int index)
{
    if (mSection < 0 || mSection >= static_cast<int>(mSections.size()))
        return;
    Section& section = mSections[mSection];
    if (index < 0 || index >= static_cast<int>(section.rows.size()))
        return;
    Row& row = section.rows[index];
    switch (row.kind)
    {
    case EKind_Toggle:
        Store(row, Value(row) == 1 ? 0 : 1);
        break;
    case EKind_Choice:
    {
        const int count = row.maximum + 1;
        Store(row, (Value(row) + 1) % count);
        break;
    }
    default:
        break;
    }
}

void PcBedrockSettings::AdjustRow(int index, int direction)
{
    if (mSection < 0 || mSection >= static_cast<int>(mSections.size()))
        return;
    Section& section = mSections[mSection];
    if (index < 0 || index >= static_cast<int>(section.rows.size()))
        return;
    Row& row = section.rows[index];
    const int value = Value(row);
    switch (row.kind)
    {
    case EKind_Toggle:
        Store(row, direction < 0 ? 0 : 1);
        break;
    case EKind_Choice:
    {
        const int count = row.maximum + 1;
        int next = value + direction;
        if (next < 0)
            next = count - 1;
        if (next >= count)
            next = 0;
        Store(row, next);
        break;
    }
    default:
    {
        const int step = row.maximum - row.minimum >= 200 ? 5 : 1;
        Store(row, value + direction * step);
        break;
    }
    }
}

void PcBedrockSettings::SelectSection(int index)
{
    if (index < 0 || index >= static_cast<int>(mSections.size()))
        return;
    mSection = index;
    mRow = 0;
    mZone = 1;
}

PcBedrockSettings::Layout PcBedrockSettings::ComputeLayout(int width, int height) const
{
    Layout layout;
    float rem = PcOreUI::Rem(width, height);
    layout.rem = rem;
    layout.header = {0.0f, 0.0f, static_cast<float>(width), kHeaderHeight * rem};
    layout.strip = {0.0f, layout.header.bottom, static_cast<float>(width),
                    layout.header.bottom + kHeaderStrip * rem};
    layout.back = {0.0f, 0.0f, kBackWidth * rem, kHeaderHeight * rem};
    const bool desktop = static_cast<float>(width) >= 128.0f * rem;
    const float rowWidth = desktop ? std::fmin(static_cast<float>(width) - 4.8f * rem, 128.0f * rem)
                                   : static_cast<float>(width);
    const float left = (static_cast<float>(width) - rowWidth) * 0.5f;
    const float pad = desktop ? 0.8f * rem : 0.0f;
    const float fraction = static_cast<float>(width) < 70.0f * rem ? 3.0f / 8.0f : 4.0f / 12.0f;
    const float boundary = left + rowWidth * fraction;
    const float top = layout.strip.bottom + 0.8f * rem;
    const float bottom = static_cast<float>(height) - 0.8f * rem;
    layout.sidebar = {left + pad, top, boundary - pad - 1.6f * rem, bottom};
    layout.content = {boundary + pad, top, left + rowWidth - pad - 1.6f * rem, bottom};
    layout.padding = 2.4f * rem;
    layout.headingTop = layout.content.top + layout.padding;
    layout.descriptionTop = layout.headingTop + 2.4f * rem;

    float tabTop = layout.sidebar.top + kTabTop * rem;
    for (int index = 0; index < static_cast<int>(mSections.size()); ++index)
    {
        layout.tabs.push_back({layout.sidebar.left, tabTop, layout.sidebar.right, tabTop + kTabHeight * rem});
        tabTop += kTabHeight * rem;
    }

    if (mSection < 0 || mSection >= static_cast<int>(mSections.size()))
        return layout;
    const Section& section = mSections[mSection];
    float rowTop = layout.descriptionTop + 2.0f * rem + 1.2f * rem;
    const float rowLeft = layout.content.left + layout.padding;
    const float rowRight = layout.content.right - layout.padding;
    for (size_t index = 0; index < section.rows.size(); ++index)
    {
        const float rowHeight = section.rows[index].kind == EKind_Slider ? kSliderRowHeight * rem : kRowHeight * rem;
        layout.rows.push_back({rowLeft, rowTop, rowRight, rowTop + rowHeight});
        rowTop += rowHeight;
    }
    return layout;
}

int PcBedrockSettings::TabAt(const Layout& layout, float pointerX, float pointerY) const
{
    const float x = pointerX / 1280.0f * static_cast<float>(mWidth);
    const float y = pointerY / 720.0f * static_cast<float>(mHeight);
    for (size_t index = 0; index < layout.tabs.size(); ++index)
        if (Contains(layout.tabs[index], x, y))
            return static_cast<int>(index);
    return -1;
}

int PcBedrockSettings::RowAt(const Layout& layout, float pointerX, float pointerY) const
{
    const float x = pointerX / 1280.0f * static_cast<float>(mWidth);
    const float y = pointerY / 720.0f * static_cast<float>(mHeight);
    for (size_t index = 0; index < layout.rows.size(); ++index)
        if (Contains(layout.rows[index], x, y))
            return static_cast<int>(index);
    return -1;
}

float PcBedrockSettings::SliderFraction(const Row& row, int value) const
{
    if (row.maximum <= row.minimum)
        return 0.0f;
    return static_cast<float>(value - row.minimum) / static_cast<float>(row.maximum - row.minimum);
}

void PcBedrockSettings::SetSliderFromPointer(const Row& row, const PcOreRect& rect, float pointerX)
{
    const float x = pointerX / 1280.0f * static_cast<float>(mWidth);
    const float inset = kThumbHalf * PcOreUI::Rem(mWidth, mHeight);
    const float start = rect.left + inset;
    const float end = rect.right - inset;
    if (end <= start)
        return;
    float fraction = (x - start) / (end - start);
    if (fraction < 0.0f)
        fraction = 0.0f;
    if (fraction > 1.0f)
        fraction = 1.0f;
    const int value = row.minimum
                      + static_cast<int>(std::floor(fraction * static_cast<float>(row.maximum - row.minimum) + 0.5f));
    Store(row, value);
}

void PcBedrockSettings::tick()
{
    UIScene::tick();
    float x = 0.0f;
    float y = 0.0f;
    const bool moved = g_pInput->ConsumeMenuPointer(m_playerId, &x, &y);
    const bool available = g_pInput->GetMenuPointer(m_playerId, &x, &y);
    const bool held = available && g_pInput->MenuPointerHeld(m_playerId);
    const Layout layout = ComputeLayout(mWidth, mHeight);

    if (moved)
    {
        mHoveredRow = RowAt(layout, x, y);
        mHoveredTab = TabAt(layout, x, y);
    }

    if (available && held && !mHeld)
    {
        mPressBack = Contains(layout.back, x / 1280.0f * static_cast<float>(mWidth),
                              y / 720.0f * static_cast<float>(mHeight));
        mPressTab = TabAt(layout, x, y);
        mPressRow = RowAt(layout, x, y);
        mDragRow = -1;
        if (mPressTab >= 0)
        {
            mZone = 0;
            mSection = mPressTab;
            mRow = 0;
        }
        else if (mPressRow >= 0 && mSection >= 0 && mSection < static_cast<int>(mSections.size()))
        {
            mZone = 1;
            mRow = mPressRow;
            const Row& row = mSections[mSection].rows[mPressRow];
            if (row.kind == EKind_Slider)
            {
                mDragRow = mPressRow;
                SetSliderFromPointer(row, layout.rows[mPressRow], x);
            }
        }
    }

    if (available && held && mDragRow >= 0 && mSection >= 0 && mSection < static_cast<int>(mSections.size()))
    {
        Section& section = mSections[mSection];
        if (mDragRow < static_cast<int>(section.rows.size()))
            SetSliderFromPointer(section.rows[mDragRow], layout.rows[mDragRow], x);
    }

    if (available && mHeld && !held)
    {
        if (mPressBack && Contains(layout.back, x / 1280.0f * static_cast<float>(mWidth),
                                   y / 720.0f * static_cast<float>(mHeight)))
            navigateBack();
        else if (mPressTab >= 0 && TabAt(layout, x, y) == mPressTab)
            SelectSection(mPressTab);
        else if (mPressRow >= 0 && RowAt(layout, x, y) == mPressRow && mSection >= 0
                 && mSection < static_cast<int>(mSections.size()))
        {
            const Row& row = mSections[mSection].rows[mPressRow];
            if (row.kind != EKind_Slider)
                ActivateRow(mPressRow);
        }
        mPressBack = false;
        mPressTab = -1;
        mPressRow = -1;
        mDragRow = -1;
    }
    mHeld = held;
}

void PcBedrockSettings::render(int width, int height, C4JRender::eViewportType)
{
    mWidth = width;
    mHeight = height;
    if (width <= 0 || height <= 0 || mSections.empty())
        return;
    if (mSection < 0 || mSection >= static_cast<int>(mSections.size()))
        mSection = 0;
    const Layout layout = ComputeLayout(width, height);
    PcOreCanvas canvas;
    canvas.width = width;
    canvas.height = height;
    canvas.rem = layout.rem;

    PcOreUI::Overlay(canvas, 128);

    PcOreUI::Fill(canvas, layout.header, kHeaderFill);
    PcOreUI::Fill(canvas, layout.strip, kHeaderStripColour);
    PcOreUI::TextCentred(canvas, L"SETTINGS",
                         {layout.back.right, 0.0f, static_cast<float>(width) - layout.back.right, layout.header.bottom},
                         2.0f, PcOreUI::TEXT_DARK, false);
    PcOreUI::Chevron(canvas, {2.0f * layout.rem, layout.header.bottom * 0.5f - 0.7f * layout.rem,
                              2.8f * layout.rem, layout.header.bottom * 0.5f + 0.7f * layout.rem},
                     PcOreUI::TEXT_DARK, false);

    PcOreUI::Panel(canvas, layout.sidebar);
    for (size_t index = 0; index < mSections.size(); ++index)
    {
        const PcOreRect rect = layout.tabs[index];
        const bool selected = static_cast<int>(index) == mSection;
        const bool focused = mZone == 0 && selected;
        if (selected)
            PcOreUI::Fill(canvas, rect, kTabSelected);
        else if (static_cast<int>(index) == mHoveredTab)
            PcOreUI::Fill(canvas, rect, kTabHovered);
        if (focused)
            PcOreUI::Frame(canvas, rect, layout.rem * 0.2f, PcOreUI::OUTLINE);
        const float iconSize = 2.4f * layout.rem;
        const float iconTop = rect.top + (rect.bottom - rect.top - iconSize) * 0.5f;
        PcOreUI::Fill(canvas, {rect.left + 1.6f * layout.rem, iconTop, rect.left + 1.6f * layout.rem + iconSize,
                               iconTop + iconSize},
                      TabColour(static_cast<int>(index)));
        const PcOreRect tabText = {rect.left + 1.6f * layout.rem + iconSize + 0.8f * layout.rem, rect.top, rect.right,
                                   rect.bottom};
        PcOreUI::TextIn(canvas, mSections[index].title, tabText, 1.6f, PcOreUI::TEXT, false);
    }

    PcOreUI::ContentPanel(canvas, layout.content);
    const Section& section = mSections[mSection];
    PcOreUI::Text(canvas, section.title, layout.content.left + layout.padding, layout.headingTop, 2.0f,
                  PcOreUI::TEXT, false);
    PcOreUI::Text(canvas, section.description, layout.content.left + layout.padding, layout.descriptionTop, 1.4f,
                  PcOreUI::TEXT_DIMMER, false);

    for (size_t index = 0; index < section.rows.size(); ++index)
    {
        const Row& row = section.rows[index];
        const PcOreRect rect = layout.rows[index];
        const bool focused = mZone == 1 && static_cast<int>(index) == mRow;
        const bool hovered = static_cast<int>(index) == mHoveredRow;
        PcOreUI::Row(canvas, rect, false, hovered, focused);
        const float textLeft = rect.left + 2.4f * layout.rem;
        const float textRight = rect.right - 2.4f * layout.rem;
        const bool hasDescription = !row.description.empty();
        const float labelTop = hasDescription ? rect.top + 1.0f * layout.rem
                                              : rect.top + (rect.bottom - rect.top - 2.0f * layout.rem) * 0.5f;
        PcOreUI::Text(canvas, row.label, textLeft, labelTop, 1.6f, PcOreUI::TEXT, false);
        if (hasDescription)
            PcOreUI::Text(canvas, row.description, textLeft, labelTop + 2.0f * layout.rem, 1.4f,
                          PcOreUI::TEXT_DIMMER, false);

        const int value = Value(row);
        if (row.kind == EKind_Toggle)
        {
            const float railWidth = 6.0f * layout.rem;
            const float railHeight = 2.8f * layout.rem;
            const float railTop = rect.top + (rect.bottom - rect.top - railHeight) * 0.5f;
            PcOreButtonState state;
            state.hovered = focused;
            state.pressed = false;
            state.focused = false;
            state.enabled = true;
            const PcOreRect rail = {rect.right - 2.4f * layout.rem - railWidth, railTop,
                                    rect.right - 2.4f * layout.rem, railTop + railHeight};
            PcOreUI::Toggle(canvas, rail, value == 1, state);
        }
        else if (row.kind == EKind_Slider)
        {
            std::wstring shown = std::to_wstring(value) + L"%";
            const float shownWidth = PcOreUI::TextWidth(canvas, shown, 1.6f);
            PcOreUI::Text(canvas, shown, textRight - shownWidth, rect.top + 1.0f * layout.rem, 1.6f,
                          PcOreUI::TEXT, false);
            PcOreButtonState state;
            state.hovered = focused;
            state.pressed = false;
            state.focused = false;
            state.enabled = true;
            const float trackTop = rect.bottom - 2.4f * layout.rem;
            PcOreUI::Slider(canvas, {textLeft, trackTop, textRight, trackTop + 1.6f * layout.rem},
                            SliderFraction(row, value), state);
        }
        else
        {
            const std::wstring& choice = row.choices[value < 0 ? 0 : (value > row.maximum ? row.maximum : value)];
            const float pillWidth = std::fmax(12.0f * layout.rem, PcOreUI::TextWidth(canvas, choice, 1.6f)
                                                                      + 3.2f * layout.rem);
            const float pillHeight = 3.6f * layout.rem;
            const float pillTop = rect.top + (rect.bottom - rect.top - pillHeight) * 0.5f;
            PcOreRect pill = {rect.right - 2.4f * layout.rem - pillWidth, pillTop, rect.right - 2.4f * layout.rem,
                              pillTop + pillHeight};
            PcOreUI::Fill(canvas, pill, focused ? kPillHovered : kPill);
            PcOreUI::Frame(canvas, pill, layout.rem * 0.2f, PcOreUI::BORDER);
            PcOreUI::TextCentred(canvas, choice, pill, 1.6f, PcOreUI::TEXT_DARK, false);
        }
    }

    PcOreUI::Present(canvas);
}

void PcBedrockSettings::handleInput(int, int action, bool repeat, bool pressed, bool, bool& handled)
{
    if (!pressed || repeat || mSections.empty())
        return;
    const int sectionCount = static_cast<int>(mSections.size());
    const int rowCount = static_cast<int>(mSections[mSection].rows.size());
    if (action == EControllerActions_MenuUp || action == EControllerActions_MenuDown)
    {
        const int direction = action == EControllerActions_MenuUp ? -1 : 1;
        if (mZone == 0)
        {
            mSection = (mSection + direction + sectionCount) % sectionCount;
            mRow = 0;
        }
        else if (rowCount > 0)
            mRow = (mRow + direction + rowCount) % rowCount;
        handled = true;
    }
    else if (action == EControllerActions_MenuLeft || action == EControllerActions_MenuRight)
    {
        const int direction = action == EControllerActions_MenuLeft ? -1 : 1;
        if (mZone == 0)
        {
            mSection = (mSection + direction + sectionCount) % sectionCount;
            mRow = 0;
        }
        else if (rowCount > 0)
            AdjustRow(mRow, direction);
        handled = true;
    }
    else if (action == EControllerActions_MenuPageUp || action == EControllerActions_MenuPageDown)
    {
        const int direction = action == EControllerActions_MenuPageUp ? -1 : 1;
        mSection = (mSection + direction + sectionCount) % sectionCount;
        mRow = 0;
        handled = true;
    }
    else if (action == EControllerActions_MenuA || action == EControllerActions_MenuOk)
    {
        if (mZone == 0)
            mZone = 1;
        else if (rowCount > 0)
            ActivateRow(mRow);
        handled = true;
    }
    else if (action == EControllerActions_MenuB || action == EControllerActions_MenuCancel)
    {
        if (mZone == 1)
            mZone = 0;
        else
            navigateBack();
        handled = true;
    }
}
