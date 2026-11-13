#ifndef GAME_UI_UISCENE_EDITSERVERMENU_HPP
#define GAME_UI_UISCENE_EDITSERVERMENU_HPP

#include <string>

#include "game/ui/UIControl_Button.hpp"
#include "game/ui/UIControl_Label.hpp"
#include "game/ui/UIControl_TextInput.hpp"
#include "game/ui/UIScene.hpp"

struct EditServerSceneData
{
    explicit EditServerSceneData(int index) : serverIndex(index), friendMode(false) {}
    EditServerSceneData(const std::string& key, const std::wstring& nicknameText, const std::string& codeText)
        : serverIndex(-1), friendMode(true), friendKey(key), nickname(nicknameText), code(codeText)
    {
    }

    int serverIndex;
    bool friendMode;
    std::string friendKey;
    std::wstring nickname;
    std::string code;
};

class UIScene_EditServerMenu : public UIScene
{
public:
    UIScene_EditServerMenu(int playerId, void* data, UILayer* layer);
    virtual ~UIScene_EditServerMenu();

    virtual std::wstring getMoviePath();
    virtual unsigned int mapElementsAndNames();
    virtual int getSceneType();
    virtual void updateTooltips();
    virtual void updateComponents();
    virtual void handleDestroy();
    virtual void handleInput(int playerId, int action, bool repeat, bool pressed, bool released, bool& handled);
    virtual void handlePress(int controlId, int childId);

private:
    enum EControl
    {
        eControl_Name,
        eControl_Address,
        eControl_Ok
    };

    void SetupControl(UIControl& control, const char* name);
    void CloseTitleGap();
    void RequestText(int control);
    void Save();
    static int KeyboardReturned(void* context, bool accepted);

    UIControl_Label m_title;
    UIControl_Label m_nameLabel;
    UIControl_Label m_addressLabel;
    UIControl_TextInput m_nameInput;
    UIControl_TextInput m_addressInput;
    UIControl_Button m_okButton;
    int m_serverIndex;
    bool m_friendMode;
    std::string m_friendKey;
    bool m_friendKnown;
    std::wstring m_name;
    std::wstring m_address;
    int m_keyboardControl;
};

#endif
