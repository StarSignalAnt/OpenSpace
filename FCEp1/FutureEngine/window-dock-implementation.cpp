#include "window-dock-implementation.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "FutureApp.h"
#include <algorithm>
#include <iostream>

//--------------------------------
// DockPanel Implementation
//--------------------------------

DockPanel::DockPanel(glm::vec2 position, glm::vec2 size, DockPosition dockPos) 
    : IControl(position, size), m_DockPosition(dockPos) {
}

void DockPanel::Update(float delta) {
    // Update active window if there is one
    if (m_ActiveWindow) {
        // Update window size to match panel size minus tab height
        glm::vec2 windowSize = m_Size;
        if (!m_DockedWindows.empty() && m_DockedWindows.size() > 1) {
            // Adjust for tab height
            windowSize.y -= m_TabHeight;
        }
        
        m_ActiveWindow->Set(glm::vec2(0, m_DockedWindows.size() > 1 ? m_TabHeight : 0), windowSize);
        m_ActiveWindow->Update(delta);
    }
}

void DockPanel::Render() {
    auto pos = GetRenderPosition();
    
    if (m_DockedWindows.empty()) {
        // Empty panel - draw with semi-transparent fill
        UIHelp::DrawRect(pos, m_Size, glm::vec4(0.2f, 0.2f, 0.2f, 0.5f));
        return;
    }
    
    // Draw panel background
    UIHelp::DrawRect(pos, m_Size, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    
    // Draw tabs if we have multiple windows
    if (m_DockedWindows.size() > 1) {
        float tabX = 0;
        
        for (size_t i = 0; i < m_DockedWindows.size(); i++) {
            auto window = m_DockedWindows[i];
            
            // Draw tab background
            glm::vec4 tabColor = (window == m_ActiveWindow) 
                ? glm::vec4(0.5f, 0.5f, 0.5f, 1.0f) 
                : glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
            
            UIHelp::DrawRect(pos + glm::vec2(tabX, 0), 
                            glm::vec2(m_TabWidth, m_TabHeight), 
                            tabColor);
            
            // Draw tab text
            UIHelp::DrawText(pos + glm::vec2(tabX + 5, 6), 
                           window->GetText(), 
                           glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            
            // Draw tab separator
            UIHelp::DrawRect(pos + glm::vec2(tabX + m_TabWidth - 1, 0), 
                            glm::vec2(1, m_TabHeight), 
                            glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
            
            tabX += m_TabWidth;
        }
    }
    
    // Render active window
    if (m_ActiveWindow) {
        // Override position to ensure proper rendering
        glm::vec2 origPos = m_ActiveWindow->GetPosition();
        glm::vec2 offset = pos;
        
        if (m_DockedWindows.size() > 1) {
            offset.y += m_TabHeight;
        }
        
        m_ActiveWindow->Set(glm::vec2(0, m_DockedWindows.size() > 1 ? m_TabHeight : 0));
        m_ActiveWindow->SetRoot(this);
        m_ActiveWindow->Render();
    }
}

void DockPanel::AddWindow(IWindow* window) {
    if (std::find(m_DockedWindows.begin(), m_DockedWindows.end(), window) == m_DockedWindows.end()) {
        m_DockedWindows.push_back(window);
        SetActiveWindow(window);
        UpdateTabPositions();
    }
}

void DockPanel::RemoveWindow(IWindow* window) {
    auto it = std::find(m_DockedWindows.begin(), m_DockedWindows.end(), window);
    if (it != m_DockedWindows.end()) {
        m_DockedWindows.erase(it);
        
        // Update active window if necessary
        if (m_ActiveWindow == window) {
            if (!m_DockedWindows.empty()) {
                m_ActiveWindow = m_DockedWindows[0];
            } else {
                m_ActiveWindow = nullptr;
            }
        }
        
        UpdateTabPositions();
    }
}

void DockPanel::SetActiveWindow(IWindow* window) {
    auto it = std::find(m_DockedWindows.begin(), m_DockedWindows.end(), window);
    if (it != m_DockedWindows.end()) {
        m_ActiveWindow = window;
    }
}

void DockPanel::SetActiveWindow(int index) {
    if (index >= 0 && index < m_DockedWindows.size()) {
        m_ActiveWindow = m_DockedWindows[index];
    }
}

void DockPanel::UpdateTabPositions() {
    // Calculate tab width based on available space and number of tabs
    if (!m_DockedWindows.empty()) {
        m_TabWidth = std::min(100.0f, m_Size.x / m_DockedWindows.size());
    }
}

bool DockPanel::InTabBounds(glm::vec2 position, int& tabIndex) {
    if (m_DockedWindows.size() <= 1) {
        tabIndex = -1;
        return false;
    }
    
    auto panelPos = GetRenderPosition();
    
    // Check if within tab area
    if (position.y >= panelPos.y && position.y <= panelPos.y + m_TabHeight &&
        position.x >= panelPos.x && position.x <= panelPos.x + m_Size.x) {
        
        // Calculate which tab was clicked
        float relativeX = position.x - panelPos.x;
        tabIndex = static_cast<int>(relativeX / m_TabWidth);
        
        if (tabIndex >= 0 && tabIndex < m_DockedWindows.size()) {
            return true;
        }
    }
    
    tabIndex = -1;
    return false;
}

//--------------------------------
// IWindowDock Implementation
//--------------------------------

IWindowDock::IWindowDock(glm::vec2 position, glm::vec2 size) 
    : IControl(position, size) {
    InitializeDockPanels();
}

void IWindowDock::InitializeDockPanels() {
    // Calculate initial sizes for each panel region
    float width = m_Size.x;
    float height = m_Size.y;
    
    float leftWidth = width * m_LeftWidth;
    float rightWidth = width * m_RightWidth;
    float centerWidth = width - leftWidth - rightWidth;
    
    float topHeight = height * m_TopHeight;
    float bottomHeight = height * m_BottomHeight;
    float centerHeight = height - topHeight - bottomHeight;
    
    // Create panels for each dock position
    m_DockPanels[DOCK_LEFT] = new DockPanel(
        glm::vec2(0, topHeight), 
        glm::vec2(leftWidth, centerHeight), 
        DOCK_LEFT
    );
    
    m_DockPanels[DOCK_RIGHT] = new DockPanel(
        glm::vec2(leftWidth + centerWidth, topHeight), 
        glm::vec2(rightWidth, centerHeight), 
        DOCK_RIGHT
    );
    
    m_DockPanels[DOCK_TOP] = new DockPanel(
        glm::vec2(0, 0), 
        glm::vec2(width, topHeight), 
        DOCK_TOP
    );
    
    m_DockPanels[DOCK_BOTTOM] = new DockPanel(
        glm::vec2(0, topHeight + centerHeight), 
        glm::vec2(width, bottomHeight), 
        DOCK_BOTTOM
    );
    
    m_DockPanels[DOCK_CENTER] = new DockPanel(
        glm::vec2(leftWidth, topHeight), 
        glm::vec2(centerWidth, centerHeight), 
        DOCK_CENTER
    );
    
    // Add panels as children and set their parent dock
    for (auto& [pos, panel] : m_DockPanels) {
        AddChild(panel);
        panel->SetParentDock(this);
    }
}

void IWindowDock::Update(float delta) {
    // Check for window dragging to enable docking
    auto* activeWindow = dynamic_cast<IWindow*>(GameInput::m_Dragging);
    
    if (activeWindow && GameInput::Buttons[MOUSE_BUTTON_LEFT]) {
        m_DraggingWindow = activeWindow;
        
        // Check if the window is in docking range
        if (IsInDockingRange(activeWindow)) {
            // Get potential drop position
            DockPosition dropPos = GetDropPosition(GameInput::MousePosition);
            
            // Show highlight for potential drop region
            HighlightDropRegion(dropPos);
        } else {
            HighlightDropRegion(DOCK_NONE);
        }
    } else if (!GameInput::Buttons[MOUSE_BUTTON_LEFT] && m_DraggingWindow) {
        // Mouse released, check if we should dock the window
        if (m_HighlightedRegion != DOCK_NONE) {
            DockWindow(m_DraggingWindow, m_HighlightedRegion);
        }
        
        m_DraggingWindow = nullptr;
        HighlightDropRegion(DOCK_NONE);
    }
    
    // Update all dock panels
    UpdateChildren(delta);
    
    // Update docked windows via their panels
    for (auto& [pos, panel] : m_DockPanels) {
        panel->Update(delta);
    }
}

void IWindowDock::Render() {
    auto pos = GetRenderPosition();
    
    // Draw dock background
    UIHelp::DrawRect(pos, m_Size, glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
    
    // Render dock panels
    for (auto& [pos, panel] : m_DockPanels) {
        panel->Render();
    }
    
    // Render drop region highlight if dragging
    if (m_HighlightedRegion != DOCK_NONE && m_DraggingWindow) {
        auto panel = m_DockPanels[m_HighlightedRegion];
        auto panelPos = panel->GetRenderPosition();
        
        // Draw highlighted region with semi-transparent overlay
        UIHelp::DrawRect(panelPos, panel->GetSize(), glm::vec4(0.0f, 0.5f, 1.0f, 0.3f));
    }
}

bool IWindowDock::DockWindow(IWindow* window, DockPosition position) {
    if (!window || position == DOCK_NONE || !m_DockPanels.count(position)) {
        return false;
    }
    
    // Get the original parent of the window
    IControl* originalParent = window->GetRoot();
    if (originalParent) {
        originalParent->RemoveChild(window);
    }
    
    // Add the window to the appropriate dock panel
    m_DockPanels[position]->AddWindow(window);
    
    // Recalculate panel sizes
    RecalculatePanelSizes();
    
    return true;
}

void IWindowDock::UndockWindow(IWindow* window) {
    // Find which panel contains this window
    for (auto& [pos, panel] : m_DockPanels) {
        auto& dockedWindows = panel->GetDockedWindows();
        auto it = std::find(dockedWindows.begin(), dockedWindows.end(), window);
        
        if (it != dockedWindows.end()) {
            panel->RemoveWindow(window);
            
            // Reset the window position
            window->Set(GameInput::MousePosition, window->GetSize());
            
            // Re-add to the root control
            if (m_RootControl) {
                m_RootControl->AddChild(window);
            }
            
            break;
        }
    }
    
    // Recalculate panel sizes
    RecalculatePanelSizes();
}

DockPosition IWindowDock::GetDropPosition(glm::vec2 mousePosition) {
    auto dockPos = GetRenderPosition();
    float width = m_Size.x;
    float height = m_Size.y;
    
    // Calculate distances from edges
    float distFromLeft = mousePosition.x - dockPos.x;
    float distFromRight = (dockPos.x + width) - mousePosition.x;
    float distFromTop = mousePosition.y - dockPos.y;
    float distFromBottom = (dockPos.y + height) - mousePosition.y;
    
    // Calculate thresholds
    float horizontalThreshold = width * 0.25f;
    float verticalThreshold = height * 0.25f;
    
    // Determine dock position based on proximity to edges
    if (distFromLeft < horizontalThreshold && distFromLeft < distFromRight) {
        return DOCK_LEFT;
    } else if (distFromRight < horizontalThreshold && distFromRight < distFromLeft) {
        return DOCK_RIGHT;
    } else if (distFromTop < verticalThreshold && distFromTop < distFromBottom) {
        return DOCK_TOP;
    } else if (distFromBottom < verticalThreshold && distFromBottom < distFromTop) {
        return DOCK_BOTTOM;
    } else {
        return DOCK_CENTER;
    }
}

void IWindowDock::HighlightDropRegion(DockPosition position) {
    m_HighlightedRegion = position;
}

bool IWindowDock::IsInDockingRange(IWindow* window) {
    if (!window) return false;
    
    auto windowPos = window->GetRenderPosition();
    auto dockPos = GetRenderPosition();
    
    // Check if window is close to the dock
    return (
        windowPos.x >= dockPos.x - m_DockingThreshold && 
        windowPos.y >= dockPos.y - m_DockingThreshold && 
        windowPos.x <= dockPos.x + m_Size.x + m_DockingThreshold && 
        windowPos.y <= dockPos.y + m_Size.y + m_DockingThreshold
    );
}

void IWindowDock::RecalculatePanelSizes() {
    float width = m_Size.x;
    float height = m_Size.y;
    
    // Adjust panel sizes based on content if needed
    // For now, we'll use fixed percentages
    
    float leftWidth = width * m_LeftWidth;
    float rightWidth = width * m_RightWidth;
    float centerWidth = width - leftWidth - rightWidth;
    
    float topHeight = height * m_TopHeight;
    float bottomHeight = height * m_BottomHeight;
    float centerHeight = height - topHeight - bottomHeight;
    
    // Update panel positions and sizes
    m_DockPanels[DOCK_LEFT]->Set(
        glm::vec2(0, topHeight), 
        glm::vec2(leftWidth, centerHeight)
    );
    
    m_DockPanels[DOCK_RIGHT]->Set(
        glm::vec2(leftWidth + centerWidth, topHeight), 
        glm::vec2(rightWidth, centerHeight)
    );
    
    m_DockPanels[DOCK_TOP]->Set(
        glm::vec2(0, 0), 
        glm::vec2(width, topHeight)
    );
    
    m_DockPanels[DOCK_BOTTOM]->Set(
        glm::vec2(0, topHeight + centerHeight), 
        glm::vec2(width, bottomHeight)
    );
    
    m_DockPanels[DOCK_CENTER]->Set(
        glm::vec2(leftWidth, topHeight), 
        glm::vec2(centerWidth, centerHeight)
    );
}

DockPanel* IWindowDock::GetDockPanel(DockPosition position) {
    auto it = m_DockPanels.find(position);
    if (it != m_DockPanels.end()) {
        return it->second;
    }
    return nullptr;
}
