#ifndef GAME_UI_UIMENUPAGE_HPP
#define GAME_UI_UIMENUPAGE_HPP

#include <string>
#include <vector>

#include "game/ui/UICustomMenuPage.hpp"

// a page of the console menu scene held as data: the rows are given to it, and the scene builds its controls from them,
// so a menu is made by filling a page and handing it to ConsoleUIController::NavigateToMenuPage
class UIMenuPage : public UICustomMenuPage
{
public:
    explicit UIMenuPage(const std::wstring& name);

    void AddButton(int id, const std::wstring& label, const std::wstring& description);
    void AddCheckbox(int id, const std::wstring& label, bool checked, const std::wstring& description);
    void AddSlider(int id, const std::wstring& label, int minimum, int maximum, int value,
                   const std::wstring& description);
    void AddValue(int id, const std::wstring& label, const std::wstring& value, const std::wstring& description);
    void AddLabel(int id, const std::wstring& label);

    virtual std::wstring GetName() const;
    virtual void Build(UIScene_CustomMenu& menu);

private:
    enum EKind
    {
        eKind_Button,
        eKind_Checkbox,
        eKind_Slider,
        eKind_Value,
        eKind_Label
    };

    struct Row
    {
        int kind;
        int id;
        std::wstring label;
        std::wstring value;
        std::wstring description;
        bool checked;
        int minimum;
        int maximum;
        int position;
    };

    void Add(int kind, int id, const std::wstring& label);

    std::wstring m_name;
    std::vector<Row> m_rows;
};

#endif
