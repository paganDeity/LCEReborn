#ifndef PLATFORM_PC_PCBEDROCKPAUSEMENU_HPP
#define PLATFORM_PC_PCBEDROCKPAUSEMENU_HPP

#include <string>
#include <vector>

#include "game/ui/UIScene.hpp"
#include "platform/pc/PcOreUI.hpp"

class PcBedrockPauseMenu : public UIScene
{
public:
    PcBedrockPauseMenu(int player, void* data, UILayer* layer);
    virtual ~PcBedrockPauseMenu();

    virtual std::wstring getMoviePath();
    virtual unsigned int mapElementsAndNames();
    virtual bool needsReloaded();
    virtual void reloadMovie(bool force);
    virtual int getSceneType();
    virtual void tick();
    virtual void render(int width, int height, C4JRender::eViewportType viewport);
    virtual void handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled);

private:
    enum EAction
    {
        EAction_Resume,
        EAction_Settings,
        EAction_Skins,
        EAction_Quit,
        EAction_Count
    };

    struct Entry
    {
        std::wstring label;
        EAction action;
        EPcOreVariant variant;
    };

    struct Layout
    {
        float rem;
        bool compact;
        PcOreRect panel;
        PcOreRect character;
        PcOreRect logo;
        std::vector<PcOreRect> actions;
        PcOreRect skins;
        float actionPadding;
        float verticalPadding;
        float actionHeight;
    };

    Layout ComputeLayout(int width, int height) const;
    int VisibleCount(const Layout& layout) const;
    int EntryIndex(const Layout& layout, int position) const;
    int ActionAt(int width, int height, float pointerX, float pointerY) const;
    void Activate(int position);
    void Quit();

    std::vector<Entry> mEntries;
    int mSelected;
    int mHovered;
    int mWidth;
    int mHeight;
    PcOrePointer mPointer;
};

#endif
