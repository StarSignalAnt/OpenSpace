#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Texture2D;

class UITheme
{
public:

	UITheme();
	virtual void SetupTheme() {};
	glm::vec4 GetBackground() { return m_Background; }
	glm::vec4 GetForeground() { return m_Foreground; }
	glm::vec4 GetWindowTileBar() { return m_WindowTitleBar; };
	float GetWindowAlpha() { return m_WindowAlpha; }
	bool GetWindowBlur() { return m_WindowBlur; }
	Texture2D* GetButton() { return m_Button; }
	Texture2D* GetScrollButton() { return m_ScrollButton; };
	glm::vec4 GetWindowLight() { return m_WindowLight; };
	glm::vec4 GetWindowDark() { return m_WindowDark; };
	bool IsGame() {
		return m_GameTheme;
	}

protected:

	glm::vec4 m_Background;
	glm::vec4 m_Foreground;
	glm::vec4 m_WindowTitleBar;
	glm::vec4 m_WindowLight;
	glm::vec4 m_WindowDark;
	bool m_WindowBlur = false;
	float m_WindowAlpha = 0.95f;
	Texture2D* m_Button;
	Texture2D* m_ScrollButton;
	bool m_GameTheme = false;


};

