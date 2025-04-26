#pragma once
#include <glm/glm.hpp>
#include <vector>
class IControl;
class IWindow;
class IDocker;
class IMainMenu;
class UITheme;

class GameUI
{
public:

	GameUI();
	void InitForGame();
	void InitForFPApp();
	void ClearMenus();
	void SetRoot(IControl* control) { m_RootControl = control; }
	IControl* GetRoot() { return m_RootControl; };
	void UpdateUI(float delta);
	void RenderUI();
	IWindow* GetDraggingWindow() { return m_DraggingWindow; }
	IControl* GetBeneathWindow();
	static GameUI* m_Inst;
	void ResetMouse();
	void SetUISize(int w, int h);
	IMainMenu* GetMainMenu() { return m_ActiveMenu; };
	void SetMainMenu(IMainMenu* menu);
	void SetDragWindow(IWindow* window);
	static UITheme* GetTheme() { return m_Inst->m_Theme; }
	IControl* GetActiveControl() { return m_ControlActive; };
	void SetDefualtMenu(IMainMenu* menu) {
		m_DefaultMenu = menu;
		if (m_ActiveMenu == nullptr) {
			m_ActiveMenu = menu;
		}
	}
	void AddWindow(IWindow* window) {
		m_Windows.push_back(window);
	}
	void SetWindowSurface(IControl* surface) {
		m_WindowSurface = surface;
	}
	IControl* GetWindowSurface() {
		return m_WindowSurface;
	}
	void SetScissor(glm::vec4 s) {
		CurrentScissor = s;
	}
	glm::vec4 GetScissor() {
		return CurrentScissor;
	}
	void Deactivate();
	IControl* GetOverControl() {
		return m_ControlOver;
	}

private:
	glm::vec4 CurrentScissor;
	bool m_WindowDockingEnabled = true;
	glm::vec2 m_LastWindowPosition;
	UITheme* m_Theme = nullptr;
	IWindow* m_DraggingWindow = nullptr;
	IControl* m_FirstClick = nullptr;
	IControl* m_RootControl;
	IControl* m_ControlOver = nullptr;
	IControl* m_ControlPressed = nullptr;
	IControl* m_ControlActive = nullptr;
	int prev_Click = 0;
	IDocker* m_DraggingDock = nullptr;
//	IWindow* m_DraggingWindow = nullptr;
	IWindow* m_DockingWindow = nullptr;
	IWindow* m_TabWindow = nullptr;
	IWindow* m_TabTarget = nullptr;
	IMainMenu* m_ActiveMenu;
	IMainMenu* m_DefaultMenu = nullptr;
	std::vector<IWindow*> m_Windows;
	IControl* m_WindowSurface;
};

