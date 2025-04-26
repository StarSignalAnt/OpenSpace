#pragma once
#include "IControl.h"
#include "IVerticalScroller.h"
#include "IHorizontalScroller.h"
#include "UIHelp.h"
#include "FutureApp.h"
#include "GameInput.h"
#include <map>

class ISizeBox : public IControl
{
public:
    ISizeBox() : IControl(),
        m_ContentWidth(0),
        m_ContentHeight(0),
        m_NeedsVerticalScroller(false),
        m_NeedsHorizontalScroller(false),
        m_ScrollerSize(10.0f),
        m_DrawFrame(true),
        m_FrameColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)),
        m_BgColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
        m_BlockChildEvents(false)
    {
        InitializeScrollers();
    }

    ISizeBox(glm::vec2 position, glm::vec2 size) : IControl(position, size),
        m_ContentWidth(0),
        m_ContentHeight(0),
        m_NeedsVerticalScroller(false),
        m_NeedsHorizontalScroller(false),
        m_ScrollerSize(10.0f),
        m_DrawFrame(true),
        m_FrameColor(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)),
        m_BgColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
        m_BlockChildEvents(false)
    {
        InitializeScrollers();
    }

    ~ISizeBox()
    {
        // No need to delete scrollers as they are children and will be cleaned up by IControl
    }

    // Override IControl virtual methods
    void Update(float delta) override
    {
        // Ensure scrollers are always on top (at the end of the children list)
        EnsureScrollersOnTop();

        // Update content bounds to check if scrollers are needed
        UpdateContentBounds();

        // Update scrollers if needed
        UpdateScrollers();

        // Apply the current content offset to children before updating them
        ApplyContentOffset();

        // Update all child controls
        UpdateChildren(delta);
    }

    void Render() override
    {
        // Ensure scrollers are always rendered last (on top)
        EnsureScrollersOnTop();

        auto renderPos = GetRenderPosition();

        // Calculate visible area dimensions
        float visibleWidth = m_Size.x;
        float visibleHeight = m_Size.y;

        if (m_NeedsVerticalScroller) {
            visibleWidth -= m_ScrollerSize;
        }

        if (m_NeedsHorizontalScroller) {
            visibleHeight -= m_ScrollerSize;
        }

        // Draw background
        UIHelp::DrawRect(renderPos, m_Size, m_BgColor);

        // Apply scissor to clip content to visible area
        UIHelp::setScissor(
            renderPos.x,
            renderPos.y,
            visibleWidth,
            visibleHeight,
            FutureApp::m_Inst->GetHeight()
        );

        // Render children except scrollers
        for (auto child : GetChildren()) {
            if (child != m_VerticalScroller && child != m_HorizontalScroller) {
                // Verify if at least part of the child is within the visible area
                glm::vec2 childPos = child->GetRenderPosition();
                glm::vec2 childSize = child->GetSize();

                // Only render if the child is at least partially visible
                if (childPos.x < renderPos.x + visibleWidth &&
                    childPos.x + childSize.x > renderPos.x &&
                    childPos.y < renderPos.y + visibleHeight &&
                    childPos.y + childSize.y > renderPos.y) {
                    child->Render();
                }
            }
        }

        // Disable scissor test
        UIHelp::RemoveScissor();

        // Render scrollers on top if needed
        if (m_NeedsVerticalScroller) {
            m_VerticalScroller->Render();
        }

        if (m_NeedsHorizontalScroller) {
            m_HorizontalScroller->Render();
        }

        // Draw frame/outline if enabled
        if (m_DrawFrame) {
            UIHelp::DrawOutlineRect(renderPos, m_Size, m_FrameColor);
        }
    }

    void PreRender() override
    {
        // Apply current scrolling before pre-rendering
        ApplyContentOffset();

        // Now pre-render all children
        PreRenderChildren();
    }

    // Override base InBounds to handle special case for scrollbars
    bool InBounds(glm::vec2 position) override
    {
        // First check if this control itself is within bounds using the base implementation
        bool withinBounds = IControl::InBounds(position);

        if (!withinBounds) {
            return false;
        }

        auto renderPos = GetRenderPosition();

        // If the mouse is over the scrollbars area, we should handle it
        if (m_NeedsVerticalScroller) {
            float scrollerX = renderPos.x + (m_Size.x - m_ScrollerSize);
            if (position.x >= scrollerX && position.x < renderPos.x + m_Size.x) {
                m_BlockChildEvents = true;  // Block child events when over scrollbar
                return true;
            }
        }

        if (m_NeedsHorizontalScroller) {
            float scrollerY = renderPos.y + (m_Size.y - m_ScrollerSize);
            if (position.y >= scrollerY && position.y < renderPos.y + m_Size.y) {
                m_BlockChildEvents = true;  // Block child events when over scrollbar
                return true;
            }
        }

        // For the main content area, the bounds check is normal
        m_BlockChildEvents = false;
        return true;
    }

    // New method to check if a child's events should be blocked
    bool ShouldBlockChildEvents(IControl* child, glm::vec2 position)
    {
        // If we're not blocking events in general, no need to check further
        if (!m_BlockChildEvents) {
            // Calculate visible content area
            auto renderPos = GetRenderPosition();
            float visibleWidth = m_Size.x - (m_NeedsVerticalScroller ? m_ScrollerSize : 0);
            float visibleHeight = m_Size.y - (m_NeedsHorizontalScroller ? m_ScrollerSize : 0);

            // Get child render position
            auto childPos = child->GetRenderPosition();
            auto childSize = child->GetSize();

            // Check if child is at least partially visible
            bool childVisible = !(childPos.x >= renderPos.x + visibleWidth ||
                childPos.x + childSize.x <= renderPos.x ||
                childPos.y >= renderPos.y + visibleHeight ||
                childPos.y + childSize.y <= renderPos.y);

            // Block events for invisible children
            return !childVisible;
        }

        return m_BlockChildEvents;
    }

    // Override AddChild to ensure scrollers always stay at the end of the children list
    void AddChild(IControl* control) override
    {
        // Don't add if it's one of our scrollers (prevents double-adding)
        if (control == m_VerticalScroller || control == m_HorizontalScroller) {
            IControl::AddChild(control);
            return;
        }

        // Store original position before adding
        m_OriginalPositions[control] = control->GetPosition();

        // Add the control as a child
        IControl::AddChild(control);

        // Ensure scrollers are at the end (highest z-order) by removing and re-adding them
        if (m_VerticalScroller) {
            IControl::RemoveChild(m_VerticalScroller);
            IControl::AddChild(m_VerticalScroller);
        }

        if (m_HorizontalScroller) {
            IControl::RemoveChild(m_HorizontalScroller);
            IControl::AddChild(m_HorizontalScroller);
        }

        // Update content bounds and scrollers
        UpdateContentBounds();
        UpdateScrollers();
        ApplyContentOffset();
    }

    // Override AfterSet to handle resizing
    void AfterSet() override
    {
        // When size changes, update scrollers
        UpdateContentBounds();
        UpdateScrollers();
        ApplyContentOffset();
    }

    // Set the width of scrollbars
    void SetScrollerSize(float size)
    {
        m_ScrollerSize = size;
        UpdateScrollers();
    }

    // Set frame visibility and color
    void SetFrameVisible(bool visible) { m_DrawFrame = visible; }
    void SetFrameColor(const glm::vec4& color) { m_FrameColor = color; }
    void SetBackgroundColor(const glm::vec4& color) { m_BgColor = color; }

private:
    IVerticalScroller* m_VerticalScroller;
    IHorizontalScroller* m_HorizontalScroller;
    bool m_DrawFrame;
    glm::vec4 m_FrameColor;
    glm::vec4 m_BgColor;

    float m_ContentWidth;
    float m_ContentHeight;

    bool m_NeedsVerticalScroller;
    bool m_NeedsHorizontalScroller;

    float m_ScrollerSize; // Width of vertical scroller, height of horizontal scroller

    // Store original positions of children to properly handle scrolling
    std::map<IControl*, glm::vec2> m_OriginalPositions;

    // Flag to indicate if child events should be blocked (when over scrollbars)
    bool m_BlockChildEvents;

    void InitializeScrollers()
    {
        // Create scrollers
        m_VerticalScroller = new IVerticalScroller();
        m_HorizontalScroller = new IHorizontalScroller();

        // Add scrollers as children (they should be at the end of the list for proper Z-ordering)
        IControl::AddChild(m_VerticalScroller);
        IControl::AddChild(m_HorizontalScroller);

        // Setup scrollers with callbacks that immediately apply the offset
        m_VerticalScroller->SetOnScrolled([this](float offset) {
            // Immediately apply the changes
            ApplyContentOffset();
            });

        m_HorizontalScroller->SetOnScrolled([this](float offset) {
            // Immediately apply the changes
            ApplyContentOffset();
            });

        // Make sure scrollbars start with correct settings
        m_VerticalScroller->SetScrollTrackColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
        m_HorizontalScroller->SetScrollTrackColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        // Enable content culling
        SetCullChildren(true);
    }

    // Helper function to move scrollers to the end of the children list (highest Z-order)
    void EnsureScrollersOnTop()
    {
        if (m_VerticalScroller) {
            IControl::RemoveChild(m_VerticalScroller);
            IControl::AddChild(m_VerticalScroller);
        }

        if (m_HorizontalScroller) {
            IControl::RemoveChild(m_HorizontalScroller);
            IControl::AddChild(m_HorizontalScroller);
        }
    }

    void UpdateContentBounds()
    {
        // Calculate the maximum extent of all child controls based on ORIGINAL positions
        m_ContentWidth = 0;
        m_ContentHeight = 0;

        for (auto child : GetChildren()) {
            // Skip scrollers when calculating content bounds
            if (child == m_VerticalScroller || child == m_HorizontalScroller) {
                continue;
            }

            // Use the original position, not the current scrolled position
            glm::vec2 originalPos = m_OriginalPositions[child];
            float childRight = originalPos.x + child->GetSize().x;
            float childBottom = originalPos.y + child->GetSize().y;

            // Update the content bounds
            m_ContentWidth = std::max(m_ContentWidth, childRight);
            m_ContentHeight = std::max(m_ContentHeight, childBottom);
        }

        // Ensure minimum size to avoid division by zero in scrollers
        m_ContentWidth = std::max(m_ContentWidth, 1.0f);
        m_ContentHeight = std::max(m_ContentHeight, 1.0f);

        // Add a small padding to ensure content at the edge is fully visible
        m_ContentWidth += 5.0f;
        m_ContentHeight += 5.0f;
    }

    void UpdateScrollers()
    {
        // First determine if we need scrollers based on raw content vs size
        bool needsVertical = m_ContentHeight > m_Size.y;
        bool needsHorizontal = m_ContentWidth > m_Size.x;

        // Check if adding one scroller necessitates the other
        if (needsVertical && !needsHorizontal) {
            needsHorizontal = m_ContentWidth > (m_Size.x - m_ScrollerSize);
        }

        if (needsHorizontal && !needsVertical) {
            needsVertical = m_ContentHeight > (m_Size.y - m_ScrollerSize);
        }

        // Store previous state to detect changes
        bool prevVertical = m_NeedsVerticalScroller;
        bool prevHorizontal = m_NeedsHorizontalScroller;

        // Set visibility and position based on needs
        m_NeedsVerticalScroller = needsVertical;
        m_NeedsHorizontalScroller = needsHorizontal;

        // Calculate visible area size
        float visibleWidth = m_Size.x - (needsVertical ? m_ScrollerSize : 0);
        float visibleHeight = m_Size.y - (needsHorizontal ? m_ScrollerSize : 0);

        // Configure vertical scroller
        if (needsVertical) {
            m_VerticalScroller->Set(
                glm::vec2(visibleWidth, 0),
                glm::vec2(m_ScrollerSize, visibleHeight)
            );

            // Important: Set the content height correctly to ensure full scrolling range
            float effectiveContentHeight = m_ContentHeight;
            m_VerticalScroller->SetContentHeight(effectiveContentHeight);

            // Ensure scrollbar position is valid (cannot scroll past end)
            if (m_VerticalScroller->GetScrollPosition() > 0.999f) {
                m_VerticalScroller->SetScrollPosition(0.999f);
            }
        }

        // Configure horizontal scroller
        if (needsHorizontal) {
            m_HorizontalScroller->Set(
                glm::vec2(0, visibleHeight),
                glm::vec2(visibleWidth, m_ScrollerSize)
            );

            // Important: Set the content width correctly to ensure full scrolling range
            float effectiveContentWidth = m_ContentWidth;
            m_HorizontalScroller->SetContentWidth(effectiveContentWidth);

            // Ensure scrollbar position is valid (cannot scroll past end)
            if (m_HorizontalScroller->GetScrollPosition() > 0.999f) {
                m_HorizontalScroller->SetScrollPosition(0.999f);
            }
        }

        // Force content offset update if scrollbar visibility changed
        if (prevVertical != needsVertical || prevHorizontal != needsHorizontal) {
            ApplyContentOffset();
        }
    }

    void ApplyContentOffset()
    {
        // Calculate the correct offset values based on scroll position
        float verticalOffset = 0;
        float horizontalOffset = 0;

        if (m_NeedsVerticalScroller) {
            // Calculate visible height (content area without horizontal scrollbar if present)
            float visibleHeight = m_Size.y - (m_NeedsHorizontalScroller ? m_ScrollerSize : 0);

            // Convert scroll position (0-1) to pixel offset
            verticalOffset = m_VerticalScroller->GetScrollPosition() * (m_ContentHeight - visibleHeight);
        }

        if (m_NeedsHorizontalScroller) {
            // Calculate visible width (content area without vertical scrollbar if present)
            float visibleWidth = m_Size.x - (m_NeedsVerticalScroller ? m_ScrollerSize : 0);

            // Convert scroll position (0-1) to pixel offset
            horizontalOffset = m_HorizontalScroller->GetScrollPosition() * (m_ContentWidth - visibleWidth);
        }

        // CRITICAL FIX: The IControl::GetRenderPosition() doesn't add its own offset
        // So instead of using SetOffset, we need to MOVE the actual position of each child
        for (auto child : GetChildren()) {
            if (child != m_VerticalScroller && child != m_HorizontalScroller) {
                // Store original position if we haven't already
                if (m_OriginalPositions.find(child) == m_OriginalPositions.end()) {
                    m_OriginalPositions[child] = child->GetPosition();
                }

                // Calculate new position = original position - scroll offset
                glm::vec2 originalPos = m_OriginalPositions[child];
                glm::vec2 newPos = originalPos - glm::vec2(horizontalOffset, verticalOffset);

                // Update the actual position instead of using offset
                child->Set(newPos, child->GetSize());
            }
        }
    }
};