#include "IComboBox.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "FutureApp.h"
#include "Texture2D.h"

IComboBox::IComboBox()
    : IControl(),
    m_SelectedIndex(-1),
    m_IsDropdownOpen(false),
    m_ItemHeight(22.0f),
    m_HoverIndex(-1),
    m_UseScroller(false),
    m_MaxDropdownHeight(300.0f),
    m_Scroller(nullptr),
    m_ScrollOffset(0.0f),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
    m_TextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_HoverColor(glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)),
    m_SelectionColor(glm::vec4(0.4f, 0.6f, 0.8f, 1.0f)),
    m_BorderColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f))
{
    m_DropdownTexture = new Texture2D("engine/ui/softrect.png");

    // Create the vertical scroller
    m_Scroller = new IVerticalScroller();
    m_Scroller->SetOnScrolled([this](float offset) {
        m_ScrollOffset = offset;
        });
}

IComboBox::IComboBox(glm::vec2 position, glm::vec2 size)
    : IControl(position, size),
    m_SelectedIndex(-1),
    m_IsDropdownOpen(false),
    m_ItemHeight(22.0f),
    m_HoverIndex(-1),
    m_UseScroller(false),
    m_MaxDropdownHeight(300.0f),
    m_Scroller(nullptr),
    m_ScrollOffset(0.0f),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
    m_TextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_HoverColor(glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)),
    m_SelectionColor(glm::vec4(0.4f, 0.6f, 0.8f, 1.0f)),
    m_BorderColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f))
{
    m_DropdownTexture = new Texture2D("engine/ui/softrect.png");

    // Create the vertical scroller (initially positioned at 0,0 with zero size - will be positioned properly later)
    m_Scroller = new IVerticalScroller(glm::vec2(0, 0), glm::vec2(15.0f, 100.0f));

    // Add the scroller as a child
    AddChild(m_Scroller);

    // Set up the callback to track scroll position
    m_Scroller->SetOnScrolled([this](float offset) {
        m_ScrollOffset = offset;
        });
}

IComboBox::~IComboBox()
{
    delete m_Scroller;
}

void IComboBox::AddItem(std::string text)
{
    m_Items.push_back(text);

    // If this is the first item, select it by default
    if (m_SelectedIndex == -1 && m_Items.size() == 1)
    {
        m_SelectedIndex = 0;
        if (OnSelectionChanged)
        {
            OnSelectionChanged(m_SelectedIndex, m_Items[m_SelectedIndex]);
        }
    }

    // Update scroller content height
    if (m_UseScroller && m_Scroller)
    {
        m_Scroller->SetContentHeight(m_Items.size() * m_ItemHeight);
    }
}

void IComboBox::RemoveItem(int index)
{
    if (index >= 0 && index < static_cast<int>(m_Items.size()))
    {
        m_Items.erase(m_Items.begin() + index);

        // Adjust selected index if needed
        if (m_SelectedIndex == index)
        {
            m_SelectedIndex = m_Items.empty() ? -1 : 0;
            if (m_SelectedIndex >= 0 && OnSelectionChanged)
            {
                OnSelectionChanged(m_SelectedIndex, m_Items[m_SelectedIndex]);
            }
        }
        else if (m_SelectedIndex > index)
        {
            m_SelectedIndex--;
        }

        // Update scroller content height
        if (m_UseScroller && m_Scroller)
        {
            m_Scroller->SetContentHeight(m_Items.size() * m_ItemHeight);
        }
    }
}

void IComboBox::ClearItems()
{
    m_Items.clear();
    m_SelectedIndex = -1;

    // Update scroller content height
    if (m_UseScroller && m_Scroller)
    {
        m_Scroller->SetContentHeight(0);
    }
}

std::string IComboBox::GetSelectedItem() const
{
    if (m_SelectedIndex >= 0 && m_SelectedIndex < static_cast<int>(m_Items.size()))
    {
        return m_Items[m_SelectedIndex];
    }
    return "";
}

int IComboBox::GetSelectedIndex() const
{
    return m_SelectedIndex;
}

void IComboBox::SetSelectedIndex(int index)
{
    if (index >= -1 && index < static_cast<int>(m_Items.size()))
    {
        bool changed = m_SelectedIndex != index;
        m_SelectedIndex = index;

        // Call the selection changed callback if it exists and the selection actually changed
        if (changed && OnSelectionChanged && m_SelectedIndex >= 0)
        {
            OnSelectionChanged(m_SelectedIndex, m_Items[m_SelectedIndex]);
        }
    }
}

void IComboBox::Update(float delta)
{
    // Call base class update to handle children
    UpdateChildren(delta);

    // Update scroller if using it
    if (m_UseScroller && m_Scroller && m_IsDropdownOpen)
    {
        // Position and size the scroller
        float visibleDropdownHeight = GetVisibleDropdownHeight();
        m_Scroller->Set(glm::vec2(m_Size.x - 15.0f, m_Size.y),
            glm::vec2(15.0f, visibleDropdownHeight));

        // Update the scroller
        m_Scroller->Update(delta);
    }
}

float IComboBox::GetDropdownHeight() const
{
    // Calculate the total height of the dropdown
    return m_Items.size() * m_ItemHeight;
}

float IComboBox::GetVisibleDropdownHeight() const
{
    if (!m_UseScroller)
    {
        return GetDropdownHeight();
    }

    // Return either the max height or the actual height, whichever is smaller
    return std::min(m_MaxDropdownHeight, GetDropdownHeight());
}

void IComboBox::Render()
{
    glm::vec2 renderPos = GetRenderPosition();

    // Draw the main combobox background
    UIHelp::DrawRect(renderPos, m_Size, m_BackgroundColor);

    // Draw the selected text
    if (m_SelectedIndex >= 0)
    {
        UIHelp::DrawText(renderPos + glm::vec2(5, (m_Size.y - UIHelp::StrHeight(GetSelectedItem())) / 2),
            GetSelectedItem(), m_TextColor);
    }

    // Draw a dropdown arrow indicator
    float arrowSize = 8.0f;
    float arrowPadding = (m_Size.y - arrowSize) / 2;
    glm::vec2 arrowPos = renderPos + glm::vec2(m_Size.x - arrowSize - arrowPadding, arrowPadding);

    // Draw a simple triangle for the arrow (pointing down when closed, up when open)
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
    if (m_IsDropdownOpen && !m_Items.empty())
    {
        float visibleDropdownHeight = GetVisibleDropdownHeight();
        float totalDropdownHeight = GetDropdownHeight();

        // Determine if we need a scrollbar based on content height
        bool needsScroller = m_UseScroller && totalDropdownHeight > visibleDropdownHeight;
        float scrollerWidth = needsScroller ? 15.0f : 0.0f;

        // Position dropdown below the combobox
        glm::vec2 dropdownPos = renderPos + glm::vec2(0, m_Size.y);
        glm::vec2 dropdownSize = glm::vec2(m_Size.x, visibleDropdownHeight);

        // Draw dropdown background
        UIHelp::DrawRect(dropdownPos, dropdownSize, m_BackgroundColor);

        // Enable scissor test to clip items that go beyond visible area
        int windowHeight = FutureApp::m_Inst->GetHeight();
        UIHelp::setScissor(dropdownPos.x, dropdownPos.y, dropdownSize.x - scrollerWidth, visibleDropdownHeight, windowHeight);

        // Draw each item, with scrolling applied if needed
        float yOffset = 0;
        if (needsScroller)
        {
            yOffset = -m_ScrollOffset;
        }

        for (size_t i = 0; i < m_Items.size(); ++i)
        {
            float itemY = yOffset + i * m_ItemHeight;

            // Skip items that are scrolled out of view
            if (itemY + m_ItemHeight < 0 || itemY > visibleDropdownHeight)
            {
                continue;
            }

            glm::vec2 itemPos = dropdownPos + glm::vec2(0, itemY);
            glm::vec2 itemSize = glm::vec2(m_Size.x - scrollerWidth, m_ItemHeight);

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
            UIHelp::DrawText(itemPos + glm::vec2(5, (m_ItemHeight - UIHelp::StrHeight(m_Items[i])) / 2),
                m_Items[i], m_TextColor);
        }

        // Disable scissor test
        UIHelp::RemoveScissor();

        // Draw dropdown border
        UIHelp::DrawOutlineRect(dropdownPos, dropdownSize, m_BorderColor);

        // Render the scroller if needed
        if (needsScroller)
        {
            // Position the scroller directly beside the dropdown - this is key!
            // The scroller's position is relative to the combobox, but we need it positioned
            // at the right edge of the dropdown and extending down from the combobox
            m_Scroller->Set(glm::vec2(m_Size.x - (scrollerWidth-5), m_Size.y),
                glm::vec2(scrollerWidth, visibleDropdownHeight));

            // Render the scroller
            m_Scroller->Render();
        }
    }

    // Render other child controls (if any)
    for (auto child : m_Children)
    {
        // Skip rendering the scroller since we handle it specially
        if (child != m_Scroller)
        {
            child->Render();
        }
    }
}

void IComboBox::PreRender()
{
    // Call base class pre-render
    IControl::PreRender();
}

void IComboBox::OnMouseDown(int button)
{
    if (button == 0) // Left mouse button
    {
        glm::vec2 mousePos = GameInput::MousePosition;
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 localPos = mousePos - renderPos;

        if (m_IsDropdownOpen)
        {
            // Calculate dropdown area
            float visibleDropdownHeight = GetVisibleDropdownHeight();
            float totalDropdownHeight = GetDropdownHeight();
            bool needsScroller = m_UseScroller && totalDropdownHeight > visibleDropdownHeight;

            // If using a scroller, check if click is on the scroller
            if (needsScroller)
            {
                // The scroller's position is relative to the combobox
                // Position of the scroller in screen space would be:
                // renderPos + scroller's position relative to combobox

                // Check if click is on the scroller area
                if (localPos.x >= m_Size.x - 15.0f &&
                    localPos.y >= m_Size.y &&
                    localPos.y <= m_Size.y + visibleDropdownHeight &&
                    localPos.x <= m_Size.x)
                {
                    // Forward to scroller
                    m_Scroller->OnMouseDown(button);
                    return;
                }
            }

            // Check if click is on an item in the dropdown
            if (localPos.y > m_Size.y && localPos.y < m_Size.y + visibleDropdownHeight &&
                localPos.x >= 0 && localPos.x < m_Size.x - (needsScroller ? 15.0f : 0.0f))
            {
                int clickedIndex = GetIndexAtPosition(localPos);
                if (clickedIndex >= 0)
                {
                    SetSelectedIndex(clickedIndex);
                    CloseDropdown();
                    Click(); // Trigger the click callback
                    return;
                }
            }

            // Click wasn't on dropdown or scroller, close it
            CloseDropdown();
        }
        else
        {
            // If dropdown is closed and clicked on the combobox, open it
            ToggleDropdown();
        }
    }
}

void IComboBox::OnMouseUp(int button)
{
    // If using scroller and it's active, forward to scroller
    if (m_UseScroller && m_IsDropdownOpen && m_Scroller)
    {
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 dropdownPos = renderPos + glm::vec2(0, m_Size.y);
        glm::vec2 scrollerPos = dropdownPos + glm::vec2(m_Size.x - 15.0f, 0);

        // Set temporary offset for proper event handling
        glm::vec2 originalOffset = m_Scroller->GetOffset();
        m_Scroller->SetOffset(scrollerPos - m_Scroller->GetRenderPosition());

        m_Scroller->OnMouseUp(button);

        // Restore original offset
        m_Scroller->SetOffset(originalOffset);
    }
}

void IComboBox::OnMouseLeave()
{
    // Reset hover index when mouse leaves
    m_HoverIndex = -1;
}

void IComboBox::OnMouseMove(glm::vec2 position, glm::vec2 delta)
{
    if (m_IsDropdownOpen)
    {
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 localPos = GameInput::MousePosition - renderPos;

        // Calculate dropdown area
        float visibleDropdownHeight = GetVisibleDropdownHeight();
        float totalDropdownHeight = GetDropdownHeight();
        bool needsScroller = m_UseScroller && totalDropdownHeight > visibleDropdownHeight;

        // Check if mouse is over the scroller
        if (needsScroller &&
            localPos.x >= m_Size.x - 15.0f &&
            localPos.y >= m_Size.y &&
            localPos.y <= m_Size.y + visibleDropdownHeight &&
            localPos.x <= m_Size.x)
        {
            // Forward to scroller
            m_Scroller->OnMouseMove(position, delta);
            return;
        }

        // Update hover index based on mouse position
        m_HoverIndex = GetIndexAtPosition(localPos);
    }
}

void IComboBox::OnMouseWheel(float delta)
{
    // Forward mouse wheel to scroller when dropdown is open
    if (m_IsDropdownOpen && m_UseScroller && m_Scroller)
    {
        m_Scroller->OnMouseWheel(delta);
    }
}

void IComboBox::SetOnSelectionChanged(std::function<void(int, std::string)> callback)
{
    OnSelectionChanged = callback;
}

void IComboBox::ToggleDropdown()
{
    m_IsDropdownOpen = !m_IsDropdownOpen;

    if (m_IsDropdownOpen)
    {
        // Initialize hover index based on current mouse position
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 localPos = GameInput::MousePosition - renderPos;
        m_HoverIndex = GetIndexAtPosition(localPos);

        // Update scroller if using it
        if (m_UseScroller && m_Scroller)
        {
            float totalHeight = GetDropdownHeight();
            float visibleHeight = GetVisibleDropdownHeight();

            // Position the scroller at the right edge of the dropdown
            // NOTE: This position is relative to the combobox since the scroller is a child
            m_Scroller->Set(glm::vec2(m_Size.x - 15.0f, m_Size.y),
                glm::vec2(15.0f, visibleHeight));

            // Set content height
            m_Scroller->SetContentHeight(totalHeight);

            // Scroll to show selected item if needed
            if (m_SelectedIndex >= 0)
            {
                float selectedItemPos = m_SelectedIndex * m_ItemHeight;
                if (selectedItemPos > visibleHeight)
                {
                    float scrollPos = (selectedItemPos - (visibleHeight / 2)) / (totalHeight - visibleHeight);
                    scrollPos = glm::clamp(scrollPos, 0.0f, 1.0f);
                    m_Scroller->SetScrollPosition(scrollPos);
                }
            }
        }
    }
    else
    {
        m_HoverIndex = -1;
    }
}

void IComboBox::CloseDropdown()
{
    m_IsDropdownOpen = false;
    m_HoverIndex = -1;
}

int IComboBox::GetIndexAtPosition(glm::vec2 localPosition)
{
    // If position is within dropdown area, calculate the index
    if (localPosition.y > m_Size.y && localPosition.y < m_Size.y + GetVisibleDropdownHeight() &&
        localPosition.x >= 0 && localPosition.x < m_Size.x - (m_UseScroller ? 15.0f : 0.0f))
    {
        float adjustedY = localPosition.y - m_Size.y;

        // Adjust for scrolling if using scroller
        if (m_UseScroller)
        {
            adjustedY += m_ScrollOffset;
        }

        int index = static_cast<int>(adjustedY / m_ItemHeight);
        if (index >= 0 && index < static_cast<int>(m_Items.size()))
        {
            return index;
        }
    }

    return -1;
}

bool IComboBox::InBounds(glm::vec2 position)
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

    // Check if within the main combobox area
    bool inMainArea = (position.x >= root.x && position.x <= root.x + m_Size.x &&
        position.y >= root.y && position.y <= root.y + m_Size.y);

    // If dropdown is open, also check if within the dropdown area
    if (m_IsDropdownOpen && !inMainArea) {
        float visibleDropdownHeight = GetVisibleDropdownHeight();
        bool inDropdownArea = (position.x >= root.x && position.x <= root.x + m_Size.x &&
            position.y >= root.y + m_Size.y && position.y <= root.y + m_Size.y + visibleDropdownHeight);

        // If using scroller, also check if within the scroller area
        if (m_UseScroller && m_Scroller)
        {
            if (m_Scroller->InBounds(position))
            {
                return true;
            }
        }

        return inDropdownArea;
    }

    return inMainArea;
}