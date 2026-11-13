#ifndef PLATFORM_PC_PCBEDROCKSETTINGS_HPP
#define PLATFORM_PC_PCBEDROCKSETTINGS_HPP

#include <string>
#include <vector>

#include "game/core/GameSettings.hpp"
#include "game/ui/UIScene.hpp"
#include "platform/pc/PcOreUI.hpp"

class PcBedrockSettings : public UIScene
{
public:
    PcBedrockSettings(int player, void* data, UILayer* layer, bool inGame);
    virtual ~PcBedrockSettings();

    virtual std::wstring getMoviePath();
    virtual unsigned int mapElementsAndNames();
    virtual bool needsReloaded();
    virtual void reloadMovie(bool force);
    virtual int getSceneType();
    virtual void tick();
    virtual void render(int width, int height, C4JRender::eViewportType viewport);
    virtual void handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled);

private:
    enum EKind
    {
        EKind_Toggle,
        EKind_Slider,
        EKind_Choice
    };

    struct Row
    {
        std::wstring label;
        std::wstring description;
        EKind kind;
        eGameSetting setting;
        int minimum;
        int maximum;
        std::vector<std::wstring> choices;
    };

    struct Section
    {
        std::wstring title;
        std::wstring description;
        std::vector<Row> rows;
    };

    struct Layout
    {
        float rem;
        PcOreRect header;
        PcOreRect strip;
        PcOreRect back;
        PcOreRect sidebar;
        PcOreRect content;
        std::vector<PcOreRect> tabs;
        std::vector<PcOreRect> rows;
        float padding;
        float headingTop;
        float descriptionTop;
    };

    void BuildSections();
    void AddToggle(Section& section, int label, const wchar_t* description, eGameSetting setting);
    void AddSlider(Section& section, int label, const wchar_t* description, eGameSetting setting, int minimum,
                   int maximum);
    void AddChoice(Section& section, int label, const wchar_t* description, eGameSetting setting,
                   const std::vector<int>& choices);
    Layout ComputeLayout(int width, int height) const;
    int Value(const Row& row) const;
    void Store(const Row& row, int value);
    void ActivateRow(int index);
    void AdjustRow(int index, int direction);
    void SelectSection(int index);
    int TabAt(const Layout& layout, float x, float y) const;
    int RowAt(const Layout& layout, float x, float y) const;
    float SliderFraction(const Row& row, int value) const;
    void SetSliderFromPointer(const Row& row, const PcOreRect& rect, float x);

    std::vector<Section> mSections;
    int mSection;
    int mRow;
    int mZone;
    int mHoveredRow;
    int mHoveredTab;
    int mWidth;
    int mHeight;
    bool mInGame;
    bool mHeld;
    bool mPressBack;
    int mPressTab;
    int mPressRow;
    int mDragRow;
};

#endif
