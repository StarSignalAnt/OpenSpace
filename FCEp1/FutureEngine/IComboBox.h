#pragma once
#include "IControl.h"
#include "IVerticalScroller.h"
#include <vector>
#include <string>
#include <functional>

class Texture2D;

class IComboBox : public IControl
{
public:
    IComboBox();
    IComboBox(glm::vec2 position, glm::vec2 size);
    ~IComboBox();

    void AddItem(std::string text);
    void RemoveItem(int index);
    void ClearItems();

    std::string GetSelectedItem() const;
    int GetSelectedIndex() const;
    void SetSelectedIndex(int index);
    size_t GetItemCount() const { return m_Items.size(); }

    // Set maximum dropdown height with scrolling
    void SetMaxDropdownHeight(float height) { m_MaxDropdownHeight = height; }
    float GetMaxDropdownHeight() const { return m_MaxDropdownHeight; }

    // Overridden methods from IControl
    void Update(float delta) override;
    void Render() override;
    void PreRender() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseLeave() override;
    void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
    void OnMouseWheel(float delta) override;
    bool InBounds(glm::vec2 position) override;

    // Callback for selection changed
    void SetOnSelectionChanged(std::function<void(int, std::string)> callback);

    // Access to the internal vertical scroller 
    IVerticalScroller* GetScroller() { return m_Scroller; }
    void SetUseScroller(bool use) { m_UseScroller = use; }
    bool GetUseScroller() const { return m_UseScroller; }

private:
    std::vector<std::string> m_Items;
    int m_SelectedIndex;
    bool m_IsDropdownOpen;
    float m_ItemHeight;
    int m_HoverIndex;
    Texture2D* m_DropdownTexture;

    // Vertical scrolling
    bool m_UseScroller;
    float m_MaxDropdownHeight;
    IVerticalScroller* m_Scroller;
    float m_ScrollOffset;

    glm::vec4 m_BackgroundColor;
    glm::vec4 m_TextColor;
    glm::vec4 m_HoverColor;
    glm::vec4 m_SelectionColor;
    glm::vec4 m_BorderColor;

    std::function<void(int, std::string)> OnSelectionChanged;

    void ToggleDropdown();
    void CloseDropdown();
    int GetIndexAtPosition(glm::vec2 localPosition);
    float GetDropdownHeight() const;
    float GetVisibleDropdownHeight() const;
};  