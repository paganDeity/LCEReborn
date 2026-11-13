#include "game/ui/UIMenuPage.hpp"

#include "game/ui/UIScene_CustomMenu.hpp"

UIMenuPage::UIMenuPage(const std::wstring& name) : m_name(name)
{
}

void UIMenuPage::Add(int kind, int id, const std::wstring& label)
{
    Row row = {kind, id, label, std::wstring(), std::wstring(), false, 0, 0, 0};
    m_rows.push_back(row);
}

void UIMenuPage::AddButton(int id, const std::wstring& label, const std::wstring& description)
{
    Add(eKind_Button, id, label);
    m_rows.back().description = description;
}

void UIMenuPage::AddCheckbox(int id, const std::wstring& label, bool checked, const std::wstring& description)
{
    Add(eKind_Checkbox, id, label);
    m_rows.back().checked = checked;
    m_rows.back().description = description;
}

void UIMenuPage::AddSlider(int id, const std::wstring& label, int minimum, int maximum, int value,
                           const std::wstring& description)
{
    Add(eKind_Slider, id, label);
    Row& row = m_rows.back();
    row.minimum = minimum;
    row.maximum = maximum;
    row.position = value;
    row.description = description;
}

void UIMenuPage::AddValue(int id, const std::wstring& label, const std::wstring& value, const std::wstring& description)
{
    Add(eKind_Value, id, label);
    m_rows.back().value = value;
    m_rows.back().description = description;
}

void UIMenuPage::AddLabel(int id, const std::wstring& label)
{
    Add(eKind_Label, id, label);
}

std::wstring UIMenuPage::GetName() const
{
    return m_name;
}

void UIMenuPage::Build(UIScene_CustomMenu& menu)
{
    for (size_t index = 0; index < m_rows.size(); ++index)
    {
        const Row& row = m_rows[index];
        switch (row.kind)
        {
        case eKind_Button:
            menu.AddButton(row.id, row.label, row.description);
            break;
        case eKind_Checkbox:
            menu.AddCheckbox(row.id, row.label, row.checked, row.description);
            break;
        case eKind_Slider:
            menu.AddSlider(row.id, row.label, row.minimum, row.maximum, row.position, row.description);
            break;
        case eKind_Value:
            menu.AddValue(row.id, row.label, row.value, row.description);
            break;
        default:
            menu.AddLabel(row.id, row.label);
            break;
        }
    }
}
