#include "window-dock-handler.h"
#include "GameInput.h"
#include <algorithm>

IWindowDockHandler* IWindowDockHandler::s_Instance = nullptr;

void IWindowDockHandler::Initialize(IControl* rootControl) {
    m_RootControl = rootControl;
}

void IWindowDockHandler::RegisterDock(IWindowDock* dock) {
    if (dock && std::find(m_Docks.begin(), m_Docks.end(), dock) == m_Docks.end()) {
        m_Docks.push_back(dock);
    }
}

void IWindowDockHandler::UnregisterDock(IWindowDock* dock) {
    auto it = std::find(m_Docks.begin(), m_Docks.end(), dock);
    if (it != m_Docks.end()) {
        m_Docks.erase(it);
    }
}

void IWindowDockHandler::HandleWindowDrag(IWindow* window, glm::vec2 mousePosition) {
    if (!window) return;
    
    m_IsDraggingOverDock = false;
    m_HighlightedDock = nullptr;
    
    // Check each dock to see if we're in docking range
    for (auto dock : m_Docks) {
        if (dock->IsInDockingRange(window)) {
            DockPosition dropPos = dock->GetDropPosition(mousePosition);
            dock->HighlightDropRegion(dropPos);
            m_IsDraggingOverDock = true;
            m_HighlightedDock = dock;
        } else {
            dock->HighlightDropRegion(DOCK_NONE);
        }
    }
}

bool IWindowDockHandler::HandleWindowDrop(IWindow* window, glm::vec2 mousePosition) {
    if (!window || !m_IsDraggingOverDock || !m_HighlightedDock) {
        return false;
    }
    
    DockPosition dropPos = m_HighlightedDock->GetDropPosition(mousePosition);
    bool success = m_HighlightedDock->DockWindow(window, dropPos);
    
    // Reset highlight state
    m_HighlightedDock->HighlightDropRegion(DOCK_NONE);
    m_IsDraggingOverDock = false;
    m_HighlightedDock = nullptr;
    
    return success;
}

bool IWindowDockHandler::IsWindowDocked(IWindow* window) {
    return GetContainingPanel(window) != nullptr;
}

DockPanel* IWindowDockHandler::GetContainingPanel(IWindow* window) {
    if (!window) return nullptr;
    
    for (auto dock : m_Docks) {
        for (int i = 0; i < 5; i++) {
            DockPosition pos = static_cast<DockPosition>(i);
            DockPanel* panel = dock->GetDockPanel(pos);
            if (panel) {
                auto& dockedWindows = panel->GetDockedWindows();
                if (std::find(dockedWindows.begin(), dockedWindows.end(), window) != dockedWindows.end()) {
                    return panel;
                }
            }
        }
    }
    
    return nullptr;
}

void IWindowDockHandler::UndockWindow(IWindow* window) {
    if (!window) return;
    
    for (auto dock : m_Docks) {
        for (int i = 0; i < 5; i++) {
            DockPosition pos = static_cast<DockPosition>(i);
            DockPanel* panel = dock->GetDockPanel(pos);
            if (panel) {
                auto& dockedWindows = panel->GetDockedWindows();
                if (std::find(dockedWindows.begin(), dockedWindows.end(), window) != dockedWindows.end()) {
                    dock->UndockWindow(window);
                    return;
                }
            }
        }
    }
}
