#pragma once
#include "IControl.h"
#include "IButton.h"

class IControlGroup;
class IHorizontalScroller;
class IVerticalScroller;
class IDocker;
class IToolBar;

enum WindowArea {
    AREA_TITLE, AREA_LEFT, AREA_RIGHT, AREA_BOTTOM, AREA_RESIZER, AREA_CLIENT, AREA_NONE,AREA_CLOSE,AREA_TOP
};


class IWindow :
    public IControl
{
public:
    IWindow(std::string title, glm::vec2 position, glm::vec2 size, bool buttons = true, bool toolbar=false) : IControl(position, size) { m_Text = title; m_Buttons = buttons;m_HasToolBar = toolbar ; InitWindow(); }

    // Get the current active area (needed for undocking detection)
    WindowArea GetCurrentArea() const { return m_CurrentArea; }
    void InitWindow();
    void Update(float delta) override;
    void Render() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
    void OnMouseDoubleClick() override;


    bool InBounds(glm::vec2 position) override;
    void AlignWindow();
    void AddClientControl(IControl* control);
    IControlGroup* GetClientArea() {
        return m_ClientArea;
    }

    void AfterSet() override;


    IToolBar* GetToolBar() {
        return m_ToolBar;
    }
    // Make dragging state accessible to friends
    friend class DockPanel;
    friend class IWindowDock;
    friend class IWindowDockHandler;
    bool m_Dragging = false;
    void SetDocked(bool dock) {
        m_Docked = dock;
    }
    void SetDock(IDocker* dock) {
        m_Dock = dock;
    }
    IDocker* GetDock() {
        return m_Dock;
    }
    void DockWindow(IWindow* window);
    void SetMenu(IMainMenu* menu) { m_ActiveMenu = menu; }
    IMainMenu* GetMenu() { return m_ActiveMenu; }
    void SetAppTitle(std::string title) { m_AppTitle = title; }
    std::string GetAppTitle() {
        return m_AppTitle;
    }
    void Close() {
        m_RootControl->RemoveChild(this);
    }
    void SetDefaultMenu(IMainMenu* menu) {
        m_DefaultMenu = menu;
    }
    bool BeingDragged() {
        return m_Dragging;
    }
    void SetIsDocked(bool dock) {
        m_IsDocked = dock;
    }
    bool GetIsDocked() {
        return m_IsDocked;
    }
private:


    bool m_MouseIn = false;
    IDocker* m_Dock = nullptr;
    bool m_Docked = false;
    WindowArea m_CurrentArea = AREA_NONE;
    IButton* m_CloseButton = nullptr;
    IButton* m_MaximizeButton = nullptr;
    IButton* m_Resizer = nullptr;
    IButton* m_MinimizeButton = nullptr;
    IControlGroup* m_ClientArea = nullptr;
    IVerticalScroller* m_YScroller = nullptr;
    IHorizontalScroller* m_XScroller = nullptr;
    bool m_Buttons = true;
    Texture2D* m_TitleBarImage;
    //IButton* m_Resizer;
    Texture2D* m_TitleBG;
    Texture2D* m_ClientBG;
    std::vector<IWindow*> m_DockedWindows;
    glm::vec2 m_OriginalSize;
    glm::vec2 m_OriginalPosition;
    int m_CurrentTab = 0;
    glm::vec2 m_MousePos;
    IControlGroup* m_BaseArea = nullptr;
    IMainMenu* m_ActiveMenu = nullptr;
    std::string m_AppTitle = "";
    bool m_Outside = false;
    IMainMenu* m_DefaultMenu = nullptr;
    bool m_HasToolBar = false;
    IToolBar* m_ToolBar = nullptr;
    bool m_Locked = false;
    glm::vec2 m_ClosePos;
    bool m_IsDocked = false;
//    WindowInteract m_InteractMode = WI_None;
};
