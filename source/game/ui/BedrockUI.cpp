#include "game/ui/BedrockUI.hpp"

namespace
{
BedrockUISceneFactory sFactory = 0;
}

void SetBedrockUISceneFactory(BedrockUISceneFactory factory)
{
    sFactory = factory;
}

UIScene* CreateBedrockUIScene(int sceneType, int player, void* data, UILayer* layer)
{
    if (sFactory == 0)
        return 0;
    return sFactory(sceneType, player, data, layer);
}
