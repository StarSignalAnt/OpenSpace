#pragma once
#include "IControl.h"
#include <vector>
#include <map>
#include <algorithm>
#include <random>
class IWindow;
class Texture2D;

enum DockArea {
    DOCK_LEFT,
    DOCK_RIGHT,
    DOCK_TOP,
    DOCK_BOTTOM,
    DOCK_CENTER,
    DOCK_NONE
};

struct DockZone {
    DockArea area;
    glm::vec2 position;  // Relative to docker
    glm::vec2 size;
    bool occupied;
    bool highlighted;    // Visual cue when a window is hovering over this zone
    int startY, endY;
    int startX, endX;
    glm::vec4 col;
    bool Valid = false;
    IWindow* Window = nullptr;

    DockZone() : area(DOCK_NONE), position(0, 0), size(0, 0),
        occupied(false), highlighted(false) {
    }

    DockZone(DockArea area, glm::vec2 position, glm::vec2 size)
        : area(area), position(position), size(size),
        occupied(false), highlighted(false) {
 
    }

    int GetStartY() {

        

    }

    int GetHeight() {

    }

};





class IDocker : public IControl
{
public:
    // Structure to define a docking zone
  

    IDocker(glm::vec2 position, glm::vec2 size);

    void Update(float delta) override;
    void Render() override;

    void WindowOver(IWindow* window, glm::vec2 position);
    void CreateZones();

    void OnMouseLeave() override {

        m_HoveringWindow = nullptr;
        m_HoveringArea = DOCK_NONE;

        // Reset animation state
        m_HighlightAlpha = 0.0f;
        m_DockingStarted = false;

    };
    DockZone GetLeft();
    
    DockZone GetRight();
    DockZone GetTop();
    DockZone GetBottom();
    DockZone GetCentre();
    DockZone GetZoneAt(glm::vec2 position);
    void DockWindow(IWindow* window, DockArea area);
    void DockWindow(IWindow* window,glm::vec2 position);

    // Get available dock area at position
   
    void RecreateZones();
    // Cancel any current dragging operation
    void WindowCancel();
    void Rebuild();
    glm::vec4 GetAreaDimensions(DockArea area,bool fill);

    glm::vec2 GetLeftSize(int sy, int ey);
    glm::vec2 GetBottomSize(int sx, int ex);
    void UndockWindow(IWindow* window);
    void AfterSet() override;
    DockZone GetDockArea(DockArea area);
    bool HasNot(IWindow* window);

private:
    std::vector<DockZone> m_DockZones;
    std::vector<DockZone> m_DockOrder;
    std::map<DockArea, std::vector<IWindow*>> m_DockedWindows;

    // Tracking window currently being dragged over the docker
    IWindow* m_HoveringWindow;
    DockArea m_HoveringArea;
    bool m_DockingStarted = false;

    // Visual cue properties
    float m_HighlightAlpha;    // For animation

    // Helper function to create initial dock zones
    void InitDockZones();

    bool Filled[3][3];
    float Size[3];
    DockZone m_OverZone;
    Texture2D* m_OverDockImage;
};