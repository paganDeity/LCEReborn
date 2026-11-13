// Generated from game/ui/UIScene.hpp by tools/hooks/hookgen.py. Do not edit.
#ifndef GAME_MODCLASS_UISCENE_GENERATED_HPP
#define GAME_MODCLASS_UISCENE_GENERATED_HPP

#include <stddef.h>

#include "game/core/ModEvents_generated.hpp"
#include "game/core/ModHooks.hpp"
#include "game/core/ModItemNames.hpp"
#include "game/world/Level.hpp"
#include "game/world/block/Block.hpp"
#include "game/world/block/BlockState.hpp"
#include "game/world/generation/Player.hpp"
#include "game/entity/mob/LivingEntity.hpp"
#include "game/ui/UIScene.hpp"

// every virtual method of UIScene a mod can answer, on top of any game class derived from it
template <class Base>
class ModUIScene : public Base
{
public:
    using Base::Base;

    template <class B>
    auto base_UIScene_reloadMovie(bool force, int) -> decltype(static_cast<B*>(this)->B::reloadMovie(force))
    {
        static_cast<B*>(this)->B::reloadMovie(force);
    }
    template <class B>
    void base_UIScene_reloadMovie(bool force, long)
    {
        static_cast<UIScene*>(this)->UIScene::reloadMovie(force);
    }

    virtual void reloadMovie(bool force)
    {
        if (ModHooks::IsActive(kModHook_UIScene_reloadMovie))
        {
            if (!ModEvents::UISceneReloadMovie(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), force))
                return;
        }
        base_UIScene_reloadMovie<Base>(force, 0);
    }

    template <class B>
    auto base_UIScene_needsReloaded(int) -> decltype(static_cast<B*>(this)->B::needsReloaded())
    {
        return static_cast<B*>(this)->B::needsReloaded();
    }
    template <class B>
    bool base_UIScene_needsReloaded(long)
    {
        return static_cast<UIScene*>(this)->UIScene::needsReloaded();
    }

    virtual bool needsReloaded()
    {
        if (ModHooks::IsActive(kModHook_UIScene_needsReloaded))
        {
            bool result = false;
            if (!ModEvents::UISceneNeedsReloaded(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_needsReloaded<Base>(0);
    }

    template <class B>
    auto base_UIScene_hasMovie(int) -> decltype(static_cast<B*>(this)->B::hasMovie())
    {
        return static_cast<B*>(this)->B::hasMovie();
    }
    template <class B>
    bool base_UIScene_hasMovie(long)
    {
        return static_cast<UIScene*>(this)->UIScene::hasMovie();
    }

    virtual bool hasMovie()
    {
        if (ModHooks::IsActive(kModHook_UIScene_hasMovie))
        {
            bool result = false;
            if (!ModEvents::UISceneHasMovie(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_hasMovie<Base>(0);
    }

    template <class B>
    auto base_UIScene_updateSafeZone(int) -> decltype(static_cast<B*>(this)->B::updateSafeZone())
    {
        static_cast<B*>(this)->B::updateSafeZone();
    }
    template <class B>
    void base_UIScene_updateSafeZone(long)
    {
        static_cast<UIScene*>(this)->UIScene::updateSafeZone();
    }

    virtual void updateSafeZone()
    {
        if (ModHooks::IsActive(kModHook_UIScene_updateSafeZone))
        {
            if (!ModEvents::UISceneUpdateSafeZone(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_updateSafeZone<Base>(0);
    }

    template <class B>
    auto base_UIScene_updateViewportTouchOffset(int) -> decltype(static_cast<B*>(this)->B::updateViewportTouchOffset())
    {
        static_cast<B*>(this)->B::updateViewportTouchOffset();
    }
    template <class B>
    void base_UIScene_updateViewportTouchOffset(long)
    {
        static_cast<UIScene*>(this)->UIScene::updateViewportTouchOffset();
    }

    virtual void updateViewportTouchOffset()
    {
        if (ModHooks::IsActive(kModHook_UIScene_updateViewportTouchOffset))
        {
            if (!ModEvents::UISceneUpdateViewportTouchOffset(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_updateViewportTouchOffset<Base>(0);
    }

    template <class B>
    auto base_UIScene_tick(int) -> decltype(static_cast<B*>(this)->B::tick())
    {
        static_cast<B*>(this)->B::tick();
    }
    template <class B>
    void base_UIScene_tick(long)
    {
        static_cast<UIScene*>(this)->UIScene::tick();
    }

    virtual void tick()
    {
        if (ModHooks::IsActive(kModHook_UIScene_tick))
        {
            if (!ModEvents::UISceneTick(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_tick<Base>(0);
    }

    template <class B>
    auto base_UIScene_SetFocusToElement(int focus, int) -> decltype(static_cast<B*>(this)->B::SetFocusToElement(focus))
    {
        static_cast<B*>(this)->B::SetFocusToElement(focus);
    }
    template <class B>
    void base_UIScene_SetFocusToElement(int focus, long)
    {
        static_cast<UIScene*>(this)->UIScene::SetFocusToElement(focus);
    }

    virtual void SetFocusToElement(int focus)
    {
        if (ModHooks::IsActive(kModHook_UIScene_SetFocusToElement))
        {
            if (!ModEvents::UISceneSetFocusToElement(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), focus))
                return;
        }
        base_UIScene_SetFocusToElement<Base>(focus, 0);
    }

    template <class B>
    auto base_UIScene_handleTimerComplete(int timerId, int) -> decltype(static_cast<B*>(this)->B::handleTimerComplete(timerId))
    {
        static_cast<B*>(this)->B::handleTimerComplete(timerId);
    }
    template <class B>
    void base_UIScene_handleTimerComplete(int timerId, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleTimerComplete(timerId);
    }

    virtual void handleTimerComplete(int timerId)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleTimerComplete))
        {
            if (!ModEvents::UISceneHandleTimerComplete(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), timerId))
                return;
        }
        base_UIScene_handleTimerComplete<Base>(timerId, 0);
    }

    template <class B>
    auto base_UIScene_stealsFocus(int) -> decltype(static_cast<B*>(this)->B::stealsFocus())
    {
        return static_cast<B*>(this)->B::stealsFocus();
    }
    template <class B>
    bool base_UIScene_stealsFocus(long)
    {
        return static_cast<UIScene*>(this)->UIScene::stealsFocus();
    }

    virtual bool stealsFocus()
    {
        if (ModHooks::IsActive(kModHook_UIScene_stealsFocus))
        {
            bool result = false;
            if (!ModEvents::UISceneStealsFocus(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_stealsFocus<Base>(0);
    }

    template <class B>
    auto base_UIScene_hasFocus(int playerId, int) -> decltype(static_cast<B*>(this)->B::hasFocus(playerId))
    {
        return static_cast<B*>(this)->B::hasFocus(playerId);
    }
    template <class B>
    bool base_UIScene_hasFocus(int playerId, long)
    {
        return static_cast<UIScene*>(this)->UIScene::hasFocus(playerId);
    }

    virtual bool hasFocus(int playerId)
    {
        if (ModHooks::IsActive(kModHook_UIScene_hasFocus))
        {
            bool result = false;
            if (!ModEvents::UISceneHasFocus(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), playerId, result))
                return result;
        }
        return base_UIScene_hasFocus<Base>(playerId, 0);
    }

    template <class B>
    auto base_UIScene_updateTooltips(int) -> decltype(static_cast<B*>(this)->B::updateTooltips())
    {
        static_cast<B*>(this)->B::updateTooltips();
    }
    template <class B>
    void base_UIScene_updateTooltips(long)
    {
        static_cast<UIScene*>(this)->UIScene::updateTooltips();
    }

    virtual void updateTooltips()
    {
        if (ModHooks::IsActive(kModHook_UIScene_updateTooltips))
        {
            if (!ModEvents::UISceneUpdateTooltips(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_updateTooltips<Base>(0);
    }

    template <class B>
    auto base_UIScene_updateComponents(int) -> decltype(static_cast<B*>(this)->B::updateComponents())
    {
        static_cast<B*>(this)->B::updateComponents();
    }
    template <class B>
    void base_UIScene_updateComponents(long)
    {
        static_cast<UIScene*>(this)->UIScene::updateComponents();
    }

    virtual void updateComponents()
    {
        if (ModHooks::IsActive(kModHook_UIScene_updateComponents))
        {
            if (!ModEvents::UISceneUpdateComponents(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_updateComponents<Base>(0);
    }

    template <class B>
    auto base_UIScene_handleGainFocus(bool hadFocus, int) -> decltype(static_cast<B*>(this)->B::handleGainFocus(hadFocus))
    {
        static_cast<B*>(this)->B::handleGainFocus(hadFocus);
    }
    template <class B>
    void base_UIScene_handleGainFocus(bool hadFocus, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleGainFocus(hadFocus);
    }

    virtual void handleGainFocus(bool hadFocus)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleGainFocus))
        {
            if (!ModEvents::UISceneHandleGainFocus(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), hadFocus))
                return;
        }
        base_UIScene_handleGainFocus<Base>(hadFocus, 0);
    }

    template <class B>
    auto base_UIScene_handleLoseFocus(int) -> decltype(static_cast<B*>(this)->B::handleLoseFocus())
    {
        static_cast<B*>(this)->B::handleLoseFocus();
    }
    template <class B>
    void base_UIScene_handleLoseFocus(long)
    {
        static_cast<UIScene*>(this)->UIScene::handleLoseFocus();
    }

    virtual void handleLoseFocus()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleLoseFocus))
        {
            if (!ModEvents::UISceneHandleLoseFocus(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_handleLoseFocus<Base>(0);
    }

    template <class B>
    auto base_UIScene_hidesLowerScenes(int) -> decltype(static_cast<B*>(this)->B::hidesLowerScenes())
    {
        return static_cast<B*>(this)->B::hidesLowerScenes();
    }
    template <class B>
    bool base_UIScene_hidesLowerScenes(long)
    {
        return static_cast<UIScene*>(this)->UIScene::hidesLowerScenes();
    }

    virtual bool hidesLowerScenes()
    {
        if (ModHooks::IsActive(kModHook_UIScene_hidesLowerScenes))
        {
            bool result = false;
            if (!ModEvents::UISceneHidesLowerScenes(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_hidesLowerScenes<Base>(0);
    }

    template <class B>
    auto base_UIScene_blocksInput(int) -> decltype(static_cast<B*>(this)->B::blocksInput())
    {
        return static_cast<B*>(this)->B::blocksInput();
    }
    template <class B>
    bool base_UIScene_blocksInput(long)
    {
        return static_cast<UIScene*>(this)->UIScene::blocksInput();
    }

    virtual bool blocksInput()
    {
        if (ModHooks::IsActive(kModHook_UIScene_blocksInput))
        {
            bool result = false;
            if (!ModEvents::UISceneBlocksInput(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_blocksInput<Base>(0);
    }

    template <class B>
    auto base_UIScene_handlesPointer(int) -> decltype(static_cast<B*>(this)->B::handlesPointer())
    {
        return static_cast<B*>(this)->B::handlesPointer();
    }
    template <class B>
    bool base_UIScene_handlesPointer(long)
    {
        return static_cast<UIScene*>(this)->UIScene::handlesPointer();
    }

    virtual bool handlesPointer()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handlesPointer))
        {
            bool result = false;
            if (!ModEvents::UISceneHandlesPointer(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_handlesPointer<Base>(0);
    }

    template <class B>
    auto base_UIScene_GetMainPanel(int) -> decltype(static_cast<B*>(this)->B::GetMainPanel())
    {
        return static_cast<B*>(this)->B::GetMainPanel();
    }
    template <class B>
    UIControl* base_UIScene_GetMainPanel(long)
    {
        return static_cast<UIScene*>(this)->UIScene::GetMainPanel();
    }

    virtual UIControl* GetMainPanel()
    {
        if (ModHooks::IsActive(kModHook_UIScene_GetMainPanel))
        {
            UIControl* result = NULL;
            if (!ModEvents::UISceneGetMainPanel(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_GetMainPanel<Base>(0);
    }

    template <class B>
    auto base_UIScene_render(int width, int height, C4JRender::eViewportType viewport, int) -> decltype(static_cast<B*>(this)->B::render(width, height, viewport))
    {
        static_cast<B*>(this)->B::render(width, height, viewport);
    }
    template <class B>
    void base_UIScene_render(int width, int height, C4JRender::eViewportType viewport, long)
    {
        static_cast<UIScene*>(this)->UIScene::render(width, height, viewport);
    }

    virtual void render(int width, int height, C4JRender::eViewportType viewport)
    {
        if (ModHooks::IsActive(kModHook_UIScene_render))
        {
            if (!ModEvents::UISceneRender(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), width, height, static_cast<int>(viewport)))
                return;
        }
        base_UIScene_render<Base>(width, height, viewport, 0);
    }

    template <class B>
    auto base_UIScene_allowRepeat(int action, int) -> decltype(static_cast<B*>(this)->B::allowRepeat(action))
    {
        return static_cast<B*>(this)->B::allowRepeat(action);
    }
    template <class B>
    bool base_UIScene_allowRepeat(int action, long)
    {
        return static_cast<UIScene*>(this)->UIScene::allowRepeat(action);
    }

    virtual bool allowRepeat(int action)
    {
        if (ModHooks::IsActive(kModHook_UIScene_allowRepeat))
        {
            bool result = false;
            if (!ModEvents::UISceneAllowRepeat(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), action, result))
                return result;
        }
        return base_UIScene_allowRepeat<Base>(action, 0);
    }

    template <class B>
    auto base_UIScene_handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled, int) -> decltype(static_cast<B*>(this)->B::handleInput(playerId, action, repeat, pressed, released, handled))
    {
        static_cast<B*>(this)->B::handleInput(playerId, action, repeat, pressed, released, handled);
    }
    template <class B>
    void base_UIScene_handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleInput(playerId, action, repeat, pressed, released, handled);
    }

    virtual void handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleInput))
        {
            if (!ModEvents::UISceneHandleInput(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), playerId, action, repeat, pressed, released, handled))
                return;
        }
        base_UIScene_handleInput<Base>(playerId, action, repeat, pressed, released, handled, 0);
    }

    template <class B>
    auto base_UIScene_handleDestroy(int) -> decltype(static_cast<B*>(this)->B::handleDestroy())
    {
        static_cast<B*>(this)->B::handleDestroy();
    }
    template <class B>
    void base_UIScene_handleDestroy(long)
    {
        static_cast<UIScene*>(this)->UIScene::handleDestroy();
    }

    virtual void handleDestroy()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleDestroy))
        {
            if (!ModEvents::UISceneHandleDestroy(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_handleDestroy<Base>(0);
    }

    template <class B>
    auto base_UIScene_handlePreUnloadForReload(int) -> decltype(static_cast<B*>(this)->B::handlePreUnloadForReload())
    {
        static_cast<B*>(this)->B::handlePreUnloadForReload();
    }
    template <class B>
    void base_UIScene_handlePreUnloadForReload(long)
    {
        static_cast<UIScene*>(this)->UIScene::handlePreUnloadForReload();
    }

    virtual void handlePreUnloadForReload()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handlePreUnloadForReload))
        {
            if (!ModEvents::UISceneHandlePreUnloadForReload(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_handlePreUnloadForReload<Base>(0);
    }

    template <class B>
    auto base_UIScene_handlePreReload(int) -> decltype(static_cast<B*>(this)->B::handlePreReload())
    {
        static_cast<B*>(this)->B::handlePreReload();
    }
    template <class B>
    void base_UIScene_handlePreReload(long)
    {
        static_cast<UIScene*>(this)->UIScene::handlePreReload();
    }

    virtual void handlePreReload()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handlePreReload))
        {
            if (!ModEvents::UISceneHandlePreReload(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_handlePreReload<Base>(0);
    }

    template <class B>
    auto base_UIScene_handleReload(int) -> decltype(static_cast<B*>(this)->B::handleReload())
    {
        static_cast<B*>(this)->B::handleReload();
    }
    template <class B>
    void base_UIScene_handleReload(long)
    {
        static_cast<UIScene*>(this)->UIScene::handleReload();
    }

    virtual void handleReload()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleReload))
        {
            if (!ModEvents::UISceneHandleReload(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_handleReload<Base>(0);
    }

    template <class B>
    auto base_UIScene_handlePress(int controlId, int childId, int) -> decltype(static_cast<B*>(this)->B::handlePress(controlId, childId))
    {
        static_cast<B*>(this)->B::handlePress(controlId, childId);
    }
    template <class B>
    void base_UIScene_handlePress(int controlId, int childId, long)
    {
        static_cast<UIScene*>(this)->UIScene::handlePress(controlId, childId);
    }

    virtual void handlePress(int controlId, int childId)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handlePress))
        {
            if (!ModEvents::UISceneHandlePress(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), controlId, childId))
                return;
        }
        base_UIScene_handlePress<Base>(controlId, childId, 0);
    }

    template <class B>
    auto base_UIScene_handleFocusChange(int focus, int childFocus, int) -> decltype(static_cast<B*>(this)->B::handleFocusChange(focus, childFocus))
    {
        static_cast<B*>(this)->B::handleFocusChange(focus, childFocus);
    }
    template <class B>
    void base_UIScene_handleFocusChange(int focus, int childFocus, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleFocusChange(focus, childFocus);
    }

    virtual void handleFocusChange(int focus, int childFocus)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleFocusChange))
        {
            if (!ModEvents::UISceneHandleFocusChange(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), focus, childFocus))
                return;
        }
        base_UIScene_handleFocusChange<Base>(focus, childFocus, 0);
    }

    template <class B>
    auto base_UIScene_handleInitFocus(int focus, int childFocus, int) -> decltype(static_cast<B*>(this)->B::handleInitFocus(focus, childFocus))
    {
        static_cast<B*>(this)->B::handleInitFocus(focus, childFocus);
    }
    template <class B>
    void base_UIScene_handleInitFocus(int focus, int childFocus, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleInitFocus(focus, childFocus);
    }

    virtual void handleInitFocus(int focus, int childFocus)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleInitFocus))
        {
            if (!ModEvents::UISceneHandleInitFocus(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), focus, childFocus))
                return;
        }
        base_UIScene_handleInitFocus<Base>(focus, childFocus, 0);
    }

    template <class B>
    auto base_UIScene_handleCheckboxToggled(int id, bool checked, int) -> decltype(static_cast<B*>(this)->B::handleCheckboxToggled(id, checked))
    {
        static_cast<B*>(this)->B::handleCheckboxToggled(id, checked);
    }
    template <class B>
    void base_UIScene_handleCheckboxToggled(int id, bool checked, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleCheckboxToggled(id, checked);
    }

    virtual void handleCheckboxToggled(int id, bool checked)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleCheckboxToggled))
        {
            if (!ModEvents::UISceneHandleCheckboxToggled(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), id, checked))
                return;
        }
        base_UIScene_handleCheckboxToggled<Base>(id, checked, 0);
    }

    template <class B>
    auto base_UIScene_handleCheckboxElementToggled(int first, int second, bool checked, int) -> decltype(static_cast<B*>(this)->B::handleCheckboxElementToggled(first, second, checked))
    {
        static_cast<B*>(this)->B::handleCheckboxElementToggled(first, second, checked);
    }
    template <class B>
    void base_UIScene_handleCheckboxElementToggled(int first, int second, bool checked, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleCheckboxElementToggled(first, second, checked);
    }

    virtual void handleCheckboxElementToggled(int first, int second, bool checked)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleCheckboxElementToggled))
        {
            if (!ModEvents::UISceneHandleCheckboxElementToggled(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), first, second, checked))
                return;
        }
        base_UIScene_handleCheckboxElementToggled<Base>(first, second, checked, 0);
    }

    template <class B>
    auto base_UIScene_handleSliderMove(int id, int value, int) -> decltype(static_cast<B*>(this)->B::handleSliderMove(id, value))
    {
        static_cast<B*>(this)->B::handleSliderMove(id, value);
    }
    template <class B>
    void base_UIScene_handleSliderMove(int id, int value, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleSliderMove(id, value);
    }

    virtual void handleSliderMove(int id, int value)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleSliderMove))
        {
            if (!ModEvents::UISceneHandleSliderMove(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), id, value))
                return;
        }
        base_UIScene_handleSliderMove<Base>(id, value, 0);
    }

    template <class B>
    auto base_UIScene_handleSliderElementMove(int first, int second, int value, int) -> decltype(static_cast<B*>(this)->B::handleSliderElementMove(first, second, value))
    {
        static_cast<B*>(this)->B::handleSliderElementMove(first, second, value);
    }
    template <class B>
    void base_UIScene_handleSliderElementMove(int first, int second, int value, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleSliderElementMove(first, second, value);
    }

    virtual void handleSliderElementMove(int first, int second, int value)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleSliderElementMove))
        {
            if (!ModEvents::UISceneHandleSliderElementMove(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), first, second, value))
                return;
        }
        base_UIScene_handleSliderElementMove<Base>(first, second, value, 0);
    }

    template <class B>
    auto base_UIScene_handleAnimationEnd(int) -> decltype(static_cast<B*>(this)->B::handleAnimationEnd())
    {
        static_cast<B*>(this)->B::handleAnimationEnd();
    }
    template <class B>
    void base_UIScene_handleAnimationEnd(long)
    {
        static_cast<UIScene*>(this)->UIScene::handleAnimationEnd();
    }

    virtual void handleAnimationEnd()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleAnimationEnd))
        {
            if (!ModEvents::UISceneHandleAnimationEnd(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_handleAnimationEnd<Base>(0);
    }

    template <class B>
    auto base_UIScene_handleSelectionChanged(int selection, int) -> decltype(static_cast<B*>(this)->B::handleSelectionChanged(selection))
    {
        static_cast<B*>(this)->B::handleSelectionChanged(selection);
    }
    template <class B>
    void base_UIScene_handleSelectionChanged(int selection, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleSelectionChanged(selection);
    }

    virtual void handleSelectionChanged(int selection)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleSelectionChanged))
        {
            if (!ModEvents::UISceneHandleSelectionChanged(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), selection))
                return;
        }
        base_UIScene_handleSelectionChanged<Base>(selection, 0);
    }

    template <class B>
    auto base_UIScene_handleRequestMoreData(int id, bool forward, int) -> decltype(static_cast<B*>(this)->B::handleRequestMoreData(id, forward))
    {
        static_cast<B*>(this)->B::handleRequestMoreData(id, forward);
    }
    template <class B>
    void base_UIScene_handleRequestMoreData(int id, bool forward, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleRequestMoreData(id, forward);
    }

    virtual void handleRequestMoreData(int id, bool forward)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleRequestMoreData))
        {
            if (!ModEvents::UISceneHandleRequestMoreData(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), id, forward))
                return;
        }
        base_UIScene_handleRequestMoreData<Base>(id, forward, 0);
    }

    template <class B>
    auto base_UIScene_handleTouchBoxRebuild(int) -> decltype(static_cast<B*>(this)->B::handleTouchBoxRebuild())
    {
        static_cast<B*>(this)->B::handleTouchBoxRebuild();
    }
    template <class B>
    void base_UIScene_handleTouchBoxRebuild(long)
    {
        static_cast<UIScene*>(this)->UIScene::handleTouchBoxRebuild();
    }

    virtual void handleTouchBoxRebuild()
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleTouchBoxRebuild))
        {
            if (!ModEvents::UISceneHandleTouchBoxRebuild(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_handleTouchBoxRebuild<Base>(0);
    }

    template <class B>
    auto base_UIScene_HandleDLCMountingComplete(int) -> decltype(static_cast<B*>(this)->B::HandleDLCMountingComplete())
    {
        static_cast<B*>(this)->B::HandleDLCMountingComplete();
    }
    template <class B>
    void base_UIScene_HandleDLCMountingComplete(long)
    {
        static_cast<UIScene*>(this)->UIScene::HandleDLCMountingComplete();
    }

    virtual void HandleDLCMountingComplete()
    {
        if (ModHooks::IsActive(kModHook_UIScene_HandleDLCMountingComplete))
        {
            if (!ModEvents::UISceneHandleDLCMountingComplete(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_HandleDLCMountingComplete<Base>(0);
    }

    template <class B>
    auto base_UIScene_HandleDLCInstalled(int) -> decltype(static_cast<B*>(this)->B::HandleDLCInstalled())
    {
        static_cast<B*>(this)->B::HandleDLCInstalled();
    }
    template <class B>
    void base_UIScene_HandleDLCInstalled(long)
    {
        static_cast<UIScene*>(this)->UIScene::HandleDLCInstalled();
    }

    virtual void HandleDLCInstalled()
    {
        if (ModHooks::IsActive(kModHook_UIScene_HandleDLCInstalled))
        {
            if (!ModEvents::UISceneHandleDLCInstalled(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_HandleDLCInstalled<Base>(0);
    }

    template <class B>
    auto base_UIScene_HandleTMSBanFileRetrieved(int) -> decltype(static_cast<B*>(this)->B::HandleTMSBanFileRetrieved())
    {
        static_cast<B*>(this)->B::HandleTMSBanFileRetrieved();
    }
    template <class B>
    void base_UIScene_HandleTMSBanFileRetrieved(long)
    {
        static_cast<UIScene*>(this)->UIScene::HandleTMSBanFileRetrieved();
    }

    virtual void HandleTMSBanFileRetrieved()
    {
        if (ModHooks::IsActive(kModHook_UIScene_HandleTMSBanFileRetrieved))
        {
            if (!ModEvents::UISceneHandleTMSBanFileRetrieved(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this))))
                return;
        }
        base_UIScene_HandleTMSBanFileRetrieved<Base>(0);
    }

    template <class B>
    auto base_UIScene_HandleMessage(EUIMessage message, void* data, int) -> decltype(static_cast<B*>(this)->B::HandleMessage(message, data))
    {
        static_cast<B*>(this)->B::HandleMessage(message, data);
    }
    template <class B>
    void base_UIScene_HandleMessage(EUIMessage message, void* data, long)
    {
        static_cast<UIScene*>(this)->UIScene::HandleMessage(message, data);
    }

    virtual void HandleMessage(EUIMessage message, void* data)
    {
        if (ModHooks::IsActive(kModHook_UIScene_HandleMessage))
        {
            if (!ModEvents::UISceneHandleMessage(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), static_cast<int>(message)))
                return;
        }
        base_UIScene_HandleMessage<Base>(message, data, 0);
    }

    template <class B>
    auto base_UIScene_HandleSaveDeviceRemoved(int deviceIndex, int) -> decltype(static_cast<B*>(this)->B::HandleSaveDeviceRemoved(deviceIndex))
    {
        static_cast<B*>(this)->B::HandleSaveDeviceRemoved(deviceIndex);
    }
    template <class B>
    void base_UIScene_HandleSaveDeviceRemoved(int deviceIndex, long)
    {
        static_cast<UIScene*>(this)->UIScene::HandleSaveDeviceRemoved(deviceIndex);
    }

    virtual void HandleSaveDeviceRemoved(int deviceIndex)
    {
        if (ModHooks::IsActive(kModHook_UIScene_HandleSaveDeviceRemoved))
        {
            if (!ModEvents::UISceneHandleSaveDeviceRemoved(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), deviceIndex))
                return;
        }
        base_UIScene_HandleSaveDeviceRemoved<Base>(deviceIndex, 0);
    }

    template <class B>
    auto base_UIScene_handleTouchInput(unsigned int playerId, int touchX, int touchY, int touchBox, bool pressed, bool repeat, bool released, int) -> decltype(static_cast<B*>(this)->B::handleTouchInput(playerId, touchX, touchY, touchBox, pressed, repeat, released))
    {
        static_cast<B*>(this)->B::handleTouchInput(playerId, touchX, touchY, touchBox, pressed, repeat, released);
    }
    template <class B>
    void base_UIScene_handleTouchInput(unsigned int playerId, int touchX, int touchY, int touchBox, bool pressed, bool repeat, bool released, long)
    {
        static_cast<UIScene*>(this)->UIScene::handleTouchInput(playerId, touchX, touchY, touchBox, pressed, repeat, released);
    }

    virtual void handleTouchInput(unsigned int playerId, int touchX, int touchY, int touchBox, bool pressed, bool repeat, bool released)
    {
        if (ModHooks::IsActive(kModHook_UIScene_handleTouchInput))
        {
            if (!ModEvents::UISceneHandleTouchInput(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), static_cast<int>(playerId), touchX, touchY, touchBox, pressed, repeat, released))
                return;
        }
        base_UIScene_handleTouchInput<Base>(playerId, touchX, touchY, touchBox, pressed, repeat, released, 0);
    }

    template <class B>
    auto base_UIScene_isReadyToDelete(int) -> decltype(static_cast<B*>(this)->B::isReadyToDelete())
    {
        return static_cast<B*>(this)->B::isReadyToDelete();
    }
    template <class B>
    bool base_UIScene_isReadyToDelete(long)
    {
        return static_cast<UIScene*>(this)->UIScene::isReadyToDelete();
    }

    virtual bool isReadyToDelete()
    {
        if (ModHooks::IsActive(kModHook_UIScene_isReadyToDelete))
        {
            bool result = false;
            if (!ModEvents::UISceneIsReadyToDelete(static_cast<UIScene*>(const_cast<ModUIScene<Base>*>(this)), result))
                return result;
        }
        return base_UIScene_isReadyToDelete<Base>(0);
    }

    // not answerable yet, a parameter or result type has no conversion: customDraw, getSafeZoneHalfHeight, getSafeZoneHalfWidth, getSubSceneType, mapElementsAndNames
};

#endif
