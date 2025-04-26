#pragma once
#include "IControl.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "Texture2D.h"

class IHorizontalScroller : public IControl
{
public:
    IHorizontalScroller() : IControl(), m_ContentWidth(0), m_ScrollPosition(0), m_ScrollButtonSize(0),
        m_IsScrolling(false), m_ScrollTrackColor(glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)),
        m_ScrollButtonColor(glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)), m_ScrollButtonHeight(15.0f) {
    
        m_ScrollImage = new Texture2D("engine/ui/softrecth.png");;
        m_Text = "HSCROLL";
    }


    IHorizontalScroller(glm::vec2 position, glm::vec2 size)
        : IControl(position, size), m_ContentWidth(0), m_ScrollPosition(0), m_ScrollButtonSize(0),
        m_IsScrolling(false), m_ScrollTrackColor(glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)),
        m_ScrollButtonColor(glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)), m_ScrollButtonHeight(15.0f) {
        m_ScrollImage = new Texture2D("engine/ui/softrecth.png");;
        m_Text = "HSCROLL";
    }

    // Set the total content width that needs to be scrolled
    void SetContentWidth(float width)
    {
        m_ContentWidth = width;
        UpdateScrollButtonSize();
    }

    // Get the current scroll position (0.0 to 1.0)
    float GetScrollPosition() const { return m_ScrollPosition; }

    // Set scroll position manually (0.0 to 1.0)
    void SetScrollPosition(float position)
    {
        m_ScrollPosition = glm::clamp(position, 0.0f, 1.0f);
        Scrolled();
    }

    float GetContentWidth() {

        return m_ContentWidth;

    }

    // Set colors for the scrollbar
    void SetScrollTrackColor(const glm::vec4& color) { m_ScrollTrackColor = color; }
    void SetScrollButtonColor(const glm::vec4& color) { m_ScrollButtonColor = color; }

    // Set the height of the scroll button
    void SetScrollButtonHeight(float height) { m_ScrollButtonHeight = height; }

    // Calculate the visible ratio and update the scroll button size
    void UpdateScrollButtonSize()
    {
        if (m_ContentWidth <= m_Size.x)
        {
            // Content fits within the visible area
            m_ScrollButtonSize = m_Size.x;
        }
        else
        {
            // Calculate the ratio of visible content to total content
            float visibleRatio = m_Size.x / m_ContentWidth;
            m_ScrollButtonSize = glm::max(m_Size.x * visibleRatio, 20.0f); // Minimum size of 20 pixels
        }
    }

    // Get the current content offset (how many pixels to offset the content)
    float GetContentOffset() const
    {
        if (m_ContentWidth <= m_Size.x)
            return 0.0f;

        return m_ScrollPosition * (m_ContentWidth - m_Size.x);
    }

    // Override the Update method from IControl
    void Update(float delta) override
    {
        UpdateScrollButtonSize();
    }

    // Override the Render method from IControl
    void Render() override
    {
        glm::vec2 renderPos = GetRenderPosition();

        // Draw the scroll track (background)
        

        // Only draw the scroll button if content requires scrolling
        if (m_ContentWidth > m_Size.x)
        {
            // Calculate the position of the scroll button
            float buttonLeft = renderPos.x + m_ScrollPosition * (m_Size.x - m_ScrollButtonSize);
            glm::vec2 buttonPos = glm::vec2(buttonLeft, renderPos.y);
//            UIHelp::DrawImage(buttUIHelp::DrawRect(renderPos, glm::vec2(m_Size.x, m_ScrollButtonHeight), m_ScrollTrackColor);onPos + glm::vec2(1, 1), glm::vec2(m_ScrollButtonWidth - 7, m_ScrollButtonSize - 2), m_ScrollImage, glm::vec4(0.1, 1, 1, 1));
            UIHelp::DrawRect(renderPos, glm::vec2(m_Size.x, m_ScrollButtonHeight-7), m_ScrollTrackColor);
            UIHelp::DrawImage(buttonPos + glm::vec2(1,1), glm::vec2(m_ScrollButtonSize-2, m_ScrollButtonHeight-7), m_ScrollImage, glm::vec4(0.678 * 1.8, 0.847 * 1.8, 0.902 * 1.8, 1));
            // Draw the scroll button (thumb)

            //UIHelp::DrawRect(buttonPos, glm::vec2(m_ScrollButtonSize, m_ScrollButtonHeight), m_ScrollButtonColor);
        }

        // Render children controls (if any)
        RenderChildren();
    }

    // Override mouse handling for scroll interaction
    void OnMouseDown(int button) override
    {
        if (button == 0) // Left mouse button
        {
            glm::vec2 mousePos = GameInput::MousePosition;
            glm::vec2 renderPos = GetRenderPosition();

            // Check if the click is on the scroll button
            if (IsPointOnScrollButton(mousePos))
            {
                m_IsScrolling = true;
                m_LastMouseX = mousePos.x;
            }
            else if (mousePos.y >= renderPos.y && mousePos.y <= renderPos.y + m_Size.y)
            {
                // Click on the track - jump to that position
                //float relativeX = mousePos.x - renderPos.x;
                //float newPos = relativeX / m_Size.x;
                //SetScrollPosition(newPos);
            }
        }
    }

    void OnMouseUp(int button) override
    {
        if (button == 0) // Left mouse button
        {
            m_IsScrolling = false;
        }
    }

    void OnMouseMove(glm::vec2 position, glm::vec2 delta) override
    {
        if (m_IsScrolling)
        {
            // Calculate the scroll movement
            float deltaX = GameInput::MousePosition.x - m_LastMouseX;
            float scrollDelta = deltaX / (m_Size.x - m_ScrollButtonSize);

            // Update the scroll position
            SetScrollPosition(m_ScrollPosition + scrollDelta);

            m_LastMouseX = GameInput::MousePosition.x;
        }
    }

    void OnMouseWheel(float delta) override
    {
        // Scroll left or right based on the wheel delta
        // Typical scroll amount is 0.1 (10% of the visible area)
        float scrollAmount = 0.1f;
        SetScrollPosition(m_ScrollPosition - delta * scrollAmount);
    }

    void SetOnScrolled(std::function<void(float)> callback) {
        OnScrolled = callback;
    }

    void Scrolled() {
        if (OnScrolled) {
            OnScrolled(GetContentOffset());
        }
    }

    void Fix() {
        if (GetContentOffset() > m_ContentWidth)
        {
            auto v = m_ScrollPosition * m_ContentWidth;
            v = v - 0.1;
            if (m_ScrollPosition > v) {
                m_ScrollPosition = v;
                Scrolled();
            }
        }
    }

private:
    float m_ContentWidth;         // Total width of content to scroll
    float m_ScrollPosition;       // Current scroll position (0.0 to 1.0)
    float m_ScrollButtonSize;     // Width of the scroll button/thumb
    bool m_IsScrolling;           // Is the user currently dragging the scroll button?
    float m_LastMouseX;           // Last mouse X position when scrolling
    glm::vec4 m_ScrollTrackColor; // Color of the scroll track
    glm::vec4 m_ScrollButtonColor; // Color of the scroll button
    float m_ScrollButtonHeight;   // Height of the scroll button
    Texture2D* m_ScrollImage;
    // Helper method to check if a point is on the scroll button
    bool IsPointOnScrollButton(const glm::vec2& point)
    {
        glm::vec2 renderPos = GetRenderPosition();
        float buttonLeft = renderPos.x + m_ScrollPosition * (m_Size.x - m_ScrollButtonSize);
        float buttonRight = buttonLeft + m_ScrollButtonSize;

        return (point.y >= renderPos.y &&
            point.y <= renderPos.y + m_ScrollButtonHeight &&
            point.x >= buttonLeft &&
            point.x <= buttonRight);
    }

    std::function<void(float)> OnScrolled = nullptr;
};