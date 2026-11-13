#include "platform/pc/PcBedrockUI.hpp"

#include "game/ui/BedrockUI.hpp"
#include "game/ui/UILayer.hpp"
#include "platform/pc/PcBedrockHome.hpp"
#include "platform/pc/PcBedrockPauseMenu.hpp"
#include "platform/pc/PcBedrockSettings.hpp"

void PcBedrockUI::Initialise()
{
    SetBedrockUISceneFactory(&PcBedrockUI::CreateScene);
}

UIScene* PcBedrockUI::CreateScene(int sceneType, int player, void* data, UILayer* layer)
{
    switch (sceneType)
    {
    case EUIScene_MainMenu:
        return new PcBedrockHome(player, data, layer);
    case EUIScene_PauseMenu:
        return new PcBedrockPauseMenu(player, data, layer);
    case EUIScene_SettingsListMenu:
        return new PcBedrockSettings(player, data, layer, false);
    case EUIScene_InGameSettingsListMenu:
        return new PcBedrockSettings(player, data, layer, true);
    default:
        return NULL;
    }
}
