#pragma once
#include "IControl.h"
#include "IWindow.h"
#include "window-dock-implementation.h"

// This class helps manage window docking events
// It should be integrated with GameUI to handle the docking logic
class IWindowDockHandler {
public:
    static IWindowDockHandler* GetInstance() {
        if (!s_Instance) {
            s_Instance = new IWindowDockHandler();
        }
        return s_Instance;
    }
    
    void Initialize(IControl* rootControl);
    
    // Register a window dock
    void RegisterDock(IWindowDock* dock);
    
    // Unregister a window dock
    void UnregisterDock(IWindowDock* dock);
    
    // Handle a window drag operation
    void HandleWindowDrag(IWindow* window, glm::vec2 mousePosition);
    
    // Handle a window drop operation
    bool HandleWindowDrop(IWindow* window, glm::vec2 mousePosition);
    
    // Check if a window is already docked
    bool IsWindowDocked(IWindow* window);
    
    // Get the dock panel containing a window
    DockPanel* GetContainingPanel(IWindow* window);
    
    // Undock a window
    void UndockWindow(IWindow* window);
    
private:
    IWindowDockHandler() {}
    static IWindowDockHandler* s_Instance;
    
    std::vector<IWindowDock*> m_Docks;
    IControl* m_RootControl = nullptr;
    
    // Indicates if a window is currently being dragged over a dock
    bool m_IsDraggingOverDock = false;
    
    // The currently highlighted dock (if any)
    IWindowDock* m_HighlightedDock = nullptr;
};
