#pragma once
#include "IControl.h"
#include "IWindow.h"
#include <map>
#include <memory>

enum DockPosition {
    DOCK_LEFT,
    DOCK_RIGHT,
    DOCK_TOP,
    DOCK_BOTTOM,
    DOCK_CENTER,
    DOCK_NONE
};

class IWindowDock;

// Class to manage a group of docked windows with tabs
class DockPanel : public IControl {
public:
    DockPanel(glm::vec2 position, glm::vec2 size, DockPosition dockPos);
    void Update(float delta) override;
    void Render() override;
    
    void AddWindow(IWindow* window);
    void RemoveWindow(IWindow* window);
    void SetActiveWindow(IWindow* window);
    void SetActiveWindow(int index);
    IWindow* GetActiveWindow() { return m_ActiveWindow; }
    std::vector<IWindow*>& GetDockedWindows() { return m_DockedWindows; }
    void UpdateTabPositions();
    
    DockPosition GetDockPosition() { return m_DockPosition; }
    void SetParentDock(IWindowDock* dock) { m_ParentDock = dock; }
    
    bool InTabBounds(glm::vec2 position, int& tabIndex);
    
private:
    std::vector<IWindow*> m_DockedWindows;
    IWindow* m_ActiveWindow = nullptr;
    DockPosition m_DockPosition;
    bool m_TabDragging = false;
    int m_DraggedTabIndex = -1;
    IWindowDock* m_ParentDock = nullptr;
    int m_TabHeight = 25;
    float m_TabWidth = 100.0f;
};

class IWindowDock : public IControl {
public:
    IWindowDock(glm::vec2 position, glm::vec2 size);
    void Update(float delta) override;
    void Render() override;
    
    // Window docking methods
    bool DockWindow(IWindow* window, DockPosition position);
    void UndockWindow(IWindow* window);
    DockPosition GetDropPosition(glm::vec2 mousePosition);
    
    // Highlights the region where a window would be dropped
    void HighlightDropRegion(DockPosition position);
    
    // Returns true if mouse is within docking threshold
    bool IsInDockingRange(IWindow* window);
    
    // Recalculate panel sizes after docking/undocking
    void RecalculatePanelSizes();
    
    // Get dock panel at position
    DockPanel* GetDockPanel(DockPosition position);

private:
    // Map to store panels for each docking position
    std::map<DockPosition, DockPanel*> m_DockPanels;
    
    // Current highlighted drop region
    DockPosition m_HighlightedRegion = DOCK_NONE;
    
    // Window currently being dragged for potential docking
    IWindow* m_DraggingWindow = nullptr;
    
    // Percentage sizes for each region
    float m_LeftWidth = 0.2f;
    float m_RightWidth = 0.2f;
    float m_TopHeight = 0.2f;
    float m_BottomHeight = 0.2f;
    
    // Threshold for docking detection when dragging windows
    float m_DockingThreshold = 40.0f;
    
    // Create initial dock panels
    void InitializeDockPanels();
};
