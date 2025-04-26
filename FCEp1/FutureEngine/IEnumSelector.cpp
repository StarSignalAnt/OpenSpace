#include "IEnumSelector.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "FutureApp.h"
#include "Texture2D.h"

IEnumSelectorBase::IEnumSelectorBase()
    : IControl(),
    m_SelectedIndex(-1),
    m_IsDropdownOpen(false),
    m_ItemHeight(22.0f),
    m_HoverIndex(-1),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
    m_TextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_HoverColor(glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)),
    m_SelectionColor(glm::vec4(0.4f, 0.6f, 0.8f, 1.0f)),
    m_BorderColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f))
{
    m_DropdownTexture = new Texture2D("engine/ui/softrect.png");
}

IEnumSelectorBase::IEnumSelectorBase(glm::vec2 position, glm::vec2 size)
    : IControl(position, size),
    m_SelectedIndex(-1),
    m_IsDropdownOpen(false),
    m_ItemHeight(22.0f),
    m_HoverIndex(-1),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
    m_TextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_HoverColor(glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)),
    m_SelectionColor(glm::vec4(0.4f, 0.6f, 0.8f, 1.0f)),
    m_BorderColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f))
{
    m_DropdownTexture = new Texture2D("engine/ui/softrect.png");
}

IEnumSelectorBase::~IEnumSelectorBase()
{
    // Texture is likely managed elsewhere in your engine
    // so not deleting m_DropdownTexture here
}

void IEnumSelectorBase::Update(float delta)
{
    UpdateChildren(delta);
}

void IEnumSelectorBase::Render()
{
    glm::vec2 renderPos = GetRenderPosition();

    // Draw the main selector background
    UIHelp::DrawRect(renderPos, m_Size, m_BackgroundColor);

    // Draw the selected value text
    std::string currentValueString = GetCurrentValueString();
    UIHelp::DrawText(renderPos + glm::vec2(5, (m_Size.y - UIHelp::StrHeight(currentValueString)) / 2),
        currentValueString, m_TextColor);

    // Draw a dropdown arrow indicator
    float arrowSize = 8.0f;
    float arrowPadding = (m_Size.y - arrowSize) / 2;
    glm::vec2 arrowPos = renderPos + glm::vec2(m_Size.x - arrowSize - arrowPadding, arrowPadding);

    // Draw a simple triangle for the arrow
    if (m_IsDropdownOpen)
    {
        // Triangle pointing up for open dropdown
        UIHelp::DrawRect(arrowPos + glm::vec2(arrowSize / 2, 0), glm::vec2(1, arrowSize), m_TextColor);
        UIHelp::DrawRect(arrowPos + glm::vec2(0, arrowSize - 1), glm::vec2(arrowSize, 1), m_TextColor);
    }
    else
    {
        // Triangle pointing down for closed dropdown
        UIHelp::DrawRect(arrowPos + glm::vec2(arrowSize / 2, 0), glm::vec2(1, arrowSize), m_TextColor);
        UIHelp::DrawRect(arrowPos, glm::vec2(arrowSize, 1), m_TextColor);
    }

    // Draw the border
    UIHelp::DrawOutlineRect(renderPos, m_Size, m_BorderColor);

    // Draw the dropdown if it's open
    if (m_IsDropdownOpen && !m_EnumNames.empty())
    {
        glm::vec2 dropdownPos = renderPos + glm::vec2(0, m_Size.y);
        float dropdownHeight = GetDropdownHeight();
        glm::vec2 dropdownSize = glm::vec2(m_Size.x, dropdownHeight);

        // Draw dropdown background
        UIHelp::DrawRect(dropdownPos, dropdownSize, m_BackgroundColor);

        // Enable scissor test to clip items that go beyond visible area
        int windowHeight = FutureApp::m_Inst->GetHeight();
        UIHelp::setScissor(dropdownPos.x, dropdownPos.y, dropdownSize.x, dropdownHeight, windowHeight);

        // Draw each item
        for (size_t i = 0; i < m_EnumNames.size(); ++i)
        {
            glm::vec2 itemPos = dropdownPos + glm::vec2(0, i * m_ItemHeight);
            glm::vec2 itemSize = glm::vec2(m_Size.x, m_ItemHeight);

            // Highlight if this is the hovered item
            if (static_cast<int>(i) == m_HoverIndex)
            {
                UIHelp::DrawRect(itemPos, itemSize, m_HoverColor);
            }
            // Highlight if this is the selected item
            else if (static_cast<int>(i) == m_SelectedIndex)
            {
                UIHelp::DrawRect(itemPos, itemSize, m_SelectionColor);
            }

            // Draw the item text
            UIHelp::DrawText(itemPos + glm::vec2(5, (m_ItemHeight - UIHelp::StrHeight(m_EnumNames[i])) / 2),
                m_EnumNames[i], m_TextColor);
        }

        // Draw dropdown border
        UIHelp::DrawOutlineRect(dropdownPos, dropdownSize, m_BorderColor);

        // Disable scissor test
        UIHelp::RemoveScissor();
    }

    // Render children controls (if any)
    RenderChildren();
}

void IEnumSelectorBase::PreRender()
{
    IControl::PreRender();
}

void IEnumSelectorBase::OnMouseDown(int button)
{
    if (button == 0) // Left mouse button
    {
        glm::vec2 mousePos = GameInput::MousePosition;
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 localPos = mousePos - renderPos;

        if (m_IsDropdownOpen)
        {
            // If dropdown is open, check if clicked on an item
            if (localPos.y > m_Size.y && localPos.y < m_Size.y + GetDropdownHeight())
            {
                int clickedIndex = GetIndexAtPosition(localPos);
                if (clickedIndex >= 0 && clickedIndex < static_cast<int>(m_EnumNames.size()))
                {
                    m_SelectedIndex = clickedIndex;
                    ValueSelected(m_SelectedIndex);
                    CloseDropdown();
                    Click(); // Trigger the click callback
                }
            }
            else
            {
                // Clicked outside the dropdown, close it
                CloseDropdown();
            }
        }
        else
        {
            // If dropdown is closed and clicked on the selector, open it
            ToggleDropdown();
        }
    }
}

void IEnumSelectorBase::OnMouseUp(int button)
{
    // Nothing special to handle here for now
}

void IEnumSelectorBase::OnMouseLeave()
{
    // Reset hover index when mouse leaves
    m_HoverIndex = -1;
}

void IEnumSelectorBase::OnMouseMove(glm::vec2 position, glm::vec2 delta)
{
    if (m_IsDropdownOpen)
    {
        // Update hover index based on mouse position
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 localPos = GameInput::MousePosition - renderPos;

        m_HoverIndex = GetIndexAtPosition(localPos);
    }
}

bool IEnumSelectorBase::InBounds(glm::vec2 position)
{
    glm::vec2 root = GetRenderPosition();

    // First check parent control's culling rules
    if (GetRoot()) {
        if (GetRoot()->GetCullChildren()) {
            if (m_RootControl != nullptr) {
                if (position.x < m_RootControl->GetRenderPosition().x || position.y < m_RootControl->GetRenderPosition().y)
                {
                    return false;
                }
                if (position.x > m_RootControl->GetRenderPosition().x + m_RootControl->GetSize().x ||
                    position.y > m_RootControl->GetRenderPosition().y + m_RootControl->GetSize().y) {
                    return false;
                }
            }
        }
    }

    // Check if within the main selector area
    bool inMainArea = (position.x >= root.x && position.x <= root.x + m_Size.x &&
        position.y >= root.y && position.y <= root.y + m_Size.y);

    // If dropdown is open, also check if within the dropdown area
    if (m_IsDropdownOpen && !inMainArea) {
        float dropdownHeight = GetDropdownHeight();
        bool inDropdownArea = (position.x >= root.x && position.x <= root.x + m_Size.x &&
            position.y >= root.y + m_Size.y && position.y <= root.y + m_Size.y + dropdownHeight);
        return inDropdownArea;
    }

    return inMainArea;
}

void IEnumSelectorBase::ToggleDropdown()
{
    m_IsDropdownOpen = !m_IsDropdownOpen;

    if (m_IsDropdownOpen)
    {
        // Initialize hover index based on current mouse position
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 localPos = GameInput::MousePosition - renderPos;
        m_HoverIndex = GetIndexAtPosition(localPos);
    }
    else
    {
        m_HoverIndex = -1;
    }
}

void IEnumSelectorBase::CloseDropdown()
{
    m_IsDropdownOpen = false;
    m_HoverIndex = -1;
}

float IEnumSelectorBase::GetDropdownHeight() const
{
    return m_EnumNames.size() * m_ItemHeight;
}

int IEnumSelectorBase::GetIndexAtPosition(glm::vec2 localPosition)
{
    // If position is within dropdown area, calculate the index
    if (localPosition.y > m_Size.y && localPosition.y < m_Size.y + GetDropdownHeight() &&
        localPosition.x >= 0 && localPosition.x < m_Size.x)
    {
        int index = static_cast<int>((localPosition.y - m_Size.y) / m_ItemHeight);
        if (index >= 0 && index < static_cast<int>(m_EnumNames.size()))
        {
            return index;
        }
    }

    return -1;
}