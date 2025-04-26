#pragma once
#include <vector>
#include <string>

class IWindow;
class Texture2D;

enum AppType {

	ATYPE_Utility,
	ATYPE_System,
	ATYPE_AI,
	ATYPE_Development,
	ATYPE_Game,
	ATYPE_Internet,
	ATYPE_Media,
	ATYPE_Terminal,
	ATYPE_Misc,


};

class FPApp
{
public:

	FPApp() {};
	void SetName(std::string name) { m_AppTitle = name; }
	std::string GetName() { return m_AppTitle; }
	virtual void Start();
	virtual void InitApp() {};
	IWindow* GetStartWindow() {
		return m_StartWindow;
	}
	AppType GetType() {
		return m_AppType;
	}
	void SetIcon(Texture2D* icon) {
		m_Icon = icon;
	}
	Texture2D* GetIcon() {
		return m_Icon;
	}
protected:

	AppType m_AppType = AppType::ATYPE_Misc;
	std::string m_AppTitle = "App";
	std::vector<IWindow*> m_Windows;
	IWindow* m_StartWindow = nullptr;
	IWindow* m_AppWIndow = nullptr;
	Texture2D* m_Icon = nullptr;
};

