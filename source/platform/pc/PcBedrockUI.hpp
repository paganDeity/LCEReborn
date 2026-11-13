#ifndef PLATFORM_PC_PCBEDROCKUI_HPP
#define PLATFORM_PC_PCBEDROCKUI_HPP

class UILayer;
class UIScene;

class PcBedrockUI
{
public:
    static void Initialise();
    static UIScene* CreateScene(int sceneType, int player, void* data, UILayer* layer);
};

#endif
