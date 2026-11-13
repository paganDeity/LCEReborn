#include "game/ui/FJ_Factory.hpp"

#include <boost/unordered_map.hpp>
#include <cstdio>
#include <cstring>
#include <string>

#include "game/ui/FJ_AchievementsMenu.hpp"
#include "game/ui/FJ_AnvilMenu.hpp"
#include "game/ui/FJ_AttackIndicatorCrosshair.hpp"
#include "game/ui/FJ_AttackIndicatorHotbar.hpp"
#include "game/ui/FJ_AvatarSelectMenu.hpp"
#include "game/ui/FJ_BeaconEffectButton_Normal.hpp"
#include "game/ui/FJ_BeaconEffectButton_Small.hpp"
#include "game/ui/FJ_BeaconMenu.hpp"
#include "game/ui/FJ_BiomeSelectMenu.hpp"
#include "game/ui/FJ_BitmapIcon.hpp"
#include "game/ui/FJ_BookAndQuillMenu.hpp"
#include "game/ui/FJ_BrewingArrow.hpp"
#include "game/ui/FJ_BrewingArrow_Small.hpp"
#include "game/ui/FJ_BrewingBubbles.hpp"
#include "game/ui/FJ_BrewingBubbles_Small.hpp"
#include "game/ui/FJ_BrewingPlate.hpp"
#include "game/ui/FJ_BrewingPlate_Small.hpp"
#include "game/ui/FJ_BrewingStandMenu.hpp"
#include "game/ui/FJ_CheckBox_Normal.hpp"
#include "game/ui/FJ_CheckBox_Small.hpp"
#include "game/ui/FJ_CheckpointMarker.hpp"
#include "game/ui/FJ_ChestMenu.hpp"
#include "game/ui/FJ_ClassicCraftingMenu.hpp"
#include "game/ui/FJ_ComponentChat.hpp"
#include "game/ui/FJ_ComponentLogo.hpp"
#include "game/ui/FJ_ControlsMenu.hpp"
#include "game/ui/FJ_CraftingMenu.hpp"
#include "game/ui/FJ_CreateWorldMenu.hpp"
#include "game/ui/FJ_CreativeMenu.hpp"
#include "game/ui/FJ_CreditsMenu.hpp"
#include "game/ui/FJ_Cursor_Normal.hpp"
#include "game/ui/FJ_Cursor_Small.hpp"
#include "game/ui/FJ_DeathMenu.hpp"
#include "game/ui/FJ_DebugBuildTools.hpp"
#include "game/ui/FJ_DebugCreateSchematic.hpp"
#include "game/ui/FJ_DebugFillArea.hpp"
#include "game/ui/FJ_DebugMarketingGuide.hpp"
#include "game/ui/FJ_DebugMenu.hpp"
#include "game/ui/FJ_DebugOptions.hpp"
#include "game/ui/FJ_DebugPuppeteer.hpp"
#include "game/ui/FJ_DebugSetCamera.hpp"
#include "game/ui/FJ_DispenserMenu.hpp"
#include "game/ui/FJ_DragonHealth_01.hpp"
#include "game/ui/FJ_DragonHealth_02.hpp"
#include "game/ui/FJ_DragonHealth_03.hpp"
#include "game/ui/FJ_DragonHealth_04.hpp"
#include "game/ui/FJ_DragonHealth_05.hpp"
#include "game/ui/FJ_DynamicHtmlText.hpp"
#include "game/ui/FJ_DynamicHtmlText_Normal.hpp"
#include "game/ui/FJ_DynamicHtmlText_Small.hpp"
#include "game/ui/FJ_DynamicHtmlText_WithPanel_Normal.hpp"
#include "game/ui/FJ_DynamicHtmlText_WithPanel_Small.hpp"
#include "game/ui/FJ_EnchantingButton_Normal.hpp"
#include "game/ui/FJ_EnchantingButton_Small.hpp"
#include "game/ui/FJ_EnchantingMenu.hpp"
#include "game/ui/FJ_EndPoem.hpp"
#include "game/ui/FJ_Eula.hpp"
#include "game/ui/FJ_ExlamationMark.hpp"
#include "game/ui/FJ_ExperienceBar.hpp"
#include "game/ui/FJ_FuiNode.hpp"
#include "game/ui/FJ_FullScreenProgress.hpp"
#include "game/ui/FJ_FurnaceArrow.hpp"
#include "game/ui/FJ_FurnaceArrow_Small.hpp"
#include "game/ui/FJ_FurnaceFlame.hpp"
#include "game/ui/FJ_FurnaceFlame_Small.hpp"
#include "game/ui/FJ_FurnaceMenu.hpp"
#include "game/ui/FJ_GameModeScoreboard.hpp"
#include "game/ui/FJ_GameModeSelection.hpp"
#include "game/ui/FJ_Gamertag.hpp"
#include "game/ui/FJ_HelpAndOptionsMenu.hpp"
#include "game/ui/FJ_HopperMenu.hpp"
#include "game/ui/FJ_HorseInventoryMenu.hpp"
#include "game/ui/FJ_HorseJumpBar.hpp"
#include "game/ui/FJ_HowToPlay.hpp"
#include "game/ui/FJ_HowToPlayMenu.hpp"
#include "game/ui/FJ_HtmlTextPanel.hpp"
#include "game/ui/FJ_HtmlTextPanelLarge.hpp"
#include "game/ui/FJ_HtmlText_Normal.hpp"
#include "game/ui/FJ_HtmlText_Small.hpp"
#include "game/ui/FJ_HtmlText_WhiteNormal.hpp"
#include "game/ui/FJ_HtmlText_WhiteSmall.hpp"
#include "game/ui/FJ_HtmlText_WithPanel_Normal.hpp"
#include "game/ui/FJ_HtmlText_WithPanel_Small.hpp"
#include "game/ui/FJ_Hud.hpp"
#include "game/ui/FJ_IconSlot.hpp"
#include "game/ui/FJ_InGameHostOptions.hpp"
#include "game/ui/FJ_InGameInfoMenu.hpp"
#include "game/ui/FJ_InGamePlayerOptions.hpp"
#include "game/ui/FJ_InGameTeleportMenu.hpp"
#include "game/ui/FJ_Intro.hpp"
#include "game/ui/FJ_InventoryMenu.hpp"
#include "game/ui/FJ_JoinGameFilterList.hpp"
#include "game/ui/FJ_JoinMenu.hpp"
#include "game/ui/FJ_KeyboardButtons_Controls_Normal.hpp"
#include "game/ui/FJ_KeyboardButtons_Controls_Small.hpp"
#include "game/ui/FJ_KeyboardButtons_Keys_Normal.hpp"
#include "game/ui/FJ_KeyboardButtons_Keys_Small.hpp"
#include "game/ui/FJ_KillBar.hpp"
#include "game/ui/FJ_KillBarSmall.hpp"
#include "game/ui/FJ_Label.hpp"
#include "game/ui/FJ_Label_BigBlack.hpp"
#include "game/ui/FJ_Label_BigGrey.hpp"
#include "game/ui/FJ_Label_BigWhite.hpp"
#include "game/ui/FJ_Label_Black.hpp"
#include "game/ui/FJ_Label_Black_Centered.hpp"
#include "game/ui/FJ_Label_Black_Centered_HUD.hpp"
#include "game/ui/FJ_Label_Black_Small_Centered.hpp"
#include "game/ui/FJ_Label_Black_Small_Centered_HUD.hpp"
#include "game/ui/FJ_Label_Black_VerySmall_Centered.hpp"
#include "game/ui/FJ_Label_Blink_Normal.hpp"
#include "game/ui/FJ_Label_Blink_Small.hpp"
#include "game/ui/FJ_Label_Book_Normal.hpp"
#include "game/ui/FJ_Label_Book_Small.hpp"
#include "game/ui/FJ_Label_Book_VerySmall.hpp"
#include "game/ui/FJ_Label_Credits_10.hpp"
#include "game/ui/FJ_Label_Credits_10_Yellow.hpp"
#include "game/ui/FJ_Label_Credits_20.hpp"
#include "game/ui/FJ_Label_Credits_20_Yellow.hpp"
#include "game/ui/FJ_Label_Credits_30.hpp"
#include "game/ui/FJ_Label_DragonHealth.hpp"
#include "game/ui/FJ_Label_DragonHealthBig.hpp"
#include "game/ui/FJ_Label_DragonHealthSmall.hpp"
#include "game/ui/FJ_Label_DragonHealthVerySmall.hpp"
#include "game/ui/FJ_Label_Green.hpp"
#include "game/ui/FJ_Label_Grey.hpp"
#include "game/ui/FJ_Label_HUD_BigWhite.hpp"
#include "game/ui/FJ_Label_HUD_Level_01.hpp"
#include "game/ui/FJ_Label_HUD_Level_02.hpp"
#include "game/ui/FJ_Label_HUD_Level_03.hpp"
#include "game/ui/FJ_Label_HUD_Level_04.hpp"
#include "game/ui/FJ_Label_HUD_Level_05.hpp"
#include "game/ui/FJ_Label_HUD_SmallWhite.hpp"
#include "game/ui/FJ_Label_HUD_Splash.hpp"
#include "game/ui/FJ_Label_HUD_SplashBig.hpp"
#include "game/ui/FJ_Label_HUD_SplashVita.hpp"
#include "game/ui/FJ_Label_HUD_VerySmallWhite.hpp"
#include "game/ui/FJ_Label_HUD_White.hpp"
#include "game/ui/FJ_Label_Html.hpp"
#include "game/ui/FJ_Label_HtmlEndPoem.hpp"
#include "game/ui/FJ_Label_HtmlEndPoemHD.hpp"
#include "game/ui/FJ_Label_HtmlSmall.hpp"
#include "game/ui/FJ_Label_OutlineLevel.hpp"
#include "game/ui/FJ_Label_OutlineLevel_Small.hpp"
#include "game/ui/FJ_Label_OutlineTitle.hpp"
#include "game/ui/FJ_Label_OutlineTitle_Small.hpp"
#include "game/ui/FJ_Label_Red.hpp"
#include "game/ui/FJ_Label_SkinSelect.hpp"
#include "game/ui/FJ_Label_SkinSelectCenter.hpp"
#include "game/ui/FJ_Label_SkinSelectCenter_Off.hpp"
#include "game/ui/FJ_Label_SkinSelectCenter_Small.hpp"
#include "game/ui/FJ_Label_SkinSelectCenter_Small_Off.hpp"
#include "game/ui/FJ_Label_SkinSelect_Off.hpp"
#include "game/ui/FJ_Label_SkinSelect_Small.hpp"
#include "game/ui/FJ_Label_SkinSelect_Small_Off.hpp"
#include "game/ui/FJ_Label_SmallBlack.hpp"
#include "game/ui/FJ_Label_SmallGreen.hpp"
#include "game/ui/FJ_Label_SmallGrey.hpp"
#include "game/ui/FJ_Label_SmallRed.hpp"
#include "game/ui/FJ_Label_SmallWhite.hpp"
#include "game/ui/FJ_Label_VerySmallBlack.hpp"
#include "game/ui/FJ_Label_VerySmallGrey.hpp"
#include "game/ui/FJ_Label_VerySmallWhite.hpp"
#include "game/ui/FJ_Label_White.hpp"
#include "game/ui/FJ_Label_WhiteHtml.hpp"
#include "game/ui/FJ_Label_WhiteHtmlSmall.hpp"
#include "game/ui/FJ_Label_White_Centered.hpp"
#include "game/ui/FJ_Label_White_Small_Centered.hpp"
#include "game/ui/FJ_Label_White_VerySmall_Centered.hpp"
#include "game/ui/FJ_LaunchMoreOptionsMenu.hpp"
#include "game/ui/FJ_LayoutButton.hpp"
#include "game/ui/FJ_LayoutButton_Small.hpp"
#include "game/ui/FJ_LeaderboardEntryNormal.hpp"
#include "game/ui/FJ_LeaderboardEntrySmall.hpp"
#include "game/ui/FJ_LeaderboardHeaderNormal.hpp"
#include "game/ui/FJ_LeaderboardHeaderSmall.hpp"
#include "game/ui/FJ_LeaderboardList.hpp"
#include "game/ui/FJ_LeaderboardMenu.hpp"
#include "game/ui/FJ_ListButtonAchievement_Normal.hpp"
#include "game/ui/FJ_ListButtonAchievement_Small.hpp"
#include "game/ui/FJ_ListButtonCheckbox_Normal.hpp"
#include "game/ui/FJ_ListButtonCheckbox_Small.hpp"
#include "game/ui/FJ_ListButtonController_Normal.hpp"
#include "game/ui/FJ_ListButtonController_Small.hpp"
#include "game/ui/FJ_ListButtonDLC_Normal.hpp"
#include "game/ui/FJ_ListButtonDLC_Small.hpp"
#include "game/ui/FJ_ListButtonIconLeft_Normal.hpp"
#include "game/ui/FJ_ListButtonIconLeft_Small.hpp"
#include "game/ui/FJ_ListButtonIconX2Left_Normal.hpp"
#include "game/ui/FJ_ListButtonIconX2Left_Small.hpp"
#include "game/ui/FJ_ListButtonLayerIcon_Normal.hpp"
#include "game/ui/FJ_ListButtonLayerIcon_Small.hpp"
#include "game/ui/FJ_ListButtonLobby_Normal.hpp"
#include "game/ui/FJ_ListButtonLobby_Small.hpp"
#include "game/ui/FJ_ListButtonPlayerList_Normal.hpp"
#include "game/ui/FJ_ListButtonPlayerList_Small.hpp"
#include "game/ui/FJ_ListButtonScoreBoard_Normal.hpp"
#include "game/ui/FJ_ListButtonScoreBoard_Small.hpp"
#include "game/ui/FJ_ListButton_Medium.hpp"
#include "game/ui/FJ_ListButton_Normal.hpp"
#include "game/ui/FJ_ListButton_Small.hpp"
#include "game/ui/FJ_ListLabel_Black.hpp"
#include "game/ui/FJ_ListLabel_SmallBlack.hpp"
#include "game/ui/FJ_List_ListButton.hpp"
#include "game/ui/FJ_List_ListButtonAchievement.hpp"
#include "game/ui/FJ_List_ListButtonCheckbox.hpp"
#include "game/ui/FJ_List_ListButtonDLC.hpp"
#include "game/ui/FJ_List_ListButtonIconLeft.hpp"
#include "game/ui/FJ_List_ListButtonLayerIcon.hpp"
#include "game/ui/FJ_List_ListButtonLobby.hpp"
#include "game/ui/FJ_List_ListButtonPlayerList.hpp"
#include "game/ui/FJ_List_ListButtonScoreBoard.hpp"
#include "game/ui/FJ_List_MenuButton.hpp"
#include "game/ui/FJ_List_MultiList.hpp"
#include "game/ui/FJ_LoadCreateJoinMenu.hpp"
#include "game/ui/FJ_LoadMenu.hpp"
#include "game/ui/FJ_LoadOrJoinMenu.hpp"
#include "game/ui/FJ_MainMenu.hpp"
#include "game/ui/FJ_MapSelectMenu.hpp"
#include "game/ui/FJ_MenuButton_Normal.hpp"
#include "game/ui/FJ_MenuButton_Small.hpp"
#include "game/ui/FJ_MessageBox.hpp"
#include "game/ui/FJ_MobEffect_Normal.hpp"
#include "game/ui/FJ_MobEffect_Small.hpp"
#include "game/ui/FJ_MultilistMenu.hpp"
#include "game/ui/FJ_Panel9GridFade.hpp"
#include "game/ui/FJ_PanelRecessFade.hpp"
#include "game/ui/FJ_Panorama.hpp"
#include "game/ui/FJ_PauseMenu.hpp"
#include "game/ui/FJ_PressStartToPlay.hpp"
#include "game/ui/FJ_ProgressBar_Loading.hpp"
#include "game/ui/FJ_QuadrantSignin.hpp"
#include "game/ui/FJ_SaveMenu.hpp"
#include "game/ui/FJ_SaveMessage.hpp"
#include "game/ui/FJ_ScrollArrow.hpp"
#include "game/ui/FJ_ScrollArrowH.hpp"
#include "game/ui/FJ_SettingsAudioMenu.hpp"
#include "game/ui/FJ_SettingsControlMenu.hpp"
#include "game/ui/FJ_SettingsGraphicsMenu.hpp"
#include "game/ui/FJ_SettingsMenu.hpp"
#include "game/ui/FJ_SettingsOptionsMenu.hpp"
#include "game/ui/FJ_SettingsUIMenu.hpp"
#include "game/ui/FJ_SignEntryMenu.hpp"
#include "game/ui/FJ_SignInput.hpp"
#include "game/ui/FJ_SkinSelect.hpp"
#include "game/ui/FJ_Slider_Normal.hpp"
#include "game/ui/FJ_Slider_Small.hpp"
#include "game/ui/FJ_SlotList.hpp"
#include "game/ui/FJ_SlotList_Slot.hpp"
#include "game/ui/FJ_SocialPost.hpp"
#include "game/ui/FJ_SpaceIndicatorBar.hpp"
#include "game/ui/FJ_SuperFlatEditLayerMenu.hpp"
#include "game/ui/FJ_SuperFlatMenu.hpp"
#include "game/ui/FJ_SuperFlatPresets.hpp"
#include "game/ui/FJ_TextInput.hpp"
#include "game/ui/FJ_TextInput_Normal.hpp"
#include "game/ui/FJ_TextInput_Small.hpp"
#include "game/ui/FJ_TextInput_Vita.hpp"
#include "game/ui/FJ_TexturePackList.hpp"
#include "game/ui/FJ_TexturePackList_Slot.hpp"
#include "game/ui/FJ_ToolTip_Normal.hpp"
#include "game/ui/FJ_ToolTip_Small.hpp"
#include "game/ui/FJ_ToolTips.hpp"
#include "game/ui/FJ_TouchControl.hpp"
#include "game/ui/FJ_TouchControlFrontend.hpp"
#include "game/ui/FJ_TradingMenu.hpp"
#include "game/ui/FJ_TutorialPopup.hpp"
#include "game/ui/FJ_UpsellButton_Normal.hpp"
#include "game/ui/FJ_UpsellButton_Small.hpp"
#include "game/ui/FJ_panel9Grid_16Fade.hpp"
#if LCE_PLATFORM_WIIU
#include "platform/wiiu/FJ_WiiUDRC.hpp"
#endif
#include "game/ui/fui.hpp"
#include "game/ui/fuiRenderNode.hpp"

typedef FJ_FuiNode* (*FJ_FactoryFunction)(fuiRenderNode*);

#if LCE_PLATFORM_PC
// Parity Not Checked
// each Create returns its own class, so calling it through the base's function type is undefined; a thunk per class
// makes the call with the right type and converts the result to the base
template <class T>
FJ_FuiNode* FJ_CreateAs(fuiRenderNode* renderNode)
{
    return T::Create(renderNode);
}
#define FJ_FACTORY(Type) (&FJ_CreateAs<Type>)
#else
#define FJ_FACTORY(Type) FJ_FACTORY(Type)
#endif
typedef boost::unordered_map<std::string, FJ_FactoryFunction> FJ_FactoryMap;

static FJ_FactoryMap g_fuiNodeFactories;
static unsigned int g_factoryNodeId;

static const std::string g_FJ_FuiNode_name("FJ_FuiNode");
static const std::string g_FJ_MenuButton_Normal_name("FJ_MenuButton_Normal");
static const std::string g_FJ_MenuButton_Small_name("FJ_MenuButton_Small");
static const std::string g_FJ_ListButton_Medium_name("FJ_ListButton_Medium");
static const std::string g_FJ_ListButton_Normal_name("FJ_ListButton_Normal");
static const std::string g_FJ_ListButton_Small_name("FJ_ListButton_Small");
static const std::string g_FJ_UpsellButton_Normal_name("FJ_UpsellButton_Normal");
static const std::string g_FJ_UpsellButton_Small_name("FJ_UpsellButton_Small");
static const std::string g_FJ_ListButtonIconLeft_Normal_name("FJ_ListButtonIconLeft_Normal");
static const std::string g_FJ_ListButtonIconLeft_Small_name("FJ_ListButtonIconLeft_Small");
static const std::string g_FJ_ListButtonIconX2Left_Normal_name("FJ_ListButtonIconX2Left_Normal");
static const std::string g_FJ_ListButtonIconX2Left_Small_name("FJ_ListButtonIconX2Left_Small");
static const std::string g_FJ_List_ListButtonIconLeft_name("FJ_List_ListButtonIconLeft");
static const std::string g_FJ_List_ListButton_name("FJ_List_ListButton");
static const std::string g_FJ_List_ListButtonCheckbox_name("FJ_List_ListButtonCheckbox");
static const std::string g_FJ_List_ListButtonDLC_name("FJ_List_ListButtonDLC");
static const std::string g_FJ_List_ListButtonLayerIcon_name("FJ_List_ListButtonLayerIcon");
static const std::string g_FJ_List_ListButtonLobby_name("FJ_List_ListButtonLobby");
static const std::string g_FJ_List_ListButtonPlayerList_name("FJ_List_ListButtonPlayerList");
static const std::string g_FJ_List_ListButtonScoreBoard_name("FJ_List_ListButtonScoreBoard");
static const std::string g_FJ_List_ListButtonAchievement_name("FJ_List_ListButtonAchievement");
static const std::string g_FJ_CheckpointMarker_name("FJ_CheckpointMarker");
static const std::string g_FJ_List_MenuButton_name("FJ_List_MenuButton");
static const std::string g_FJ_BeaconEffectButton_Normal_name("FJ_BeaconEffectButton_Normal");
static const std::string g_FJ_BeaconEffectButton_Small_name("FJ_BeaconEffectButton_Small");
static const std::string g_FJ_EnchantingButton_Normal_name("FJ_EnchantingButton_Normal");
static const std::string g_FJ_EnchantingButton_Small_name("FJ_EnchantingButton_Small");
static const std::string g_FJ_KeyboardButtons_Controls_Normal_name("FJ_KeyboardButtons_Controls_Normal");
static const std::string g_FJ_KeyboardButtons_Controls_Small_name("FJ_KeyboardButtons_Controls_Small");
static const std::string g_FJ_KeyboardButtons_Keys_Normal_name("FJ_KeyboardButtons_Keys_Normal");
static const std::string g_FJ_KeyboardButtons_Keys_Small_name("FJ_KeyboardButtons_Keys_Small");
static const std::string g_FJ_LayoutButton_name("FJ_LayoutButton");
static const std::string g_FJ_LayoutButton_Small_name("FJ_LayoutButton_Small");
static const std::string g_FJ_ListButtonAchievement_Normal_name("FJ_ListButtonAchievement_Normal");
static const std::string g_FJ_ListButtonAchievement_Small_name("FJ_ListButtonAchievement_Small");
static const std::string g_FJ_ListButtonCheckbox_Normal_name("FJ_ListButtonCheckbox_Normal");
static const std::string g_FJ_ListButtonCheckbox_Small_name("FJ_ListButtonCheckbox_Small");
static const std::string g_FJ_ListButtonDLC_Normal_name("FJ_ListButtonDLC_Normal");
static const std::string g_FJ_ListButtonDLC_Small_name("FJ_ListButtonDLC_Small");
static const std::string g_FJ_ListButtonLayerIcon_Normal_name("FJ_ListButtonLayerIcon_Normal");
static const std::string g_FJ_ListButtonLayerIcon_Small_name("FJ_ListButtonLayerIcon_Small");
static const std::string g_FJ_ListButtonLobby_Normal_name("FJ_ListButtonLobby_Normal");
static const std::string g_FJ_ListButtonLobby_Small_name("FJ_ListButtonLobby_Small");
static const std::string g_FJ_ListButtonPlayerList_Normal_name("FJ_ListButtonPlayerList_Normal");
static const std::string g_FJ_ListButtonPlayerList_Small_name("FJ_ListButtonPlayerList_Small");
static const std::string g_FJ_ListButtonScoreBoard_Normal_name("FJ_ListButtonScoreBoard_Normal");
static const std::string g_FJ_ListButtonScoreBoard_Small_name("FJ_ListButtonScoreBoard_Small");
static const std::string g_FJ_ListButtonController_Normal_name("FJ_ListButtonController_Normal");
static const std::string g_FJ_ListButtonController_Small_name("FJ_ListButtonController_Small");
static const std::string g_FJ_LeaderboardList_name("FJ_LeaderboardList");
static const std::string g_FJ_LeaderboardEntryNormal_name("FJ_LeaderboardEntryNormal");
static const std::string g_FJ_LeaderboardEntrySmall_name("FJ_LeaderboardEntrySmall");
static const std::string g_FJ_LeaderboardHeaderNormal_name("FJ_LeaderboardHeaderNormal");
static const std::string g_FJ_LeaderboardHeaderSmall_name("FJ_LeaderboardHeaderSmall");
static const std::string g_FJ_IconSlot_name("FJ_IconSlot");
static const std::string g_FJ_ToolTip_Normal_name("FJ_ToolTip_Normal");
static const std::string g_FJ_ToolTip_Small_name("FJ_ToolTip_Small");
static const std::string g_FJ_Label_name("FJ_Label");
static const std::string g_FJ_Label_Blink_Normal_name("FJ_Label_Blink_Normal");
static const std::string g_FJ_Label_Blink_Small_name("FJ_Label_Blink_Small");
static const std::string g_FJ_ScrollArrow_name("FJ_ScrollArrow");
static const std::string g_FJ_ScrollArrowH_name("FJ_ScrollArrowH");
static const std::string g_FJ_Slider_Normal_name("FJ_Slider_Normal");
static const std::string g_FJ_Slider_Small_name("FJ_Slider_Small");
static const std::string g_FJ_TexturePackList_name("FJ_TexturePackList");
static const std::string g_FJ_TexturePackList_Slot_name("FJ_TexturePackList_Slot");
static const std::string g_FJ_SlotList_name("FJ_SlotList");
static const std::string g_FJ_SlotList_Slot_name("FJ_SlotList_Slot");
static const std::string g_FJ_CheckBox_Normal_name("FJ_CheckBox_Normal");
static const std::string g_FJ_CheckBox_Small_name("FJ_CheckBox_Small");
static const std::string g_FJ_ExlamationMark_name("FJ_ExlamationMark");
static const std::string g_FJ_Cursor_Normal_name("FJ_Cursor_Normal");
static const std::string g_FJ_Cursor_Small_name("FJ_Cursor_Small");
static const std::string g_FJ_MobEffect_Small_name("FJ_MobEffect_Small");
static const std::string g_FJ_MobEffect_Normal_name("FJ_MobEffect_Normal");
static const std::string g_FJ_TouchControl_name("FJ_TouchControl");
static const std::string g_FJ_TouchControlFrontend_name("FJ_TouchControlFrontend");
static const std::string g_FJ_BitmapIcon_name("FJ_BitmapIcon");
static const std::string g_FJ_List_MultiList_name("FJ_List_MultiList");
static const std::string g_CreditsMenu_name("CreditsMenu");
static const std::string g_EndPoem_name("EndPoem");
static const std::string g_MainMenu_name("MainMenu");
static const std::string g_LoadMenu_name("LoadMenu");
static const std::string g_SaveMessage_name("SaveMessage");
static const std::string g_ComponentLogo_name("ComponentLogo");
static const std::string g_Panorama_name("Panorama");
static const std::string g_ToolTips_name("ToolTips");
static const std::string g_Intro_name("Intro");
static const std::string g_LoadCreateJoinMenu_name("LoadCreateJoinMenu");
static const std::string g_Hud_name("Hud");
static const std::string g_HowToPlayMenu_name("HowToPlayMenu");
static const std::string g_HowToPlay_name("HowToPlay");
static const std::string g_CreativeMenu_name("CreativeMenu");
static const std::string g_CraftingMenu_name("CraftingMenu");
static const std::string g_InventoryMenu_name("InventoryMenu");
static const std::string g_FullscreenProgress_name("FullscreenProgress");
static const std::string g_Eula_name("Eula");
static const std::string g_MessageBox_name("MessageBox");
static const std::string g_PauseMenu_name("PauseMenu");
static const std::string g_HelpAndOptionsMenu_name("HelpAndOptionsMenu");
static const std::string g_SettingsMenu_name("SettingsMenu");
static const std::string g_AnvilMenu_name("AnvilMenu");
static const std::string g_BeaconMenu_name("BeaconMenu");
static const std::string g_BrewingStandMenu_name("BrewingStandMenu");
static const std::string g_ClassicCraftingMenu_name("ClassicCraftingMenu");
static const std::string g_ChestMenu_name("ChestMenu");
static const std::string g_DispenserMenu_name("DispenserMenu");
static const std::string g_EnchantingMenu_name("EnchantingMenu");
static const std::string g_FurnaceMenu_name("FurnaceMenu");
static const std::string g_HopperMenu_name("HopperMenu");
static const std::string g_HorseInventoryMenu_name("HorseInventoryMenu");
static const std::string g_BookAndQuillMenu_name("BookAndQuillMenu");
static const std::string g_ControlsMenu_name("ControlsMenu");
static const std::string g_PressStartToPlay_name("PressStartToPlay");
static const std::string g_SuperFlatMenu_name("SuperFlatMenu");
static const std::string g_SuperFlatPresets_name("SuperFlatPresets");
static const std::string g_SuperFlatEditLayerMenu_name("SuperFlatEditLayerMenu");
static const std::string g_QuadrantSignin_name("QuadrantSignin");
static const std::string g_LeaderboardMenu_name("LeaderboardMenu");
static const std::string g_TradingMenu_name("TradingMenu");
static const std::string g_GameModeScoreboard_name("GameModeScoreboard");
static const std::string g_BiomeSelectMenu_name("BiomeSelectMenu");
static const std::string g_ComponentChat_name("ComponentChat");
static const std::string g_DeathMenu_name("DeathMenu");
static const std::string g_DebugBuildTools_name("DebugBuildTools");
static const std::string g_DebugCreateSchematic_name("DebugCreateSchematic");
static const std::string g_DebugFillArea_name("DebugFillArea");
static const std::string g_DebugMarketingGuide_name("DebugMarketingGuide");
static const std::string g_DebugMenu_name("DebugMenu");
static const std::string g_DebugOptions_name("DebugOptions");
static const std::string g_DebugPuppeteer_name("DebugPuppeteer");
static const std::string g_DebugSetCamera_name("DebugSetCamera");
static const std::string g_GameModeSelection_name("GameModeSelection");
static const std::string g_Gamertag_name("Gamertag");
static const std::string g_JoinGameFilterList_name("JoinGameFilterList");
static const std::string g_JoinMenu_name("JoinMenu");
static const std::string g_LoadOrJoinMenu_name("LoadOrJoinMenu");
static const std::string g_SaveMenu_name("SaveMenu");
static const std::string g_SettingsAudioMenu_name("SettingsAudioMenu");
static const std::string g_SettingsControlMenu_name("SettingsControlMenu");
static const std::string g_SettingsGraphicsMenu_name("SettingsGraphicsMenu");
static const std::string g_SettingsOptionsMenu_name("SettingsOptionsMenu");
static const std::string g_SettingsUIMenu_name("SettingsUIMenu");
static const std::string g_SignEntryMenu_name("SignEntryMenu");
static const std::string g_SocialPost_name("SocialPost");
static const std::string g_AchievementsMenu_name("AchievementsMenu");
static const std::string g_AvatarSelectMenu_name("AvatarSelectMenu");
static const std::string g_CreateWorldMenu_name("CreateWorldMenu");
static const std::string g_LaunchMoreOptionsMenu_name("LaunchMoreOptionsMenu");
static const std::string g_TutorialPopup_name("TutorialPopup");
static const std::string g_MultilistMenu_name("MultilistMenu");
static const std::string g_SkinSelect_name("SkinSelect");
static const std::string g_InGamePlayerOptions_name("InGamePlayerOptions");
static const std::string g_InGameHostOptions_name("InGameHostOptions");
static const std::string g_InGameInfoMenu_name("InGameInfoMenu");
static const std::string g_InGameTeleportMenu_name("InGameTeleportMenu");
static const std::string g_MapSelectMenu_name("MapSelectMenu");
static const std::string g_FJ_DragonHealth_01_name("FJ_DragonHealth_01");
static const std::string g_FJ_DragonHealth_02_name("FJ_DragonHealth_02");
static const std::string g_FJ_DragonHealth_03_name("FJ_DragonHealth_03");
static const std::string g_FJ_DragonHealth_04_name("FJ_DragonHealth_04");
static const std::string g_FJ_DragonHealth_05_name("FJ_DragonHealth_05");
static const std::string g_FJ_HorseJumpBar_name("FJ_HorseJumpBar");
static const std::string g_FJ_ExperienceBar_name("FJ_ExperienceBar");
static const std::string g_FJ_BrewingArrow_name("FJ_BrewingArrow");
static const std::string g_FJ_BrewingArrow_Small_name("FJ_BrewingArrow_Small");
static const std::string g_FJ_BrewingBubbles_name("FJ_BrewingBubbles");
static const std::string g_FJ_BrewingBubbles_Small_name("FJ_BrewingBubbles_Small");
static const std::string g_FJ_BrewingPlate_name("FJ_BrewingPlate");
static const std::string g_FJ_BrewingPlate_Small_name("FJ_BrewingPlate_Small");
static const std::string g_FJ_FurnaceArrow_name("FJ_FurnaceArrow");
static const std::string g_FJ_FurnaceArrow_Small_name("FJ_FurnaceArrow_Small");
static const std::string g_FJ_FurnaceFlame_name("FJ_FurnaceFlame");
static const std::string g_FJ_FurnaceFlame_Small_name("FJ_FurnaceFlame_Small");
static const std::string g_FJ_KillBar_name("FJ_KillBar");
static const std::string g_FJ_KillBarSmall_name("FJ_KillBarSmall");
static const std::string g_FJ_ProgressBar_Loading_name("FJ_ProgressBar_Loading");
static const std::string g_FJ_AttackIndicatorCrosshair_name("FJ_AttackIndicatorCrosshair");
static const std::string g_FJ_AttackIndicatorHotbar_name("FJ_AttackIndicatorHotbar");
static const std::string g_FJ_SpaceIndicatorBar_name("FJ_SpaceIndicatorBar");
static const std::string g_FJ_TextInput_name("FJ_TextInput");
static const std::string g_FJ_TextInput_Small_name("FJ_TextInput_Small");
static const std::string g_FJ_TextInput_Normal_name("FJ_TextInput_Normal");
static const std::string g_FJ_TextInput_Vita_name("FJ_TextInput_Vita");
static const std::string g_FJ_HtmlTextPanel_name("FJ_HtmlTextPanel");
static const std::string g_FJ_HtmlTextPanelLarge_name("FJ_HtmlTextPanelLarge");
static const std::string g_FJ_Label_BigBlack_name("FJ_Label_BigBlack");
static const std::string g_FJ_Label_BigGrey_name("FJ_Label_BigGrey");
static const std::string g_FJ_Label_BigWhite_name("FJ_Label_BigWhite");
static const std::string g_FJ_Label_Black_name("FJ_Label_Black");
static const std::string g_FJ_Label_Black_Centered_name("FJ_Label_Black_Centered");
static const std::string g_FJ_Label_Black_Centered_HUD_name("FJ_Label_Black_Centered_HUD");
static const std::string g_FJ_Label_Black_Small_Centered_name("FJ_Label_Black_Small_Centered");
static const std::string g_FJ_Label_Black_Small_Centered_HUD_name("FJ_Label_Black_Small_Centered_HUD");
static const std::string g_FJ_Label_Black_VerySmall_Centered_name("FJ_Label_Black_VerySmall_Centered");
static const std::string g_FJ_Label_Book_Normal_name("FJ_Label_Book_Normal");
static const std::string g_FJ_Label_Book_Small_name("FJ_Label_Book_Small");
static const std::string g_FJ_Label_Book_VerySmall_name("FJ_Label_Book_VerySmall");
static const std::string g_FJ_Label_Credits_10_name("FJ_Label_Credits_10");
static const std::string g_FJ_Label_Credits_10_Yellow_name("FJ_Label_Credits_10_Yellow");
static const std::string g_FJ_Label_Credits_20_name("FJ_Label_Credits_20");
static const std::string g_FJ_Label_Credits_20_Yellow_name("FJ_Label_Credits_20_Yellow");
static const std::string g_FJ_Label_Credits_30_name("FJ_Label_Credits_30");
static const std::string g_FJ_Label_DragonHealth_name("FJ_Label_DragonHealth");
static const std::string g_FJ_Label_DragonHealthBig_name("FJ_Label_DragonHealthBig");
static const std::string g_FJ_Label_DragonHealthSmall_name("FJ_Label_DragonHealthSmall");
static const std::string g_FJ_Label_DragonHealthVerySmall_name("FJ_Label_DragonHealthVerySmall");
static const std::string g_FJ_Label_Green_name("FJ_Label_Green");
static const std::string g_FJ_Label_Grey_name("FJ_Label_Grey");
static const std::string g_FJ_Label_Html_name("FJ_Label_Html");
static const std::string g_FJ_Label_HtmlEndPoem_name("FJ_Label_HtmlEndPoem");
static const std::string g_FJ_Label_HtmlEndPoemHD_name("FJ_Label_HtmlEndPoemHD");
static const std::string g_FJ_Label_HtmlSmall_name("FJ_Label_HtmlSmall");
static const std::string g_FJ_Label_HUD_Level_01_name("FJ_Label_HUD_Level_01");
static const std::string g_FJ_Label_HUD_Level_02_name("FJ_Label_HUD_Level_02");
static const std::string g_FJ_Label_HUD_Level_03_name("FJ_Label_HUD_Level_03");
static const std::string g_FJ_Label_HUD_Level_04_name("FJ_Label_HUD_Level_04");
static const std::string g_FJ_Label_HUD_Level_05_name("FJ_Label_HUD_Level_05");
static const std::string g_FJ_Label_HUD_SmallWhite_name("FJ_Label_HUD_SmallWhite");
static const std::string g_FJ_Label_HUD_BigWhite_name("FJ_Label_HUD_BigWhite");
static const std::string g_FJ_Label_HUD_Splash_name("FJ_Label_HUD_Splash");
static const std::string g_FJ_Label_HUD_SplashBig_name("FJ_Label_HUD_SplashBig");
static const std::string g_FJ_Label_HUD_SplashVita_name("FJ_Label_HUD_SplashVita");
static const std::string g_FJ_Label_HUD_VerySmallWhite_name("FJ_Label_HUD_VerySmallWhite");
static const std::string g_FJ_Label_HUD_White_name("FJ_Label_HUD_White");
static const std::string g_FJ_Label_OutlineLevel_name("FJ_Label_OutlineLevel");
static const std::string g_FJ_Label_OutlineLevel_Small_name("FJ_Label_OutlineLevel_Small");
static const std::string g_FJ_Label_OutlineTitle_name("FJ_Label_OutlineTitle");
static const std::string g_FJ_Label_OutlineTitle_Small_name("FJ_Label_OutlineTitle_Small");
static const std::string g_FJ_Label_Red_name("FJ_Label_Red");
static const std::string g_FJ_Label_SkinSelect_name("FJ_Label_SkinSelect");
static const std::string g_FJ_Label_SkinSelectCenter_name("FJ_Label_SkinSelectCenter");
static const std::string g_FJ_Label_SkinSelectCenter_Off_name("FJ_Label_SkinSelectCenter_Off");
static const std::string g_FJ_Label_SkinSelectCenter_Small_name("FJ_Label_SkinSelectCenter_Small");
static const std::string g_FJ_Label_SkinSelectCenter_Small_Off_name("FJ_Label_SkinSelectCenter_Small_Off");
static const std::string g_FJ_Label_SkinSelect_Off_name("FJ_Label_SkinSelect_Off");
static const std::string g_FJ_Label_SkinSelect_Small_name("FJ_Label_SkinSelect_Small");
static const std::string g_FJ_Label_SkinSelect_Small_Off_name("FJ_Label_SkinSelect_Small_Off");
static const std::string g_FJ_Label_SmallBlack_name("FJ_Label_SmallBlack");
static const std::string g_FJ_Label_SmallGreen_name("FJ_Label_SmallGreen");
static const std::string g_FJ_Label_SmallGrey_name("FJ_Label_SmallGrey");
static const std::string g_FJ_Label_SmallRed_name("FJ_Label_SmallRed");
static const std::string g_FJ_Label_SmallWhite_name("FJ_Label_SmallWhite");
static const std::string g_FJ_Label_VerySmallBlack_name("FJ_Label_VerySmallBlack");
static const std::string g_FJ_Label_VerySmallGrey_name("FJ_Label_VerySmallGrey");
static const std::string g_FJ_Label_VerySmallWhite_name("FJ_Label_VerySmallWhite");
static const std::string g_FJ_Label_White_name("FJ_Label_White");
static const std::string g_FJ_Label_WhiteHtml_name("FJ_Label_WhiteHtml");
static const std::string g_FJ_Label_WhiteHtmlSmall_name("FJ_Label_WhiteHtmlSmall");
static const std::string g_FJ_Label_White_Centered_name("FJ_Label_White_Centered");
static const std::string g_FJ_Label_White_Small_Centered_name("FJ_Label_White_Small_Centered");
static const std::string g_FJ_Label_White_VerySmall_Centered_name("FJ_Label_White_VerySmall_Centered");
static const std::string g_FJ_ListLabel_Black_name("FJ_ListLabel_Black");
static const std::string g_FJ_ListLabel_SmallBlack_name("FJ_ListLabel_SmallBlack");
static const std::string g_FJ_HtmlText_Normal_name("FJ_HtmlText_Normal");
static const std::string g_FJ_HtmlText_Small_name("FJ_HtmlText_Small");
static const std::string g_FJ_HtmlText_WhiteNormal_name("FJ_HtmlText_WhiteNormal");
static const std::string g_FJ_HtmlText_WhiteSmall_name("FJ_HtmlText_WhiteSmall");
static const std::string g_FJ_HtmlText_WithPanel_Normal_name("FJ_HtmlText_WithPanel_Normal");
static const std::string g_FJ_HtmlText_WithPanel_Small_name("FJ_HtmlText_WithPanel_Small");
static const std::string g_FJ_DynamicHtmlText_name("FJ_DynamicHtmlText");
static const std::string g_FJ_DynamicHtmlText_Normal_name("FJ_DynamicHtmlText_Normal");
static const std::string g_FJ_DynamicHtmlText_Small_name("FJ_DynamicHtmlText_Small");
static const std::string g_FJ_DynamicHtmlText_WithPanel_Normal_name("FJ_DynamicHtmlText_WithPanel_Normal");
static const std::string g_FJ_DynamicHtmlText_WithPanel_Small_name("FJ_DynamicHtmlText_WithPanel_Small");
static const std::string g_FJ_SignInput_name("FJ_SignInput");
static const std::string g_FJ_panel9Grid_16Fade_name("FJ_panel9Grid_16Fade");
static const std::string g_FJ_Panel9GridFade_name("FJ_Panel9GridFade");
static const std::string g_FJ_PanelRecessFade_name("FJ_PanelRecessFade");
#if LCE_PLATFORM_WIIU
static const std::string g_WiiUDRC_name("WiiUDRC");
#endif

void FJ_Factory::initialise()
{
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_FuiNode_name, &FJ_FuiNode::Create));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_MenuButton_Normal_name,
                                             FJ_FACTORY(FJ_MenuButton_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_MenuButton_Small_name, FJ_FACTORY(FJ_MenuButton_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ListButton_Medium_name,
                                             FJ_FACTORY(FJ_ListButton_Medium)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ListButton_Normal_name,
                                             FJ_FACTORY(FJ_ListButton_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButton_Small_name, FJ_FACTORY(FJ_ListButton_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_UpsellButton_Normal_name,
                                             FJ_FACTORY(FJ_UpsellButton_Normal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_UpsellButton_Small_name,
                                             FJ_FACTORY(FJ_UpsellButton_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonIconLeft_Normal_name,
                       FJ_FACTORY(FJ_ListButtonIconLeft_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonIconLeft_Small_name,
                       FJ_FACTORY(FJ_ListButtonIconLeft_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonIconX2Left_Normal_name,
                       FJ_FACTORY(FJ_ListButtonIconX2Left_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonIconX2Left_Small_name,
                       FJ_FACTORY(FJ_ListButtonIconX2Left_Small)));
    g_fuiNodeFactories.insert(std::make_pair(
        g_FJ_List_ListButtonIconLeft_name, FJ_FACTORY(FJ_List_ListButtonIconLeft)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_List_ListButton_name, FJ_FACTORY(FJ_List_ListButton)));
    g_fuiNodeFactories.insert(std::make_pair(
        g_FJ_List_ListButtonCheckbox_name, FJ_FACTORY(FJ_List_ListButtonCheckbox)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_List_ListButtonDLC_name,
                                             FJ_FACTORY(FJ_List_ListButtonDLC)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_List_ListButtonLayerIcon_name,
                       FJ_FACTORY(FJ_List_ListButtonLayerIcon)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_List_ListButtonLobby_name,
                                             FJ_FACTORY(FJ_List_ListButtonLobby)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_List_ListButtonPlayerList_name,
                       FJ_FACTORY(FJ_List_ListButtonPlayerList)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_List_ListButtonScoreBoard_name,
                       FJ_FACTORY(FJ_List_ListButtonScoreBoard)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_List_ListButtonAchievement_name,
                       FJ_FACTORY(FJ_List_ListButtonAchievement)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_CheckpointMarker_name, &FJ_CheckpointMarker::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_List_MenuButton_name, FJ_FACTORY(FJ_List_MenuButton)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_BeaconEffectButton_Normal_name,
                       FJ_FACTORY(FJ_BeaconEffectButton_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_BeaconEffectButton_Small_name,
                       FJ_FACTORY(FJ_BeaconEffectButton_Small)));
    g_fuiNodeFactories.insert(std::make_pair(
        g_FJ_EnchantingButton_Normal_name, FJ_FACTORY(FJ_EnchantingButton_Normal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_EnchantingButton_Small_name,
                                             FJ_FACTORY(FJ_EnchantingButton_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_KeyboardButtons_Controls_Normal_name,
                       FJ_FACTORY(FJ_KeyboardButtons_Controls_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_KeyboardButtons_Controls_Small_name,
                       FJ_FACTORY(FJ_KeyboardButtons_Controls_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_KeyboardButtons_Keys_Normal_name,
                       FJ_FACTORY(FJ_KeyboardButtons_Keys_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_KeyboardButtons_Keys_Small_name,
                       FJ_FACTORY(FJ_KeyboardButtons_Keys_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_KeyboardButtons_Controls_Normal_name,
                       FJ_FACTORY(FJ_KeyboardButtons_Controls_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_LayoutButton_name, FJ_FACTORY(FJ_LayoutButton)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_LayoutButton_Small_name,
                                             FJ_FACTORY(FJ_LayoutButton_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonAchievement_Normal_name,
                       FJ_FACTORY(FJ_ListButtonAchievement_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonAchievement_Small_name,
                       FJ_FACTORY(FJ_ListButtonAchievement_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonCheckbox_Normal_name,
                       FJ_FACTORY(FJ_ListButtonCheckbox_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonCheckbox_Small_name,
                       FJ_FACTORY(FJ_ListButtonCheckbox_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ListButtonDLC_Normal_name,
                                             FJ_FACTORY(FJ_ListButtonDLC_Normal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ListButtonDLC_Small_name,
                                             FJ_FACTORY(FJ_ListButtonDLC_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonLayerIcon_Normal_name,
                       FJ_FACTORY(FJ_ListButtonLayerIcon_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonLayerIcon_Small_name,
                       FJ_FACTORY(FJ_ListButtonLayerIcon_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ListButtonLobby_Normal_name,
                                             FJ_FACTORY(FJ_ListButtonLobby_Normal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ListButtonLobby_Small_name,
                                             FJ_FACTORY(FJ_ListButtonLobby_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonPlayerList_Normal_name,
                       FJ_FACTORY(FJ_ListButtonPlayerList_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonPlayerList_Small_name,
                       FJ_FACTORY(FJ_ListButtonPlayerList_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonScoreBoard_Normal_name,
                       FJ_FACTORY(FJ_ListButtonScoreBoard_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonScoreBoard_Small_name,
                       FJ_FACTORY(FJ_ListButtonScoreBoard_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonController_Normal_name,
                       FJ_FACTORY(FJ_ListButtonController_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListButtonController_Small_name,
                       FJ_FACTORY(FJ_ListButtonController_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_LeaderboardList_name, &FJ_LeaderboardList::Create));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_LeaderboardEntryNormal_name,
                                             FJ_FACTORY(FJ_LeaderboardEntryNormal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_LeaderboardEntrySmall_name,
                                             FJ_FACTORY(FJ_LeaderboardEntrySmall)));
    g_fuiNodeFactories.insert(std::make_pair(
        g_FJ_LeaderboardHeaderNormal_name, FJ_FACTORY(FJ_LeaderboardHeaderNormal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_LeaderboardHeaderSmall_name,
                                             FJ_FACTORY(FJ_LeaderboardHeaderSmall)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_LeaderboardList_name, &FJ_LeaderboardList::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_IconSlot_name, FJ_FACTORY(FJ_IconSlot)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ToolTip_Normal_name, FJ_FACTORY(FJ_ToolTip_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ToolTip_Small_name, FJ_FACTORY(FJ_ToolTip_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_name, FJ_FACTORY(FJ_Label)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_Blink_Normal_name,
                                             FJ_FACTORY(FJ_Label_Blink_Normal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_Blink_Small_name,
                                             FJ_FACTORY(FJ_Label_Blink_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ScrollArrow_name, &FJ_ScrollArrow::Create));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ScrollArrowH_name, &FJ_ScrollArrowH::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Slider_Normal_name, FJ_FACTORY(FJ_Slider_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Slider_Small_name, FJ_FACTORY(FJ_Slider_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_TexturePackList_name, FJ_FACTORY(FJ_TexturePackList)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_TexturePackList_Slot_name,
                                             FJ_FACTORY(FJ_TexturePackList_Slot)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_SlotList_name, &FJ_SlotList::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_SlotList_Slot_name, FJ_FACTORY(FJ_SlotList_Slot)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_CheckBox_Normal_name, FJ_FACTORY(FJ_CheckBox_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_CheckBox_Small_name, FJ_FACTORY(FJ_CheckBox_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_SlotList_name, &FJ_SlotList::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_SlotList_Slot_name, FJ_FACTORY(FJ_SlotList_Slot)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ExlamationMark_name, &FJ_ExlamationMark::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Cursor_Normal_name, FJ_FACTORY(FJ_Cursor_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Cursor_Small_name, FJ_FACTORY(FJ_Cursor_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_MobEffect_Small_name, FJ_FACTORY(FJ_MobEffect_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_MobEffect_Normal_name, FJ_FACTORY(FJ_MobEffect_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_TouchControl_name, FJ_FACTORY(FJ_TouchControl)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_TouchControlFrontend_name,
                                             FJ_FACTORY(FJ_TouchControlFrontend)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_BitmapIcon_name, &FJ_BitmapIcon::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_List_MultiList_name, FJ_FACTORY(FJ_List_MultiList)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_CreditsMenu_name, FJ_FACTORY(FJ_CreditsMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_EndPoem_name, FJ_FACTORY(FJ_EndPoem)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_MainMenu_name, FJ_FACTORY(FJ_MainMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_LoadMenu_name, FJ_FACTORY(FJ_LoadMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SaveMessage_name, FJ_FACTORY(FJ_SaveMessage)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_ComponentLogo_name, FJ_FACTORY(FJ_ComponentLogo)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_Panorama_name, FJ_FACTORY(FJ_Panorama)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_ToolTips_name, FJ_FACTORY(FJ_ToolTips)));
    g_fuiNodeFactories.insert(std::make_pair(g_Intro_name, FJ_FACTORY(FJ_Intro)));
    g_fuiNodeFactories.insert(std::make_pair(g_LoadCreateJoinMenu_name,
                                             FJ_FACTORY(FJ_LoadCreateJoinMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_Hud_name, FJ_FACTORY(FJ_Hud)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_HowToPlayMenu_name, FJ_FACTORY(FJ_HowToPlayMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_HowToPlay_name, FJ_FACTORY(FJ_HowToPlay)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_CreativeMenu_name, FJ_FACTORY(FJ_CreativeMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_CraftingMenu_name, FJ_FACTORY(FJ_CraftingMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_InventoryMenu_name, FJ_FACTORY(FJ_InventoryMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_FullscreenProgress_name,
                                             FJ_FACTORY(FJ_FullScreenProgress)));
    g_fuiNodeFactories.insert(std::make_pair(g_Eula_name, FJ_FACTORY(FJ_Eula)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_MessageBox_name, FJ_FACTORY(FJ_MessageBox)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_PauseMenu_name, FJ_FACTORY(FJ_PauseMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_HelpAndOptionsMenu_name,
                                             FJ_FACTORY(FJ_HelpAndOptionsMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SettingsMenu_name, FJ_FACTORY(FJ_SettingsMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_AnvilMenu_name, FJ_FACTORY(FJ_AnvilMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_BeaconMenu_name, FJ_FACTORY(FJ_BeaconMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_BrewingStandMenu_name, FJ_FACTORY(FJ_BrewingStandMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_ClassicCraftingMenu_name,
                                             FJ_FACTORY(FJ_ClassicCraftingMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_ChestMenu_name, FJ_FACTORY(FJ_ChestMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DispenserMenu_name, FJ_FACTORY(FJ_DispenserMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_EnchantingMenu_name, FJ_FACTORY(FJ_EnchantingMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FurnaceMenu_name, FJ_FACTORY(FJ_FurnaceMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_HopperMenu_name, FJ_FACTORY(FJ_HopperMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_HorseInventoryMenu_name,
                                             FJ_FACTORY(FJ_HorseInventoryMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_BookAndQuillMenu_name, FJ_FACTORY(FJ_BookAndQuillMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_ControlsMenu_name, FJ_FACTORY(FJ_ControlsMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_PressStartToPlay_name, FJ_FACTORY(FJ_PressStartToPlay)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SuperFlatMenu_name, FJ_FACTORY(FJ_SuperFlatMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SuperFlatPresets_name, FJ_FACTORY(FJ_SuperFlatPresets)));
    g_fuiNodeFactories.insert(std::make_pair(g_SuperFlatEditLayerMenu_name,
                                             FJ_FACTORY(FJ_SuperFlatEditLayerMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_QuadrantSignin_name, FJ_FACTORY(FJ_QuadrantSignin)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_LeaderboardMenu_name, FJ_FACTORY(FJ_LeaderboardMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_TradingMenu_name, FJ_FACTORY(FJ_TradingMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_GameModeScoreboard_name,
                                             FJ_FACTORY(FJ_GameModeScoreboard)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_BiomeSelectMenu_name, FJ_FACTORY(FJ_BiomeSelectMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_ComponentChat_name, &FJ_ComponentChat::Create));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DeathMenu_name, FJ_FACTORY(FJ_DeathMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DebugBuildTools_name, FJ_FACTORY(FJ_DebugBuildTools)));
    g_fuiNodeFactories.insert(std::make_pair(g_DebugCreateSchematic_name,
                                             FJ_FACTORY(FJ_DebugCreateSchematic)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DebugFillArea_name, FJ_FACTORY(FJ_DebugFillArea)));
    g_fuiNodeFactories.insert(std::make_pair(g_DebugMarketingGuide_name,
                                             FJ_FACTORY(FJ_DebugMarketingGuide)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DebugMenu_name, FJ_FACTORY(FJ_DebugMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DebugOptions_name, FJ_FACTORY(FJ_DebugOptions)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DebugPuppeteer_name, FJ_FACTORY(FJ_DebugPuppeteer)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_DebugSetCamera_name, FJ_FACTORY(FJ_DebugSetCamera)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_GameModeSelection_name, FJ_FACTORY(FJ_GameModeSelection)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_Gamertag_name, FJ_FACTORY(FJ_Gamertag)));
    g_fuiNodeFactories.insert(std::make_pair(g_JoinGameFilterList_name,
                                             FJ_FACTORY(FJ_JoinGameFilterList)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_JoinMenu_name, FJ_FACTORY(FJ_JoinMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_LoadOrJoinMenu_name, FJ_FACTORY(FJ_LoadOrJoinMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SaveMenu_name, FJ_FACTORY(FJ_SaveMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SettingsAudioMenu_name, FJ_FACTORY(FJ_SettingsAudioMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_SettingsControlMenu_name,
                                             FJ_FACTORY(FJ_SettingsControlMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_SettingsGraphicsMenu_name,
                                             FJ_FACTORY(FJ_SettingsGraphicsMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_SettingsOptionsMenu_name,
                                             FJ_FACTORY(FJ_SettingsOptionsMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SettingsUIMenu_name, FJ_FACTORY(FJ_SettingsUIMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SignEntryMenu_name, FJ_FACTORY(FJ_SignEntryMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SocialPost_name, FJ_FACTORY(FJ_SocialPost)));
#if LCE_PLATFORM_WIIU
    g_fuiNodeFactories.insert(
        std::make_pair(g_WiiUDRC_name, FJ_FACTORY(FJ_WiiUDRC)));
#endif
    g_fuiNodeFactories.insert(
        std::make_pair(g_AchievementsMenu_name, FJ_FACTORY(FJ_AchievementsMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_AvatarSelectMenu_name, FJ_FACTORY(FJ_AvatarSelectMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_CreateWorldMenu_name, FJ_FACTORY(FJ_CreateWorldMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_LaunchMoreOptionsMenu_name,
                                             FJ_FACTORY(FJ_LaunchMoreOptionsMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_TutorialPopup_name, FJ_FACTORY(FJ_TutorialPopup)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_MultilistMenu_name, FJ_FACTORY(FJ_MultilistMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_SkinSelect_name, FJ_FACTORY(FJ_SkinSelect)));
    g_fuiNodeFactories.insert(std::make_pair(g_InGamePlayerOptions_name,
                                             FJ_FACTORY(FJ_InGamePlayerOptions)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_InGameHostOptions_name, FJ_FACTORY(FJ_InGameHostOptions)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_InGameInfoMenu_name, FJ_FACTORY(FJ_InGameInfoMenu)));
    g_fuiNodeFactories.insert(std::make_pair(g_InGameTeleportMenu_name,
                                             FJ_FACTORY(FJ_InGameTeleportMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_MapSelectMenu_name, FJ_FACTORY(FJ_MapSelectMenu)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_DragonHealth_01_name, FJ_FACTORY(FJ_DragonHealth_01)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_DragonHealth_02_name, FJ_FACTORY(FJ_DragonHealth_02)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_DragonHealth_03_name, FJ_FACTORY(FJ_DragonHealth_03)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_DragonHealth_04_name, FJ_FACTORY(FJ_DragonHealth_04)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_DragonHealth_05_name, FJ_FACTORY(FJ_DragonHealth_05)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_HorseJumpBar_name, FJ_FACTORY(FJ_HorseJumpBar)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ExperienceBar_name, FJ_FACTORY(FJ_ExperienceBar)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_BrewingArrow_name, FJ_FACTORY(FJ_BrewingArrow)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_BrewingArrow_Small_name,
                                             FJ_FACTORY(FJ_BrewingArrow_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_BrewingBubbles_name, FJ_FACTORY(FJ_BrewingBubbles)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_BrewingBubbles_Small_name,
                                             FJ_FACTORY(FJ_BrewingBubbles_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_BrewingPlate_name, FJ_FACTORY(FJ_BrewingPlate)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_BrewingPlate_Small_name,
                                             FJ_FACTORY(FJ_BrewingPlate_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_FurnaceArrow_name, FJ_FACTORY(FJ_FurnaceArrow)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_FurnaceArrow_Small_name,
                                             FJ_FACTORY(FJ_FurnaceArrow_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_FurnaceFlame_name, FJ_FACTORY(FJ_FurnaceFlame)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_FurnaceFlame_Small_name,
                                             FJ_FACTORY(FJ_FurnaceFlame_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_KillBar_name, FJ_FACTORY(FJ_KillBar)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_KillBarSmall_name, FJ_FACTORY(FJ_KillBarSmall)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ProgressBar_Loading_name,
                                             FJ_FACTORY(FJ_ProgressBar_Loading)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_AttackIndicatorCrosshair_name,
                       FJ_FACTORY(FJ_AttackIndicatorCrosshair)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_AttackIndicatorHotbar_name,
                                             FJ_FACTORY(FJ_AttackIndicatorHotbar)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_SpaceIndicatorBar_name,
                                             FJ_FACTORY(FJ_SpaceIndicatorBar)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_TextInput_name, FJ_FACTORY(FJ_TextInput)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_TextInput_Small_name, FJ_FACTORY(FJ_TextInput_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_TextInput_Normal_name, FJ_FACTORY(FJ_TextInput_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_TextInput_Vita_name, FJ_FACTORY(FJ_TextInput_Vita)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_HtmlTextPanel_name, FJ_FACTORY(FJ_HtmlTextPanel)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_HtmlTextPanelLarge_name,
                                             FJ_FACTORY(FJ_HtmlTextPanelLarge)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_BigBlack_name, FJ_FACTORY(FJ_Label_BigBlack)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_BigGrey_name, FJ_FACTORY(FJ_Label_BigGrey)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_BigWhite_name, FJ_FACTORY(FJ_Label_BigWhite)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Black_name, FJ_FACTORY(FJ_Label_Black)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_Black_Centered_name,
                                             FJ_FACTORY(FJ_Label_Black_Centered)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Black_Centered_HUD_name,
                       FJ_FACTORY(FJ_Label_Black_Centered_HUD)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Black_Small_Centered_name,
                       FJ_FACTORY(FJ_Label_Black_Small_Centered)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Black_Small_Centered_HUD_name,
                       FJ_FACTORY(FJ_Label_Black_Small_Centered_HUD)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Black_VerySmall_Centered_name,
                       FJ_FACTORY(FJ_Label_Black_VerySmall_Centered)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_Blink_Normal_name,
                                             FJ_FACTORY(FJ_Label_Blink_Normal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_Blink_Small_name,
                                             FJ_FACTORY(FJ_Label_Blink_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_Book_Normal_name,
                                             FJ_FACTORY(FJ_Label_Book_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Book_Small_name, FJ_FACTORY(FJ_Label_Book_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_Book_VerySmall_name,
                                             FJ_FACTORY(FJ_Label_Book_VerySmall)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Credits_10_name, FJ_FACTORY(FJ_Label_Credits_10)));
    g_fuiNodeFactories.insert(std::make_pair(
        g_FJ_Label_Credits_10_Yellow_name, FJ_FACTORY(FJ_Label_Credits_10_Yellow)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Credits_20_name, FJ_FACTORY(FJ_Label_Credits_20)));
    g_fuiNodeFactories.insert(std::make_pair(
        g_FJ_Label_Credits_20_Yellow_name, FJ_FACTORY(FJ_Label_Credits_20_Yellow)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Credits_30_name, FJ_FACTORY(FJ_Label_Credits_30)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_DragonHealth_name,
                                             FJ_FACTORY(FJ_Label_DragonHealth)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_DragonHealthBig_name,
                                             FJ_FACTORY(FJ_Label_DragonHealthBig)));
    g_fuiNodeFactories.insert(std::make_pair(
        g_FJ_Label_DragonHealthSmall_name, FJ_FACTORY(FJ_Label_DragonHealthSmall)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_DragonHealthVerySmall_name,
                       FJ_FACTORY(FJ_Label_DragonHealthVerySmall)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Green_name, FJ_FACTORY(FJ_Label_Green)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Grey_name, FJ_FACTORY(FJ_Label_Grey)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Html_name, FJ_FACTORY(FJ_Label_Html)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HtmlEndPoem_name,
                                             FJ_FACTORY(FJ_Label_HtmlEndPoem)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HtmlEndPoemHD_name,
                                             FJ_FACTORY(FJ_Label_HtmlEndPoemHD)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_HtmlSmall_name, FJ_FACTORY(FJ_Label_HtmlSmall)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_Level_01_name,
                                             FJ_FACTORY(FJ_Label_HUD_Level_01)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_Level_02_name,
                                             FJ_FACTORY(FJ_Label_HUD_Level_02)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_Level_03_name,
                                             FJ_FACTORY(FJ_Label_HUD_Level_03)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_Level_04_name,
                                             FJ_FACTORY(FJ_Label_HUD_Level_04)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_Level_05_name,
                                             FJ_FACTORY(FJ_Label_HUD_Level_05)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_SmallWhite_name,
                                             FJ_FACTORY(FJ_Label_HUD_SmallWhite)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_BigWhite_name,
                                             FJ_FACTORY(FJ_Label_HUD_BigWhite)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_HUD_Splash_name, FJ_FACTORY(FJ_Label_HUD_Splash)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_SplashBig_name,
                                             FJ_FACTORY(FJ_Label_HUD_SplashBig)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_HUD_SplashVita_name,
                                             FJ_FACTORY(FJ_Label_HUD_SplashVita)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_HUD_VerySmallWhite_name,
                       FJ_FACTORY(FJ_Label_HUD_VerySmallWhite)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_HUD_White_name, FJ_FACTORY(FJ_Label_HUD_White)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_OutlineLevel_name,
                                             FJ_FACTORY(FJ_Label_OutlineLevel)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_OutlineLevel_Small_name,
                       FJ_FACTORY(FJ_Label_OutlineLevel_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_OutlineTitle_name,
                                             FJ_FACTORY(FJ_Label_OutlineTitle)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_OutlineTitle_Small_name,
                       FJ_FACTORY(FJ_Label_OutlineTitle_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_Red_name, FJ_FACTORY(FJ_Label_Red)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SkinSelect_name, FJ_FACTORY(FJ_Label_SkinSelect)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_SkinSelectCenter_name,
                                             FJ_FACTORY(FJ_Label_SkinSelectCenter)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SkinSelectCenter_Off_name,
                       FJ_FACTORY(FJ_Label_SkinSelectCenter_Off)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SkinSelectCenter_Small_name,
                       FJ_FACTORY(FJ_Label_SkinSelectCenter_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SkinSelectCenter_Small_Off_name,
                       FJ_FACTORY(FJ_Label_SkinSelectCenter_Small_Off)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_SkinSelect_Off_name,
                                             FJ_FACTORY(FJ_Label_SkinSelect_Off)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_SkinSelect_Small_name,
                                             FJ_FACTORY(FJ_Label_SkinSelect_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SkinSelect_Small_Off_name,
                       FJ_FACTORY(FJ_Label_SkinSelect_Small_Off)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SmallBlack_name, FJ_FACTORY(FJ_Label_SmallBlack)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SmallGreen_name, FJ_FACTORY(FJ_Label_SmallGreen)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SmallGrey_name, FJ_FACTORY(FJ_Label_SmallGrey)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SmallRed_name, FJ_FACTORY(FJ_Label_SmallRed)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_SmallWhite_name, FJ_FACTORY(FJ_Label_SmallWhite)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_VerySmallBlack_name,
                                             FJ_FACTORY(FJ_Label_VerySmallBlack)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_VerySmallGrey_name,
                                             FJ_FACTORY(FJ_Label_VerySmallGrey)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_VerySmallWhite_name,
                                             FJ_FACTORY(FJ_Label_VerySmallWhite)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_White_name, FJ_FACTORY(FJ_Label_White)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_WhiteHtml_name, FJ_FACTORY(FJ_Label_WhiteHtml)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_WhiteHtmlSmall_name,
                                             FJ_FACTORY(FJ_Label_WhiteHtmlSmall)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Label_White_Centered_name,
                                             FJ_FACTORY(FJ_Label_White_Centered)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_White_Small_Centered_name,
                       FJ_FACTORY(FJ_Label_White_Small_Centered)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_Label_White_VerySmall_Centered_name,
                       FJ_FACTORY(FJ_Label_White_VerySmall_Centered)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_ListLabel_Black_name, FJ_FACTORY(FJ_ListLabel_Black)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_ListLabel_SmallBlack_name,
                                             FJ_FACTORY(FJ_ListLabel_SmallBlack)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_HtmlText_Normal_name, FJ_FACTORY(FJ_HtmlText_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_HtmlText_Small_name, FJ_FACTORY(FJ_HtmlText_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_HtmlText_WhiteNormal_name,
                                             FJ_FACTORY(FJ_HtmlText_WhiteNormal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_HtmlText_WhiteSmall_name,
                                             FJ_FACTORY(FJ_HtmlText_WhiteSmall)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_HtmlText_WithPanel_Normal_name,
                       FJ_FACTORY(FJ_HtmlText_WithPanel_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_HtmlText_WithPanel_Small_name,
                       FJ_FACTORY(FJ_HtmlText_WithPanel_Small)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_DynamicHtmlText_name, &FJ_DynamicHtmlText::Create));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_DynamicHtmlText_Normal_name,
                                             FJ_FACTORY(FJ_DynamicHtmlText_Normal)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_DynamicHtmlText_Small_name,
                                             FJ_FACTORY(FJ_DynamicHtmlText_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_DynamicHtmlText_WithPanel_Normal_name,
                       FJ_FACTORY(FJ_DynamicHtmlText_WithPanel_Normal)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_DynamicHtmlText_WithPanel_Small_name,
                       FJ_FACTORY(FJ_DynamicHtmlText_WithPanel_Small)));
    g_fuiNodeFactories.insert(
        std::make_pair(g_FJ_SignInput_name, FJ_FACTORY(FJ_SignInput)));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_panel9Grid_16Fade_name, &FJ_panel9Grid_16Fade::Create));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_Panel9GridFade_name, &FJ_Panel9GridFade::Create));
    g_fuiNodeFactories.insert(std::make_pair(g_FJ_PanelRecessFade_name, &FJ_PanelRecessFade::Create));
}

FJ_FuiNode* FJ_Factory::createObject(const char* symbol, fuiRenderNode* renderNode, bool createFromSymbol,
                                     const char* className)
{
    static bool initialized = false;
    if (!initialized)
    {
        initialise();
        initialized = true;
    }

    const char* factoryName = symbol;
    const char* end = symbol + std::strlen(symbol);
    const char* separator = std::strrchr(symbol, '.');
    if (separator != NULL)
        factoryName = separator + 1;

    FJ_FactoryMap::iterator factory = g_fuiNodeFactories.find(std::string(factoryName, end - factoryName));

    bool hasFactory = separator != NULL && factory != g_fuiNodeFactories.end();

    if (!createFromSymbol)
    {
        if (!hasFactory)
            return new FJ_FuiNode(renderNode);
        return factory->second(renderNode);
    }

    fuiRenderNode* nodeRender = (fuiRenderNode*)gFui->createNodeFromSymbol(symbol, renderNode);
    FJ_FuiNode* node;
    if (!hasFactory)
        node = new FJ_FuiNode(nodeRender);
    else
        node = factory->second(nodeRender);

    nodeRender->m_node = node;
    node->ASConstructor();
    nodeRender->m_flags &= ~0x40;

    if (className == NULL)
        ::snprintf(nodeRender->m_name, 0x40, "Fac%d", g_factoryNodeId++);
    else
        std::strncpy(nodeRender->m_name, className, 0x40);

    return node;
}
