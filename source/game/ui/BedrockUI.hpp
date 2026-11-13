#ifndef GAME_UI_BEDROCKUI_HPP
#define GAME_UI_BEDROCKUI_HPP

class UILayer;
class UIScene;

typedef UIScene* (*BedrockUISceneFactory)(int sceneType, int player, void* data, UILayer* layer);

void SetBedrockUISceneFactory(BedrockUISceneFactory factory);
UIScene* CreateBedrockUIScene(int sceneType, int player, void* data, UILayer* layer);

#endif
